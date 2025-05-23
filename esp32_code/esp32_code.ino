#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  

// WiFi credentials
const char* ssid = "wifi-ssid-here";
const char* password = "wifi-password-here";
const char* serverUrl = "http://wifi-ip-goes-here:5000/get-latest-binary";  

// LED pins
const int greenLED = 12;   // Biodegradable
const int redLED = 13;     // Non-biodegradable

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);

  Serial.println("Connecting to WiFi...");
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(200);
  WiFi.begin(ssid, password);

  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 30) { // Increased retries
    delay(500);
    Serial.print(".");
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi Connection Failed! Restarting...");
    ESP.restart();  // Restart ESP32 to attempt reconnection
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    
    int httpCode = http.GET(); 

    if (httpCode == 200) {
      String payload = http.getString();
      Serial.print("Received response: ");
      Serial.println(payload);

      DynamicJsonDocument doc(256);
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.println("JSON parsing failed. Retrying...");
        return;
      }

      int binaryValue = doc["binary"];
      Serial.print("Binary value: ");
      Serial.println(binaryValue);

      if (binaryValue == 1) {
        digitalWrite(greenLED, HIGH);
        digitalWrite(redLED, LOW);
      } else if (binaryValue == 0) {
        digitalWrite(greenLED, LOW);
        digitalWrite(redLED, HIGH);
      } else {
        digitalWrite(greenLED, LOW);
        digitalWrite(redLED, LOW);
      }
    } else {
      Serial.print("HTTP request failed. Code: ");
      Serial.println(httpCode);
    }

    http.end();
  } else {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.disconnect();
    WiFi.reconnect();
  }

  delay(5000);
}
