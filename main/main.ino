#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "led_alert.h"
#include "sound.h"

#define LED_WIFI_PIN D8

const char* ssid = "IT-CENTER_2.4GHz";
const char* password = "74108520";

unsigned long apiPreviousMillis = 0;
unsigned long apiInterval = 15000;

void setup() {
  pinMode(LED_WIFI_PIN, OUTPUT);
  digitalWrite(LED_WIFI_PIN, LOW);

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    blinkWifiLed();
  }
  digitalWrite(LED_WIFI_PIN, LOW); // Turn off WiFi LED when connected
  Serial.println("Connected to WiFi");

  initLedAlert(D4); // กำหนดพินของไฟแจ้งเตือนในโมดูล led_alert
}

void loop() {
  unsigned long currentMillis = millis();

  // เรียก API และตรวจสอบสถานะ
  if (currentMillis - apiPreviousMillis >= apiInterval) {
    apiPreviousMillis = currentMillis;
    handleApiCheck();
  }

  // จัดการไฟกระพริบ
  updateLedAlert(currentMillis);

  // เล่นเพลงถ้ามีการแจ้งเตือน
  if (isPlayingMelody) {
    playMelody(currentMillis);
  }
}

void handleApiCheck() {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  if (https.begin(client, "https://npmh.moph.go.th/api/itwork/api/rxipd-order")) {
    int httpCode = https.GET();

    if (httpCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = https.getString();
        Serial.print("API Response: ");
        Serial.println(payload);

        int apiValue = payload.toInt();
        if (apiValue >= 1) {
          Serial.println("API Value >= 1. Alert will play.");
          setLedAlertState(true); // เปิดไฟกระพริบ
          isPlayingMelody = true; // เริ่มเล่นเพลง
        } else {
          Serial.println("API Value = 0. No alert.");
          setLedAlertState(false); // ปิดไฟกระพริบ
          stopMelody(); // หยุดเล่นเพลง
        }
      }
    } else {
      Serial.printf("HTTP GET failed, error: %s\n", https.errorToString(httpCode).c_str());
    }
    https.end();
  } else {
    Serial.println("Unable to connect to HTTPS server.");
    blinkWifiLed();
  }
}

void blinkWifiLed() {
  static unsigned long previousMillis = 0;
  static bool ledState = false;

  if (millis() - previousMillis >= 500) {
    previousMillis = millis();
    ledState = !ledState;
    digitalWrite(LED_WIFI_PIN, ledState);
  }
} 