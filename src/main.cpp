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
bool alarm = false;

// Define Recipient Email Address
String send_to_address = "engineerjoe440@yahoo.com";

//****************************************************************************//
// Define Web Server Port
AsyncWebServer server(80);
// Configure One-Wire Temperature Sensor
OneWire oneWire(ONEWIREBUS);          // Initialize OneWire
DallasTemperature sensors(&oneWire);  // Initialize Temperature Sensor
// Define Email Sender Object
EMailSender emailSend(local_e_address, local_e_password);
//****************************************************************************//

//****************************************************************************//
// Define Data Retrieval Functions
float get_temp(void) {
  sensors.requestTemperatures();
  return max(sensors.getTempFByIndex(0), float(-40));
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
bool get_pump_alarm(void) {
  return alarm;
}
// Define String Formatting Functions
String bool_to_tf_string(bool state){
  if (state){ return "True"; }
  else { return "False"; }
}
String bool_to_onoff_string(bool state){
  if (state){ return "ON"; }
  else { return "OFF"; }
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
  if(var == "PUMPSTATE"){ return bool_to_onoff_string( get_pump() ); }
  if(var == "PUMPALARM"){ return bool_to_tf_string( get_pump_alarm() ); }
  if(var == "HEATERSTATE"){ return bool_to_onoff_string( get_heater() ); }
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
String json_pump_alarm(){
  return String("{\"pump_alarm\":\""+ String(get_pump_alarm()) +"\"}");
}
String json_heat(){
  return String("{\"heater_state\":\""+ String(get_temp()) +"\"}");
}
//****************************************************************************//

//****************************************************************************//
// Define Email Sender Function
void send_email(  const String& recipient,
                  const String& subject,
                  const String& message) {
  // Declare Character Array for Recipient Address
  char addrBuf[255];
  recipient.toCharArray(addrBuf, 255);

  // Generate Message From Arguments
  EMailSender::EMailMessage e_message;
  e_message.subject = subject;  // Load Subject
  e_message.message = message;  // Load Message

  // Send Message
  EMailSender::Response resp = emailSend.send(addrBuf, e_message);
}
//****************************************************************************//

//****************************************************************************//
// Main Initialization Function
void setup() {
  // Define Pin Modes
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(HALLSENSOR, INPUT);
  pinMode(HEATEROUT, OUTPUT);
  pinMode(PUMPOUT, OUTPUT);

  // Attach Interrupt
  attachInterrupt(HALLSENSOR, pulseCounter, RISING);

  // Initialize SPIFFS (File System)
  if(!SPIFFS.begin()){ return; }

  // Initialize WiFi; Attempt first with Primary Network
  int wifi_cntr = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    // Try Primary Network First, then Fail Over to InventorNet
    if (wifi_cntr == 20) { WiFi.begin(failover_ssid, failover_password); }
    else if (wifi_cntr>40) { return; }
    if (wifi_cntr < 41) { wifi_cntr++; }
  }

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
  server.on("/api/pump_alarm", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/json", json_pump_alarm() );
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

  // Send Email to Signal Successful Boot
  send_email( send_to_address,  // ADDRESS
              "HogWater BOOT",  // SUBJECT
              "The HogWater System has Booted Successfully!" // BODY
            );
}
//****************************************************************************//

//****************************************************************************//
// Main System Code
void loop() {
  // Local Variable Declaration
  uint failure_cntr = 0;

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

  // Evaluate Pump Failure
  if (pump && (flow_rate < float(1))) {
    alarm = true;
    // Manage Sending Alarm Notice Emails
    if (failure_cntr == 0) {
      failure_cntr = 1000; // Set to Countdown Time; 1000 Seconds
      // Send Email to Signal Successful Boot
      send_email( send_to_address,  // ADDRESS
          "HogWater Pump Failure",  // SUBJECT
          String("The HogWater System has Experienced a Pump Failure. ") +
          String("There appears to be no water flow. Please Review ASAP.")
                );
    } else { failure_cntr--; }
  }
  else {
    // Reset Alarm Count and Indicator
    alarm = false;
    failure_cntr = 0;
  }
}
//****************************************************************************//

// END
