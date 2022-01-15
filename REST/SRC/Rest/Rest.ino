#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN 4    
#define DHTTYPE DHT11

// DECLARATION LED
int LED_ROUGE = 23;
int ledRouge_state = LOW;

// BUFFER A ENVOYER
String bufferToSendTemp = "";
String bufferToSendHum = "";

// CONNEXION WIFI
const char *ssid = "Batiana mohamed";
const char *password = "60010758";

// TIMER NON BLOQUANT
unsigned long interval = 2000;
unsigned long previousMillis = 0;

// JASON DOCUMENT
StaticJsonDocument<250> jsonDocument;
char buffer[250];

WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

float t;
float h;

void setup() {
  
 
  pinMode(LED_ROUGE,OUTPUT);
  digitalWrite(LED_ROUGE,ledRouge_state);

 Serial.begin(115200);
 
  // INIT DHT22
  dht.begin();

  // CONNEXION WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());

 
  server.on("/env", getEnv);
  server.on("/led", HTTP_POST, handlePost);    
  server.begin();  
  
}

void loop() {

  server.handleClient();
  unsigned long currentMillis = millis();
  
  if (currentMillis > previousMillis + interval) {

    previousMillis = currentMillis;
      
    t = dht.readTemperature();
    h = dht.readHumidity();
    Serial.print("Temp:");
    Serial.println(t);
    Serial.print("Hum:");
    Serial.println(h);
    
  }
   
}

void create_json(char *tag, float value, char *unit) {  
  jsonDocument.clear();  
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);
}

void add_json_object(char *tag, float value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit; 
}

void getTemp() {
  Serial.println("Get temperature");
  create_json("temperature", t, "°C");
  server.send(200, "application/json", buffer);
}
 
void getHum() {
  Serial.println("Get humidity");
  create_json("humidity", h, "%");
  server.send(200, "application/json", buffer);
}

void getEnv() {
  Serial.println("Get env");
  jsonDocument.clear(); // Clear json buffer
  add_json_object("temperature", t, "°C");
  add_json_object("humidity", h, "%");
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

void handlePost() {
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  
  // Get R  
  int red = jsonDocument["stateRouge"];
  digitalWrite(LED_ROUGE,red);

  
  Serial.print("Red: ");
  Serial.print(red);
  // Respond to the client
  server.send(200, "application/json", "{}");
}
