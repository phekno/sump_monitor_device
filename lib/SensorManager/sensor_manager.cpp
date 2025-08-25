#include "sensor_manager.h"

// Static member initialization
SensorManager* SensorManager::instance = nullptr;

SensorManager::SensorManager() {
  instance = this;
  sonar = nullptr;
}

SensorManager::~SensorManager() {
  delete sonar;
}

void SensorManager::init() {
  // Initialize pins
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  pinMode(CURRENT_SENSOR_PIN, INPUT);
  
  // Configure ADC
  analogSetAttenuation(ADC_11db);
  analogSetWidth(12);
  
  // Initialize ultrasonic sensor
  sonar = new NewPing(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
  
  // Attach flow sensor interrupt
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowSensorInterrupt, FALLING);
  
  lastFlowCalculation = millis();
  
  Serial.println("SensorManager initialized");
}

void SensorManager::readAll() {
  readCurrentSensor();
  readFlowSensor();
  readUltrasonicSensor();
  currentData.timestamp = millis();
}

void SensorManager::readCurrentSensor() {
  int adcValue = analogRead(CURRENT_SENSOR_PIN);
  float voltage = (adcValue * VCC_VOLTAGE) / ADC_RESOLUTION;
  
  // Calculate current (assuming ACS712 sensor)
  currentData.current = abs((voltage - (VCC_VOLTAGE / 2.0)) / CURRENT_SENSITIVITY);
  
  // Apply deadband to filter noise
  if (currentData.current < 0.1) {
    currentData.current = 0.0;
  }
}

void SensorManager::readFlowSensor() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastFlowCalculation >= 1000) { // Calculate every second
    // Calculate actual time elapsed in seconds
    float timeElapsedSeconds = (currentTime - lastFlowCalculation) / 1000.0;
    
    // Disable interrupts while reading pulse count
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
    int pulses = flowPulseCount;
    flowPulseCount = 0;
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowSensorInterrupt, FALLING);
    
    // Calculate flow rate in liters per minute
    float flowLPM = (pulses / CALIBRATION_FACTOR) * (60.0 / timeElapsedSeconds);
    
    // Convert to gallons per minute (1 liter = 0.264172 gallons)
    currentData.flowRate = flowLPM * 0.264172;
    
    // Add to total volume (in gallons) - use actual time elapsed
    currentData.totalVolume += (currentData.flowRate * timeElapsedSeconds) / 60.0;
    
    lastFlowCalculation = currentTime;
  }
}

void SensorManager::readUltrasonicSensor() {
  currentData.distance = sonar->ping_cm();
  
  if (currentData.distance == 0) {
    currentData.distance = MAX_DISTANCE; // Assume maximum distance if no echo
  }
  
  // Calculate basket fullness percentage
  float waterLevel = currentData.distance - SENSOR_OFFSET;
  currentData.basketFullness = 100.0 - ((waterLevel / BASKET_DEPTH) * 100.0);
  currentData.basketFullness = constrain(currentData.basketFullness, 0.0, 100.0);
}

void IRAM_ATTR SensorManager::flowSensorInterrupt() {
  if (instance != nullptr) {
    instance->flowPulseCount++;
  }
}