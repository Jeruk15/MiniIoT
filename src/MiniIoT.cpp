#include "MiniIoT.h"

// Global instance pointer
MiniIoT* _miniIoTInstance = nullptr;

MiniIoT::MiniIoT(const char* deviceEui, Client& client) : _mqttClient(client) {
  _client = &client;
  _deviceEui = String(deviceEui);
  _mqttPort = 1883;
  _sendInterval = 5000;
  _heartbeatInterval = 60000;
  _lastSend = 0;
  _lastReconnectAttempt = 0;
  _lastHeartbeat = 0;
  _autoSend = true;
  _wasConnected = false;
  _debug = false;
  _currentReadPin = -1;
  
  _connectedHandler = nullptr;
  _disconnectedHandler = nullptr;
  
  // Initialize virtual pins
  for (int i = 0; i < MINIIOT_MAX_VPINS; i++) {
    _virtualPins[i].number = i;
    _virtualPins[i].value = 0;
    _virtualPins[i].name = "V" + String(i);
    _virtualPins[i].changed = false;
    _virtualPins[i].lastUpdate = 0;
    _vpinReadHandlers[i] = nullptr;
    _vpinWriteHandlers[i] = nullptr;
  }
  
  // Setup topics
  _dataTopic = "device/" + _deviceEui + "/data";
  _commandTopic = "device/" + _deviceEui + "/command";
  _statusTopic = "device/" + _deviceEui + "/status";
}

void MiniIoT::begin(const char* server, int port) {
  _mqttServer = String(server);
  _mqttPort = port;
  
  _mqttClient.setServer(server, port);
  _mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
    this->handleCommand(String(topic), payload, length);
  });
  
  debugPrint("🔧 MQTT configured: " + _mqttServer + ":" + String(_mqttPort));
}

void MiniIoT::setAuth(const char* user, const char* password) {
  _mqttUser = String(user);
  _mqttPassword = String(password);
  debugPrint("🔐 MQTT authentication set");
}

void MiniIoT::setAutoSend(bool enable, unsigned long interval) {
  _autoSend = enable;
  _sendInterval = interval;
  debugPrint("⏱️  Auto-send: " + String(enable ? "ON" : "OFF") + " (interval: " + String(interval) + "ms)");
}

void MiniIoT::setHeartbeat(unsigned long interval) {
  _heartbeatInterval = interval;
  debugPrint("💓 Heartbeat interval: " + String(interval) + "ms");
}

void MiniIoT::virtualWrite(int pin, float value) {
  if (pin >= 0 && pin < MINIIOT_MAX_VPINS) {
    _virtualPins[pin].value = value;
    _virtualPins[pin].changed = true;
    _virtualPins[pin].lastUpdate = millis();
    debugPrint("📝 " + _virtualPins[pin].name + " = " + String(value));
  }
}

void MiniIoT::virtualWrite(int pin, int value) {
  virtualWrite(pin, (float)value);
}

void MiniIoT::virtualWrite(int pin, String value) {
  virtualWrite(pin, value.toFloat());
}

void MiniIoT::virtualWrite(int pin, float v1, float v2) {
  if (pin >= 0 && pin < MINIIOT_MAX_VPINS) {
    _virtualPins[pin].value = v1;  // Store first value
    _virtualPins[pin].changed = true;
    _virtualPins[pin].lastUpdate = millis();
    debugPrint("📝 " + _virtualPins[pin].name + " = [" + String(v1) + ", " + String(v2) + "]");
  }
}

void MiniIoT::virtualWrite(int pin, float v1, float v2, float v3) {
  if (pin >= 0 && pin < MINIIOT_MAX_VPINS) {
    _virtualPins[pin].value = v1;  // Store first value
    _virtualPins[pin].changed = true;
    _virtualPins[pin].lastUpdate = millis();
    debugPrint("📝 " + _virtualPins[pin].name + " = [" + String(v1) + ", " + String(v2) + ", " + String(v3) + "]");
  }
}

float MiniIoT::virtualRead(int pin) {
  if (pin >= 0 && pin < MINIIOT_MAX_VPINS) {
    _currentReadPin = pin;
    // Trigger read handler if exists
    if (_vpinReadHandlers[pin] != nullptr) {
      _vpinReadHandlers[pin]();
    }
    return _virtualPins[pin].value;
  }
  return 0;
}

void MiniIoT::setPinName(int pin, String name) {
  if (pin >= 0 && pin < MINIIOT_MAX_VPINS) {
    _virtualPins[pin].name = name;
  }
}

String MiniIoT::getPinName(int pin) {
  if (pin >= 0 && pin < MINIIOT_MAX_VPINS) {
    return _virtualPins[pin].name;
  }
  return "";
}

void MiniIoT::onConnected(ConnectedHandler handler) {
  _connectedHandler = handler;
}

void MiniIoT::onDisconnected(DisconnectedHandler handler) {
  _disconnectedHandler = handler;
}

void MiniIoT::reconnectMQTT() {
  // Don't attempt reconnect too frequently
  unsigned long now = millis();
  if (now - _lastReconnectAttempt < 5000) {
    return;
  }
  _lastReconnectAttempt = now;
  
  if (_mqttClient.connected()) {
    return;
  }
  
  debugPrint("🔄 Connecting to MQTT broker...");
  
  String clientId = "MiniIoT-" + _deviceEui;
  bool connected = false;
  
  // Connect with or without authentication
  if (_mqttUser.length() > 0) {
    connected = _mqttClient.connect(clientId.c_str(), _mqttUser.c_str(), _mqttPassword.c_str());
  } else {
    connected = _mqttClient.connect(clientId.c_str());
  }
  
  if (connected) {
    debugPrint("✅ MQTT Connected!");
    
    // Subscribe to command topic
    _mqttClient.subscribe(_commandTopic.c_str());
    debugPrint("📥 Subscribed to: " + _commandTopic);
    
    // Publish status
    _mqttClient.publish(_statusTopic.c_str(), "{\"status\":\"online\"}");
    
    // Trigger connected callback
    if (!_wasConnected && _connectedHandler != nullptr) {
      _connectedHandler();
    }
    _wasConnected = true;
  } else {
    debugPrint("❌ MQTT failed, rc=" + String(_mqttClient.state()));
    
    // Trigger disconnected callback
    if (_wasConnected && _disconnectedHandler != nullptr) {
      _disconnectedHandler();
    }
    _wasConnected = false;
  }
}

void MiniIoT::publishData() {
  StaticJsonDocument<512> doc;
  
  // Add device info
  doc["deviceEui"] = _deviceEui;
  doc["timestamp"] = millis();
  
  // Add virtual pins with changes
  JsonObject pins = doc.createNestedObject("pins");
  
  bool hasData = false;
  for (int i = 0; i < MINIIOT_MAX_VPINS; i++) {
    if (_virtualPins[i].changed || !_autoSend) {
      pins[_virtualPins[i].name] = _virtualPins[i].value;
      _virtualPins[i].changed = false;
      hasData = true;
    }
  }
  
  // Only send if there's data
  if (!hasData) {
    return;
  }
  
  // Serialize and publish
  String output;
  serializeJson(doc, output);
  
  if (_mqttClient.publish(_dataTopic.c_str(), output.c_str())) {
    debugPrint("📤 Data sent: " + output);
  } else {
    debugPrint("❌ Publish failed!");
  }
}

void MiniIoT::handleCommand(String topic, byte* payload, unsigned int length) {
  // Parse JSON command
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  
  if (error) {
    debugPrint("❌ JSON parse error: " + String(error.c_str()));
    return;
  }
  
  debugPrint("📥 Command received: " + topic);
  
  // Handle virtual pin writes from server
  if (doc.containsKey("pin")) {
    int pin = doc["pin"];
    
    // Clear current param
    _currentParam = MiniIoTParam();
    
    // Add values to param
    if (doc.containsKey("value")) {
      if (doc["value"].is<JsonArray>()) {
        JsonArray arr = doc["value"];
        for (JsonVariant v : arr) {
          _currentParam.add(v.as<float>());
        }
      } else {
        _currentParam.add(doc["value"].as<float>());
      }
    }
    
    // Set pin value
    float value = _currentParam.asFloat();
    virtualWrite(pin, value);
    
    // Trigger write handler
    _currentReadPin = pin;
    if (_vpinWriteHandlers[pin] != nullptr) {
      _vpinWriteHandlers[pin]();
    }
  }
  
  // Handle sync request
  if (doc.containsKey("sync")) {
    if (doc["sync"] == "all") {
      syncAll();
    } else {
      int pin = doc["sync"];
      syncVirtual(pin);
    }
  }
}

void MiniIoT::sendHeartbeat() {
  if (!_mqttClient.connected()) return;
  
  StaticJsonDocument<128> doc;
  doc["deviceEui"] = _deviceEui;
  doc["uptime"] = millis();
  doc["freeHeap"] = ESP.getFreeHeap();
  
  String output;
  serializeJson(doc, output);
  _mqttClient.publish(_statusTopic.c_str(), output.c_str());
  
  debugPrint("💓 Heartbeat sent");
}

void MiniIoT::sendData() {
  if (_mqttClient.connected()) {
    publishData();
  }
}

void MiniIoT::sendPins(int pins[], int count) {
  StaticJsonDocument<512> doc;
  doc["deviceEui"] = _deviceEui;
  
  JsonObject pinData = doc.createNestedObject("pins");
  
  for (int i = 0; i < count; i++) {
    int pin = pins[i];
    if (pin >= 0 && pin < MINIIOT_MAX_VPINS) {
      pinData[_virtualPins[pin].name] = _virtualPins[pin].value;
    }
  }
  
  String output;
  serializeJson(doc, output);
  _mqttClient.publish(_dataTopic.c_str(), output.c_str());
}

void MiniIoT::flush() {
  sendData();
}

void MiniIoT::syncAll() {
  for (int i = 0; i < MINIIOT_MAX_VPINS; i++) {
    _virtualPins[i].changed = true;
  }
  sendData();
}

void MiniIoT::syncVirtual(int pin) {
  if (pin >= 0 && pin < MINIIOT_MAX_VPINS) {
    _virtualPins[pin].changed = true;
    sendData();
  }
}

void MiniIoT::run() {
  // Reconnect if needed
  if (!_mqttClient.connected()) {
    reconnectMQTT();
  }
  
  // Process MQTT
  _mqttClient.loop();
  
  // Auto-send data
  if (_autoSend && _mqttClient.connected() && millis() - _lastSend > _sendInterval) {
    sendData();
    _lastSend = millis();
  }
  
  // Send heartbeat
  if (_heartbeatInterval > 0 && _mqttClient.connected() && millis() - _lastHeartbeat > _heartbeatInterval) {
    sendHeartbeat();
    _lastHeartbeat = millis();
  }
}

bool MiniIoT::connected() {
  return _mqttClient.connected();
}

String MiniIoT::getDeviceEui() {
  return _deviceEui;
}

void MiniIoT::enableDebug(bool enable) {
  _debug = enable;
}

void MiniIoT::logEvent(String event) {
  debugPrint("📋 " + event);
}

void MiniIoT::debugPrint(String message) {
  if (_debug) {
    Serial.println(message);
  }
}