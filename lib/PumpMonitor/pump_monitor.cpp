#include "pump_monitor.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Static member initialization
PumpMonitor* PumpMonitor::instance = nullptr;

PumpMonitor::PumpMonitor() {
  instance = this;
}

PumpMonitor::~PumpMonitor() {
  if (currentTimer != nullptr) {
    timerEnd(currentTimer);
  }
}

void PumpMonitor::init() {
  // Initialize pump event data
  currentEvent.reset();
  lastCompletedEvent.reset();
  
  // Setup high-frequency current monitoring
  setupCurrentMonitoringTimer();
  
  Serial.println("PumpMonitor initialized with interrupt-driven current detection");
}

void PumpMonitor::setupCurrentMonitoringTimer() {
  // Set up timer for 10kHz current monitoring
  currentTimer = timerBegin(0, 80, true);  // Timer 0, prescaler 80 (1MHz)
  timerAttachInterrupt(currentTimer, &currentMonitorISR, true);
  timerAlarmWrite(currentTimer, 100, true);  // 100 ticks = 10kHz
  timerAlarmEnable(currentTimer);
}

void PumpMonitor::handle() {
  // Handle pump state changes triggered by interrupt
  if (pumpStateChanged) {
    portENTER_CRITICAL(&currentMux);
    bool thresholdState = currentAboveThreshold;
    pumpStateChanged = false;
    portEXIT_CRITICAL(&currentMux);
    
    if (thresholdState && !currentEvent.isActive) {
      // Pump just started
      Serial.println("Pump event started (interrupt-triggered)");
      // Note: We'll start the event in updateEvent when we have sensor data
    } else if (!thresholdState && currentEvent.isActive) {
      // Pump just stopped
      Serial.println("Pump event ended (interrupt-triggered)");
      endPumpEvent();
    }
  }
}

void PumpMonitor::updateEvent(const SensorData& sensorData) {
  // Update pump state based on current reading
  previousState = currentState;
  currentState = determinePumpState(sensorData.current);
  
  // Start event if pump just started and we don't have an active event
  if (currentState == PumpState::RUNNING && !currentEvent.isActive && currentAboveThreshold) {
    startPumpEvent(sensorData.totalVolume);
  }
  
  // Update ongoing pump event
  if (currentEvent.isActive) {
    // Update peak current
    if (sensorData.current > currentEvent.peakCurrent) {
      currentEvent.peakCurrent = sensorData.current;
    }
    
    // Accumulate for average current calculation
    currentSum += sensorData.current;
    currentReadings++;
    
    // Update volume pumped and duration
    currentEvent.volumePumped = sensorData.totalVolume - eventStartVolume;
    currentEvent.duration = currentEvent.getDuration();
  }
}

void PumpMonitor::startPumpEvent(float currentVolume) {
  currentEvent.startTime = millis();
  currentEvent.isActive = true;
  currentEvent.peakCurrent = 0.0; // Will be updated in updateEvent
  currentEvent.volumePumped = 0.0;
  eventStartVolume = currentVolume;
  currentSum = 0.0;
  currentReadings = 0;
  
  Serial.printf("Pump event started at %lu ms\n", currentEvent.startTime);
  
  // Send pump start event to API (if connected)
  // This could be moved to main.cpp if network access is needed
}

void PumpMonitor::endPumpEvent() {
  if (!currentEvent.isActive) return;
  
  currentEvent.endTime = millis();
  currentEvent.isActive = false;
  
  // Calculate final statistics
  if (currentReadings > 0) {
    currentEvent.avgCurrent = currentSum / currentReadings;
  }
  currentEvent.duration = (currentEvent.endTime - currentEvent.startTime) / 1000.0;
  
  // Copy to last completed event
  lastCompletedEvent = currentEvent;
  
  Serial.printf("Pump event completed:\n");
  Serial.printf("  Duration: %.1f seconds\n", currentEvent.duration);
  Serial.printf("  Volume: %.3f gallons\n", currentEvent.volumePumped);
  Serial.printf("  Peak Current: %.2f A\n", currentEvent.peakCurrent);
  Serial.printf("  Avg Current: %.2f A\n", currentEvent.avgCurrent);
  
  // Reset current event
  currentEvent.reset();
}

PumpState PumpMonitor::determinePumpState(float current) {
  if (current > 1.0) {
    return PumpState::RUNNING;
  } else if (current > 0.1) {
    return PumpState::STANDBY;
  } else {
    return PumpState::OFF;
  }
}

void IRAM_ATTR PumpMonitor::currentMonitorISR() {
  if (instance == nullptr) return;
  
  portENTER_CRITICAL_ISR(&instance->currentMux);
  
  // Fast ADC read
  int adcValue = analogRead(CURRENT_SENSOR_PIN);
  
  // Check if current crossed threshold
  bool currentHigh = (adcValue > CURRENT_THRESHOLD_ADC);
  
  if (currentHigh != instance->currentAboveThreshold) {
    instance->currentAboveThreshold = currentHigh;
    instance->pumpStateChanged = true;
  }
  
  portEXIT_CRITICAL_ISR(&instance->currentMux);
}