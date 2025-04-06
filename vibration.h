#pragma once

// Library to configures GPIO12 for vibration. This is a workaround for ESPHome not
// letting us use the pin, which is assigned to flash SPI. However, the SPI pin is
// not really needed if not using quad mode.

namespace badge {

constexpr auto VIBRATION_GPIO = GPIO_NUM_12;

// Configured the vibration GPIO. To be called on boot.
void setup_vibration_motor() {
  ESP_LOGD("vibration", "Configuring vibration motor GPIO");
  gpio_config_t cfg;
  memset(&cfg, 0x0, sizeof(cfg));
  cfg.pin_bit_mask |= (1LL << VIBRATION_GPIO);
  cfg.intr_type = GPIO_INTR_DISABLE;
  cfg.mode = GPIO_MODE_OUTPUT;
  cfg.pull_up_en = GPIO_PULLUP_DISABLE;
  cfg.pull_down_en = GPIO_PULLDOWN_ENABLE;
  esp_err_t result = gpio_config(&cfg);
  if (result != ESP_OK) {
    ESP_LOGE("vibration", "GPIO configuration failed: %s", esp_err_to_name(result));
    return;
  }
  // Make sure it starts off.
  gpio_set_level(VIBRATION_GPIO, 0);
  // Make vibration stronger by driving higher current.
  result = gpio_set_drive_capability(VIBRATION_GPIO, GPIO_DRIVE_CAP_3);
  if (result != ESP_OK) {
    ESP_LOGE("vibration", "GPIO drive capability failed: %s", esp_err_to_name(result));
    return;
  }
}

}  // namespace badge
