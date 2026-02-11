
/************ BLYNK CONFIG ************/
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "your_blynk_template_id"
#define BLYNK_TEMPLATE_NAME "Smart Fish Feeding system"
#define BLYNK_AUTH_TOKEN "your_blynk_auth_token_here"


#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>            
#include <time.h>


/************ WIFI ************/
char ssid[] = "***********";
char pass[] = "*****";


/************ NTP TIME ************/
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 5 * 3600 + 1800;
const int daylightOffset_sec = 0;


/************ PINS ************/
#define DS18B20_PIN 4
#define PH_PIN 35
#define TDS_PIN 34
#define SERVO_PIN 15
#define BUTTON_PIN 13
#define BUZZER_PIN 19
#define RED_LED_PIN 18
#define GREEN_LED_PIN 16   // RX2


/************ OLED ************/
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDR 0x3C


/************ IDEAL RANGES ************/
#define PH_MIN 6.5
#define PH_MAX 7.5
#define PH_IDEAL_LOW 6.8
#define PH_IDEAL_HIGH 7.2


#define TDS_MIN 100
#define TDS_MAX 300
#define TDS_IDEAL_LOW 150
#define TDS_IDEAL_HIGH 250


#define TEMP_MIN 24
#define TEMP_MAX 28
#define TEMP_IDEAL_LOW 25
#define TEMP_IDEAL_HIGH 27


#define TDS_CALIBRATION 0.9


/************ WATER QUALITY ************/
enum WaterQuality { GOOD, MODERATE, POOR };
WaterQuality quality = GOOD;
String waterQuality = "";


/************ BLYNK NOTIFICATION CONTROL ************/
unsigned long lastNotifyTime = 0;
const unsigned long notifyInterval = 5 * 60 * 1000; // 5 minutes
WaterQuality lastNotifiedQuality = GOOD;


/************ OBJECTS ************/
Servo feederServo;
OneWire oneWire(DS18B20_PIN);
DallasTemperature tempSensor(&oneWire);
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
BlynkTimer timer;


/************ VARIABLES ************/
float tempC = 0, pH = 0, tds = 0;


/************ BLYNK CACHE (OPTIMIZATION) ************/
float lastTemp = -1000;
float lastPH = -1000;
float lastTDS = -1000;
int lastQualityScore = -1;
String lastQualityText = "";


/************ BUZZER ************/
unsigned long lastBeep = 0;
bool beepState = false;


/************ OLED SLIDE ************/
int slideIndex = 0;
unsigned long lastSlideChange = 0;
const unsigned long slideInterval = 3000;


/************ FEED FLAGS ************/
bool fedMorning = false;
bool fedEvening = false;


/************ TDS FUNCTION ************/
float readTDS(float temperature) {
  float sum = 0;
  for (int i = 0; i < 30; i++) {
    sum += analogRead(TDS_PIN);
    delay(2);
  }
  float voltage = (sum / 30.0) * (3.3 / 4095.0);
  float compensation = 1.0 + 0.02 * (temperature - 25.0);
  return (voltage / compensation) * 500 * TDS_CALIBRATION;
}


/************ pH FUNCTION (UNCHANGED) ************/
float readPH() {
  int buffer_arr[10], temp;
  unsigned long int avgval = 0;
  float calibration_value = 21.25;  


  // Take 10 samples
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(PH_PIN);
    delay(30);
  }


  // Sort values
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }


  // Average middle 6 values
  for (int i = 2; i < 8; i++) avgval += buffer_arr[i];


  // Convert ADC to Voltage (ESP32 → 0–4095, 0–3.3V)
  float volt = (float)(avgval / 6.0) * 3.3 / 4095.0;


  // Convert to pH
  float ph_act = -5.70 * volt + calibration_value;


  // Debug (optional, you can keep or remove)
  Serial.print("pH Sensor Voltage: ");
  Serial.print(volt, 3);
  Serial.print(" V  |  pH Value: ");
  Serial.println(ph_act, 2);

  return ph_act;
}




/************ SENSOR READ ************/
void readSensors() {
  tempSensor.requestTemperatures();
  tempC = tempSensor.getTempCByIndex(0);


  pH = readPH();
  tds = readTDS(tempC);
 
  int score = 0;




  if (pH < PH_MIN || pH > PH_MAX) score += 3;
  else if (pH < PH_IDEAL_LOW || pH > PH_IDEAL_HIGH) score += 2;




  if (tds < TDS_MIN || tds > TDS_MAX) score += 3;
  else if (tds < TDS_IDEAL_LOW || tds > TDS_IDEAL_HIGH) score += 2;




  if (tempC < TEMP_MIN || tempC > TEMP_MAX) score += 3;
  else if (tempC < TEMP_IDEAL_LOW || tempC > TEMP_IDEAL_HIGH) score += 2;




  if (score <= 2) quality = GOOD;
  else if (score <= 5) quality = MODERATE;
  else quality = POOR;




  if (quality == GOOD) waterQuality = "GOOD";
  else if (quality == MODERATE) waterQuality = "MODERATE";
  else waterQuality = "POOR";




  int qualityScore = (quality == GOOD) ? 10 : (quality == MODERATE ? 5 : 0);


  /************ BLYNK NOTIFICATIONS ************/
unsigned long now = millis();


if (quality != lastNotifiedQuality && (now - lastNotifyTime > notifyInterval)) {


  if (quality == POOR) {
    Blynk.logEvent(
      "water_quality",
      "⚠️ WATER QUALITY POOR!\nImmediate attention required."
    );
  }
  else if (quality == MODERATE) {
    Blynk.logEvent(
      "water_quality",
      "ℹ️ Water quality is MODERATE.\nPlease monitor conditions."
    );
  }


  lastNotifyTime = now;
  lastNotifiedQuality = quality;
}




  /* ===== OPTIMIZED BLYNK UPDATES ===== */




  if (abs(tempC - lastTemp) > 0.1) {
    Blynk.virtualWrite(V0, tempC);
    lastTemp = tempC;
  }




  if (abs(pH - lastPH) > 0.05) {
    Blynk.virtualWrite(V1, pH);
    lastPH = pH;
  }




  if (abs(tds - lastTDS) > 5) {
    Blynk.virtualWrite(V2, tds);
    lastTDS = tds;
  }




  if (waterQuality != lastQualityText) {
    Blynk.virtualWrite(V3, waterQuality);
    lastQualityText = waterQuality;
  }




  if (qualityScore != lastQualityScore) {
    Blynk.virtualWrite(V5, qualityScore);
    lastQualityScore = qualityScore;
  }




  digitalWrite(GREEN_LED_PIN, quality == GOOD);
  digitalWrite(RED_LED_PIN, quality == POOR);
}




/************ BUZZER ************/
void handleBuzzer() {
  unsigned long now = millis();


  // GOOD → No beep
  if (quality == GOOD) {
    digitalWrite(BUZZER_PIN, HIGH); // OFF (active LOW)
    beepState = false;
  }


  // MODERATE → Beep every 1 second
  else if (quality == MODERATE) {
    if (now - lastBeep >= 1000) {   // 1 second delay
      beepState = !beepState;
      digitalWrite(BUZZER_PIN, beepState ? LOW : HIGH);
      lastBeep = now;
    }
  }


  // POOR → Continuous beep
  else { // POOR
    digitalWrite(BUZZER_PIN, LOW);  // ON continuously
  }
}






/************ OLED DISPLAY ************/
void updateSlideDisplay() {
  if (millis() - lastSlideChange > slideInterval) {
    slideIndex = (slideIndex + 1) % 4;
    lastSlideChange = millis();
  }




  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);




  switch (slideIndex) {
    case 0:
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.printf("T:%.1fC", tempC);
      display.setCursor(0, 32);
      display.printf("pH:%.2f", pH);
      break;




    case 1:
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.printf("TDS\n\n%.0fppm", tds);
      break;




    case 2:
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.println(" Status\n");
      display.println(waterQuality);
      break;




    case 3: {
      struct tm timeinfo;
      if (getLocalTime(&timeinfo)) {
        display.setTextSize(2);
        display.setCursor(0, 0);
        display.printf("%02d/%02d/%04d\n",
                       timeinfo.tm_mday,
                       timeinfo.tm_mon + 1,
                       timeinfo.tm_year + 1900);
        display.printf("%02d:%02d:%02d",
                       timeinfo.tm_hour,
                       timeinfo.tm_min,
                       timeinfo.tm_sec);
      }
      break;
    }
  }
  display.display();
}




/************ FEED FUNCTION ************/
void feedFish() {
  int feedDelay;
  if (quality == GOOD) feedDelay = 800;
  else if (quality == MODERATE) feedDelay = 400;
  else feedDelay = 200;




  feederServo.write(75);
  delay(feedDelay);
  feederServo.write(0);
}




/************ TIME CHECK ************/
void checkFeedingTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;




  if (timeinfo.tm_hour == 8 && timeinfo.tm_min == 0 && !fedMorning) {
    feedFish();
    fedMorning = true;
  }




  if (timeinfo.tm_hour == 18 && timeinfo.tm_min == 0 && !fedEvening) {
    feedFish();
    fedEvening = true;
  }




  if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
    fedMorning = false;
    fedEvening = false;
  }
}




/************ MANUAL FEED ************/
BLYNK_WRITE(V6) {
  if (param.asInt()) feedFish();
}




/************ SETUP ************/
void setup() {
  Serial.begin(115200);




  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);




  feederServo.attach(SERVO_PIN);
  feederServo.write(0);




  Wire.begin(21, 22);
  delay(500);




  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED init failed");
    while (true);
  }




  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("OLED OK");
  display.display();
  delay(1000);




  tempSensor.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);




  timer.setInterval(2000L, readSensors);
  timer.setInterval(60000L, checkFeedingTime);
}




/************ LOOP ************/
void loop() {
  Blynk.run();
  timer.run();
  handleBuzzer();
  updateSlideDisplay();




  if (digitalRead(BUTTON_PIN) == LOW) feederServo.write(75);
  else feederServo.write(0);
}





