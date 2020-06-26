#ifndef _matrix__h_
#define _matrix__h_

#include <Arduino.h>

#include <SPI.h>
#include <bitBangedSPI.h>
#include <MAX7219_Dot_Matrix.h>


void matrixInit(void);
void blinkLoop(void);
void blinkSet(int tim);
void brightnessSet(int light);
void dataSet(String data);

#endif
