#include <AlPlc_Opta.h>

/* opta_1.1.0
      - WiFi (1.2.7)
      - ArduinoMqttClient (0.1.8)
      - ArduinoJson (6.21.5)
*/

struct PLCSharedVarsInput_t
{
	bool wificonnection;
	bool mqttconnection;
	float acceleration;
	float temperature;
	char messageTopic[32];
};
PLCSharedVarsInput_t& PLCIn = (PLCSharedVarsInput_t&)m_PLCSharedVarsInputBuf;

struct PLCSharedVarsOutput_t
{
};
PLCSharedVarsOutput_t& PLCOut = (PLCSharedVarsOutput_t&)m_PLCSharedVarsOutputBuf;


AlPlc AxelPLC(-17995374);

#include <WiFi.h>
#include <ArduinoMqttClient.h>

// Wi-Fi credentials
const char* ssid = "SKYATTVN";
const char* password = "cWYSQ51DUgsv";

// MQTT broker details
const char* mqtt_server = "192.168.0.51";
const int mqtt_port = 1883;
const char* mqtt_user = "user";
const char* mqtt_password = "123qweasd";
const char* mqtt_topic_sub = "RotaryBearings/Neuron/values";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

unsigned long lastPublishTime = 0;

void setup() {
    
    Serial.begin(9600);
    Serial.println("Serial communication started.");
  // Initialize Wi-Fi connection
  connect_wifi();

  // Set MQTT client ID and credentials
  mqttClient.setId("OptaPLC");
  mqttClient.setUsernamePassword(mqtt_user, mqtt_password);

  // Connect to MQTT server
  connect_mqtt();
// Subscribe to the specific topic
  mqttClient.subscribe(mqtt_topic_sub, 1);
  // Set callback for receiving messages
 mqttClient.onMessage(onMqttMessage);
  Serial.print("Waiting for messages on topic: ");
  Serial.println(mqtt_topic_sub);
  

	AxelPLC.Run();
}

void loop() {
  // Check Wi-Fi connection
  mqttClient.poll();
  if (WiFi.status() != WL_CONNECTED) {
    PLCIn.wificonnection = false;
    connect_wifi();  // Attempt to reconnect if disconnected
  } else {
    PLCIn.wificonnection = true;
  }

  // Check MQTT connection and reconnect if necessary
  if (!mqttClient.connected() && PLCIn.wificonnection) {
    PLCIn.mqttconnection = false;
    connect_mqtt();
  } else if (mqttClient.connected()) {
    PLCIn.mqttconnection = true;
  }
  
}

// Wi-Fi connection function
void connect_wifi() {
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  const unsigned long wifiTimeout = 10000;  // 10-second timeout for Wi-Fi connection

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
    delay(100);  // Yield control
  }

  PLCIn.wificonnection = (WiFi.status() == WL_CONNECTED);
}

// MQTT connection function
void connect_mqtt() {
  if (mqttClient.connect(mqtt_server, mqtt_port)) {
    PLCIn.mqttconnection = true;
    //mqttClient.subscribe(mqtt_topic_sub, 1);  // Subscribe to the topic on successful connect
  } else {
    PLCIn.mqttconnection = false;
  }
}
/*
void onMqttMessage(int messageSize) {
  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
  Serial.println();
}
*/
void onMqttMessage(int messageSize) {
  // Create a character buffer to hold the payload
  char payload[64];  // Adjust size as needed
  int index = 0;

  // Read the payload into the character buffer
  while (mqttClient.available() && index < sizeof(payload) - 1) {
    payload[index++] = (char)mqttClient.read();
  }
  payload[index] = '\0';  // Null-terminate the string

  // Print the received payload for verification
  Serial.print("Received payload: ");
  Serial.println(payload);

  // Initialize variables for acceleration and temperature
  float acceleration = 0.0;
  float temperature = 0.0;

  // Parse the payload assuming it's in format "[value1,value2]"
  int startIdx = 1;  // Skip the '[' character
  int separatorIdx = -1;

  // Locate the comma separator between the values
  for (int i = startIdx; i < index; i++) {
    if (payload[i] == ',') {
      separatorIdx = i;
      break;
    }
  }

  // Ensure we found a separator and it's correctly positioned
  if (separatorIdx > startIdx && separatorIdx < index - 1) {
    // Convert the first number to a float (acceleration)
    acceleration = atof(&payload[startIdx]);
    
    // Convert the second number to a float (temperature)
    temperature = atof(&payload[separatorIdx + 1]);
  }

  // Print the parsed values for debugging
  Serial.print("Parsed acceleration: ");
  Serial.println(acceleration);
  Serial.print("Parsed temperature: ");
  Serial.println(temperature);

  // Now you can assign these values to your variables or structure members
  // For example:
  PLCIn.acceleration = acceleration;
  PLCIn.temperature = temperature;
}
