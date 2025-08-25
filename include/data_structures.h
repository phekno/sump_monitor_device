// include/data_structures.h
#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <Arduino.h>

struct SensorData {
  float current = 0.0;
  float flowRate = 0.0;
  float totalVolume = 0.0;
  float distance = 0.0;
  float basketFullness = 0.0;
  unsigned long timestamp = 0;
  
  // Constructor
  SensorData() : current(0.0), flowRate(0.0), totalVolume(0.0), 
                 distance(0.0), basketFullness(0.0), timestamp(0) {}
};

struct PumpEventData {
  unsigned long startTime = 0;
  unsigned long endTime = 0;
  float volumePumped = 0.0;
  float peakCurrent = 0.0;
  float avgCurrent = 0.0;
  float duration = 0.0;
  bool isActive = false;
  
  // Constructor
  PumpEventData() : startTime(0), endTime(0), volumePumped(0.0),
                    peakCurrent(0.0), avgCurrent(0.0), duration(0.0), isActive(false) {}
  
  // Reset the event
  void reset() {
    startTime = 0;
    endTime = 0;
    volumePumped = 0.0;
    peakCurrent = 0.0;
    avgCurrent = 0.0;
    duration = 0.0;
    isActive = false;
  }
  
  // Calculate duration in seconds
  float getDuration() const {
    if (isActive && startTime > 0) {
      return (millis() - startTime) / 1000.0;
    } else if (endTime > 0 && startTime > 0) {
      return (endTime - startTime) / 1000.0;
    }
    return 0.0;
  }
};

enum class SystemStatus {
  INITIALIZING,
  NORMAL,
  WARNING,
  ERROR,
  OFFLINE
};

enum class PumpState {
  OFF,
  STANDBY,
  RUNNING,
  ERROR
};

struct SystemHealth {
  bool wifiConnected = false;
  int wifiSignalStrength = 0;
  unsigned long freeHeap = 0;
  unsigned long uptime = 0;
  int failedApiCalls = 0;
  SystemStatus status = SystemStatus::INITIALIZING;
  
  // Constructor
  SystemHealth() : wifiConnected(false), wifiSignalStrength(0), freeHeap(0),
                   uptime(0), failedApiCalls(0), status(SystemStatus::INITIALIZING) {}
};

#endif // DATA_STRUCTURES_H