#ifndef PUMP_MONITOR_H
#define PUMP_MONITOR_H

#include <Arduino.h>
#include "data_structures.h"
#include "config.h"

class PumpMonitor {
private:
  PumpEventData currentEvent;
  PumpEventData lastCompletedEvent;
  
  volatile bool pumpStateChanged = false;
  volatile bool currentAboveThreshold = false;
  PumpState currentState = PumpState::OFF;
  PumpState previousState = PumpState::OFF;
  
  float eventStartVolume = 0.0;
  float currentSum = 0.0;
  int currentReadings = 0;
  
  // Timer for current monitoring
  hw_timer_t* currentTimer = nullptr;
  portMUX_TYPE currentMux = portMUX_INITIALIZER_UNLOCKED;
  
  // Static instance for interrupt access
  static PumpMonitor* instance;
  static void IRAM_ATTR currentMonitorISR();
  
  void setupCurrentMonitoringTimer();
  void startPumpEvent(float currentVolume);
  void endPumpEvent();
  PumpState determinePumpState(float current);
  
public:
  PumpMonitor();
  ~PumpMonitor();
  
  void init();
  void handle();
  void updateEvent(const SensorData& sensorData);
  
  PumpEventData getCurrentEvent() const { return currentEvent; }
  PumpEventData getLastCompletedEvent() const { return lastCompletedEvent; }
  PumpState getPumpState() const { return currentState; }
  bool isPumpRunning() const { return currentState == PumpState::RUNNING; }
  bool hasActiveEvent() const { return currentEvent.isActive; }
};

#endif // PUMP_MONITOR_H
