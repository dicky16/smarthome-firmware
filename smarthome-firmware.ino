#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// MQTT broker credentials
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const int mqtt_port = 1883; // Default MQTT port
const char* mqtt_user = "YOUR_MQTT_USERNAME";  // Optional if broker requires username
const char* mqtt_password = "YOUR_MQTT_PASSWORD"; // Optional if broker requires password

// MQTT topic
const char* lamp_topic = "home/room1/lamp1"; // Topic to control the lamp

// GPIO pin for the lamp (connected to a relay)
const int lampPin = 2; // Adjust according to your ESP32 pin layout

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // Set lamp pin as output
  pinMode(lampPin, OUTPUT);
  digitalWrite(lampPin, LOW); // Set initial state to off

  // Connect to WiFi
  setup_wifi();

  // Set MQTT server and callback
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
}

void loop() {
  // Reconnect if connection is lost
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Process MQTT messages
}

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttCallback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Check if the message is to turn the lamp ON or OFF
  if (String(topic) == lamp_topic) {
    if (messageTemp == "ON") {
      digitalWrite(lampPin, HIGH); // Turn on the lamp
      Serial.println("Lamp turned ON");
    } else if (messageTemp == "OFF") {
      digitalWrite(lampPin, LOW); // Turn off the lamp
      Serial.println("Lamp turned OFF");
    }
  }
}

void reconnect() {
  // Reconnect until successful
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");

      // Subscribe to lamp topic
      client.subscribe(lamp_topic);
      Serial.print("Subscribed to topic: ");
      Serial.println(lamp_topic);
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}