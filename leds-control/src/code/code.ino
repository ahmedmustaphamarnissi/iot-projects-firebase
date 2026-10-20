#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// =====================================================
// Wi-Fi
// =====================================================

const char* ssid = "";
const char* password = "";

// =====================================================
// Firebase credentials
// =====================================================

#define API_KEY ""          // 🔹 Found in Project Settings > General
#define DATABASE_URL ""  // 🔹 From Realtime Database URL
#define USER_EMAIL ""  // 🔹 Firebase Authentication email
#define USER_PASSWORD ""  

// =====================================================
// Firebase objects
// =====================================================

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// =====================================================
// LED pins
// =====================================================

#define LED_GREEN  4
#define LED_YELLOW 5
#define LED_RED    6

// =====================================================
// Switch pins
// =====================================================

#define SW_GREEN   16
#define SW_YELLOW  17
#define SW_RED     18

// =====================================================
// Variables
// =====================================================

// Previous physical switch states
bool lastGreenSwitch = HIGH;
bool lastYellowSwitch = HIGH;
bool lastRedSwitch = HIGH;

// Current LED states
bool greenLedState = false;
bool yellowLedState = false;
bool redLedState = false;

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  // ---------------------------------------------------
  // LED configuration
  // ---------------------------------------------------

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // Start with LEDs OFF
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  // ---------------------------------------------------
  // Switch configuration
  // ---------------------------------------------------

  pinMode(SW_GREEN, INPUT_PULLUP);
  pinMode(SW_YELLOW, INPUT_PULLUP);
  pinMode(SW_RED, INPUT_PULLUP);

  // ---------------------------------------------------
  // Connect to Wi-Fi
  // ---------------------------------------------------

  Serial.println();
  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // ---------------------------------------------------
  // Firebase configuration
  // ---------------------------------------------------

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  Serial.println("Firebase initialized!");

  // ---------------------------------------------------
  // Read initial values from Firebase
  // ---------------------------------------------------

  if (Firebase.ready()) {

    if (Firebase.RTDB.getBool(&fbdo, "/led1")) {

      greenLedState = fbdo.boolData();

      digitalWrite(
        LED_GREEN,
        greenLedState ? HIGH : LOW
      );

      Serial.print("LED1 initial state: ");
      Serial.println(greenLedState ? "ON" : "OFF");
    }

    if (Firebase.RTDB.getBool(&fbdo, "/led2")) {

      yellowLedState = fbdo.boolData();

      digitalWrite(
        LED_YELLOW,
        yellowLedState ? HIGH : LOW
      );

      Serial.print("LED2 initial state: ");
      Serial.println(yellowLedState ? "ON" : "OFF");
    }

    if (Firebase.RTDB.getBool(&fbdo, "/led3")) {

      redLedState = fbdo.boolData();

      digitalWrite(
        LED_RED,
        redLedState ? HIGH : LOW
      );

      Serial.print("LED3 initial state: ");
      Serial.println(redLedState ? "ON" : "OFF");
    }
  }

  // ---------------------------------------------------
  // Save initial switch states
  // ---------------------------------------------------

  lastGreenSwitch = digitalRead(SW_GREEN);
  lastYellowSwitch = digitalRead(SW_YELLOW);
  lastRedSwitch = digitalRead(SW_RED);
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  // ===================================================
  // Read switches
  // ===================================================

  bool greenSwitch = digitalRead(SW_GREEN);
  bool yellowSwitch = digitalRead(SW_YELLOW);
  bool redSwitch = digitalRead(SW_RED);

  // ===================================================
  // GREEN SWITCH
  // ===================================================

  if (greenSwitch != lastGreenSwitch) {

    delay(20);

    greenSwitch = digitalRead(SW_GREEN);

    if (greenSwitch != lastGreenSwitch) {

      // Switch pressed = LOW
      greenLedState = (greenSwitch == LOW);

      // Update physical LED
      digitalWrite(
        LED_GREEN,
        greenLedState ? HIGH : LOW
      );

      // Update Firebase
      if (Firebase.ready()) {

        if (Firebase.RTDB.setBool(
              &fbdo,
              "/led1",
              greenLedState
            )) {

          Serial.print("LED1 = ");
          Serial.println(
            greenLedState ? "ON" : "OFF"
          );

        } else {

          Serial.print("Firebase error LED1: ");
          Serial.println(fbdo.errorReason());
        }
      }

      lastGreenSwitch = greenSwitch;
    }
  }

  // ===================================================
  // YELLOW SWITCH
  // ===================================================

  if (yellowSwitch != lastYellowSwitch) {

    delay(20);

    yellowSwitch = digitalRead(SW_YELLOW);

    if (yellowSwitch != lastYellowSwitch) {

      yellowLedState = (yellowSwitch == LOW);

      // Update physical LED
      digitalWrite(
        LED_YELLOW,
        yellowLedState ? HIGH : LOW
      );

      // Update Firebase
      if (Firebase.ready()) {

        if (Firebase.RTDB.setBool(
              &fbdo,
              "/led2",
              yellowLedState
            )) {

          Serial.print("LED2 = ");
          Serial.println(
            yellowLedState ? "ON" : "OFF"
          );

        } else {

          Serial.print("Firebase error LED2: ");
          Serial.println(fbdo.errorReason());
        }
      }

      lastYellowSwitch = yellowSwitch;
    }
  }

  // ===================================================
  // RED SWITCH
  // ===================================================

  if (redSwitch != lastRedSwitch) {

    delay(20);

    redSwitch = digitalRead(SW_RED);

    if (redSwitch != lastRedSwitch) {

      redLedState = (redSwitch == LOW);

      // Update physical LED
      digitalWrite(
        LED_RED,
        redLedState ? HIGH : LOW
      );

      // Update Firebase
      if (Firebase.ready()) {

        if (Firebase.RTDB.setBool(
              &fbdo,
              "/led3",
              redLedState
            )) {

          Serial.print("LED3 = ");
          Serial.println(
            redLedState ? "ON" : "OFF"
          );

        } else {

          Serial.print("Firebase error LED3: ");
          Serial.println(fbdo.errorReason());
        }
      }

      lastRedSwitch = redSwitch;
    }
  }

  // ===================================================
  // Firebase → ESP32
  // ===================================================

  if (Firebase.ready()) {

    // -------------------------------------------------
    // LED 1
    // -------------------------------------------------

    if (Firebase.RTDB.getBool(&fbdo, "/led1")) {

      bool firebaseLed1 = fbdo.boolData();

      if (firebaseLed1 != greenLedState) {

        greenLedState = firebaseLed1;

        digitalWrite(
          LED_GREEN,
          greenLedState ? HIGH : LOW
        );

        Serial.print("Firebase changed LED1: ");
        Serial.println(
          greenLedState ? "ON" : "OFF"
        );
      }
    }

    // -------------------------------------------------
    // LED 2
    // -------------------------------------------------

    if (Firebase.RTDB.getBool(&fbdo, "/led2")) {

      bool firebaseLed2 = fbdo.boolData();

      if (firebaseLed2 != yellowLedState) {

        yellowLedState = firebaseLed2;

        digitalWrite(
          LED_YELLOW,
          yellowLedState ? HIGH : LOW
        );

        Serial.print("Firebase changed LED2: ");
        Serial.println(
          yellowLedState ? "ON" : "OFF"
        );
      }
    }

    // -------------------------------------------------
    // LED 3
    // -------------------------------------------------

    if (Firebase.RTDB.getBool(&fbdo, "/led3")) {

      bool firebaseLed3 = fbdo.boolData();

      if (firebaseLed3 != redLedState) {

        redLedState = firebaseLed3;

        digitalWrite(
          LED_RED,
          redLedState ? HIGH : LOW
        );

        Serial.print("Firebase changed LED3: ");
        Serial.println(
          redLedState ? "ON" : "OFF"
        );
      }
    }
  }

  delay(100);
}