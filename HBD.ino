#define BUZZER_PIN D5 // เปลี่ยนพินจาก D2 เป็น D5

int melody[] = {
  262, 262, 294, 262, 349, 330,  // Happy birthday to you
  262, 262, 294, 262, 392, 349,  // Happy birthday to you
  262, 262, 523, 440, 349, 330, 294, // Happy birthday dear [Name]
  466, 466, 440, 349, 392, 349   // Happy birthday to you
};

int noteDurations[] = {
  300, 300, 300, 300, 300, 600,  // ระยะเวลาแต่ละโน้ต
  300, 300, 300, 300, 300, 600,
  300, 300, 300, 300, 300, 300, 600,
  200, 200, 200, 300, 300, 600
};

unsigned long previousMillis = 0; // ตัวแปรสำหรับจับเวลา
unsigned long interval = 5000; // เวลาหน่วง 5 วินาที ก่อนเล่นเพลง

int currentNote = 0; // ตัวแปรเก็บหมายเลขโน้ตปัจจุบัน
bool songStarted = false; // ตัวแปรเช็คว่าเพลงเริ่มเล่นแล้วหรือยัง

void setup() {
  pinMode(BUZZER_PIN, OUTPUT); // กำหนดพินที่เชื่อมต่อกับ Buzzer เป็น OUTPUT
  Serial.begin(115200); // เริ่มต้น Serial Monitor
}

void loop() {
  unsigned long currentMillis = millis(); // เก็บเวลาปัจจุบัน

  // รอ 5 วินาทีก่อนเริ่มเล่นเพลง
  if (!songStarted && currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // รีเซ็ตเวลา
    songStarted = true; // ตั้งค่าให้เพลงเริ่มเล่น
    Serial.println("Start playing music"); // แสดงข้อความใน Serial Monitor ว่าเริ่มเล่นเพลง
  }

  // เมื่อถึงเวลาที่กำหนดแล้วเริ่มเล่นเพลง
  if (songStarted) {
    if (currentNote < sizeof(melody) / sizeof(melody[0])) {
      int noteDuration = noteDurations[currentNote];
      tone(BUZZER_PIN, melody[currentNote], noteDuration); // เล่นโน้ต
      delay(noteDuration * 1.1); // เพิ่มเวลาหลังจากเล่นโน้ตแต่ละตัว
      noTone(BUZZER_PIN); // หยุดเสียงหลังจากโน้ตเสร็จ

      currentNote++; // ขยับไปยังโน้ตถัดไป
    } else {
      currentNote = 0; // รีเซ็ตกลับไปที่โน้ตแรก
      delay(2000); // ดีเลย์ 2 วินาทีก่อนเริ่มเล่นเพลงใหม่
      Serial.println("Song completed, waiting 2 seconds..."); // แสดงข้อความใน Serial Monitor
    }
  }
}
