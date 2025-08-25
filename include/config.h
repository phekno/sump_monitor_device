// include/config.h
#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define WIFI_TIMEOUT 30000  // 30 seconds
#define WIFI_RETRY_INTERVAL 30000  // Try reconnecting every 30 seconds

// API Configuration
#define API_ENDPOINT "https://your-api-server.com/api/sensor-data"
#define DEVICE_ID "sump-pump-01"
#define MAX_FAILED_REQUESTS 5

// Pin Definitions
#define FLOW_SENSOR_PIN 2
#define CURRENT_SENSOR_PIN 36
#define TRIGGER_PIN 4
#define ECHO_PIN 5

// Sensor Configuration
#define MAX_DISTANCE 200
#define CURRENT_THRESHOLD_ADC 150
#define CALIBRATION_FACTOR 4.5
#define CURRENT_SENSITIVITY 0.066
#define VCC_VOLTAGE 3.3
#define ADC_RESOLUTION 4096
#define BASKET_DEPTH 30.0
#define SENSOR_OFFSET 5.0

// Timing Configuration
#define SENSOR_READ_INTERVAL 1000
#define DATA_SEND_INTERVAL 5000
#define CURRENT_MONITOR_FREQUENCY 10000  // 10kHz

// Debug Configuration
#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(x, ...) Serial.printf(x, __VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(x, ...)
#endif

#endif // CONFIG_H