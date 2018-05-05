/*
 It sends temperature values taken from a DS18B20 sensor, via websockets.
 It is aimed to be used with an ESP8266 based board, such as the NodeMCU.
 Note that only ONE client at once is supported!!.
 Copyright Tuxedoar 2018. 
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <Hash.h>
// DS18B20 sensor needed libraries.
#include <OneWire.h>
#include <DallasTemperature.h>

// Use PIN 2 for the NodeMCU board. 
#define DS_SENSOR_PIN 2

OneWire oneWire(DS_SENSOR_PIN);
DallasTemperature ds_sensor(&oneWire);

ESP8266WiFiMulti WiFiMulti;
WebSocketsServer webSocket = WebSocketsServer(81);

float temp;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
while(type == WStype_CONNECTED){
  IPAddress ip = webSocket.remoteIP(num);
  Serial.printf("[%u] Connected from %d url: %s\n", num, ip[0], payload);
  ds_sensor.requestTemperatures();
  temp = ds_sensor.getTempCByIndex(0);
  String strtemp = String(temp, 2); // Convert float value to string with two decimal places
  webSocket.broadcastTXT(strtemp); // send data to all connected clients
  delay(2000);
  }
 }

void setup() {
    // USE_SERIAL.begin(921600);
    Serial.begin(9600);


    Serial.println();
    Serial.println();
    Serial.println();

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFiMulti.addAP("MY_WIFI", "TopSecret");

    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(500);
    }

    Serial.println("");
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);

    ds_sensor.begin();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();        
}

