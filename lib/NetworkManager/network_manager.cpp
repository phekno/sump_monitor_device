#include "network_manager.h"

NetworkManager::NetworkManager() : wifiConnected(false), failedRequests(0), lastConnectionAttempt(0) {
}

void NetworkManager::init() {
  WiFi.mode(WIFI_STA);
  connectToWiFi();
}

void NetworkManager::handle() {
  unsigned long currentTime = millis();
  
  // Check WiFi connection status
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    
    // Attempt reconnection if enough time has passed
    if (currentTime - lastConnectionAttempt >= WIFI_RETRY_INTERVAL) {
      Serial.println("WiFi connection lost, attempting reconnection...");
      connectToWiFi();
      lastConnectionAttempt = currentTime;
    }
  } else {
    wifiConnected = true;
  }
  
  // If too many API failures, force WiFi reconnection
  if (failedRequests >= MAX_FAILED_REQUESTS) {
    Serial.printf("Too many failed requests (%d), forcing WiFi reconnection\n", failedRequests);
    disconnect();
    connectToWiFi();
    failedRequests = 0;
  }
}

bool NetworkManager::connectToWiFi() {
  Serial.println("Starting WiFi connection...");
  
  // Set WiFiManager timeout
  wifiManager.setTimeout(WIFI_TIMEOUT / 1000);
  
  // Try to connect using saved credentials, or start captive portal
  if (wifiManager.autoConnect("SumpMonitor-Setup")) {
    Serial.println("WiFi connected successfully!");
    Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Signal strength: %d dBm\n", WiFi.RSSI());
    
    wifiConnected = true;
    failedRequests = 0;
    return true;
  } else {
    Serial.println("WiFi connection failed!");
    wifiConnected = false;
    return false;
  }
}

void NetworkManager::disconnect() {
  WiFi.disconnect();
  wifiConnected = false;
  Serial.println("WiFi disconnected");
}