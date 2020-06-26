#include "buzzer.h"
long offTime = 0;
void setBuzzerOff(long timeoff){
  pinMode(BUZZER_PIN,OUTPUT);
  Serial.println(millis());
  digitalWrite(BUZZER_PIN,HIGH);
  offTime = timeoff;
}
void checkBuzzer(void){
  if (millis() >= offTime && digitalRead(BUZZER_PIN))
  {
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println(millis());
  }
}
