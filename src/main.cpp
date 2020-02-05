/*******************************************************************************
Hog Water - A simple hog water heater control system by Stanley Solutions.
(c) Stanley Solutions - 2020
*******************************************************************************/

// Include Dependencies
#include <main.h>

// Define Global Variables
volatile long hallEffctCnt = 0;
int flow_rate = 0;
bool heater = false;
bool pump = false;

//****************************************************************************//
// Define Web Server Port
AsyncWebServer server(80);
// Configure One-Wire Temperature Sensor
OneWire oneWire(ONEWIREBUS);          // Initialize OneWire
DallasTemperature sensors(&oneWire);  // Initialize Temperature Sensor
//****************************************************************************//

//****************************************************************************//
// Define Data Retrieval Functions
float get_temp(void) {
  sensors.requestTemperatures();
  return sensors.getTempFByIndex(0);
}
float get_flow(void) {
  return flow_rate;
}
bool get_heater(void) {
  return heater;
}
bool get_pump(void) {
  return pump;
}
// Define Simple Interrupt Handler
void ICACHE_RAM_ATTR pulseCounter() {
  hallEffctCnt++;
}
//****************************************************************************//

//****************************************************************************//
// Define Index Template Handler
String index_template(const String& var)
{
  if(var == "AMBIENTTEMP"){ return String( get_temp() ); }
  if(var == "FLOWRATE"){ return String( get_flow() ); }
  if(var == "PUMPSTATE"){ return String( get_pump() ); }
  if(var == "HEATERSTATE"){ return String( get_heater() ); }
  return String(); // Return Empty String Otherwise
}
String json_temp(){
  return String("{\"temperature\":\""+ String(get_temp()) +"\"}");
}
String json_flow(){
  return String("{\"water_flow_rate\":\""+ String(get_temp()) +"\"}");
}
String json_pump(){
  return String("{\"pump_state\":\""+ String(get_temp()) +"\"}");
}
String json_heat(){
  return String("{\"heater_state\":\""+ String(get_temp()) +"\"}");
}
//****************************************************************************//

//****************************************************************************//
// Main Initialization Function
void setup() {
  // Initialization Function
  Serial.begin(9600);

  // Define Pin Modes
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(HALLSENSOR, INPUT);
  pinMode(HEATEROUT, OUTPUT);
  pinMode(PUMPOUT, OUTPUT);

  // Attach Interrupt
  attachInterrupt(HALLSENSOR, pulseCounter, RISING);

  // Initialize SPIFFS (File System)
  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }

  // Initialize WiFi; Attempt first with Primary Network
  int wifi_cntr = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    // Try Primary Network First
    if (wifi_cntr < 20) { Serial.println("Connecting to WiFi.."); }
    else if (wifi_cntr == 20) { WiFi.begin(failover_ssid, failover_password); }
    // Try Failover Network
    else if (wifi_cntr<40) { Serial.println("Connecting to Failover WiFi.."); }
    else { Serial.println("Failed to Connect."); return; }
    if (wifi_cntr < 41) { wifi_cntr++; }
  }

  // Print Local IP Address on Active Serial
  Serial.println(WiFi.localIP());

  // Define Main Index Template
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // Perform additional call-related functions here
    request->send(SPIFFS, "/index.html", "text/html", false, index_template);
  });

  // Define Various RESTful API Endpoints
  server.on("/api/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/json", json_temp() );
  });
  server.on("/api/temp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/json", json_temp() );
  });
  server.on("/api/flow", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/json", json_flow() );
  });
  server.on("/api/flowrate", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/json", json_flow() );
  });
  server.on("/api/pump", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/json", json_pump() );
  });
  server.on("/api/heat", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/json", json_heat() );
  });
  server.on("/api/heater", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/json", json_heat() );
  });

  // Start Object Controls
  sensors.begin();
  server.begin();
}
//****************************************************************************//

//****************************************************************************//
void loop() {
  // Main System Code

  // Evaluate Flow Rate
  cli(); // Disable Interrrupts Momentarily
  flow_rate = (hallEffctCnt/(5.5*3.785)); // Pulse freq / (5.5*3.785) = flow gpm
  hallEffctCnt = 0; // Reset Counter
  sei(); // Re-Enable Interrupts

  // Make Control Decisions to Enable Pump
  if (get_temp() < 35) { pump = true; }
  else { pump = false; }
  // Make Control Decisions to Enable Heater
  if (get_temp() < 32) { heater = true; }
  else { heater = false; }
  digitalWrite(HEATEROUT, !heater); // Invert since N-MOS is used
  digitalWrite(PUMPOUT, !pump); // Invert since N-MOS is used

  // Indicate System Operation and Delay Processing
  digitalWrite(LED_BUILTIN, LOW); // HEARTBEAT LED
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH); // HEARTBEAT LED
  delay(500);
}
//****************************************************************************//

// END
