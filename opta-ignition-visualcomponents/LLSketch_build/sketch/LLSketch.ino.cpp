#include <Arduino.h>
#line 1 "C:\\Users\\3057675\\OneDrive - Queen's University Belfast\\2024_Manufacturing digitalization\\AMIC\\rotary bearings\\opta-ignition-visualcomponents\\LLSketch\\LLSketch.ino"
#include <AlPlc_Opta.h>

/* opta_1.1.0
*/

struct PLCSharedVarsInput_t
{
	bool wificonnection;
};
PLCSharedVarsInput_t& PLCIn = (PLCSharedVarsInput_t&)m_PLCSharedVarsInputBuf;

struct PLCSharedVarsOutput_t
{
};
PLCSharedVarsOutput_t& PLCOut = (PLCSharedVarsOutput_t&)m_PLCSharedVarsOutputBuf;


AlPlc AxelPLC(1338440454);

#include <WiFi.h>

// Wi-Fi credentials
const char* ssid = "VM1858024";
const char* password = "c4Eykoad4tMspsor";

unsigned long lastPublishTime = 0;
/*
IPAddress local_IP(192,168,0,125 ); // Your desired static IP
IPAddress gateway(192,168,0, 1);    // Network gateway
IPAddress subnet(255, 255, 255, 0);   // Subnet mask
IPAddress dns(8, 8, 8, 8);          // Preferred DNS server
*/

#line 34 "C:\\Users\\3057675\\OneDrive - Queen's University Belfast\\2024_Manufacturing digitalization\\AMIC\\rotary bearings\\opta-ignition-visualcomponents\\LLSketch\\LLSketch.ino"
void setup();
#line 46 "C:\\Users\\3057675\\OneDrive - Queen's University Belfast\\2024_Manufacturing digitalization\\AMIC\\rotary bearings\\opta-ignition-visualcomponents\\LLSketch\\LLSketch.ino"
void loop();
#line 54 "C:\\Users\\3057675\\OneDrive - Queen's University Belfast\\2024_Manufacturing digitalization\\AMIC\\rotary bearings\\opta-ignition-visualcomponents\\LLSketch\\LLSketch.ino"
void connect_wifi();
#line 34 "C:\\Users\\3057675\\OneDrive - Queen's University Belfast\\2024_Manufacturing digitalization\\AMIC\\rotary bearings\\opta-ignition-visualcomponents\\LLSketch\\LLSketch.ino"
void setup() {
  Serial.begin(9600);
  Serial.println("Serial communication started.");

  // Initialize Wi-Fi connection
  connect_wifi();

	AxelPLC.InitFileSystem();

	AxelPLC.Run();
}

void loop() {
  // Check Wi-Fi connection and attempt reconnection if necessary
  if (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
  }
}

// Wi-Fi connection function
void connect_wifi() {
  //WiFi.config(local_IP, gateway, subnet, dns);
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  const unsigned long wifiTimeout = 10000;  // 10-second timeout for Wi-Fi connection

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
    delay(100);  // Yield control
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected successfully.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to Wi-Fi.");
  }
}

