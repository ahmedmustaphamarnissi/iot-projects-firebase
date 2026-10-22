#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <ESP32Servo.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// =====================================================
// Wi-Fi
// =====================================================

const char* ssid = "Your wifi name";
const char* password = "YOUR_WIFI_PASSWORD";

// =====================================================
// Firebase credentials
// =====================================================

#define API_KEY "YOUR_API_KEY"

#define DATABASE_URL \
"https://esp32-radar-926d9-default-rtdb.firebaseio.com/"

#define USER_EMAIL "YOUR_FIREBASE_EMAIL"
#define USER_PASSWORD "YOUR_FIREBASE_PASSWORD"

// =====================================================
// Firebase objects
// =====================================================

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// =====================================================
// Servo
// =====================================================

Servo myServo;

// =====================================================
// Pins
// =====================================================

#define SERVO_PIN 5
#define buzzer 4
#define trig 15
#define echo 16
#define gnd 8
#define green 9
#define red 10
// =====================================================
// Radar settings
// =====================================================

// Current servo angle
int currentAngle = 0;

// 1 = increasing
// -1 = decreasing
int direction = 1;

// How many degrees per movement
const int scanStep = 2;

// Delay between movements
const int servoDelay = 8;

// Object detection distance
const float detectionDistance = 10.0;

// =====================================================
// Firebase variables
// =====================================================

bool autoSearch = false;

int firebaseAngle = 0;

// =====================================================
// Timers
// =====================================================

unsigned long lastFirebaseCheck = 0;

unsigned long lastDetectionSend = 0;

// =====================================================
// Get ultrasonic distance
// =====================================================

float getDistance()
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 15000);

  // No echo
  if (duration == 0)
  {
    return -1;
  }

  float distance = duration * 0.0343 / 2.0;

  return distance;
}

// =====================================================
// Send detection to Firebase
// =====================================================

void sendDetection(float distance)
{
  if (!Firebase.ready())
  {
    return;
  }

  // Don't send Firebase data too frequently
  if (millis() - lastDetectionSend < 500)
  {
    return;
  }

  lastDetectionSend = millis();

  // Save distance
  if (Firebase.RTDB.setFloat(
        &fbdo,
        "/distance",
        distance))
  {
    Serial.print("Distance sent: ");
    Serial.print(distance);
    Serial.println(" cm");
  }
  else
  {
    Serial.print("Distance Firebase error: ");
    Serial.println(fbdo.errorReason());
  }

  // Save detection state
  if (Firebase.RTDB.setBool(
        &fbdo,
        "/detect",
        true))
  {
    Serial.println("Detection = TRUE");
  }
  else
  {
    Serial.print("Detection Firebase error: ");
    Serial.println(fbdo.errorReason());
  }
}

// =====================================================
// Check object detection
// =====================================================

void checkDetection(float distance)
{
  // Object detected
  if (distance > 0 && distance < detectionDistance)
  {
    digitalWrite(buzzer, HIGH);
    digitalWrite(red,HIGH);
    digitalWrite(green,LOW);

    Serial.print("OBJECT DETECTED | ");
    Serial.print(distance);
    Serial.println(" cm");

    sendDetection(distance);
  }

  // Nothing detected
  else
  {
    digitalWrite(red,LOW);
    digitalWrite(green,HIGH);
    digitalWrite(buzzer, LOW);
  }
}

// =====================================================
// Read AUTO state
// =====================================================

void readAuto()
{
  if (!Firebase.ready())
  {
    return;
  }

  if (Firebase.RTDB.getBool(
        &fbdo,
        "/auto"))
  {
    autoSearch = fbdo.boolData();
  }
  else
  {
    Serial.print("Firebase /auto error: ");
    Serial.println(fbdo.errorReason());
  }
}

// =====================================================
// Read manual angle
// =====================================================

void readAngle()
{
  if (!Firebase.ready())
  {
    return;
  }

  if (Firebase.RTDB.getInt(
        &fbdo,
        "/angle"))
  {
    firebaseAngle = fbdo.intData();

    // Safety limits
    if (firebaseAngle < 0)
    {
      firebaseAngle = 0;
    }

    if (firebaseAngle > 180)
    {
      firebaseAngle = 180;
    }
  }
  else
  {
    Serial.print("Firebase /angle error: ");
    Serial.println(fbdo.errorReason());
  }
}

// =====================================================
// Read Firebase
// =====================================================

void readFirebase()
{
  readAuto();

  if (!autoSearch)
  {
    readAngle();
  }
}

// =====================================================
// AUTO RADAR SCAN
// =====================================================

void radarScan()
{
  // ---------------------------------------------------
  // Move servo
  // ---------------------------------------------------

  myServo.write(currentAngle);

  // ---------------------------------------------------
  // Read ultrasonic
  // ---------------------------------------------------

  float distance = getDistance();

  // ---------------------------------------------------
  // Print values
  // ---------------------------------------------------

  Serial.print("Angle: ");
  Serial.print(currentAngle);

  Serial.print(" | Distance: ");

  if (distance > 0)
  {
    Serial.print(distance);
    Serial.println(" cm");
  }
  else
  {
    Serial.println("No echo");
  }

  // ---------------------------------------------------
  // Check detection
  // ---------------------------------------------------

  checkDetection(distance);

  // ---------------------------------------------------
  // Change angle
  // ---------------------------------------------------

  currentAngle += direction * scanStep;

  // ---------------------------------------------------
  // Reached 180°
  // ---------------------------------------------------

  if (currentAngle >= 180)
  {
    currentAngle = 180;

    direction = -1;
  }

  // ---------------------------------------------------
  // Reached 0°
  // ---------------------------------------------------

  if (currentAngle <= 0)
  {
    currentAngle = 0;

    direction = 1;
  }

  // ---------------------------------------------------
  // Small delay
  // ---------------------------------------------------

  delay(servoDelay);
}

// =====================================================
// MANUAL MODE
// =====================================================

void manualMode()
{
  // Move to Firebase angle
  myServo.write(firebaseAngle);

  // Keep local angle synchronized
  currentAngle = firebaseAngle;

  // Read distance
  float distance = getDistance();

  // Print
  Serial.print("Manual angle: ");
  Serial.print(firebaseAngle);

  Serial.print(" | Distance: ");

  if (distance > 0)
  {
    Serial.print(distance);
    Serial.println(" cm");
  }
  else
  {
    Serial.println("No echo");
  }

  // Check object
  checkDetection(distance);
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  // ===================================================
  // Pins
  // ===================================================
  pinMode(gnd,OUTPUT);
  digitalWrite(gnd,LOW);

  pinMode(buzzer, OUTPUT);

  pinMode(trig, OUTPUT);

  pinMode(echo, INPUT);
  
  pinMode(green,OUTPUT);
  pinMode(red,OUTPUT);

  digitalWrite(buzzer, LOW);

  // ===================================================
  // Servo
  // ===================================================

  myServo.attach(SERVO_PIN);

  myServo.write(0);

  // ===================================================
  // Wi-Fi
  // ===================================================

  Serial.println();
  Serial.println("=================================");
  Serial.println("ESP32 RADAR");
  Serial.println("=================================");

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

  // ===================================================
  // Firebase configuration
  // ===================================================

  config.api_key = API_KEY;

  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;

  auth.user.password = USER_PASSWORD;

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(
    &config,
    &auth
  );

  Firebase.reconnectWiFi(true);

  Serial.println("Firebase initialized!");

  // ===================================================
  // Wait for Firebase
  // ===================================================

  Serial.println("Waiting for Firebase...");

  unsigned long startTime = millis();

  while (
    !Firebase.ready() &&
    millis() - startTime < 10000
  )
  {
    delay(100);
  }

  // ===================================================
  // Firebase ready
  // ===================================================

  if (Firebase.ready())
  {
    Serial.println("Firebase ready!");

    readFirebase();

    Serial.print("Auto mode: ");

    if (autoSearch)
    {
      Serial.println("ON");
    }
    else
    {
      Serial.println("OFF");
    }

    Serial.print("Firebase angle: ");

    Serial.println(firebaseAngle);

    // Start at Firebase angle
    currentAngle = firebaseAngle;

    myServo.write(currentAngle);
  }
  else
  {
    Serial.println("Firebase NOT ready!");
  }
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
  // ===================================================
  // AUTO MODE
  // ===================================================

  if (autoSearch)
  {
    // Run radar continuously
    radarScan();

    // -------------------------------------------------
    // Check Firebase only every 500 ms
    // -------------------------------------------------

    if (millis() - lastFirebaseCheck >= 500)
    {
      lastFirebaseCheck = millis();

      readAuto();

      // If AUTO was turned OFF
      if (!autoSearch)
      {
        Serial.println();
        Serial.println("AUTO MODE OFF");

        digitalWrite(buzzer, LOW);

        readAngle();

        currentAngle = firebaseAngle;

        myServo.write(firebaseAngle);
      }
    }
  }

  // ===================================================
  // MANUAL MODE
  // ===================================================

  else
  {
    // -------------------------------------------------
    // Update Firebase every 100 ms
    // -------------------------------------------------

    if (millis() - lastFirebaseCheck >= 100)
    {
      lastFirebaseCheck = millis();

      readFirebase();

      // If Firebase switched to AUTO
      if (autoSearch)
      {
        Serial.println();
        Serial.println("AUTO MODE ON");

        // Start scanning from current angle
        currentAngle = firebaseAngle;

        direction = 1;

        return;
      }

      // Manual servo
      manualMode();
    }
  }
}