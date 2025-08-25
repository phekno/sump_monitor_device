#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include "config.h"

class NetworkManager {
private:
  WiFiManager wifiManager;
  bool wifiConnected = false;
  int failedRequests = 0;
  unsigned long lastConnectionAttempt = 0;
  
public:
  NetworkManager();
  
  void init();
  void handle();
  bool connectToWiFi();
  void disconnect();
  
  bool isConnected() const { return wifiConnected && (WiFi.status() == WL_CONNECTED); }
  int getSignalStrength() const { return WiFi.RSSI(); }
  String getLocalIP() const { return WiFi.localIP().toString(); }
  
  void incrementFailureCount() { failedRequests++; }
  void resetFailureCount() { failedRequests = 0; }
  int getFailureCount() const { return failedRequests; }
};

#endif // NETWORK_MANAGER_H