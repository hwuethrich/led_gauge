#pragma once

#include "esphome/components/light/addressable_light.h"
#include "esphome/core/color.h"
#include "esphome/core/helpers.h"
#include <cmath>

using namespace esphome;
using namespace esphome::light;

// Constants for wave effect
static constexpr float WAVE_CYCLES = 6.0f;  // Number of wave cycles across the strip
static constexpr float WAVE_AMPLITUDE = 0.4f;  // Wave brightness amplitude (0.0-1.0)

/**
 * Renders a gauge effect on an addressable LED strip with traveling wave
 * animation
 *
 * @param it The addressable light iterator
 * @param value The gauge value (0.0 to 1.0) - determines how many LEDs are lit
 * @param c1 First color in the gradient (start color)
 * @param c2 Second color in the gradient (middle color)
 * @param c3 Third color in the gradient (end color)
 * @param wave_speed Wave animation speed (-1.0 to 1.0), negative for reverse
 * direction
 * @param phase Reference to the phase accumulator for wave animation continuity
 * @param dt Time delta since last update in seconds
 */
inline void led_gauge(AddressableLight &it, float value, Color c1,
                      Color c2, Color c3, float wave_speed,
                      float &phase, float dt) {

  // Constrain value to 0-1 range using ESPHome's clamp
  value = clamp(value, 0.0f, 1.0f);

  // Constrain wave speed to -1 to 1 range
  wave_speed = clamp(wave_speed, -1.0f, 1.0f);

  // Update phase based on wave speed
  // Base frequency is 1Hz, wave_speed scales it
  float angular_velocity = 2.0f * M_PI * wave_speed;
  phase += angular_velocity * dt;

  // Wrap phase to keep it in 0 to 2π range
  while (phase > 2.0f * M_PI)
    phase -= 2.0f * M_PI;
  while (phase < 0)
    phase += 2.0f * M_PI;

  // Get number of LEDs and calculate how many should be lit
  int num_leds = it.size();
  int lit_leds = static_cast<int>(value * num_leds);

  // Render each LED
  for (int i = 0; i < num_leds; i++) {
    float position = static_cast<float>(i) / static_cast<float>(num_leds);

    Color color = Color::BLACK;

    if (i < lit_leds) {
      // Calculate gradient color based on position
      if (position < 0.5f) {
        // Gradient from c1 to c2 in first half
        uint8_t gradient_pos =
            static_cast<uint8_t>(remap(position, 0.0f, 0.5f, 0.0f, 255.0f));
        color = c1.gradient(c2, gradient_pos);
      } else {
        // Gradient from c2 to c3 in second half
        uint8_t gradient_pos =
            static_cast<uint8_t>(remap(position, 0.5f, 1.0f, 0.0f, 255.0f));
        color = c2.gradient(c3, gradient_pos);
      }

      // Apply traveling wave effect
      // Multiple wave cycles across the strip for more dynamic effect
      float wave_shift = WAVE_CYCLES * M_PI * position;
      float wave_value = sin(phase - wave_shift);
      float wave_amplitude = std::abs(wave_speed) * WAVE_AMPLITUDE;

      // Use squared sine for sharper peaks
      // wave_value = wave_value > 0.0f ? wave_value * wave_value : 0.0f;

      // Calculate brightness scale by remapping wave_value to brightness range
      float min_brightness = 1.0f - WAVE_AMPLITUDE - wave_amplitude;
      float max_brightness = 1.0f - WAVE_AMPLITUDE + wave_amplitude;

      uint8_t brightness_scale =
          static_cast<uint8_t>(remap(wave_value, -1.0f, 1.0f, min_brightness, max_brightness) * 255.0f);

      // Apply brightness using Color's multiplication operator
      color *= brightness_scale;
    }

    it[i] = color;
  }
}

/**
 * Simplified version of led_gauge that manages its own timing
 * Useful when you don't need external control over the phase
 */
inline void led_gauge_simple(AddressableLight &it, float value, Color c1,
                             Color c2, Color c3,
                             float wave_speed) {
  static float phase = 0.0f;
  static unsigned long last_update = 0;

  unsigned long now = millis();
  float dt = (now - last_update) / 1000.0f;
  last_update = now;

  led_gauge(it, value, c1, c2, c3, wave_speed, phase, dt);
}
