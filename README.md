# IoT Projects

A collection of small, self-contained IoT projects built with ESP32, Firebase, and various sensors and actuators. Each project explores a specific aspect of connected hardware — from basic GPIO control to cloud-backed remote monitoring.

## ⚡ TL;DR

This repository demonstrates practical IoT development through focused, working projects. Rather than one large application, each folder is an independent experiment that covers a specific hardware/software integration — useful as a reference or starting point for similar builds.

---

## 📦 Projects

| Project | Description | Technologies |
|---------|-------------|-------------|
| [leds-control](leds-control/) | Control three LEDs via physical switches and a web dashboard, with bidirectional Firebase sync | ESP32 · Firebase RTDB · Firebase Auth · HTML/JS |
| [multi-task](multi-task/) | Remote LED and buzzer control combined with live DHT11 temperature and humidity monitoring | ESP32 · DHT11 · Firebase RTDB · Firebase Auth · HTML/JS |

---

## 🗂️ Repository Structure

```
iot-projects/
├── leds-control/
│   ├── images/
│   │   ├── connections.png
│   │   ├── DashboardPage.png
│   │   ├── firebase.png
│   │   └── loginPage.png
│   ├── src/
│   │   ├── code/
│   │   │   └── code.ino
│   │   └── index.html
│   └── README.md
├── multi-task/
│   ├── images/
│   │   ├── dashboard.png
│   │   ├── firebase.png
│   │   ├── loginPage.png
│   │   └── wiring&Composants.png
│   ├── src/
│   │   ├── code/
│   │   │   └── code.ino
│   │   └── index.html
│   └── README.md
└── README.md
```

---

## 🛠️ Technologies

| Category | Tools |
|----------|-------|
| Microcontroller | ESP32 |
| Firmware language | C/C++ (Arduino framework) |
| IDE | Arduino IDE |
| Sensor | DHT11 (temperature & humidity) |
| Cloud database | Firebase Realtime Database |
| Authentication | Firebase Authentication |
| Firebase library (ESP32) | Firebase ESP Client |
| Sensor library | DHT sensor library (Adafruit) |
| Web dashboard | Vanilla HTML / CSS / JavaScript |
| Firebase SDK (web) | Firebase JS SDK v9.6.10 |

---

## 🧠 What I Am Practicing

- **GPIO control** — driving LED outputs and reading digital inputs (push buttons with pull-up)
- **Sensor integration** — reading temperature and humidity from a DHT11 sensor
- **Cloud communication** — connecting an ESP32 to Firebase over Wi-Fi
- **Firebase Realtime Database** — reading and writing structured data from both firmware and a web client
- **Remote device control** — toggling physical outputs (LEDs, buzzer) from a browser in real time
- **Bidirectional sync** — keeping hardware state and database state consistent from two directions
- **Firebase Authentication** — securing access to hardware controls with email/password login
- **Web dashboard development** — building browser UIs that interact with IoT hardware through Firebase

---

## 🚀 Getting Started

Clone the repository:

```bash
git clone https://github.com/ahmedmustaphamarnissi/iot-projects.git
cd iot-projects
```

Each project is independent. Navigate into the project folder you want to run and follow the setup instructions in its own README:

- [leds-control/README.md](leds-control/README.md)
- [multi-task/README.md](multi-task/README.md)

Each README covers:
- Required Arduino libraries
- Wi-Fi and Firebase configuration
- Wiring table with exact GPIO numbers
- How to open and use the web dashboard

---

## 📚 What I Learned

Working through these projects gave me hands-on experience with the full IoT stack: from wiring hardware and writing firmware in C/C++, to integrating a cloud database and building a browser interface that reacts to real hardware events. The main takeaways are understanding how to keep physical and cloud state in sync, how to structure Firebase data for simple device control, and how to secure access to connected hardware with authentication.
