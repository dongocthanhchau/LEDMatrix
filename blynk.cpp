#include "blynk.h"
#include "Arduino.h"
int delayLED = 500/portTICK_PERIOD_MS;
void blinkLed(int pinout)
{
  digitalWrite(pinout, !digitalRead(pinout));
}
void setDelay(int delayTime)
{
  delayLED = delayTime / portTICK_PERIOD_MS;
}

int getDelay()
{
  return delayLED;
}
