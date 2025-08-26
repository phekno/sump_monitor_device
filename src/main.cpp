// src/main.cpp
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <NewPing.h>
#include "config.h"
#include "sensor_manager.h"
#include "network_manager.h"
#include "pump_monitor.h"

// Function declarations
void printSensorData();
void sendDataToAPI();
String getDeviceId();

// Global objects
NetworkManager networkManager;
SensorManager sensorManager;
PumpMonitor pumpMonitor;

// Timing variables
unsigned long lastDataSend = 0;
unsigned long lastSensorRead = 0;
const unsigned long dataInterval = 5000; // Send data every 5 seconds
const unsigned long sensorInterval = 1000; // Read sensors every 1 second

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Sump Pump Monitor Starting...");
  
  // Initialize components
  sensorManager.init();
  pumpMonitor.init();
  networkManager.init();
  
  Serial.println("ESP32 Sump Pump Monitor Initialized");
  Serial.printf("Device ID: %s\n", getDeviceId().c_str());
  Serial.printf("API Endpoint: %s\n", API_ENDPOINT);
  Serial.println("Time(s)\tFlow(GPM)\tCurrent(A)\tDistance(cm)\tBasket(%)");
  Serial.println("---------------------------------------------------------------");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Handle network management
  networkManager.handle();
  
  // Handle pump state changes (interrupt-driven)
  pumpMonitor.handle();
  
  // Read sensors periodically
  if (currentTime - lastSensorRead >= sensorInterval) {
    sensorManager.readAll();
    pumpMonitor.updateEvent(sensorManager.getData());
    printSensorData();
    lastSensorRead = currentTime;
  }
  
  // Send data to API periodically
  if (networkManager.isConnected() && (currentTime - lastDataSend >= dataInterval)) {
    sendDataToAPI();
    lastDataSend = currentTime;
  }
  
  delay(10);
}

void sendDataToAPI() {
  if (!networkManager.isConnected()) return;
  
  HTTPClient http;
  http.begin(API_ENDPOINT);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000); // 5 second timeout
  
  // Create JSON payload
  DynamicJsonDocument doc(1024);
  SensorData data = sensorManager.getData();
  PumpEventData eventData = pumpMonitor.getCurrentEvent();
  
  doc["device_id"] = getDeviceId();
  doc["timestamp"] = millis();
  doc["current"] = data.current;
  doc["flow_rate"] = data.flowRate;
  doc["total_volume"] = data.totalVolume;
  doc["distance"] = data.distance;
  doc["basket_fullness"] = data.basketFullness;
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["free_heap"] = ESP.getFreeHeap();
  doc["uptime"] = millis() / 1000;
  
  // Add current pump event data if active
  if (eventData.isActive) {
    doc["current_event_active"] = true;
    doc["current_event_volume"] = eventData.volumePumped;
    doc["current_event_duration"] = eventData.duration;
  } else {
    doc["current_event_active"] = false;
  }
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.printf("API Response: %d\n", httpResponseCode);
    networkManager.resetFailureCount();
  } else {
    Serial.printf("API Error: %d\n", httpResponseCode);
    networkManager.incrementFailureCount();
  }
  
  http.end();
}

void printSensorData() {
  SensorData data = sensorManager.getData();
  float timeSeconds = millis() / 1000.0;
  
  Serial.printf("%.1f\t%.2f\t\t%.2f\t\t%.1f\t\t%.1f%%\n",
    timeSeconds, data.flowRate, data.current, data.distance, data.basketFullness);
}

String getDeviceId() {
  String macAddress = WiFi.macAddress();
  macAddress.replace(":", "");
  macAddress.toLowerCase();
  return "sump_pump_" + macAddress;
}