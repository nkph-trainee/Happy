#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h> // รองรับ HTTPS

#define BUZZER_PIN D1

// ข้อมูล Wi-Fi
const char* ssid = "IT-CENTER_2.4GHz";
const char* password = "74108520";

// เพลงและโน้ต
int melody[] = { 262, 262, 294, 262, 349, 330, 262, 262, 294, 262, 392, 349, 262, 262, 523, 440, 349, 330, 294, 466, 466, 440, 349, 392, 349 };
int noteDurations[] = { 300, 300, 300, 300, 300, 600, 300, 300, 300, 300, 300, 600, 300, 300, 300, 300, 300, 300, 600, 200, 200, 200, 300, 300, 600 };

// ตัวแปรจับเวลา
unsigned long previousNoteMillis = 0; // สำหรับการเล่นเพลง
unsigned long previousApiMillis = 0;  // สำหรับการดึงข้อมูล API
unsigned long noteInterval = 0;       // ระยะเวลาในแต่ละโน้ต
unsigned long apiInterval = 15000;    // ระยะเวลาในการดึง API ทุก 15 วินาที

int currentNote = 0; // โน้ตปัจจุบัน
bool songPlaying = false; // สถานะการเล่นเพลง

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(115200);

  // เชื่อมต่อ Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  unsigned long currentMillis = millis();

  // เล่นเพลงทีละโน้ตโดยไม่ใช้ delay
  if (songPlaying && currentMillis - previousNoteMillis >= noteInterval) {
    if (currentNote < sizeof(melody) / sizeof(melody[0])) {
      noteInterval = noteDurations[currentNote] * 1.1; // เพิ่มระยะเวลาหลังเล่นโน้ต
      tone(BUZZER_PIN, melody[currentNote], noteDurations[currentNote]); // เล่นโน้ต
      previousNoteMillis = currentMillis;
      currentNote++;
    } else {
      songPlaying = false; // จบเพลง
      currentNote = 0;
      Serial.println("Song completed.");
    }
  }

  // ดึงข้อมูลจาก API ทุกๆ 15 วินาที
  if (currentMillis - previousApiMillis >= apiInterval) {
    previousApiMillis = currentMillis;
    fetchApiData();
  }
}

void fetchApiData() {
  WiFiClientSecure client;
  client.setInsecure(); // ไม่ตรวจสอบใบรับรอง SSL (ใช้สำหรับทดสอบ)

  HTTPClient https;
  Serial.println("Connecting to API...");

  if (https.begin(client, "https://npmh.moph.go.th/api/itwork/api/rxipd-order")) {
    int httpCode = https.GET();

    if (httpCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String payload = https.getString();
        Serial.println("API Response: " + payload);

        int apiValue = payload.toInt();
        if (apiValue == 1) {
          Serial.println("API Value = 1. Starting alert sound.");
          if (!songPlaying) {
            songPlaying = true; // เริ่มเล่นเพลง
            previousNoteMillis = millis(); // ตั้งค่าตัวจับเวลาเริ่มเล่นเพลง
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
