//==================================
#include "wifi.h"
#include "matrix.h"
#include "config.h"
#include "buzzer.h"
#include <WiFi.h>
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  spiffBegin();
  matrixInit();
  delay(200);
  Serial.println(wifiInit()?"WIFI DONE":"WIFI FAIL");
}

void loop() {
  // put your main code here, to run repeatedly:
  checkBuzzer();
}
