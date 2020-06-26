
#include "matrix.h"
long t =0;
int blinkSpd;
String str;
MAX7219_Dot_Matrix l1 (4, 22);  

void matrixInit(void)
{
  l1.begin();
  for (int i=1;i<5;i++){
    blinkSet(-1);
    brightnessSet(0);
    dataSet("");
  }
}

//void blinkLoop(){
//    if ( (blinkSpd)  && ( (millis() - t) >= blinkSpd) ){
//      t = millis();
//        if (str!="") {
//          l1.sendString("");
//          str="";
//        } else{
//          //str = EreadString(200);
//          l1.sendString( str.c_str() );
//        }
//      }
//}
void blinkSet(int tim){
    if (tim ==-1){
      //blinkSpd[0] = EreadString(225).toInt();
    } else {
      blinkSpd = tim;
      //EwriteString(225,String(tim));
    }
}
void brightnessSet(int light){
    if (light ==-1){
      //l1.setIntensity(EreadString(223).toInt()*7.5);
    } else {
      l1.setIntensity(light*7.5);
      //EwriteString(223,String(light));
    }
  
}
void dataSet(String data){
    if (data ==""){
      l1.sendString(str.c_str());
    } else {
      str = data;
      l1.sendString(data.c_str());
    }
  
}
