# 🚀 MiniIoT Library for ESP32

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino Library](https://img.shields.io/badge/Arduino-Library-blue.svg)](https://www.arduino.cc/reference/en/libraries/)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-green.svg)](https://www.espressif.com/en/products/socs/esp32)

IoT library for ESP32 with **virtual pins** (Blynk-style) for easy MQTT integration.

Perfect for beginners and professionals who want to quickly build IoT projects with real-time data visualization.

---

## ✨ Features

- 🎯 **Virtual Pins (V0-V19)** - Blynk-style pin management
- 🔌 **Auto WiFi & MQTT Connection** - Handles reconnection automatically
- 📤 **Auto-Send Data** - Configurable interval
- 🎣 **Event Callbacks** - onRead/onWrite handlers
- 🐛 **Debug Logging** - Built-in serial debugging
- 🌐 **Multi-Device Support** - Multiple ESP32 on same broker
- 🔄 **Pin Sync** - Sync individual or all pins
- 📝 **Custom Pin Names** - Readable JSON output

---

## 📦 Installation

### Method 1: Arduino Library Manager (Recommended) ⭐

**This is the easiest way! Dependencies auto-install.**

1. Open **Arduino IDE**
2. Go to **Sketch** → **Include Library** → **Manage Libraries...**
3. Search for **"MiniIoT"**
4. Click **Install**
5. ✅ Done! (PubSubClient and ArduinoJson will auto-install)

---

### Method 2: GitHub ZIP Download

**For users who download from GitHub releases:**

#### Step 1: Download Library
- Go to [Releases](https://github.com/Jeruk15/MiniIoT/releases)
- Download `MiniIoT.zip`

#### Step 2: Install Library
- Arduino IDE → **Sketch** → **Include Library** → **Add .ZIP Library...**
- Select downloaded `MiniIoT.zip`
- Click **Open**

#### Step 3: Install Dependencies

Arduino IDE will **automatically prompt** you to install dependencies:

![Dependency Prompt](https://via.placeholder.com/500x150.png?text=Install+Dependencies+Prompt)

Click **Install all** → Done! ✨

**If auto-install doesn't work:**

Manual install:
1. **Sketch** → **Include Library** → **Manage Libraries...**
2. Search **"PubSubClient"** by Nick O'Leary → Install
3. Search **"ArduinoJson"** by Benoit Blanchon → Install **version 6.x** (not 7.x)

---

### Method 3: Git Clone (For Developers)

```bash
cd ~/Documents/Arduino/libraries/
git clone https://github.com/yourusername/MiniIoT.git
```

Then install dependencies via Library Manager.

**Restart Arduino IDE after installation.**

---

## 🚀 Quick Start

### Basic Example

```cpp
#include <MiniIoT.h>

// WiFi credentials
const char* ssid = "YourWiFi";
const char* password = "YourPassword";

// MQTT broker IP
const char* mqtt_server = "192.168.1.100";

// Create MiniIoT instance with unique device ID
MiniIoT iot("esp32-room1");

void setup() {
  Serial.begin(115200);
  
  // Enable debug output
  iot.enableDebug(true);
  
  // Connect to WiFi
  iot.begin(ssid, password);
  
  // Configure MQTT broker
  iot.config(mqtt_server, 1883);
  
  // Set readable pin names (optional)
  iot.setPinName(0, "temperature");
  iot.setPinName(1, "humidity");
  iot.setPinName(2, "light");
  
  // Auto-send data every 5 seconds
  iot.setAutoSend(true, 5000);
  
  Serial.println("✅ Setup complete!");
}

void loop() {
  // Keep connection alive (handles reconnect automatically)
  iot.run();
  
  // Read sensors
  float temperature = readTemperatureSensor(); // Your sensor code
  float humidity = readHumiditySensor();       // Your sensor code
  int light = analogRead(34);
  
  // Write to virtual pins
  iot.virtualWrite(0, temperature);
  iot.virtualWrite(1, humidity);
  iot.virtualWrite(2, light);
  
  delay(100);
}
```

**Output on Serial Monitor:**
```
🔌 Connecting to WiFi: YourWiFi
......
✅ WiFi connected!
📡 IP Address: 192.168.1.105
🔧 MQTT configured: 192.168.1.100:1883
🔄 Connecting to MQTT broker...
✅ MQTT Connected!
📥 Subscribed to: device/esp32-room1/command
📤 Data sent: {"deviceEui":"esp32-room1","timestamp":5234,"pins":{"temperature":25.3,"humidity":62.1,"light":456}}
```

---

## 📚 Examples

Check **File** → **Examples** → **MiniIoT** in Arduino IDE:

### 1. **Basic** - Simple temperature & humidity
```cpp
// Send sensor data every 5 seconds
iot.virtualWrite(0, temperature);
iot.virtualWrite(1, humidity);
```

### 2. **VirtualPins** - Advanced with callbacks
```cpp
// Handle commands from server
MINIIOT_WRITE(2) {
  digitalWrite(LED_PIN, value > 0 ? HIGH : LOW);
}

// Read sensor on demand
MINIIOT_READ(0) {
  iot.virtualWrite(0, readTemperature());
}
```

### 3. **MultiSensor** - Multiple sensors
```cpp
iot.setPinName(0, "temp");
iot.setPinName(1, "humidity");
iot.setPinName(2, "pressure");
iot.setPinName(3, "light");
iot.setPinName(4, "motion");
```

### 4. **ManualSend** - Control when to send
```cpp
iot.setAutoSend(false); // Disable auto-send

// Send manually when needed
if (sensorChanged) {
  iot.sendData();
}
```

---

## 🔧 API Reference

### Initialization

```cpp
MiniIoT iot("device-id");
```

### WiFi Connection

```cpp
// Simple WiFi connection
iot.begin("SSID", "password");

// Static IP configuration
iot.begin("SSID", "password", 
  IPAddress(192,168,1,100),  // Local IP
  IPAddress(192,168,1,1),    // Gateway
  IPAddress(255,255,255,0)   // Subnet
);
```

### MQTT Configuration

```cpp
iot.config("broker.example.com", 1883);
iot.setAutoSend(true, 5000); // Auto-send every 5 seconds
```

### Virtual Pin Operations

```cpp
// Write values
iot.virtualWrite(0, 25.3);        // Float
iot.virtualWrite(1, 100);         // Integer
iot.virtualWrite(2, "Hello");     // String (converted to float)

// Read values
float temp = iot.virtualRead(0);

// Set pin names
iot.setPinName(0, "temperature");
```

### Event Handlers

```cpp
// Called when server requests pin value
iot.onVirtualRead(0, [](int pin) {
  float value = readSensor();
  iot.virtualWrite(pin, value);
});

// Called when server sends value to pin
iot.onVirtualWrite(2, [](int pin, float value) {
  digitalWrite(LED_PIN, value > 0 ? HIGH : LOW);
});
```

### Manual Send

```cpp
// Send all changed pins
iot.sendData();

// Send specific pins
int pins[] = {0, 1, 2};
iot.sendPins(pins, 3);

// Sync all pins (force send)
iot.syncAll();

// Sync single pin
iot.syncPin(0);
```

### Utilities

```cpp
// Check connection status
if (iot.isConnected()) {
  // MQTT is connected
}

// Get device ID
String deviceId = iot.getDeviceEui();

// Enable debug logging
iot.enableDebug(true);
```

---

## 📊 MQTT Topics

Library automatically creates topics based on device ID:

| Topic | Direction | Purpose |
|-------|-----------|---------|
| `device/{deviceEui}/data` | ESP32 → Server | Send sensor data |
| `device/{deviceEui}/command` | Server → ESP32 | Receive commands |
| `device/{deviceEui}/status` | ESP32 → Server | Device status |

### Data Format (JSON)

```json
{
  "deviceEui": "esp32-room1",
  "timestamp": 12345,
  "pins": {
    "temperature": 25.3,
    "humidity": 62.1,
    "light": 456
  }
}
```

---

## 🌐 Multi-Device Setup

Run multiple ESP32 devices on same broker:

```cpp
// Device 1
MiniIoT iot1("esp32-living-room");

// Device 2
MiniIoT iot2("esp32-bedroom");

// Device 3
MiniIoT iot3("esp32-kitchen");
```

Each device sends data to its own topic:
- `device/esp32-living-room/data`
- `device/esp32-bedroom/data`
- `device/esp32-kitchen/data`

---

## 🐛 Troubleshooting

### Problem: Library not found after install

**Solution:**
- Restart Arduino IDE
- Check: Sketch → Include Library → Should see "MiniIoT"

### Problem: `PubSubClient.h: No such file`

**Solution:**
- Install dependency: Sketch → Include Library → Manage Libraries
- Search "PubSubClient" → Install

### Problem: `ArduinoJson.h: No such file`

**Solution:**
- Install dependency: Sketch → Include Library → Manage Libraries
- Search "ArduinoJson" → Install **v6.x.x** (not v7)

### Problem: WiFi won't connect

**Solution:**
```cpp
iot.enableDebug(true); // Enable debug
// Check Serial Monitor for WiFi errors
```

### Problem: MQTT won't connect

**Solution:**
- Check broker IP is correct
- Ensure broker is running: `mosquitto -v`
- Check firewall allows port 1883

### Problem: Data not sending

**Solution:**
```cpp
// Check connection
if (!iot.isConnected()) {
  Serial.println("MQTT disconnected!");
}

// Force send
iot.syncAll();
```

---

## 🔗 Dependencies

This library requires:

| Library | Version | Author |
|---------|---------|--------|
| [PubSubClient](https://github.com/knolleary/pubsubclient) | ≥2.8.0 | Nick O'Leary |
| [ArduinoJson](https://github.com/bblanchon/ArduinoJson) | ≥6.19.0, <7.0.0 | Benoit Blanchon |

**Auto-installed when you install MiniIoT via Library Manager!** ✨

---

## 📖 Documentation

- [Getting Started Guide](docs/GETTING_STARTED.md)
- [API Reference](docs/API.md)
- [Examples](examples/)
- [FAQ](docs/FAQ.md)

---

## 🤝 Contributing

Contributions welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) first.

1. Fork the repo
2. Create feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open Pull Request

---

## 📝 License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file.

---

## 🌟 Support

- ⭐ Star this repo if you find it useful!
- 🐛 [Report bugs](https://github.com/yourusername/MiniIoT/issues)
- 💡 [Request features](https://github.com/yourusername/MiniIoT/issues)
- 📧 Email: support@miniiot.com

---

## 🙏 Acknowledgments

- Inspired by [Blynk](https://blynk.io/) platform
- Built on top of [PubSubClient](https://github.com/knolleary/pubsubclient)
- JSON handling by [ArduinoJson](https://arduinojson.org/)

---

## 📸 Screenshots

### Arduino IDE - Library Manager
![Library Manager](https://via.placeholder.com/800x400.png?text=Arduino+Library+Manager)

### Serial Monitor Output
![Serial Monitor](https://via.placeholder.com/800x400.png?text=Serial+Monitor+Output)

### MQTT Dashboard
![Dashboard](https://via.placeholder.com/800x400.png?text=Web+Dashboard)

---

**Made with ❤️ by Mini IoT Team**