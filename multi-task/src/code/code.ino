#include <Wire.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>

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
// Pins
// =====================================================

#define LED_GREEN   4
#define LED_YELLOW  5
#define LED_RED     6
#define BUZZER      7

#define DHT_PIN     16
#define DHT_TYPE    DHT11

// =====================================================
// DHT
// =====================================================

DHT dht(DHT_PIN, DHT_TYPE);

// =====================================================
// Variables
// =====================================================

bool greenLedState = false;
bool yellowLedState = false;
bool redLedState = false;
bool buzzerState = false;

float lastTemperature = 0;
float lastHumidity = 0;

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  // ---------------------------------------------------
  // DHT
  // ---------------------------------------------------

  dht.begin();

  // ---------------------------------------------------
  // Pin configuration
  // ---------------------------------------------------

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Start with everything OFF

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(BUZZER, LOW);

  // ---------------------------------------------------
  // Connect to Wi-Fi
  // ---------------------------------------------------

  Serial.println();
  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
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

  if (Firebase.ready())
  {
    // -------------------------------------------------
    // LED 1
    // -------------------------------------------------

    if (Firebase.RTDB.getBool(&fbdo, "/led1"))
    {
      greenLedState = fbdo.boolData();

      digitalWrite(
        LED_GREEN,
        greenLedState ? HIGH : LOW
      );

      Serial.print("LED1 initial state: ");
      Serial.println(greenLedState ? "ON" : "OFF");
    }

    // -------------------------------------------------
    // LED 2
    // -------------------------------------------------

    if (Firebase.RTDB.getBool(&fbdo, "/led2"))
    {
      yellowLedState = fbdo.boolData();

      digitalWrite(
        LED_YELLOW,
        yellowLedState ? HIGH : LOW
      );

      Serial.print("LED2 initial state: ");
      Serial.println(yellowLedState ? "ON" : "OFF");
    }

    // -------------------------------------------------
    // LED 3
    // -------------------------------------------------

    if (Firebase.RTDB.getBool(&fbdo, "/led3"))
    {
      redLedState = fbdo.boolData();

      digitalWrite(
        LED_RED,
        redLedState ? HIGH : LOW
      );

      Serial.print("LED3 initial state: ");
      Serial.println(redLedState ? "ON" : "OFF");
    }

    // -------------------------------------------------
    // Buzzer
    // -------------------------------------------------

    if (Firebase.RTDB.getBool(&fbdo, "/buzzer"))
    {
      buzzerState = fbdo.boolData();

      digitalWrite(
        BUZZER,
        buzzerState ? HIGH : LOW
      );

      Serial.print("Buzzer initial state: ");
      Serial.println(buzzerState ? "ON" : "OFF");
    }
  }
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
  // ===================================================
  // Firebase → ESP32
  // ===================================================

  if (Firebase.ready())
  {
    // -------------------------------------------------
    // LED 1
    // -------------------------------------------------

    if (Firebase.RTDB.getBool(&fbdo, "/led1"))
    {
      bool firebaseLed1 = fbdo.boolData();

      if (firebaseLed1 != greenLedState)
      {
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

    if (Firebase.RTDB.getBool(&fbdo, "/led2"))
    {
      bool firebaseLed2 = fbdo.boolData();

      if (firebaseLed2 != yellowLedState)
      {
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

    if (Firebase.RTDB.getBool(&fbdo, "/led3"))
    {
      bool firebaseLed3 = fbdo.boolData();

      if (firebaseLed3 != redLedState)
      {
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

    // -------------------------------------------------
    // Buzzer
    // -------------------------------------------------

    if (Firebase.RTDB.getBool(&fbdo, "/buzzer"))
    {
      bool firebaseBuzzerState = fbdo.boolData();

      if (firebaseBuzzerState != buzzerState)
      {
        buzzerState = firebaseBuzzerState;

        digitalWrite(
          BUZZER,
          buzzerState ? HIGH : LOW
        );

        Serial.print("Firebase changed Buzzer: ");
        Serial.println(
          buzzerState ? "ON" : "OFF"
        );
      }
    }
  }

  // ===================================================
  // DHT11
  // ===================================================

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("Failed to read DHT11!");
    delay(2000);
    return;
  }

  // ---------------------------------------------------
  // Temperature → Firebase
  // ---------------------------------------------------

  if (temperature != lastTemperature)
  {
    if (Firebase.ready())
    {
      if (Firebase.RTDB.setFloat(
            &fbdo,
            "/temperature",
            temperature
          ))
      {
        Serial.print("Temperature = ");
        Serial.println(temperature);

        lastTemperature = temperature;
      }
      else
      {
        Serial.print("Firebase error temperature: ");
        Serial.println(fbdo.errorReason());
      }
    }
  }

  // ---------------------------------------------------
  // Humidity → Firebase
  // ---------------------------------------------------

  if (humidity != lastHumidity)
  {
    if (Firebase.ready())
    {
      if (Firebase.RTDB.setFloat(
            &fbdo,
            "/humidity",
            humidity
          ))
      {
        Serial.print("Humidity = ");
        Serial.println(humidity);

        lastHumidity = humidity;
      }
      else
      {
        Serial.print("Firebase error humidity: ");
        Serial.println(fbdo.errorReason());
      }
    }
  }

  // DHT11 should not be read too frequently
  delay(2000);
}