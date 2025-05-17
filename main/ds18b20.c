#include "freeRTOS\freeRTOS.h"
#include "freeRTOS\task.h"

#include "mqtt_client.h"

#include "ds18b20.h"

#include "esp_log.h"
#include <math.h>
#include "mqtt_topics.h"

#include "mqtt.h"

#include <ds18x20.h>

#define SENSOR_READ_FREQUENCY_MS 30 * 1000
#define MAXDEVS              1

static const gpio_num_t SENSOR_GPIO = GPIO_NUM_4;

static const char* TAG = "ds18b20";

void publishTemperature(const char* temperature) {
    esp_mqtt_client_enqueue(mqtt_client, MQTT_WATER_TEMP_TOPIC, temperature, 0, 0, 0, true);
}

void loopDs18b20() {
    float temperature;
    esp_err_t res;
    while (1) {
        res = ds18b20_measure_and_read(SENSOR_GPIO, DS18X20_ANY, &temperature);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Could not read from sensor: %d (%s)", res, esp_err_to_name(res));
            publishTemperature("");
        }
        else {
            ESP_LOGI(TAG, "Sensor: %.2f°C", temperature);
             char temp_str[16];
            snprintf(temp_str, sizeof(temp_str), "%.1f", temperature);
            publishTemperature(temp_str);            
        }

        vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_FREQUENCY_MS));
    }
}

void initDs18b20() {
    ESP_LOGI(TAG, "initDs18b20");

    xTaskCreate(loopDs18b20, "ds18b20", 4096, NULL, 2, NULL);
}
