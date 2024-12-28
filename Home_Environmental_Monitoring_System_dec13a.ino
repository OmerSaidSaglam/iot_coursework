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

  int chk = DHT11.read(DHT11_PIN); // Read the data from the DHT11 sensor
  float hum = (float)DHT11.humidity; // Get the humidity value
  float temp = (float)DHT11.temperature; // Get the temperature value

  Serial.print("Humidity: "); // Print the humidity value to the Serial Monitor for debugging
  Serial.println(hum);
  Serial.print("Temperature: ");
  Serial.println(temp); // Print the temperature value to the Serial Monitor for debugging

  // Update the cloud variables with the latest readings for humidity and temperature
  humidity = hum;
  temperature = temp;

  int airQualityValue = analogRead(MQ135_PIN); // Read the air quality value from the MQ135 sensor
  Serial.print("Air Quality: ");
  Serial.println(airQualityValue); // Print the air quality value to the Serial Monitor for debugging

  airQuality = airQualityValue; // Update the cloud variable with the latest reading for air quality

  // Check if any condition is true and trigger the buzzer
  if (temp > 24 || hum > 75 || airQualityValue > 130) {
    digitalWrite(BUZZER_PIN, HIGH); // Turn on the buzzer
  } else {
    digitalWrite(BUZZER_PIN, LOW); // Turn off the buzzer
  }

  unsigned long currentTime = millis();
  if (currentTime - lastSwitchTime > switchInterval) { // Check if the specified interval (4 seconds) has passed
    // Increment currentScreen by 1, then apply modulo 2 to keep the value within the range of 0 and 1
    // For example, if currentScreen is 0: (0 + 1) % 2 = 1
    // If currentScreen is 1: (1 + 1) % 2 = 0
    // This ensures that currentScreen alternates between 0 and 1 every time
    currentScreen = (currentScreen + 1) % 2;
    lastSwitchTime = currentTime; // Update the last time the screen was switched to the current time
  }
 
  if (currentScreen != lastScreen) {
    lcd.clear(); // Clear the LCD screen for a new page
    lastScreen = currentScreen; // Update the last screen to the current one
  }

  // Display logic based on page
  if (currentScreen == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print(" "); // Space between the temperature and the degree symbol
    lcd.print((char)223); // Degree symbol
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("Hum: " + String(hum) + " %");
  } else if (currentScreen == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Air Quality: ");
    lcd.print(airQualityValue);
  }
  // Pause for 0.5 seconds to slow down the loop and make the readings readable on the LCD screen
  delay(500);
}

// The methods below are required for handling changes to the cloud variables
// They are empty but must be defined to avoid errors
void onAirQualityChange() {}
void onHumidityChange() {}
void onTemperatureChange() {}
