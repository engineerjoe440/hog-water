/*******************************************************************************
Hog Water - A simple hog water heater control system by Stanley Solutions.
(c) Stanley Solutions - 2020
*******************************************************************************/

// Include Dependencies
#include <FS.h>
#include <stdio.h>
#include <Arduino.h>
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <EMailSender.h>
#include <ESPAsyncWebServer.h>
#include <DallasTemperature.h>

//****************************************************************************//
// Define Primary Network Params
const char* ssid = "BarnNet";
const char* password =  "$t@nl3yt3ch";

// Define Debug Network Params
const char* failover_ssid = "InventorNet";
const char* failover_password =  "M!cr0c0ntr0l";

// Define Failover Access Point Parameters
const char* ap_ssid = "HogWaterAP";
const char* ap_pass = "$t@nl3yt3ch";
//****************************************************************************//

//****************************************************************************//
// Define Email Parameters
const char* local_e_address = "stanleyembeddedtech@gmail.com";
const char* local_e_password =  "St@nl3ys0lut!0n$t3ch";
//****************************************************************************//

//****************************************************************************//
// Define Pins
#define ONEWIREBUS 1  // TX
#define HALLSENSOR 3  // RX
#define HEATEROUT 0   // GPIO0
#define PUMPOUT 2     // GPIO2 (ON-BOARD LED)
//****************************************************************************//

// END
