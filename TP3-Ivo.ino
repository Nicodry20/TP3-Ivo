#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "IoTB";  // Reemplaza con el nombre de tu red WiFi
const char* password = "inventaronelVAR";  // Reemplaza con la contraseña de tu red WiFi

// Define the pin where the DHT11 is connected
#define DHTPIN 21
#define DHTTYPE DHT11

// Initialize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Define LED pins
#define GREEN_LED 13
#define YELLOW_LED1 12
#define YELLOW_LED2 14w
#define RED_LED1 27
#define RED_LED2 26

// Define button pins
#define BUTTON_UP 4
#define BUTTON_DOWN 5

// Initial sensitivity (5°C per LED)
float sensitivity = 5.0;

// Create a WebServer object
WebServer server(80);  // Puerto 80 para HTTP

// Function declarations
void allLedsOff();
void displayTemperatureOnLeds(float temperature);
void checkButtons();
void handleRoot();

void setup() {
  Serial.begin(115200);

  // Initialize WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", handleRoot);  // Define root page handler
  server.begin();
  Serial.println("HTTP server started");

  // Initialize the DHT sensor
  dht.begin();

  // Set the LED pins as outputs
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED1, OUTPUT);
  pinMode(YELLOW_LED2, OUTPUT);
  pinMode(RED_LED1, OUTPUT);
  pinMode(RED_LED2, OUTPUT);

  // Set the button pins as inputs with pull-up resistors
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);

  // Turn off all LEDs initially
  allLedsOff();
}

void loop() {
  // Read temperature as Celsius
  float temperature = dht.readTemperature();

  // Check if any reads failed
  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Check if buttons are pressed to adjust sensitivity
  checkButtons();

  // Print temperature and sensitivity
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C, Sensitivity: ");
  Serial.print(sensitivity);
  Serial.println("°C per LED");

  // Light up LEDs based on temperature and sensitivity
  displayTemperatureOnLeds(temperature);

  // Handle HTTP server requests
  server.handleClient();

  delay(1000);  // Wait 2 seconds before the next reading
}

void allLedsOff() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED1, LOW);
  digitalWrite(YELLOW_LED2, LOW);
  digitalWrite(RED_LED1, LOW);
  digitalWrite(RED_LED2, LOW);
}

void displayTemperatureOnLeds(float temperature) {
  allLedsOff();  // Reset LEDs

  if (temperature >= sensitivity * 1) digitalWrite(GREEN_LED, HIGH);
  if (temperature >= sensitivity * 2) digitalWrite(YELLOW_LED1, HIGH);
  if (temperature >= sensitivity * 3) digitalWrite(YELLOW_LED2, HIGH);
  if (temperature >= sensitivity * 4) digitalWrite(RED_LED1, HIGH);
  if (temperature >= sensitivity * 5) digitalWrite(RED_LED2, HIGH);
}

void checkButtons() {
  if (digitalRead(BUTTON_UP) == LOW) {
    sensitivity += 1.0;
    delay(200);
  }
  if (digitalRead(BUTTON_DOWN) == LOW) {
    sensitivity -= 1.0;
    if (sensitivity < 1.0) sensitivity = 1.0;
    delay(200);
  }
}

// HTTP request handler for root page
void handleRoot() {
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    server.send(200, "text/plain", "Failed to read from DHT sensor!");
    return;
  }

  String html = "<!DOCTYPE html><html><head><title>Temperature Sensor</title></head><body>";
  html += "<h1>Temperature Sensor Data</h1>";
  html += "<p>Temperature: " + String(temperature) + " °C</p>";
  html += "<p>Sensitivity: " + String(sensitivity) + " °C per LED</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}