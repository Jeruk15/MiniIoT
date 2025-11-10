#ifndef MINIIOT_H
#define MINIIOT_H

/*
 * MiniIoT Library for ESP32
 * Version: 1.0.0
 * Author: Mini IoT Team
 * License: MIT
 * 
 * Blynk-style IoT library with virtual pins and MQTT backend
 */

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Client.h>

// Library version
#define MINIIOT_VERSION "1.0.0"

// Maximum virtual pins
#define MINIIOT_MAX_VPINS 32

// Connection timeout
#define MINIIOT_TIMEOUT_MS 5000

// Pin types
enum PinType {
  VIRTUAL_PIN,
  DIGITAL_PIN,
  ANALOG_PIN
};

// Virtual Pin data structure
struct VirtualPin {
  int number;
  float value;
  String name;
  bool changed;
  unsigned long lastUpdate;
};

// Callback function types
typedef void (*VPinReadHandler)();
typedef void (*VPinWriteHandler)();
typedef void (*ConnectedHandler)();
typedef void (*DisconnectedHandler)();

// Blynk-style virtual pin parameter class
class MiniIoTParam {
private:
  float _values[8];
  int _count;
  
public:
  MiniIoTParam() : _count(0) {}
  
  void add(float value) {
    if (_count < 8) {
      _values[_count++] = value;
    }
  }
  
  void add(int value) { add((float)value); }
  void add(String value) { add(value.toFloat()); }
  
  float asFloat() const { return _count > 0 ? _values[0] : 0; }
  int asInt() const { return (int)asFloat(); }
  String asStr() const { return String(asFloat()); }
  
  float operator[](int index) const {
    return (index >= 0 && index < _count) ? _values[index] : 0;
  }
  
  int getLength() const { return _count; }
};

class MiniIoT {
private:
  Client* _client;
  PubSubClient _mqttClient;
  
  String _deviceEui;
  String _mqttServer;
  String _mqttUser;
  String _mqttPassword;
  int _mqttPort;
  
  VirtualPin _virtualPins[MINIIOT_MAX_VPINS];
  VPinReadHandler _vpinReadHandlers[MINIIOT_MAX_VPINS];
  VPinWriteHandler _vpinWriteHandlers[MINIIOT_MAX_VPINS];
  
  ConnectedHandler _connectedHandler;
  DisconnectedHandler _disconnectedHandler;
  
  String _dataTopic;
  String _commandTopic;
  String _statusTopic;
  
  unsigned long _lastSend;
  unsigned long _sendInterval;
  unsigned long _lastReconnectAttempt;
  unsigned long _heartbeatInterval;
  unsigned long _lastHeartbeat;
  
  bool _autoSend;
  bool _wasConnected;
  bool _debug;
  
  int _currentReadPin;
  MiniIoTParam _currentParam;
  
  void reconnectMQTT();
  void publishData();
  void handleCommand(String topic, byte* payload, unsigned int length);
  void debugPrint(String message);
  void sendHeartbeat();
  
public:
  MiniIoT(const char* deviceEui, Client& client);
  
  // MQTT Setup
  void begin(const char* server, int port = 1883);
  void setAuth(const char* user, const char* password);
  void setAutoSend(bool enable, unsigned long interval = 5000);
  void setHeartbeat(unsigned long interval = 60000);
  
  // Virtual Pin operations (Blynk-style)
  void virtualWrite(int pin, float value);
  void virtualWrite(int pin, int value);
  void virtualWrite(int pin, String value);
  void virtualWrite(int pin, float v1, float v2);
  void virtualWrite(int pin, float v1, float v2, float v3);
  float virtualRead(int pin);
  
  // Pin naming
  void setPinName(int pin, String name);
  String getPinName(int pin);
  
  // Event handlers (Blynk-style)
  void onConnected(ConnectedHandler handler);
  void onDisconnected(DisconnectedHandler handler);
  
  // Send data manually
  void sendData();
  void sendPins(int pins[], int count);
  void flush();  // Send all changed pins immediately
  
  // Sync operations
  void syncAll();
  void syncVirtual(int pin);
  
  // Connection management
  void run();
  bool connected();
  String getDeviceEui();
  
  // Debug
  void enableDebug(bool enable = true);
  void logEvent(String event);
  
  // Direct pin access (internal use)
  int getCurrentPin() { return _currentReadPin; }
  MiniIoTParam& getParam() { return _currentParam; }
  
  // Version
  static const char* version() { return MINIIOT_VERSION; }
};

// Global instance pointer for macros
extern MiniIoT* _miniIoTInstance;

// Blynk-style macros
#define MINIIOT_WRITE(pin) void miniIotWrite##pin()
#define MINIIOT_READ(pin) void miniIotRead##pin()
#define MINIIOT_CONNECTED() void miniIotConnected()
#define MINIIOT_DISCONNECTED() void miniIotDisconnected()

// Helper macro to attach handlers
#define MINIIOT_ATTACH(iot) _miniIoTInstance = &iot

#endif