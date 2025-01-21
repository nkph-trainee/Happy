#ifndef SOUND_H
#define SOUND_H

#include <Arduino.h>

#define BUZZER_PIN D1

// เพลง
const int melody[] = { 262, 262, 294, 262, 349, 330, 262, 262, 294, 262, 392, 349, 262, 262, 523, 440, 349, 330, 294, 466, 466, 440, 349, 392, 349 };
const int noteDurations[] = { 300, 300, 300, 300, 300, 600, 300, 300, 300, 300, 300, 600, 300, 300, 300, 300, 300, 300, 600, 200, 200, 200, 300, 300, 600 };

unsigned long melodyPreviousMillis = 0;
unsigned long noteDuration = 0;
int currentNote = 0;
bool isPlayingMelody = false;

void playMelody(unsigned long currentMillis) {
  if (currentMillis - melodyPreviousMillis >= noteDuration) {
    melodyPreviousMillis = currentMillis;

    if (currentNote < sizeof(melody) / sizeof(melody[0])) {
      noteDuration = noteDurations[currentNote];
      tone(BUZZER_PIN, melody[currentNote], noteDuration);
      currentNote++;
    } else {
      noTone(BUZZER_PIN);
      currentNote = 0;
      isPlayingMelody = false;
      Serial.println("Melody completed.");
    }
  }
}

void stopMelody() {
  noTone(BUZZER_PIN);
  isPlayingMelody = false;
  currentNote = 0;
}

#endif
