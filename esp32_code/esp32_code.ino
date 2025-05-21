#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  

// WiFi credentials
const char* ssid = "GlobeAtHome_887BD_2.4";
const char* password = "xUju7JQX";
const char* serverUrl = "http://192.168.254.107:5000/get-latest-binary";  

// LED pins
const int greenLED = 12;  
const int redLED = 13;    

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 20) { 
    delay(1000);
    Serial.print(".");
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi Connection Failed!");
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
        Serial.println("JSON parsing failed");
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
    Serial.println("WiFi disconnected. Retrying...");
    WiFi.reconnect();
  }

  delay(5000);
}
