
#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>

#define DHTPIN 4
#define DHTTYPE DHT11
const int PinLed = 23;


WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 

//Ip = http://192.168.43.163/


DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Batiana mohamed";
const char* password = "60010758";


char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;

void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
//  mqttClient.setCallback(callback);
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        mqttClient.subscribe("esp32/message");//Souscription au nom du topic envoyé par le mqtt out dans nod red et récupéré dans hivemq
      }      
  }
}

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
  if(topic=="esp32/message"){
      Serial.print("Changing LED state ");
      if(messageTemp == "ON"){
        digitalWrite(PinLed, HIGH);
        Serial.print("true");
      }
      else if(messageTemp == "OFF"){
        digitalWrite(PinLed, LOW);
        Serial.print("false");
      }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      
    Serial.print ("IP du serveur:");
    Serial.println (WiFi.localIP ());

      Serial.print(".");
    } 
    Serial.println("");
     Serial.println("Connected to Wi-Fi");

     pinMode(23,OUTPUT);
     
  setupMQTT();
  mqttClient.setCallback(callback); //Activé la réception de donnée envoyé par le notered via le broker d'hivemq
    dht.begin();
}


void loop() {
  if (!mqttClient.connected())
    reconnect();
  mqttClient.loop();
  long now = millis();
  long previous_time = 0;
  
  if (now - previous_time > 1000) {
    previous_time = now;


float h = dht.readHumidity();

float t = dht.readTemperature();


//Serial.print(F("Humidity: "));
//Serial.print(h);
    char humString[8];
    dtostrf(h, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);
    mqttClient.publish("esp32/humidity", humString);



//   Serial.println(F("%  Temperature: "));
//  Serial.print(t);
//  Serial.print(F("°C "));
   char tempString[8];
    dtostrf(t, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    mqttClient.publish("esp32/temperature", tempString);


    delay(1000);

    
  }
}
 
