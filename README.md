# ESP32 Sump Pump Monitor

A professional IoT monitoring system for sump pumps built with ESP32 and PlatformIO. Features real-time sensor monitoring, pump event tracking, and cloud connectivity.

## 🚀 Features

- **Real-time Monitoring**: Current draw, flow rate, and water level sensors
- **Interrupt-driven Detection**: Sub-millisecond pump start/stop detection  
- **Event Tracking**: Complete pump cycle analysis with volume and duration
- **Cloud Connectivity**: JSON API integration with automatic retry logic
- **Professional Architecture**: Modular, maintainable codebase using modern C++ practices

## 📋 Hardware Requirements

- ESP32 Development Board
- Water flow sensor (with pulse output)
- Current sensor (ACS712 or similar)  
- Ultrasonic distance sensor (HC-SR04 or similar)
- Breadboard and jumper wires

## 🔧 Pin Configuration

| Component | ESP32 Pin | Notes |
|-----------|-----------|-------|
| Flow Sensor | GPIO 2 | Must support interrupts |
| Current Sensor | A0 | Analog input |
| Ultrasonic Trigger | GPIO 4 | Digital output |
| Ultrasonic Echo | GPIO 5 | Digital input |

## 🛠️ Software Setup

### Prerequisites

1. [PlatformIO Core](https://docs.platformio.org/en/latest/core/) or [PlatformIO IDE](https://platformio.org/platformio-ide)
2. Git for version control

### Installation

1. Clone this repository:
```bash
git clone https://github.com/yourusername/esp32-sump-monitor.git
cd esp32-sump-monitor
```

2. Configure your settings in `include/config.h`:
```cpp
#define WIFI_SSID "Your_WiFi_Network"
#define WIFI_PASSWORD "Your_WiFi_Password"
#define API_ENDPOINT "http://your-api-server.com/api/sensor-data"
#define DEVICE_ID "sump-pump-01"
```

3. Build and upload:
```bash
pio run --target upload
```

4. Monitor serial output:
```bash
pio device monitor
```

## 📊 Data Output

### Serial Monitor Output
```
ESP32 Sump Pump Monitor Initialized
Device ID: sump-pump-01
API Endpoint: http://your-api-server.com/api/sensor-data
Time(s) Flow(GPM)  Current(A)  Distance(cm)  Basket(%)
---------------------------------------------------------------
15.2    0.00       0.15        25.4          15.2%
20.3    2.45       3.21        25.1          16.8%
```

### JSON API Payload
```json
{
  "device_id": "sump-pump-01",
  "timestamp": 1234567890,
  "current": 2.45,
  "flow_rate": 15.3,
  "total_volume": 125.67,
  "distance": 12.5,
  "basket_fullness": 65.2,
  "wifi_rssi": -45,
  "free_heap": 234567,
  "uptime": 3600,
  "current_event_active": true,
  "current_event_volume": 2.34,
  "current_event_duration": 45.2
}
```

## 🔧 Configuration Options

### Sensor Calibration

Edit values in `include/config.h`:

```cpp
// Flow sensor calibration (pulses per liter)
#define CALIBRATION_FACTOR 4.5

// Current sensor sensitivity (V/A)  
#define CURRENT_SENSITIVITY 0.066  // ACS712-30A

// Sump basket dimensions (cm)
#define BASKET_DEPTH 30.0
#define SENSOR_OFFSET 5.0

// Current threshold for pump detection (ADC counts)
#define CURRENT_THRESHOLD_ADC 150
```

### Network Settings

```cpp
// API communication intervals
#define DATA_SEND_INTERVAL 5000    // Send data every 5 seconds
#define WIFI_TIMEOUT 30000         // WiFi connection timeout
#define MAX_FAILED_REQUESTS 5      // Trigger reconnection after failures
```

## 🏗️ Architecture

### Modular Design

- **SensorManager**: Handles all sensor readings and calibration
- **PumpMonitor**: Tracks pump events with interrupt-driven detection  
- **NetworkManager**: Manages WiFi connectivity and API communication
- **Main Application**: Coordinates all components and handles timing

### Interrupt-Driven Performance

- **Flow Sensor**: Hardware interrupt on each pulse for accurate volume measurement
- **Current Monitor**: 10kHz timer interrupt for sub-millisecond pump detection
- **Non-blocking**: All operations designed to prevent blocking the main loop

## 📈 Pump Event Tracking

The system automatically detects and tracks pump cycles:

1. **Start Detection**: Current threshold crossed (< 0.1ms response time)
2. **Event Monitoring**: Peak current, average current, volume pumped
3. **End Detection**: Current drops below threshold  
4. **Data Logging**: Complete event statistics logged and transmitted

## 🔍 Debugging

Enable debug output by adding to `platformio.ini`:

```ini
build_flags = 
    -DDEBUG
    -DCORE_DEBUG_LEVEL=3
```

### Common Issues

1. **WiFi Connection Issues**: Check SSID/password, signal strength
2. **Sensor Readings**: Verify pin connections and power supply  
3. **API Communication**: Confirm server endpoint and network connectivity
4. **Flow Sensor**: Ensure proper interrupt pin usage and calibration

## 🚀 Advanced Features

### Over-the-Air Updates
```ini
; Add to platformio.ini
upload_protocol = espota
upload_port = 192.168.1.100
```

### Custom Alerting
Modify `PumpMonitor::endPumpEvent()` to add custom alert logic:

```cpp
// Alert on long pump cycles
if (currentEvent.duration > 120.0) {
    sendAlert("Long pump cycle detected");
}

// Alert on low flow rate
if (currentEvent.volumePumped < 1.0 && currentEvent.duration > 30.0) {
    sendAlert("Low flow rate detected");
}
```

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)  
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🆘 Support

- Create an [issue](https://github.com/yourusername/esp32-sump-monitor/issues) for bug reports
- Check the [wiki](https://github.com/yourusername/esp32-sump-monitor/wiki) for detailed documentation
- Join our [Discord community](https://discord.gg/your-invite) for real-time help

## 🙏 Acknowledgments

- [PlatformIO](https://platformio.org/) for excellent embedded development tools
- [NewPing Library](https://github.com/teckel12/NewPing) for ultrasonic sensor support
- [ArduinoJson](https://arduinojson.org/) for efficient JSON handling