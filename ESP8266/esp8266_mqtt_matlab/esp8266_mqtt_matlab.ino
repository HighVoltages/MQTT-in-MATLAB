#include <Adafruit_Sensor.h> //Library for Adafruit sensors , we are using for DHT
#include <DHT_U.h> //DHT library which uses some func from Adafruit Sensor library
#include <ESP8266WiFi.h> //library for using ESP8266 WiFi 
#include <PubSubClient.h> //library for MQTT
#include <ArduinoJson.h> //library for Parsing JSON

//defining Pins
#define DHTPIN 5
#define LED D2

//DHT parameters
#define DHTTYPE    DHT11     // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

//MQTT Credentials
const char* ssid = "SSID";//setting your ap ssid
const char* password = "Password";//setting your ap psk
const char* mqttServer = "iot.reyax.com"; //MQTT URL
const char* mqttUserName = "MQTT username";  // MQTT username
const char* mqttPwd = "Password";  // MQTT password
const char* clientID = "username0001"; // client id username+0001
const char* topic = "ESP8266/data"; //publish topic

//parameters for using non-blocking delay
unsigned long previousMillis = 0;
const long interval = 5000;

String msgStr = "";      // MQTT message buffer

float temp, hum;


//setting up wifi and mqtt client
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(clientID, mqttUserName, mqttPwd)) {
      Serial.println("MQTT connected");
      client.subscribe("Led1/status");
      Serial.println("Topic Subscribed");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);  // wait 5sec and retry
    }

  }

}

//subscribe call back
void callback(char*topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String data = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    data += (char)payload[i];
  }



  if (data == "1") {
    Serial.println("LED");
    digitalWrite(LED, HIGH);
  }
  else {
    digitalWrite(LED, LOW);
  }
}


void setup() {
  Serial.begin(115200);
  // Initialize device.
  dht.begin();
  // get temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  setup_wifi();

  client.setServer(mqttServer, 1883); //setting MQTT server
  client.setCallback(callback); //defining function which will be called when message is recieved.
  randomSeed(analogRead(0));


}

void loop() {
  if (!client.connected()) { //if client is not connected
    reconnect(); //try to reconnect
  }
  client.loop();

  unsigned long currentMillis = millis(); //read current time

  if (currentMillis - previousMillis >= interval) { //if current time - last time > 5 sec
    previousMillis = currentMillis;

    //read temp and humidity
    sensors_event_t event;
    dht.temperature().getEvent(&event);


    if (isnan(event.temperature)) {
      Serial.println(F("Error reading temperature!"));
    }
    else {
      Serial.print(F("Temperature: "));
      temp = event.temperature;
      Serial.print(temp);
      Serial.println(F("°C"));
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("Error reading humidity!"));
    }
    else {
      Serial.print(F("Humidity: "));
      hum = event.relative_humidity;
      Serial.print(hum);
      Serial.println(F("%"));
    }
    int rand_num = random(90);

    msgStr = "{\"parameters\" : { \"temp\" : " + String(temp) + ",\"humidity\": " + String(hum) + ",\"Random\": " + String(rand_num) + "}}";
    byte arrSize = msgStr.length() + 1;
    char msg[arrSize];

    Serial.print("PUBLISH DATA:");
    Serial.println(msgStr);
    msgStr.toCharArray(msg, arrSize);
    client.publish(topic, msg);
    msgStr = "";
    delay(50);

  }

}