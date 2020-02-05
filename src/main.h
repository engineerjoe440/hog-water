/*******************************************************************************
Hog Water - A simple hog water heater control system by Stanley Solutions.
(c) Stanley Solutions - 2020
*******************************************************************************/

// Include Dependencies
#include <FS.h>
#include <stdio.h>
#include <Arduino.h>
#include <OneWire.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DallasTemperature.h>

//****************************************************************************//
// Define Primary Network Params
const char* ssid = "BarnNet";
const char* password =  "$t@nl3yt3ch";

// Define Debug Network Params
const char* failover_ssid = "InventorNet";
const char* failover_password =  "M!cr0c0ntr0l";
//****************************************************************************//

//****************************************************************************//
// Define Pins
#define ONEWIREBUS 4
#define HALLSENSOR 2
#define HEATEROUT 3
#define PUMPOUT 5
//****************************************************************************//

// END
