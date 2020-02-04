/*******************************************************************************
Hog Water - A simple hog water heater control system by Stanley Solutions.
(c) Stanley Solutions - 2020
*******************************************************************************/

// Includes
#include <FS.h>
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

// Define Test Response
String processor(const String& var)
{

  if(var == "PLACEHOLDER"){
    return String(random(1,20));
  }

  return String();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Define LED Pin Mode
  pinMode(LED_BUILTIN, OUTPUT);

  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "You've Reached the Index!");
  });

  server.on("/template", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("This is the Template!");
    request->send(SPIFFS, "/test.html", "text/html", false, processor);
  });

  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello from ESP8266 server route");
  });

  server.on("/json", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "{'test':'json-test'}");
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
  digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level)
  delay(1000); // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
  delay(1000); // Wait for two seconds
}
