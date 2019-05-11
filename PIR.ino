#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "FS.h"

#define MIN_VALUE 148
#define MAX_VALUE 1024
#define SENSOR_PIN A0
#define DEBUG 1
#define Status 12
#define SENSOR 13
#define CAPTOR_ID 1
#define WSID "WIFILIX"
#define PSWD "yooothug"
#define HOST "****"
#define PORT 3007
#define authUsername "***"
#define authPassword "***"
#define BASE_URL "/motion/"

ESP8266WiFiMulti WiFiMulti;

/* * * * * * Send request  * * * * * */ 
/* Return url String : The url where request are sent 
 */
String sendRequest(float level, int sensorValue) {
  WiFiClient client;
  HTTPClient http; 
  int httpCode;
  String payload;
  String url = getUrl(BASE_URL, level);

  if ( DEBUG ) {
    Serial.println("---------------");
    Serial.println(url);
    Serial.println("LUX = ");
    Serial.println(level);
    Serial.println("Captor Level = ");        
    Serial.println(sensorValue);        
  }
    
  if (http.begin(client, HOST, PORT, url)) {  // HTTP
    http.setAuthorization(authUsername, authPassword);

    httpCode = http.GET();
    if (httpCode > 0) {  
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        payload = http.getString();  
    Serial.println(payload);        
      }
    } else if ( DEBUG) Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str()); 

    http.end();
  } else if ( DEBUG) Serial.printf("[HTTP} Unable to connect\n");
}


/* * * * * * Get URL * * * * * */ 
/* Return url String : The url where request are sent 
 */
String getUrl(char* url_base , float lux) {
  String url = url_base; 
  url += "/"; 
  url += CAPTOR_ID; 
  url += "/";
  url += String(lux, 0);
  return url;
}


/* * * * * * SETUP * * * * * */ 
void setup() {
  Serial.begin(9600);
  pinMode(SENSOR, INPUT); // declare sensor as input  
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WSID, PSWD);
}


/* * * * * * LOOP * * * * * */ 
void loop(){
  float level;   
  int sensorValue;
  long state = digitalRead(SENSOR);

  delay(1000);
  
  if(state == HIGH){
    digitalWrite (Status, HIGH);
    sensorValue = analogRead(SENSOR_PIN);
    level = ((sensorValue - MIN_VALUE) * 100 / (MAX_VALUE - MIN_VALUE));

    if ((WiFiMulti.run() == WL_CONNECTED)) {

      sendRequest(level, sensorValue);
    }
  }
  else digitalWrite (Status, LOW);  
}
