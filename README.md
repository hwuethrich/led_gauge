# LED Gauge Library for ESPHome

A reusable LED gauge effect library for ESPHome that displays values as a colored gauge with animated wave effects. This library leverages ESPHome's built-in helper functions for optimal integration.

## Features

- Display any value (0.0 to 1.0) as a lit portion of an LED strip
- Customizable three-color gradient
- Animated traveling wave effect with adjustable speed
- Supports both forward and reverse wave directions
- Easy integration with ESPHome sensors
- Leverages ESPHome's Color class operators for efficient color manipulation

## Installation

1. Copy `led_gauge.h` to your ESPHome configuration directory
2. Include it in your YAML configuration:

```yaml
esphome:
  includes:
    - led_gauge.h
```

## Usage

### Basic Usage

The library provides two main functions:

#### `led_gauge()`
Full control over all parameters including timing:

```cpp
led_gauge(it, value, c1, c2, c3, wave_speed, phase, dt);
```

Parameters:
- `it`: The addressable light iterator
- `value`: Gauge value from 0.0 to 1.0
- `c1`, `c2`, `c3`: Three colors for the gradient
- `wave_speed`: Animation speed from -1.0 to 1.0 (negative for reverse)
- `phase`: Reference to phase accumulator (for animation continuity)
- `dt`: Time delta since last update in seconds

#### `led_gauge_simple()`
Simplified version that manages timing internally:

```cpp
led_gauge_simple(it, value, c1, c2, c3, wave_speed);
```

### Example: CO₂ Monitor

```yaml
light:
  - platform: neopixelbus
    type: GRB
    variant: WS2812
    pin: GPIO12
    num_leds: 12
    name: "CO2 Indicator"
    id: led_strip
    effects:
      - addressable_lambda:
          name: "CO₂"
          update_interval: 50ms
          lambda: |-
            // Normalize CO₂ value to 0-1 range
            float value = id(co2_sensor).state;
            float normalized = (value - 400.0) / (2000.0 - 400.0);
            
            // Green → Yellow → Red gradient
            Color c1(0, 255, 0);
            Color c2(255, 255, 0);
            Color c3(255, 0, 0);
            
            // Slow wave for steady values
            led_gauge_simple(it, normalized, c1, c2, c3, 0.3);
```

### Example: Temperature Gauge

```yaml
- addressable_lambda:
    name: "Temperature"
    update_interval: 50ms
    lambda: |-
      // Map temperature to 0-1 range (e.g., 0°C to 40°C)
      float temp = id(temperature).state;
      float normalized = (temp - 0.0) / 40.0;
      
      // Blue → Cyan → Red gradient
      Color cold(0, 0, 255);
      Color moderate(0, 255, 255);
      Color hot(255, 0, 0);
      
      led_gauge_simple(it, normalized, cold, moderate, hot, 0.5);
```

### Example: Dynamic Wave Speed Based on Trend

```yaml
- addressable_lambda:
    name: "Dynamic Gauge"
    update_interval: 50ms
    lambda: |-
      static float phase = 0.0;
      
      // Get value and trend
      float value = id(sensor).state;
      float trend = id(sensor_trend);
      
      // Normalize value using ESPHome's clamp helper
      float normalized = clamp((value - id(min_val)) / (id(max_val) - id(min_val)), 0.0f, 1.0f);
            
      // Map trend to wave speed
      float wave_speed = clamp(trend / 50.0, -1.0f, 1.0f);
      
      // Time delta
      unsigned long now = millis();
      static unsigned long last_update = 0;
      float dt = (now - last_update) / 1000.0;
      last_update = now;
      
      // Custom colors
      Color c1(255, 255, 255);  // White
      Color c2(128, 0, 255);    // Purple
      Color c3(255, 0, 128);    // Pink
      
      led_gauge(it, normalized, c1, c2, c3, wave_speed, phase, dt);
```

## Color Schemes

### Suggested color gradients:

- **Air Quality**: Green → Yellow → Red
- **Temperature**: Blue → Cyan → Red  
- **Humidity**: White → Blue → Purple
- **Battery**: Red → Yellow → Green
- **Volume**: Blue → Cyan → White
- **Speed**: Green → Yellow → Orange

## Advanced Features

### Multiple Modes

You can create a multi-mode gauge that switches between different sensors:

```yaml
globals:
  - id: mode
    type: int
    initial_value: '0'
  - id: pulse_phase
    type: float
    initial_value: '0.0'

effects:
  - addressable_lambda:
      name: "Multi Gauge"
      update_interval: 50ms
      lambda: |-
        float value, vmin, vmax;
        Color c1, c2, c3;
        
        // Select mode
        if (id(mode) == 0) {
          // CO₂ mode
          value = id(co2_sensor).state;
          vmin = 400; vmax = 2000;
          c1 = Color(0, 255, 0);
          c2 = Color(255, 255, 0);
          c3 = Color(255, 0, 0);
        } else if (id(mode) == 1) {
          // Temperature mode
          value = id(temp_sensor).state;
          vmin = 0; vmax = 40;
          c1 = Color(0, 0, 255);
          c2 = Color(0, 255, 255);
          c3 = Color(255, 0, 0);
        }
        
        float normalized = (value - vmin) / (vmax - vmin);
        led_gauge_simple(it, normalized, c1, c2, c3, 0.4);
```

## Integration with Home Assistant

The gauge can be controlled from Home Assistant using template numbers:

```yaml
number:
  - platform: template
    name: "Gauge Min Value"
    id: gauge_min
    min_value: 0
    max_value: 1000
    initial_value: 400
    step: 10
    optimistic: true
    restore_value: true

  - platform: template
    name: "Gauge Max Value"
    id: gauge_max
    min_value: 1000
    max_value: 5000
    initial_value: 2000
    step: 10
    optimistic: true
    restore_value: true
```

## Tips

1. Use `update_interval: 50ms` for smooth animations
2. Keep wave_speed between -0.5 and 0.5 for subtle effects
3. Use contrasting colors for better visibility
4. The library uses ESPHome's built-in `clamp()`, `lerp()`, and `remap()` helpers for value manipulation
5. Store phase as a global or static variable for smooth animations
6. All values are automatically clamped to their valid ranges
7. The `led_gauge_simple()` function uses static variables and is not thread-safe - use only one instance per device
8. Division by zero is handled automatically when normalizing values (returns 0.0 if min equals max)

## Configuration Constants

The library defines the following constants that control the wave effect:

- `WAVE_CYCLES`: Number of wave cycles across the strip (default: 4.0)
- `MIN_BRIGHTNESS`: Minimum brightness factor for the wave effect (default: 0.6)
- `MAX_BRIGHTNESS`: Maximum brightness factor for the wave effect (default: 1.0)

These create a wave that oscillates between 60% and 100% brightness with 4 complete cycles across the LED strip.

## Color Class Operations

The LED gauge library uses ESPHome's Color class, which provides several useful operators:

### Multiplication (Dimming/Brightness)
```cpp
Color dimmed = base_color * 128;  // 50% brightness
Color off = base_color * 0;       // Completely off
```

### Addition (Color Mixing)
```cpp
Color yellow = Color(255, 0, 0) + Color(0, 255, 0);  // Red + Green
Color white = Color(85, 85, 85) + Color(170, 170, 170);
```

### Subtraction (Color Removal)
```cpp
Color cyan = Color(255, 255, 255) - Color(255, 0, 0);  // White - Red
```

### Gradient Interpolation
```cpp
Color mid = color1.gradient(color2, 128);  // 50% between color1 and color2
```

### Fade Effects
```cpp
Color faded_black = color.fade_to_black(200);  // Fade 78% toward black
Color faded_white = color.fade_to_white(100);  // Fade 39% toward white
```

### Lighten/Darken
```cpp
Color lighter = color.lighten(50);   // Add 50 to each channel
Color darker = color.darken(50);     // Subtract 50 from each channel
```

The library internally uses the multiplication operator (`*`) to apply the wave brightness effect, making the code cleaner and more efficient.

## License

This library is provided as-is for use with ESPHome projects.