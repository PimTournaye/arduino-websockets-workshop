#include <Arduino.h>
#include <SPI.h> // This library is part of the standard library included in Arduino
#include <WiFiNINA.h> // This library is part of the standard library included in Arduino
#include <WebSocketsClient.h> // This is part of the arduinoWebsockets library, make sure its installed
#include <Arduino_JSON.h> // Make sure to install this library

// Your WiFi network's name
#define WIFI_SSID ""
// The WiFi network's password
#define WIFI_PASS ""
// The IP or URL where the server is running
#define HOST_ADDRESS ""
// The port that the server uses
#define PORT 3000

int status = WL_IDLE_STATUS;
WiFiClient client;           // Initializing a client to communicate through WiFi
WebSocketsClient webSocket;  // Initialize a client to do websocket things

/**
 * WebSocket Event Handler
 * This function handles all the different types of WebSocket events that can occur.
 * Add code in this function per case to add functionality when you receive messages from the server.
 * 
 * @param type    The type of WebSocket event (defined by WStype_t enum)
 * @param payload The data received in the event (if any)
 * @param length  The length of the payload data
 * 
 * Common scenarios:
 * 1. Initial connection to server
 * 2. Receiving messages from server
 * 3. Connection lost/disconnected
 * 4. Errors in communication
 */
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      // This event occurs when:
      // - The connection to the server is lost
      // - The server closes the connection
      // - WiFi connection is lost
      Serial.println("[WebSocket] Disconnected!");
      break;

    case WStype_CONNECTED:
      // This event occurs when:
      // - Successfully connected to the WebSocket server
      // - The initial handshake is complete
      // Common uses:
      // - Send initial state or configuration
      // - Request initial data from server
      Serial.println("[WebSocket] Connected!");
      webSocket.sendTXT("Connected");
      break;

    case WStype_TEXT:
      // This event occurs when:
      // - Server sends a text message
      // - Typically JSON data or command strings
      // Example incoming messages:
      // - {"command": "led", "state": "on"}
      // - {"request": "status"}
      Serial.print("[WebSocket] Received text: ");
      Serial.println((char *)payload);

      // Example of handling incoming JSON:
      // JSONVar incoming = JSON.parse((char *)payload);
      // if (JSON.typeof(incoming) == "object") {
      //   if (incoming.hasOwnProperty("command")) {
      //     String command = (const char*) incoming["command"];
      //     // Handle command
      //   }
      // }

      // IMPORTANT: Always validate JSON before processing!
      // Invalid JSON can crash your Arduino
      // if (JSON.typeof(incoming) == "undefined") {
      //   Serial.println("Invalid JSON received");
      //   return;
      // }
      break;

    case WStype_BIN:
      // This event occurs when:
      // - Server sends binary data
      // - Useful for efficient data transfer
      // - Less common in simple applications
      Serial.println("[WebSocket] Received binary data");
      // Example: webSocket.sendBIN(payload, length);
      break;

    case WStype_ERROR:
      // This event occurs when:
      // - Connection errors
      // - Data transmission errors
      // - Protocol errors
      Serial.println("[WebSocket] Error occurred!");
      break;
  }
}

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    // Wait for serial port to connect
    // Note: Only needed for native USB port - the Nano has this
    ;
  }

  // Connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    status = WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait 10 seconds between connection attempts
    delay(10000);
  }

  Serial.println("Connected to WiFi");

  // Initialize WebSocket connection
  webSocket.begin(HOST_ADDRESS, PORT);
  webSocket.onEvent(webSocketEvent);     // Register event handler
  webSocket.setReconnectInterval(5000);  // Attempt reconnection every 5 seconds
}

void loop() {
  // Keep the WebSocket connection alive and handle incoming messages
  webSocket.loop();
}