# ESP32 LED Gauge - Environmental Monitor

An ESP32-based environmental monitoring device that displays CO₂, temperature, and humidity data from an SCD41 sensor, plus Aare river temperature, using a colorful LED strip gauge with animated wave effects.

> [!IMPORTANT]
> This project is currently in development and may not be fully functional.

## Features

- **SCD41 Sensor Integration**: Monitors CO₂ (ppm), temperature (°C), and humidity (%)
- **Aare River Data**: Fetches current temperature from aare.guru API
- **Visual LED Gauge**: 12-LED WS2812 strip with animated wave effects
- **Multiple Display Modes**: Switch between CO₂, temperature, humidity, and Aare temperature
- **Home Assistant Integration**: Full ESPHome integration with configurable parameters
- **Web Interface**: Built-in web server for local monitoring
- **WireGuard VPN**: Secure remote access capability

## Images

<table>
  <tr>
    <td><img src="images/IMG_3400.jpeg" width="250" alt="LED Gauge Device"></td>
    <td><img src="images/IMG_3401.jpeg" width="250" alt="Device in Action"></td>
    <td><img src="images/IMG_3406.jpeg" width="250" alt="Different Display Modes"></td>
  </tr>
</table>

## Hardware

- **MCU**: ESP32 (ESP32-DevKit-C or similar)
- **Sensor**: SCD41 CO₂/Temperature/Humidity sensor (I2C)
- **LED Strip**: 12x WS2812 RGB LEDs (NeoPixel compatible)
- **Connectivity**: Wi-Fi with WireGuard VPN support

### Wiring

- **SCD41 Sensor**:
  - SDA → GPIO21
  - SCL → GPIO22
  - VCC → 3.3V
  - GND → GND

- **LED Strip**:
  - Data → GPIO12
  - VCC → 5V
  - GND → GND

## Software Architecture

### Core Components

1. **LED Gauge Library** (`led_gauge.h`): Custom C++ library for rendering gauge effects
2. **ESPHome Configuration** (`led_gauge.yaml`): Main device configuration
3. **API Integration**: HTTP client for Aare river data
4. **Sensor Management**: SCD41 driver with trend calculation

### Display Modes

#### 1. CO₂ Gauge (Default)
- **Range**: 400-2000 ppm
- **Colors**: Green → Yellow → Red
- **Animation**: Slow wave (0.3 speed)
- **Thresholds**:
  - Good: < 800 ppm (Green)
  - Moderate: 800-1200 ppm (Yellow)
  - Poor: > 1200 ppm (Red)

#### 2. Temperature Gauge
- **Range**: 15-30°C
- **Colors**: Blue → Cyan → Red
- **Animation**: Medium wave (0.5 speed)

#### 3. Humidity Gauge
- **Range**: 30-70% RH
- **Colors**: Orange → Blue → Purple
- **Animation**: Fast wave (0.7 speed)

#### 4. Aare Temperature Gauge
- **Range**: 0-25°C
- **Colors**: Deep Blue → Cyan → Warm White
- **Animation**: Gentle wave (0.4 speed)
- **Data Source**: [aare.guru API](https://aare.guru/)

## Installation

### 1. Hardware Setup
Wire the components as described in the Hardware section above.

### 2. ESPHome Configuration

1. Copy the project files to your ESPHome directory
2. Create a `secrets.yaml` file with your credentials:

```yaml
wifi_ssid: "YourWiFiNetwork"
wifi_password: "YourWiFiPassword"
api_encryption: "your-32-character-api-key"
ota_password: "your-ota-password"
```

3. Compile and upload to your ESP32:

```bash
esphome compile led_gauge.yaml
esphome upload led_gauge.yaml
```

### 3. Home Assistant Integration

The device automatically appears in Home Assistant via ESPHome integration. You'll get:

**Sensors:**
- CO₂ Level (ppm)
- Temperature (°C)
- Humidity (%)
- Aare Temperature (°C)
- Aare Flow (m³/s)
- Aare Height (m)

**Controls:**
- Gauge mode selection
- Color customization (RGB values for each gradient point)
- Animation speed control
- Min/max value adjustment
- Manual data refresh button

## LED Gauge Library

The custom `led_gauge.h` library provides smooth gradient rendering with traveling wave animations:

### Key Features
- **Three-color gradients** with smooth interpolation
- **Wave animation** with configurable speed and direction
- **ESPHome integration** using native Color class operations
- **Optimized performance** for real-time updates at 50ms intervals

### Usage Example

```cpp
// CO₂ visualization
float co2_normalized = (co2_value - 400.0) / (2000.0 - 400.0);
Color green(0, 255, 0);
Color yellow(255, 255, 0);
Color red(255, 0, 0);
led_gauge_simple(it, co2_normalized, green, yellow, red, 0.3);
```

## API Integration

### Aare River Data
The device fetches real-time data from the aare.guru API every 5 minutes:

- **Temperature**: Current water temperature
- **Flow**: Water flow rate in m³/s
- **Height**: Water level in meters
- **Status**: Text descriptions (e.g., "Perfect for swimming")

API endpoint: `https://aareguru.existenz.ch/v2018/current`

## Configuration Options

### Customizable Parameters (via Home Assistant)

**Gauge Ranges:**
- CO₂ Min/Max values
- Temperature Min/Max values
- Humidity Min/Max values
- Aare Temperature Min/Max values

**Visual Settings:**
- Color 1, 2, 3 for each gauge mode (RGB values)
- Wave animation speed (-1.0 to 1.0)
- Update intervals

**Network:**
- Wi-Fi credentials
- WireGuard VPN configuration
- Static IP assignment

## Advanced Features

### Trend Calculation
The device calculates value trends and can adjust wave speed based on rate of change:

```cpp
// Wave speed based on CO₂ trend
float trend = (current_co2 - previous_co2) / time_delta;
float wave_speed = clamp(trend / 50.0, -1.0f, 1.0f);
```

### WireGuard VPN
Secure remote access is configured for safe monitoring away from home network.

### Web Server
Access the device directly at its IP address for:
- Real-time sensor readings
- Manual gauge mode switching
- Network diagnostics
- Log viewing

## Troubleshooting

### Common Issues

**LED strip not working:**
- Check GPIO12 connection
- Verify 5V power supply
- Ensure common ground between ESP32 and LED strip

**SCD41 sensor not detected:**
- Verify I2C connections (GPIO21/22)
- Check sensor power (3.3V)
- Use I2C scanner to detect device address (0x62)

**API requests failing:**
- Check Wi-Fi connection
- Verify internet access
- Monitor logs for HTTP error codes

**Home Assistant not discovering device:**
- Ensure API encryption key matches
- Check network connectivity
- Restart ESPHome device

### Logs and Debugging

Enable verbose logging in `led_gauge.yaml`:

```yaml
logger:
  level: DEBUG
  logs:
    aare: DEBUG
    sensor: DEBUG
    light: DEBUG
```

## Contributing

This project uses:
- [ESPHome](https://esphome.io/) for device firmware
- [SCD4x component](https://esphome.io/components/sensor/scd4x.html) for sensor integration
- [NeoPixelBus](https://esphome.io/components/light/neopixelbus.html) for LED control
- [aare.guru API](https://aare.guru/) for river data

## License

This project is open source. Feel free to modify and adapt for your own environmental monitoring needs.

---

*For more advanced ESPHome configurations and LED effects, check out the [ESPHome documentation](https://esphome.io/).*
