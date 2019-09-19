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

//exposed vars and funcs
int getDoorState();
int wifiSignalStrength = WiFi.RSSI();
int doorState = getDoorState();

void setup () {
  //start serial on debug
  Serial.begin(115200);

  //set default pin modes
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT); // Relais-Module contro
  pinMode(D6, INPUT);  //Sensor door is up 0/1
  pinMode(D7, INPUT);  //Sensor door is down 0/1
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
  
  rest.function("open",openDoor);
  rest.function("close",closeDoor);
  rest.variable("wifi", &wifiSignalStrength);
  rest.variable("state", &doorState);
};

void loop () {
  // update variables
  wifiSignalStrength = WiFi.RSSI();
  doorState = getDoorState();
  
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

/*
 * close door
 * sets door toggle pin to high
 * returns integer
 */
int closeDoor(String command) {
  digitalWrite(DOOR_TOGGLE_PIN, HIGH);
  return 1;
}

/*
 * open door
 * sets door toggle pin to low
 * returns integer
 */
int openDoor(String command) {
  digitalWrite(DOOR_TOGGLE_PIN, LOW);
  return 1;
}

/*
 * get door state
 * -1 = unknown
 *  0 = open
 *  1 = closed
 *  
 *  returns integer
 */
int getDoorState() {
  int result = -1;
  
  if(digitalRead(DOOR_SENSOR_UP_PIN)){
    result = 0;
  }
  if(digitalRead(DOOR_SENSOR_DOWN_PIN)){
    result = 1;
  }
  return result;
}
