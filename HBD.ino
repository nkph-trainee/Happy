#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h> // รองรับ HTTPS

#define BUZZER_PIN D5

int melody[] = { 262, 262, 294, 262, 349, 330, 262, 262, 294, 262, 392, 349, 262, 262, 523, 440, 349, 330, 294, 466, 466, 440, 349, 392, 349 };
int noteDurations[] = { 300, 300, 300, 300, 300, 600, 300, 300, 300, 300, 300, 600, 300, 300, 300, 300, 300, 300, 600, 200, 200, 200, 300, 300, 600 };

unsigned long previousMillis = 0;
unsigned long interval = 5000;
unsigned long apiInterval = 15000;
unsigned long previousApiMillis = 0;

int currentNote = 0;
bool songStarted = false;
bool alertPlaying = false;

const char* ssid = "IT-CENTER_2.4GHz";
const char* password = "74108520";

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  unsigned long currentMillis = millis();

  if (!songStarted && currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    songStarted = true;
    Serial.println("Start playing music");
  }

  if (songStarted && !alertPlaying) {
    if (currentNote < sizeof(melody) / sizeof(melody[0])) {
      int noteDuration = noteDurations[currentNote];
      tone(BUZZER_PIN, melody[currentNote], noteDuration);
      delay(noteDuration * 1.1);
      noTone(BUZZER_PIN);
      currentNote++;
    } else {
      currentNote = 0;
      delay(2000);
      Serial.println("Song completed, waiting 2 seconds...");
    }
  }

  if (currentMillis - previousApiMillis >= apiInterval) {
    previousApiMillis = currentMillis;

    WiFiClientSecure client; // รองรับ HTTPS
    client.setInsecure(); // ไม่ตรวจสอบใบรับรอง SSL (สำหรับการทดสอบเท่านั้น)

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
          if (apiValue == 1) {
            Serial.println("API Value = 1. Alert will play.");
            if (!alertPlaying) {
              alertPlaying = true;
              playAlertSound();
            }
          } else if (apiValue == 0) {
            Serial.println("API Value = 0. No alert.");
          } else {
            Serial.println("API Value is not 1 or 0.");
          }
        }
      } else {
        Serial.printf("HTTP GET failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    } else {
      Serial.println("Unable to connect to HTTPS server.");
    }
  }
}

void playAlertSound() {
  Serial.println("Playing alert sound...");
  for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
    int noteDuration = noteDurations[i];
    tone(BUZZER_PIN, melody[i], noteDuration);
    delay(noteDuration * 1.1);
    noTone(BUZZER_PIN);
  }
  alertPlaying = false;
  Serial.println("Alert sound completed.");
}
