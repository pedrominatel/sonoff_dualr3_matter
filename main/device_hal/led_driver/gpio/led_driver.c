// Copyright 2021 Espressif Systems (Shanghai) CO LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include "driver/gpio.h"
#include <esp_log.h>

#include <led_driver.h>

static const char *TAG = "led_driver_gpio";
static bool current_power = false;

led_driver_handle_t led_driver_init(led_driver_config_t *config)
{
    ESP_LOGI(TAG, "Initializing light driver");
    esp_err_t err = ESP_OK;

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL<<config->gpio),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };

    err = gpio_config(&io_conf);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "gpio_config failed");
    }

    /* Using (gpio + 1) as handle */
    return (led_driver_handle_t)(config->gpio + 1);
}

esp_err_t led_driver_set_power(led_driver_handle_t handle, bool power)
{
    current_power = power;
    return gpio_set_level((int)handle - 1, current_power);
}

esp_err_t led_driver_set_brightness(led_driver_handle_t handle, uint8_t brightness)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t led_driver_set_hue(led_driver_handle_t handle, uint16_t hue)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t led_driver_set_saturation(led_driver_handle_t handle, uint8_t saturation)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t led_driver_set_temperature(led_driver_handle_t handle, uint32_t temperature)
{
    return ESP_ERR_NOT_SUPPORTED;
}
