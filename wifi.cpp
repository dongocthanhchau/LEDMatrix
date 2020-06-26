
#include "wifi.h"

#include <DNSServer.h>
#include "Arduino.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <String.h>
#include "ESPAsyncWebServer.h"
#include "html.h"
#include "AsyncJson.h"
#include "json.h"
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include "config.h"
#include "blynk.h"
#include "matrix.h"
#include "buzzer.h"
//==========================

//==========================
String ssid = "Config-";
String pass = "";
String wifist="";
int n=0,_n;

AsyncWebServer Aserver(80);
DNSServer dnsServer;

const char* PARAM_MESSAGE = "message";

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    //wifiScanString();
    response->print(pageWifiA);
    response->print(wifist);
    response->print(pageWifiB);
    request->send(response);
  }
};
void wificfReqHandle(void)
{
  wifiScanString();
  Aserver.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", pageWifiA+wifist+pageWifiB);
    //wifiScanString();
  });
}
void mqttcfReqHandle(void)
{
  Aserver.on("/mqtt", HTTP_GET, [](AsyncWebServerRequest *request){
    pagemqttCf.replace("hostvalue",getData("host"));
    pagemqttCf.replace("portvalue",getData("port"));
    pagemqttCf.replace("uservalue",getData("username"));
    pagemqttCf.replace("passvalue",getData("pass"));
    pagemqttCf.replace("statusvalue",getData("status"));
    pagemqttCf.replace("cmdvalue",getData("command"));
    pagemqttCf.replace("resultvalue",getData("result"));
    pagemqttCf.replace("deviceID",getData("deviceID"));
    request->send(200, "text/html", pagemqttCf);
  });
}
void notFound(AsyncWebServerRequest *request) {
    request->send(200, "text/html", page404);
}
void wifiReqHandle(void)
{
  Aserver.on("/wificf", HTTP_GET, [](AsyncWebServerRequest *request){
      ssid = request->getParam("ssid")->value();
      pass = request->getParam("pass")->value();
      request->send(200, "text/html", pageRestart);
      //request->redirect("http://"+WiFi.localIP()+"/wifi");
      vTaskDelay(2000/portTICK_PERIOD_MS);
      //Serial.println(ssid);
      //Serial.println(pass);
      //WiFi.end();
      changeData("ssid",ssid);
      changeData("password",pass);
      WiFi.begin(ssid.c_str(), pass.c_str());
      //Serial.println("CONNECTING TO "+ssid);
      //vTaskDelay(2000/portTICK_PERIOD_MS);
      //while (WiFi.status() != WL_CONNECTED) {
//        //delay(500);        
      //}
      //delay(5000);
      
      ESP.restart();
    });
}
void mqttReqHandle(void)
{
  Aserver.on("/mqttcf", HTTP_GET, [](AsyncWebServerRequest *request){
      String ho= request->getParam("host")->value();
      String po= request->getParam("port")->value();
      String us= request->getParam("user")->value();
      String pa= request->getParam("pass")->value();
      String st= request->getParam("stat")->value();
      String cm= request->getParam("cmnd")->value();
      String re= request->getParam("rest")->value();
      
      request->send(200, "text/html", pageRestart);
      
      vTaskDelay(2000/portTICK_PERIOD_MS);
      changeData("host",ho);
      changeData("port",po);
      changeData("username",us);
      changeData("pass",pa);
      changeData("status",st);
      changeData("command",cm);
      changeData("result",re);
      delay(100);
      ESP.restart();
    });
}
String json ="{\"status\":\"received\",\"cmd\":\"command\",\"data\":\"receiveddata\"}";
void matrixReqHandle(void)
{
  Aserver.on("/text", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send(200, "text/plain", "{\"status\":\"received\"}");
    if (request->hasParam("h1")){
      String h1 = request->getParam("h1")->value();
      String dat = json;
      dat.replace("command","text");
      dat.replace("receiveddata",h1);
      request->send(200, "text/plain", dat );
      dataSet(h1);
    }else request->send(200, "text/plain", "{\"status\":\"error\"}");
  });
  Aserver.on("/bright", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send(200, "text/plain", "{\"status\":\"received\"}");
    if (request->hasParam("b1")){
      String b1 = request->getParam("b1")->value();
      String dat = json;
      dat.replace("command","bright");
      dat.replace("receiveddata",b1);
      request->send(200, "text/plain", dat );
      brightnessSet(b1.toInt());
    }else request->send(200, "text/plain", "{\"status\":\"error\"}");
  });
}

void buzzerReqHandle(void){
  Aserver.on("/buzzer", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send(200, "text/plain", "{\"status\":\"received\"}");
    if (request->hasParam("t1")){
      String t1 = request->getParam("t1")->value();
      String dat = json;
      dat.replace("command","buzzer");
      dat.replace("receiveddata",t1);
      request->send(200, "text/plain", dat );
      setBuzzerOff(millis()+long(t1.toInt()));
    } else request->send(200, "text/plain", "{\"status\":\"error\"}");
  });
}
//===========================================================================================//
// Begin with this things


//End Fix Code Here
//===============================================================//

void wifiScanString(void)
{
    n = WiFi.scanNetworks();
    //while (WiFi.status() != WL_SCAN_COMPLETED) 
    vTaskDelay(1000/portTICK_PERIOD_MS);
    Serial.print("SCANNED");
    Serial.println(n);
    //delay(1000);
    if (n>0)
    {
      _n = n;
      wifist ="";
      for (int i=0; i < n; ++i)
      {
        wifist+="<option value=\""+String(WiFi.SSID(i))+"\">"+String(WiFi.SSID(i))+"</option>";
        delay(10);
      }
    }
}


void ApMode(void)
{
  static bool ap = false;
  if (ap) return;
  ap = true;

  while (_n<=0) 
  {
    wifiScanString();
    delay(2000);
  }
  static long tim = millis();
  
  //wifiScanString();
  
  
  //while (WiFi.status()==!WL_SCAN_COMPLETED);
  WiFi.begin(" "," ");
  delay(1000);
  WiFi.mode(WIFI_AP);
  //wifiScanString();
  delay(1000);
  String WFSSID = getData("deviceID");
  WFSSID.toUpperCase();
  ssid += WFSSID;
  WiFi.softAP(ssid.c_str(), pass.c_str());
  delay(1000);
  
  dnsServer.start(53, "*", WiFi.softAPIP());
  Aserver.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  
  Aserver.begin();
  //wifiScanString();
//----------------------------------------------  
 
  MDNS.addService("http", "tcp", 80);
 
  Serial.println("AP Mode");
  setDelay(1);
  while (1)
  {
    dnsServer.processNextRequest();
    if (millis()-tim>300000) ESP.restart();
    if ((millis()%5000)<50) wifiScanString();
    //vTaskDelay(20/portTICK_PERIOD_MS);
  }
}
//=========================================


//============================================

bool wifiInit(void)
{
  
  
  Serial.print("CONNECTING TO... ");
  Serial.println(getData("ssid"));
  Serial.println(getData("password"));
  wificfReqHandle();
  wifiReqHandle();
  mqttcfReqHandle();
  mqttReqHandle();
  
//----------------------------------------------  
  Aserver.onNotFound(notFound);
  
//  Serial.print("-");
//  Serial.println(getData("password"));
  String s = getData("ssid");
  String p = getData("password");
  
  const char* ssid = s.c_str();
  const char* pass = p.c_str();
  //wifiScanString();
  //vTaskDelay(5000/portTICK_PERIOD_MS);
  //WiFi.begin("KhongCoPass","123456798");
  WiFi.begin(ssid,pass);
  long t = millis();
  long ti = 100;
  while (WiFi.status() != WL_CONNECTED) {
      //delay(1000);
      
      delay(ti);
      ti*=2;
      //WiFi.disconnect();
      //getData("ssid").c_str(),getData("password").c_str());
      //WiFi.begin(ssid,pass);
      Serial.print(".");
      if ((millis()-t)>120000) 
      {
        WiFi.disconnect();
        delay(100);
        //wifiScanString();
        //vTaskDelay(500/portTICK_PERIOD_MS);
        ApMode();
        return 0;
        break;     
      }
  }
  //wifiScanString();
  Serial.println();
  delay(500);
  Serial.println("CONNECTED to: "+WiFi.SSID());
  delay(500);
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip.toString());
  //wifiScanString();
//----------------------------------------------
//HTTP Req Handle
//  wificfReqHandle();
//  wifiReqHandle();
//  mqttcfReqHandle();
//  mqttReqHandle();
//  
////----------------------------------------------  
//  Aserver.onNotFound(notFound);
  matrixReqHandle();
  buzzerReqHandle();
  Aserver.onNotFound(notFound);
  Aserver.begin();
  changeData("ipaddress",ip.toString());
  dataSet(String(WiFi.localIP()[3]));
  wifiScanString();
  return 1;
}
