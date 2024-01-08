#include <WiFi.h>

const char *ssid = "ESP32-Flame";
const char *password = "12345678";

const int ledPin = 22;  // GPIO pin connected to LED
const int serverPort = 80;

WiFiClient client;

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);

  connectToWiFi();  // Initial connection to Wi-Fi
}

void connectToWiFi() {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to WiFi. IP Address: ");
    Serial.println(WiFi.localIP());

    // Set up the client to connect to the sender ESP32
    IPAddress senderIP(192, 168, 4, 1); // Change to the IP address of the sender ESP32
    client.connect(senderIP, serverPort);
  } else {
    Serial.println("Failed to connect to WiFi. Please check your credentials and connection.");
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    connectToWiFi(); // Attempt to reconnect to Wi-Fi
  }

  if (client.connected()) {
    while (client.available() > 0) {
      // Read the received data as a string
      String receivedData = client.readStringUntil('\n');

      // Split the data into individual values
      int flameValue = receivedData.substring(0, receivedData.indexOf(',')).toInt();
      receivedData.remove(0, receivedData.indexOf(',') + 1);
      float temperatureValue = receivedData.substring(0, receivedData.indexOf(',')).toFloat();
      receivedData.remove(0, receivedData.indexOf(',') + 1);
      int gasValue = receivedData.toInt();

      // Check conditions and control LED
      if (flameValue == 0 || temperatureValue > 40.0 || gasValue > 700) {
        digitalWrite(ledPin, HIGH);  // Turn on LED
      } else {
        digitalWrite(ledPin, LOW);   // Turn off LED
      }
    }
  } else {
    Serial.println("Connection lost. Reconnecting...");
    
    // Attempt to reconnect to the sender ESP32
    IPAddress senderIP(192, 168, 4, 1); // Change to the IP address of the sender ESP32
    client.connect(senderIP, serverPort);
  }

  delay(1000); // Adjust delay according to your requirements
}
