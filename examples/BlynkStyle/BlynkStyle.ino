/*************************************************************
  MiniIoT - Blynk Style Example
  
  This example shows how to use MiniIoT library in Blynk-style
  with macros and callbacks.
  
  Hardware: ESP32
  
  Author: Mini IoT Team
  License: MIT
 *************************************************************/

#include <WiFi.h>
#include <MiniIoT.h>

// ========== WiFi Configuration ==========
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ========== MQTT Configuration ==========
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* device_eui = "esp32-demo-001";

// ========== Create Instances ==========
WiFiClient wifiClient;
MiniIoT iot(device_eui, wifiClient);

// ========== Virtual Pin Handlers (Blynk-style) ==========

// This function is called when server writes to V0
MINIIOT_WRITE(V0) {
  // Get parameter values
  MiniIoTParam param = iot.getParam();
  int value = param.asInt();
  
  Serial.println("📥 V0 received: " + String(value));
  
  // Control LED based on value
  digitalWrite(LED_BUILTIN, value ? HIGH : LOW);
}

// This function is called when server writes to V1
MINIIOT_WRITE(V1) {
  MiniIoTParam param = iot.getParam();
  int brightness = param.asInt();
  
  Serial.println("📥 V1 brightness: " + String(brightness));
  
  // Control PWM LED
  // analogWrite(LED_PIN, brightness);
}

// This function is called when server reads from V2
MINIIOT_READ(V2) {
  // Read sensor and send to server
  float temperature = random(200, 300) / 10.0;  // Simulated
  iot.virtualWrite(2, temperature);
  
  Serial.println("📤 V2 temperature sent: " + String(temperature));
}

// Called when successfully connected to MQTT
MINIIOT_CONNECTED() {
  Serial.println("🎉 Connected to MiniIoT Platform!");
  Serial.println("✅ Device is online and ready");
  
  // Sync all virtual pins
  iot.syncAll();
}

// Called when disconnected from MQTT
MINIIOT_DISCONNECTED() {
  Serial.println("⚠️  Disconnected from MiniIoT Platform");
  Serial.println("🔄 Will attempt to reconnect...");
}

// ========== Setup ==========
void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   MiniIoT Platform - Blynk Style      ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  Serial.println("Device EUI: " + String(device_eui));
  Serial.println();
  
  // Setup LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Enable debug mode
  iot.enableDebug(true);
  
  // Connect to WiFi
  Serial.print("🔌 Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("✅ WiFi Connected!");
  Serial.println("📡 IP Address: " + WiFi.localIP().toString());
  Serial.println("📶 Signal: " + String(WiFi.RSSI()) + " dBm");
  Serial.println();
  
  // Configure MQTT
  iot.begin(mqtt_server, mqtt_port);
  
  // Optional: Set authentication
  // iot.setAuth("username", "password");
  
  // Set pin names for better readability
  iot.setPinName(0, "led_control");
  iot.setPinName(1, "brightness");
  iot.setPinName(2, "temperature");
  iot.setPinName(3, "humidity");
  iot.setPinName(4, "pressure");
  
  // Set connection callbacks
  iot.onConnected(miniIotConnected);
  iot.onDisconnected(miniIotDisconnected);
  
  // Auto-send changed pins every 5 seconds
  iot.setAutoSend(true, 5000);
  
  // Send heartbeat every 60 seconds
  iot.setHeartbeat(60000);
  
  // Attach global instance for macros
  MINIIOT_ATTACH(iot);
  
  Serial.println("✅ Setup complete!");
  Serial.println("🚀 Starting main loop...");
  Serial.println();
}

// ========== Loop ==========
void loop() {
  // Keep MiniIoT connection alive
  iot.run();
  
  // Simulate sensor readings
  static unsigned long lastRead = 0;
  if (millis() - lastRead > 10000) {  // Every 10 seconds
    
    // Read sensors (simulated)
    float temperature = random(200, 300) / 10.0;  // 20-30°C
    float humidity = random(400, 800) / 10.0;     // 40-80%
    float pressure = random(9800, 10200) / 10.0;  // 980-1020 hPa
    
    // Write to virtual pins
    iot.virtualWrite(2, temperature);
    iot.virtualWrite(3, humidity);
    iot.virtualWrite(4, pressure);
    
    Serial.println("📊 Sensor data updated:");
    Serial.println("   🌡️  Temperature: " + String(temperature) + "°C");
    Serial.println("   💧 Humidity: " + String(humidity) + "%");
    Serial.println("   🔘 Pressure: " + String(pressure) + " hPa");
    Serial.println();
    
    lastRead = millis();
  }
  
  // Check connection status
  static bool wasConnected = false;
  bool isConnected = iot.connected();
  
  if (isConnected != wasConnected) {
    if (isConnected) {
      Serial.println("🟢 Status: ONLINE");
    } else {
      Serial.println("🔴 Status: OFFLINE");
    }
    wasConnected = isConnected;
  }
  
  delay(100);
}
