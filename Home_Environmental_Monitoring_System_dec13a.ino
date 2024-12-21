#include <dht11.h>
#include <LiquidCrystal_I2C.h>
#include "arduino_secrets.h"
#include "thingProperties.h"

// Pin connected to the DHT11 sensor (D4 on ESP8266)
#define DHT11_PIN 2 
// Pin connected to the buzzer (D6 on ESP8266)
#define BUZZER_PIN 12
// Pin connected to the MQ-135 sensor
#define MQ135_PIN A0

LiquidCrystal_I2C lcd(0x27, 16, 2);
dht11 DHT11;

int currentScreen = 0; // Tracks the current screen being shown (0 or 1)
unsigned long lastSwitchTime = 0; // Last time the screen switched
const unsigned long switchInterval = 4000; // The page will switch every 4 seconds 

// Variable to track the last screen shown to avoid unnecessary clearing
int lastScreen = -1; // Set to -1 initially because no screen has been shown yet

void setup() {
  Serial.begin(9600);
  delay(1000);

  lcd.init();
  lcd.backlight();

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Ensure the buzzer is off at startup
}

void loop() {
  ArduinoCloud.update();
}

// The methods below are required for handling changes to the cloud variables
// They are empty but must be defined to avoid errors
void onAirQualityChange() {}
void onHumidityChange() {}
void onTemperatureChange() {}
