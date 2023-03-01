/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>

#include <app_priv.h>
#include <app_reset.h>

static const char *TAG = "app_main";
uint16_t light_endpoint_id = 0;

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address changed");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        break;

    case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:
        ESP_LOGI(TAG, "Commissioning failed, fail safe timer expired");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
        ESP_LOGI(TAG, "Commissioning session started");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped:
        ESP_LOGI(TAG, "Commissioning session stopped");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:
        ESP_LOGI(TAG, "Commissioning window opened");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
        ESP_LOGI(TAG, "Commissioning window closed");
        break;

    default:
        break;
    }
}

static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                       void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: type: %d, effect: %d", type, effect_id);
    return ESP_OK;
}

static esp_err_t app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == PRE_UPDATE) {
        /* Driver update */
        app_driver_handle_t driver_handle = (app_driver_handle_t)priv_data;
        err = app_driver_attribute_update(driver_handle, endpoint_id, cluster_id, attribute_id, val);
    }

    return err;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Initialize driver */
    app_driver_handle_t light_handle_ch1 = app_driver_light_init_ch1();
    app_driver_handle_t light_handle_ch2 = app_driver_light_init_ch2();
    app_driver_handle_t button_handle = app_driver_button_init();
    app_reset_button_register(button_handle);

    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);


    /* Create the Light Channel 1 config*/
    on_off_light::config_t light_ch1_config;
    light_ch1_config.on_off.on_off = DEFAULT_POWER;
    light_ch1_config.on_off.lighting.start_up_on_off = nullptr;
    endpoint_t *endpoint_ch1 = on_off_light::create(node, &light_ch1_config, ENDPOINT_FLAG_NONE, light_handle_ch1);

    /* These node and endpoint handles can be used to create/add other endpoints and clusters. */
    if (!node || !endpoint_ch1) {
        ESP_LOGE(TAG, "Matter node creation failed");
    }

    light_endpoint_id = endpoint::get_id(endpoint_ch1);
    ESP_LOGW(TAG, "Light created with endpoint_id %d", light_endpoint_id);

    /* Starting driver with default values */
    app_driver_light_set_defaults(light_endpoint_id);

    /* Create the Light Channel 1 config*/
    on_off_light::config_t light_ch2_config;
    light_ch2_config.on_off.on_off = DEFAULT_POWER;
    light_ch2_config.on_off.lighting.start_up_on_off = nullptr;
    endpoint_t *endpoint_ch2 = on_off_light::create(node, &light_ch2_config, ENDPOINT_FLAG_NONE, light_handle_ch2);

    /* These node and endpoint handles can be used to create/add other endpoints and clusters. */
    if (!node || !endpoint_ch2) {
        ESP_LOGE(TAG, "Matter node creation failed");
    }

    light_endpoint_id = endpoint::get_id(endpoint_ch2);
    ESP_LOGW(TAG, "Light created with endpoint_id %d", light_endpoint_id);

    /* Starting driver with default values */
    app_driver_light_set_defaults(light_endpoint_id);

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Matter start failed: %d", err);
    }

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::init();
#endif
}
