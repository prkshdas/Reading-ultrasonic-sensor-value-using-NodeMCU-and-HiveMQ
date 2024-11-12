#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "realme 9 5G";  // Replace with your WiFi SSID
const char* password = "@1234567";  // Replace with your WiFi password

// HiveMQ credentials
const char* mqtt_server = "broker.hivemq.com";  // HiveMQ broker address
const char* topic = "ultrasonic/topic";  // Your MQTT topic

WiFiClient espClient;
PubSubClient client(espClient);

const int trigPin = D1; // Trigger pin
const int echoPin = D2; // Echo pin

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
}

long readUltrasonicDistance() {
  // Trigger the sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echo pin
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance in cm
  long distance = duration * 0.034 / 2; 
  return distance;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long distance = readUltrasonicDistance();
  Serial.print("Distance: ");
  Serial.println(distance);
  
  // Publish the distance to HiveMQ
  String distanceStr = String(distance);
  client.publish(topic, distanceStr.c_str());
  
  delay(2000); // Delay between readings
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("NodeMCUClient")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}
