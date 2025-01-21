#ifndef LED_ALERT_H
#define LED_ALERT_H

#include <Arduino.h>

unsigned long ledAlertPreviousMillis = 0;
unsigned long ledBlinkInterval = 500;
bool ledAlertState = false;
bool shouldBlinkUntilZero = false;
int ledAlertPin = -1;

void initLedAlert(int pin) {
  ledAlertPin = pin;
  pinMode(ledAlertPin, OUTPUT);
  digitalWrite(ledAlertPin, LOW);
}

void setLedAlertState(bool state) {
  shouldBlinkUntilZero = state;
  if (!state) {
    digitalWrite(ledAlertPin, LOW);
  }
}

void updateLedAlert(unsigned long currentMillis) {
  if (shouldBlinkUntilZero) {
    if (currentMillis - ledAlertPreviousMillis >= ledBlinkInterval) {
      ledAlertPreviousMillis = currentMillis;
      ledAlertState = !ledAlertState;
      digitalWrite(ledAlertPin, ledAlertState);
    }
  }
}

#endif
