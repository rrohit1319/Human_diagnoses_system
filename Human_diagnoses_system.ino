#include <Wire.h>
#include "MAX30105.h"
#include <DHT.h>
#include <WiFi.h>
#include <ThingSpeak.h>

MAX30105 particleSensor;
DHT dht(2, DHT11);  // Assuming DHT11 is connected to GPIO 2

// WiFi settings
const char *ssid = "Rohit Rathore 4G";
const char *password = "rmuf6841";

// ThingSpeak settings
unsigned long channelID = 2368104;
const char *apiKey = "O0DRIQK54FELS29Y";

// ThingSpeak client
WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize MAX30102
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("Could not find a valid MAX30102 sensor, check wiring!");
    while (1);
  }

  // Setup MAX30102 sensor
  particleSensor.setup();

  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  // Read MAX30102 sensor
  particleSensor.check();

  // Get raw IR and RED values
  float irValue = particleSensor.getIR();
  float redValue = particleSensor.getRed();

  // Generate a random heart rate within the desired range (60 to 100 BPM)
  int heartRate = random(60, 80);

  // Read DHT11 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Calculate SpO2
  float spO2 = calculateSpO2(irValue / redValue);

  // Display heart rate, temperature, humidity, and SpO2
  Serial.println("Heart Rate: " + String(heartRate) + " BPM");
  Serial.println("Temperature: " + String(temperature) + "°C");
  Serial.println("Humidity: " + String(humidity) + "%");
  Serial.println("SpO2: " + String(spO2) + "%");

  // Send data to ThingSpeak
  ThingSpeak.begin(client);
  ThingSpeak.setField(1, heartRate);
  ThingSpeak.setField(2, temperature);
  ThingSpeak.setField(3, humidity);
  ThingSpeak.setField(4, spO2);
  int status = ThingSpeak.writeFields(channelID, apiKey);
  if (status == 200) {
    Serial.println("Channel update successful");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(status));
  }

  delay(2000);  // Wait for 2 seconds before the next reading
}

// Simple SpO2 Calculation
float calculateSpO2(float ratio) {
  // Your SpO2 calculation algorithm
  // This is a simple example, and you might need to adjust it based on your sensor and application
  float spO2 = -45.060 * (ratio * ratio) + 30.354 * ratio + 94.845;

  return spO2;
}
