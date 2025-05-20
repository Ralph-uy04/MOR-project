#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  // Include the ArduinoJson library

// WiFi credentials
const char* ssid = "Huawei";
const char* password = "Jayson020300";
const char* serverUrl = "http://192.168.43.66:5000/get-latest-binary";  // Flask route that returns 0 or 1

// LED pins
const int greenLED = 12;  // Biodegradable
const int redLED = 13;    // Non-biodegradable

void printMAC() {
  // Print MAC address and chip info
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  delay(100);

  Serial.print("MAC Address (WiFi): ");
  Serial.println(WiFi.macAddress());

  uint64_t chipId = ESP.getEfuseMac();
  Serial.print("Chip ID: ");
  Serial.println((uint32_t)(chipId >> 32), HEX);
  Serial.println((uint32_t)(chipId & 0xFFFFFFFF), HEX);

  Serial.print("MAC Address (from efuse): ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", (uint8_t)(chipId >> (8 * (5 - i))) & 0xFF);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  Serial.print("SDK Version: ");
  Serial.println(ESP.getSdkVersion());
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Print MAC address and chip info
  printMAC();

  // Set LED pins as output
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  digitalWrite(greenLED, LOW);  // Ensure LEDs are off initially
  digitalWrite(redLED, LOW);

  Serial.println("Starting WiFi Debugging...");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Setup complete.");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);

    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      Serial.print("Received binary from server: ");
      Serial.println(payload);

      DynamicJsonDocument doc(1024);  /
      deserializeJson(doc, payload);  

      int binaryValue = doc["binary"];  

      Serial.print("Binary value: ");
      Serial.println(binaryValue);

      if (binaryValue == 1) {
        digitalWrite(greenLED, HIGH);  // Turn on green LED
        digitalWrite(redLED, LOW);     // Turn off red LED
      } else if (binaryValue == 0) {
        digitalWrite(greenLED, LOW);   // Turn off green LED
        digitalWrite(redLED, HIGH);    // Turn on red LED
      } else {
        digitalWrite(greenLED, LOW);   // Turn off both LEDs if invalid value
        digitalWrite(redLED, LOW);
      }

    } else {
      Serial.print("HTTP request failed. Code: ");
      Serial.println(httpCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected.");
  }

  delay(5000);  
