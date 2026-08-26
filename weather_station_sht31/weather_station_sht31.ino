#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "secrets.h"

// =====================================================
// WiFi
// =====================================================

WiFiServer server(80);

// =====================================================
// DHT11
// =====================================================

Adafruit_SHT31 sht31 = Adafruit_SHT31();

float temperature = NAN;
float humidity = NAN;

unsigned long lastSensorRead = 0;
const unsigned long sensorInterval = 2000;

// =====================================================
// OLED
// =====================================================

#define SDA_PIN 21
#define SCL_PIN 22

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// =====================================================
// BUTTON
// =====================================================

#define BUTTON_PIN 16

bool displayOn = true;

unsigned long displayStarted = 0;
const unsigned long displayDuration = 10000;

bool lastButtonState = LOW;

unsigned long lastDisplayUpdate = 0;
const unsigned long displayInterval = 500;

// =====================================================
// Setup
// =====================================================

void setup() {

  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT);

  // -----------------------------
  // Start I2C / OLED
  // -----------------------------

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {

    Serial.println("SSD1306 allocation failed");

    while (true) {
      delay(1000);
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);

  display.println("Starting...");

  display.display();

  // SHT

  if (!sht31.begin(0x44)) {
    Serial.println("SHT31 not found");
  }

  // -----------------------------
  // Connect to WiFi
  // -----------------------------

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 20) {

    delay(500);
    Serial.print(".");

    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println();
    Serial.println("WiFi connection failed");

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);

    display.println("WiFi connection");
    display.println("failed");

    display.display();

    return;
  }

  Serial.println();
  Serial.println("Connected to WiFi");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // -----------------------------
  // Start HTTP server
  // -----------------------------

  server.begin();

  Serial.println("HTTP server started");

  // Read sensor immediately
  readSensor();
  turnDisplayOn();
}

// =====================================================
// Main loop
// =====================================================

void loop() {

  unsigned long now = millis();

  // -----------------------------------------
  // Button
  // -----------------------------------------

  bool buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == HIGH && lastButtonState == LOW) {
    turnDisplayOn();
  }

  lastButtonState = buttonState;

  // -----------------------------------------
  // Turn display off after timeout
  // -----------------------------------------

  if (displayOn && now - displayStarted >= displayDuration) {
    turnDisplayOff();
  }

  // -----------------------------------------
  // Read DHT11 every 2 seconds
  // -----------------------------------------

  if (now - lastSensorRead >= sensorInterval) {

    lastSensorRead = now;

    readSensor();
  }

  // -----------------------------------------
  // Update OLED
  // -----------------------------------------

  if (displayOn && now - lastDisplayUpdate >= displayInterval) {

    lastDisplayUpdate = now;

    updateDisplay();
  }

  // -----------------------------------------
  // Handle HTTP
  // -----------------------------------------

  handleWebServer();
}

// =====================================================
// Read sensor
// =====================================================

void readSensor() {

  float newTemperature = sht31.readTemperature();
  float newHumidity = sht31.readHumidity();

  if (isnan(newTemperature) || isnan(newHumidity)) {

    Serial.println("Failed to read SHT31");

    return;
  }

  temperature = newTemperature;
  humidity = newHumidity;

  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.print(" C");

  Serial.print(" | Humidity: ");
  Serial.print(humidity, 1);
  Serial.println(" %");
}

// =====================================================
// Update OLED
// =====================================================

void updateDisplay() {

  display.clearDisplay();

  // -----------------------------
  // Temperature
  // -----------------------------

  display.setTextSize(2);
  display.setCursor(0, 0);

  display.print("Temp:");

  if (isnan(temperature)) {
    display.println("--");
  }
  else {
    display.print(temperature, 1);
    display.println("C");
  }

  // -----------------------------
  // Humidity
  // -----------------------------

  display.setCursor(0, 25);

  display.print("Hum:");

  if (isnan(humidity)) {
    display.println("--");
  }
  else {
    display.print(humidity, 1);
    display.println("%");
  }

  // -----------------------------
  // WiFi status
  // -----------------------------

  display.setTextSize(1);
  display.setCursor(0, 52);

  if (WiFi.status() == WL_CONNECTED) {

    display.print("WiFi: ");
    display.print(WiFi.localIP());

  }
  else {

    display.print("WiFi disconnected");
  }

  display.display();
}

void turnDisplayOn() {
  displayOn = true;
  displayStarted = millis();

  display.ssd1306_command(SSD1306_DISPLAYON);

  updateDisplay();
}

void turnDisplayOff() {
  displayOn = false;

  display.ssd1306_command(SSD1306_DISPLAYOFF);
}

// =====================================================
// HTTP server
// =====================================================

void handleWebServer() {

  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  Serial.println("Client connected");

  unsigned long timeout = millis();

  // Wait briefly for the HTTP request to arrive
  while (!client.available() && millis() - timeout < 100) {
    delay(1);
  }

  if (!client.available()) {
    client.stop();
    return;
  }

  // Read the first line of the HTTP request
  String request = client.readStringUntil('\r');

  // Consume the remaining newline
  client.read();

  Serial.print("Request: ");
  Serial.println(request);

  // ---------------------------------------------------
  // GET /data
  // ---------------------------------------------------

  if (request.startsWith("GET /data")) {

    if (isnan(temperature) || isnan(humidity)) {

      client.println("HTTP/1.1 500 Internal Server Error");
      client.println("Content-Type: application/json");
      client.println("Connection: close");
      client.println();

      client.println(
        "{\"error\":\"Failed to read DHT11\"}"
      );

    } else {

      String json = "{";

      json += "\"temperature\":";
      json += String(temperature, 1);

      json += ",\"humidity\":";
      json += String(humidity, 1);

      json += "}";

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Connection: close");
      client.println();

      client.println(json);

      Serial.print("JSON: ");
      Serial.println(json);
    }
  }

  // ---------------------------------------------------
  // Anything else
  // ---------------------------------------------------

  else {

    client.println("HTTP/1.1 404 Not Found");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();

    client.println(
      "{\"error\":\"Not found\"}"
    );
  }

  delay(1);
  client.stop();

  Serial.println("Client disconnected");
}
















// #include <WiFi.h>
// #include <DHT.h>
// #include <Adafruit_Sensor.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// // Replace with your network credentials
// const char* ssid = "";
// const char* password = "";

// // DHT11 Sensor setup
// #define DHTPIN 4          // Pin where the DHT11 is connected
// #define DHTTYPE DHT11     // DHT 11
// DHT dht(DHTPIN, DHTTYPE);

// // Create a web server on port 80
// WiFiServer server(80);

// void setup() {

//   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//   display.clearDisplay();
//   display.setTextSize(2);
//   display.setTextColor(WHITE);
//   display.setCursor(10, 10);
//   display.println("Subscribe");
//   display.display();

//   Serial.begin(115200);
//   dht.begin();

//   // Connect to Wi-Fi
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.print(".");
//   }
//   Serial.println("Connected to WiFi");
//   Serial.print("IP Address: ");
//   Serial.println(WiFi.localIP());

//   // Start the server
//   server.begin();
// }

// void loop() {
//   WiFiClient client = server.available(); // Listen for incoming clients

//   if (client) {
//     String currentLine = "";
//     while (client.connected()) {
//       if (client.available()) {
//         char c = client.read();
//         Serial.write(c);
//         if (c == '\n') {
//           // If the current line is blank, you got two newline characters in a row.
//           // That's the end of the client HTTP request, so send a response:
//           if (currentLine.length() == 0) {
//             // Read DHT11
//           float humidity = dht.readHumidity();
//           float temperature = dht.readTemperature();

//           // Check sensor reading
//           if (isnan(humidity) || isnan(temperature)) {

//             client.println("HTTP/1.1 500 Internal Server Error");
//             client.println("Content-Type: application/json");
//             client.println("Connection: close");
//             client.println();

//             client.println("{\"error\":\"Failed to read DHT11\"}");

//           } else {

//             // Create JSON response
//             String json = "{";
//             json += "\"temperature\":" + String(temperature, 1) + ",";
//             json += "\"humidity\":" + String(humidity, 1);
//             json += "}";

//             // HTTP response
//             client.println("HTTP/1.1 200 OK");
//             client.println("Content-Type: application/json");
//             client.println("Connection: close");
//             client.println();

//             client.println(json);

//             Serial.println();
//             Serial.println("Response:");
//             Serial.println(json);
//           }

//           break;
//           } else {
//             currentLine = "";
//           }
//         } else if (c != '\r') {
//           currentLine += c;
//         }
//       }
//     }
//     delay(1);
//     client.stop();
//   }
// }