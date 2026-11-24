#include <WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>
#define DHTPIN 15
#define DHTTYPE DHT22
#define LED_PIN 2
const float threshold = 30.0;
const char* ssid = "Wokwi-GUEST";
const char* password = "";
unsigned long channelID = 0; // Replace with your channel ID
const char* writeAPIKey = "YOUR_WRITE_API_KEY";
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
unsigned long lastUpload = 0;
const unsigned long uploadInterval = 20000;
void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  WiFi.begin(ssid, password, 6);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
  ThingSpeak.begin(client);
  Serial.println("Smart Room Monitoring Started");
}
void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Sensor reading failed!");
    delay(2000);
    return;
  }
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  int ledStatus = temperature >= threshold ? 1 : 0;
  digitalWrite(LED_PIN, ledStatus ? HIGH : LOW);
  Serial.println(ledStatus ? "LED: ON" : "LED: OFF");
  Serial.println(ledStatus ? "High Temperature Alert!" : "Normal Temperature");
  if (millis() - lastUpload >= uploadInterval) {
    lastUpload = millis();
    if (channelID == 0) {
      Serial.println("Configure ThingSpeak credentials to enable uploads.");
    } else if (WiFi.status() == WL_CONNECTED) {
      ThingSpeak.setField(1, temperature);
      ThingSpeak.setField(2, humidity);
      ThingSpeak.setField(3, ledStatus);
      int response = ThingSpeak.writeFields(channelID, writeAPIKey);
      if (response == 200) Serial.println("Data uploaded successfully!");
      else {
        Serial.print("Upload failed. Error: ");
        Serial.println(response);
      }
    } else {
      Serial.println("Wi-Fi disconnected!");
      WiFi.reconnect();
    }
  }
  Serial.println("--------------------");
  delay(2000);
}
