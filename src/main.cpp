/*******************************************************************************
Hog Water - A simple hog water heater control system by Stanley Solutions.
(c) Stanley Solutions - 2020
*******************************************************************************/

// Includes
#include <stdio.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

// Define Debug Network Params
const char* ssid = "InventorNet";
const char* password =  "M!cr0c0ntr0l";

// Define Release Network Params
//const char* ssid = "BarnNet";
//const char* password =  "$t@nl3yt3ch";

// Define Web Server Port
AsyncWebServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "You've Reached the Index!");
  });

  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello from ESP8266 server route");
  });

  server.on("/redirect/internal", HTTP_GET, [](AsyncWebServerRequest *request){
    request->redirect("/hello");
  });

  server.on("/redirect/external", HTTP_GET, [](AsyncWebServerRequest *request){
    request->redirect("https://techtutorialsx.com/");
  });

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
}
