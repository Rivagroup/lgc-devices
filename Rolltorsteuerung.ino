/*
 * RESTful roller door control
 * controlling a roller door over REST using a wemos d1 mini pro with
 * a 1-relais module by Oliver Oehme 
 */

//import libraries
#include <ESP8266WiFi.h>
#include <aREST.h> //https://github.com/marcoschwartz/aREST (v2.8.0)

//define wemos d1 mini pro pinout gpio
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
const int DOOR_TOGGLE_PIN = D5;
const int DOOR_SENSOR_UP_PIN = D6;
const int DOOR_SENSOR_DOWN_PIN = D7;

//wifi parameters
const char* WIFI_SSID = "MegaraKasseLaser";
const char* WIFI_PASS = "megaralaserg";

//webserver parameters
#define LISTEN_PORT 80

//create instances
WiFiServer server(LISTEN_PORT);
aREST rest = aREST();

//Variables to be exposed to the API
int wifiSignalStrength = WiFi.RSSI();

void setup () {
  //start serial on debug
  Serial.begin(115200);

  //set default pin modes
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT); // Relais-Module contro
  pinMode(D6, INPUT); //Sensor door is up
  pinMode(D7, INPUT); //Sensor door is down
  pinMode(D8, OUTPUT);

  //set pins to low by default
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);
  digitalWrite(D8, LOW);

  // connect to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
  
  rest.set_id("rd01");
  rest.set_name("rollerDoor01");
  
  rest.function("open",doorOpen);
  rest.function("close",doorClose);
  rest.function("state",doorState);
  rest.variable("wifi", &wifiSignalStrength); 
};

void loop () {
  // update wifi signal strength
  wifiSignalStrength = WiFi.RSSI();
  
  // Handle REST calls
  WiFiClient client = server.available();
  if (client) {
 
    while(!client.available()){
      delay(5);
    }
    rest.handle(client);
    Serial.println("Handle REST call.");
  }
};

int doorClose(String command) {
  digitalWrite(DOOR_TOGGLE_PIN, HIGH);
  int timeout = 10;
  int count = 0;
  int success = 1;
  while(!getDoorIsDown()){
    if(count >= timeout){
      success = 0;
      break;
    }
    count++;
    delay(1000);
  }
  
  return success;
}

int doorOpen(String command) {
  digitalWrite(DOOR_TOGGLE_PIN, LOW);
  int timeout = 10;
  int count = 0;
  int success = 1;
  while(!getDoorIsUp()){
    if(count >= timeout){
      success = 0;
      break;
    }
    count++;
    delay(1000);
  }
  return success;
}

int doorState(String command) {
  int result = -1;
  
  if(getDoorIsDown()){
    result = 1;
  }

  if(getDoorIsUp()){
    result = 0;
  }
  
  return result;
}

int getDoorIsUp(){
  Serial.print("getDoorIsUp(): ");
  Serial.println(digitalRead(DOOR_SENSOR_UP_PIN));
  return digitalRead(DOOR_SENSOR_UP_PIN);
}

int getDoorIsDown(){
  Serial.print("getDoorIsDown(): ");
  Serial.println(digitalRead(DOOR_SENSOR_DOWN_PIN));
  return digitalRead(DOOR_SENSOR_DOWN_PIN);
}
