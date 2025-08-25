#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <NewPing.h>
#include "data_structures.h"
#include "config.h"

class SensorManager {
private:
  NewPing* sonar;
  SensorData currentData;
  volatile int flowPulseCount = 0;
  unsigned long lastFlowCalculation = 0;
  
  // Flow sensor interrupt handler
  static SensorManager* instance;
  static void IRAM_ATTR flowSensorInterrupt();
  
public:
  SensorManager();
  ~SensorManager();
  
  void init();
  void readAll();
  void readCurrentSensor();
  void readFlowSensor();
  void readUltrasonicSensor();
  
  SensorData getData() const { return currentData; }
  float getCurrentReading() const { return currentData.current; }
  float getFlowRate() const { return currentData.flowRate; }
  float getTotalVolume() const { return currentData.totalVolume; }
  float getDistance() const { return currentData.distance; }
  float getBasketFullness() const { return currentData.basketFullness; }
};

#endif // SENSOR_MANAGER_H