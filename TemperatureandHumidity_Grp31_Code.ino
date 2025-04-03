#include <WiFi.h>
#include "DHT.h"
#include "ThingSpeak.h"

// Define DHT sensor type and GPIO pin
#define DHTPIN 4          // DHT sensor connected to GPIO4
#define DHTTYPE DHT22     // Change to DHT11 if using DHT11
DHT dht(DHTPIN, DHTTYPE);

// Define LED pins
#define GREEN_LED 5
#define RED_LED 18

// Define Buzzer pin
#define BUZZER 19

// Temperature & Humidity Thresholds
#define TEMP_THRESHOLD 30.0   // Temperature threshold in °C
#define HUM_THRESHOLD 60.0    // Humidity threshold in %

const char* WIFI_NAME = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";
const int channelNumber = 2898709;
const char* myApiKey = "3J5Z4E94VE6YQO2U";
const char* server = "api.thingspeak.com";

WiFiClient client;

void connectWiFi() {
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_NAME);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
    
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 20) {  // Max 20 attempts (20 seconds)
        delay(1000);
        Serial.print(".");
        attempt++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected!");
        Serial.print("Local IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi Connection Failed! Restarting...");
        ESP.restart();
    }
}

void setup() {
    Serial.begin(115200);
    dht.begin();

    // Set up LED and buzzer pins as OUTPUT
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    connectWiFi();  // Ensuring WiFi connection
    ThingSpeak.begin(client);
    
    // Initial state
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    Serial.println("DHT22 Sensor Simulation on Wokwi!");
}

void loop() {
    // Read temperature and humidity
    float temp = 35;
    float hum = 65;
    
    // Error checking
    if (isnan(temp) || isnan(hum)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }
    
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print(" °C  |  Humidity: ");
    Serial.print(hum);
    Serial.println(" %");

    // Check if temperature or humidity exceeds threshold
    if (temp > TEMP_THRESHOLD || hum > HUM_THRESHOLD) {
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, HIGH);

        Serial.println("\u26a0\ufe0f ALERT! High Temperature or Humidity!");

        // Making the buzzer beep 5 times
        for (int i = 0; i < 5; i++) {
            digitalWrite(BUZZER, HIGH);
            delay(200);  // Beep ON for 200ms
            digitalWrite(BUZZER, LOW);
            delay(200);  // Beep OFF for 200ms
        }
    } else {
        digitalWrite(GREEN_LED, HIGH);
        digitalWrite(RED_LED, LOW);
        digitalWrite(BUZZER, LOW);
    }
    
    // Sending data to ThingSpeak
    ThingSpeak.setField(1, temp);
    ThingSpeak.setField(2, hum);
    int check = ThingSpeak.writeFields(channelNumber, myApiKey);
    
    if (check == 200) {
        Serial.println("Data pushed successfully");
    } 
    Serial.println("****");
    
    delay(2000);  // Waiting for 2 seconds before next reading
}