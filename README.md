# 🐟 Feedify - Smart Fish Feeding System
### IT1140 - Fundamentals of Computing Project (SLIIT)

![Feedify Logo](images/Feedify.jpeg)

**Feedify** is an intelligent ESP32-based IoT solution designed to automate fish feeding and monitor water quality in real time. Using the Blynk IoT platform, the system tracks pH, TDS, and Temperature, ensuring your aquatic pets live in an ideal environment.

---

## 🎯 Overview
This project addresses the challenges of maintaining consistent aquarium health by automating feeding schedules and providing constant water quality surveillance. It prevents overfeeding/underfeeding and alerts users to dangerous chemical shifts in the water.

* **Project Code:** Y1S1Mtr16
* **Academic Year:** Year 1, Semester 1 - 2025
* **Status:** ✅ Completed

---

## ✨ Features

* **Automated Feeding:** Scheduled morning (08:00) and evening (18:00) feeds with adaptive portions.
* **Water Quality Scoring:** Real-time analysis (GOOD, MODERATE, POOR) using a weighted algorithm.
* **Local & Remote Monitoring:** 0.96" I2C OLED local display + Blynk Mobile & Web Dashboards.
* **Safety Alerts:** Physical buzzer patterns and status LEDs + mobile push notifications for critical conditions.
* **Manual Override:** Physical push button and mobile app toggle for immediate manual feeding.

---

## 📸 Project Gallery

### Hardware Implementation
| Front View (Status LEDs) | Complete System Setup |
| :---: | :---: |
| ![Frontside](images/Frontside.jpeg) | ![Final Setup](images/Final%20Setup.png) |

### Monitoring Interface
| Blynk Mobile Dashboard | Blynk Web Interface |
| :---: | :---: |
| ![Mobile](images/BLYNK_Mobile_Dashboard.jpg) | ![Web](images/BLYNK_Web_Dashboard.jpg) |

### System Internals
| Internal Wiring | Feeder Mechanism |
| :---: | :---: |
| ![Testing](images/Sensor%20&%20OLED%20Display%20testing.jpeg) | ![Mechanism](images/Feeder%20mechanism.jpg) |

---

## 🛠️ Hardware Components
* **Microcontroller:** ESP32 DevKit V1
* **Sensors:**
    * pH Sensor (Analog) - Water acidity monitoring
    * TDS Sensor (Total Dissolved Solids) - Purity measurement
    * DS18B20 - Waterproof Temperature sensor
* **Actuators:** MG90S/SG90 Servo Motor (Feeder control)
* **Indicators:** 128x64 I2C OLED, Active Buzzer, Red/Green LEDs
* **Power:** High-capacity 18650 Li-ion battery system (4-cell configuration)

---

## 🔌 System Architecture

### Circuit Diagram
![Wiring Diagram](images/Smart%20Fish%20Feeder%20System%20(Feedy)%20wiring%20diagram.png)

### Power Management
The system utilizes a **Dual-Bank Power Supply** using four 18650 (3.7V) batteries:
1. **Logic Bank (2x Batteries):** Powers the ESP32 and sensitive sensors (pH/TDS) to ensure stable readings and prevent noise interference.
2. **Motor Bank (2x Batteries):** Dedicated power for the Servo motor to handle peak current draws during feeding without crashing the MCU.

---

## 👥 Team Members & Responsibilities (Y1S1Mtr6)

| IT Number | Name | Primary Responsibility | Key Contributions |
| :--- | :--- | :--- | :--- |
| **IT25103932** | Gamage K. T. | Hardware Design | Feeder mechanism design, Servo calibration for dynamic feeding, Buzzer for abnormal conditions, Push Button for manual feeding and physical assembly. |
| **IT25103933** | Wakishta N. N. | pH Monitoring | pH sensor integration, calibration, and local display logic. |
| **IT25103934** | Ramanayake N. R. D. M. | Thermal Systems | Temperature data processing and local display. |
| **IT25103935** | Anjana S. A. D. | Mobile IoT Development | Blynk Mobile app design and status LED logic. |
| **IT25103936** | Nirmala S. G. R. S. | Software Architecture | Implementing the dynamic Water quality logic and the Web dashboard also helped debug others’ code. |
| **IT25103937** | Jayasundara W. G. C. | TDS Monitoring | TDS sensor calibration and purity analysis logic. |

---

## 📂 Documentation
Access the full project paperwork in the [docs](./docs/) folder:
* [Project Proposal](./docs/Proposal.pdf)
* [Progress Report](./docs/Progress_Report.pdf)

---

## ⚙️ Installation & Setup
1. **Libraries:** Install `Blynk`, `ESP32Servo`, `DallasTemperature`, and `Adafruit_SSD1306`.
2. **Configuration:** * Update `BLYNK_TEMPLATE_ID` and `BLYNK_AUTH_TOKEN`.
   * Configure Virtual Pins: V7 (Type), V8 (Size), V9 (Count).
3. **Calibration:** Perform a two-point calibration for the pH sensor using the `calibration_value` constant.
4. **Upload:** Flash the code to your ESP32 via Arduino IDE (115200 baud).

---

## 🔮 Future Enhancements
* **AI Computer Vision:** Use an ESP32-CAM to monitor fish behavior and count fish automatically.
* **Nutritional Database:** Link feeding amounts to an API based on specific species' caloric needs.
* **Dynamic Portion Control:** Intelligent food dispensing logic that adjusts servo angle and opening duration based on user-defined fish type, size, and count via the mobile app.

### ⚙️ Dynamic Feeding Logic (Main Future Update)
The system calculates the volume of food dispensed using the following parameters from the Blynk Cloud:
* **Input Parameters:** Fish Breed, Individual Size (Small/Medium/Large), and Total Population.
* **Actuation Logic:**
    * **Servo Angle:** Calibrated to accommodate different pellet sizes.
    * **Opening Delay:** The delay (time the feeder stays open) is calculated using a base multiplier adjusted by the fish count and size to prevent overfeeding.

---

## 👨‍💻 Project Lead
**KalanaML** - *Lead Developer & System Integrator*

---
🎓 *Developed as part of the BSc in IT curriculum at the Sri Lanka Institute of Information Technology (SLIIT).*
