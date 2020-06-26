#include <WiFi.h>
#include <PubSubClient.h>
#include "json.h"
#include "mqtt.h"
#include "config.h"

//const char* mqtt_server = "cretabase.kbvision.tv";
//#define mqtt_port 1883
//#define MQTT_USER "leiioxwr"
//#define MQTT_PASSWORD "98NW8KaE_FCH"
//#define MQTT_SMS_RESULT "/result"
//#define MQTT_CMD "/cmd"
WiFiClient wifiClient;
PubSubClient client(wifiClient);

void cmndcallback(char* topic, byte *payload, unsigned int length)
{

  String data;
  String tp;
  for (int i=0;i<length;i++) {
    data += String((char)payload[i]);
  }
  Serial.println(data);
  Serial.println(topic);
      String resTopic = getData("result");
      resTopic.replace("%%ID%%",getData("deviceID"));
      String to;
      String text;
      int function;
      String messageId;
      char* dat = &data[0];
      jsonDeserialize(dat, to, text, function, messageId);
      
}

void reconnect() {
  static int re = 0;

  if(WiFi.status() == WL_DISCONNECTED){
    Serial.println("Reconnecting Wifi");
    WiFi.reconnect();
  }
  
  if (WiFi.localIP() != IPAddress(0,0,0,0) ) {
  
  while (!client.connected()) {
    Serial.print("Connecting to MQTT");
//    client.setServer(getData("host").c_str(), getData("port").toInt());
//    Serial.print("\"" + getData("host") + "\"");
//    Serial.println("\"" + getData("port") + "\"");
    delay(1000);
    //client.setCallback(cmndcallback);
    
    String clientId = "ESP32-";
    clientId += String(random(10000));
    clientId += getData("deviceID");
    
    // Attempt to connect
    if (client.connect(clientId.c_str(),getData("username").c_str(),getData("pass").c_str())) {
      
      re = 0;
      String cmdTopic = getData("command");
      cmdTopic.replace("%%ID%%",getData("deviceID"));
      client.subscribe(cmdTopic.c_str());
      Serial.println("connected");
    } else {
      Serial.print(".");
      Serial.print(client.state());
      re++;
      delay(10000);
      if (re==5){
        String host = getData("host");
        const char* mqtt_server =  host.c_str();
        String port = getData("port");
        int mqtt_port =  port.toInt();
        client.setServer(mqtt_server, mqtt_port);
      }
      if (re>7)
	    {
		    Serial.println("CAN'T CONNECT TO MQTT");
		    Serial.println("RESETTING, PLEASE WAIT...");
        
		    ESP.restart();
	    }
      }
    }
  }
}
void publishData(const char *topic,const char *serialData){
  if (WiFi.status() != WL_CONNECTED) return;
  if (!client.connected()) {
    reconnect();
  }
  Serial.print("PUBLISH: \"");
  Serial.print(topic);
  Serial.print("\" : \"");
  Serial.print(serialData);
  Serial.println("\"");
  client.publish(topic, serialData);
}

void MQTTsetup() {
 
  String host = getData("host");
  const char* mqtt_server =  host.c_str();
  String port = getData("port");
  int mqtt_port =  port.toInt();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(cmndcallback);
  delay(50);
  reconnect();
}

void MQTTloop(void)
{
 client.loop();
// if ((micros() % 1000000)<20)
// {
// Serial.print("MQTT State: ");
// Serial.println(client.state());
// Serial.print("Wifi State: ");
// Serial.println(WiFi.status());}
// if (!client.connected()) {
//    reconnect();
//  }	
}
bool mqttconnected(void)
{
  return client.connected();
}
