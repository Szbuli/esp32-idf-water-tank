#include "freeRTOS\freeRTOS.h"
#include "freeRTOS\task.h"

#include "onewire.h"

#include "mqtt_client.h"

#include "ds18b20.h"

#include "esp_log.h"
#include <math.h>
#include "mqtt_topics.h"

#include "mqtt.h"

#define GPIO_DS18B20_0       22
#define MAXDEVS              1

static const char* TAG = "ds18b20";

OW ow;

void publishTemperature(const char* temperature) {
    esp_mqtt_client_enqueue(mqtt_client, MQTT_WATER_TEMP_TOPIC, temperature, 0, 0, 0, true);
}

void loopDs18b20() {
    while (1) {
        if (ow_reset(&ow)) {
            // scan bus for slaves
            uint64_t romcode[MAXDEVS];
            int num_devs = ow_romsearch(&ow, romcode, MAXDEVS, OW_SEARCH_ROM);

            if (num_devs == 1) {
                ESP_LOGV(TAG, "Found %d devices\n", num_devs);
                for (int i = 0; i < num_devs; i += 1) {
                    ESP_LOGV(TAG, "\t%d: 0x%llx\n", i, romcode[i]);
                }

                ow_reset(&ow);
                ow_send(&ow, OW_SKIP_ROM);
                ow_send(&ow, DS18B20_CONVERT_T);

                ESP_LOGV(TAG, "wait for the conversions to finish");
                // wait for the conversions to finish
                while (ow_read(&ow) == 0);


                ow_reset(&ow);
                ow_send(&ow, OW_SKIP_ROM);
                ow_send(&ow, DS18B20_READ_SCRATCHPAD);
                int16_t temp = 0;
                temp = ow_read(&ow) | (ow_read(&ow) << 8);


                float temperature = temp / 16.0;
                char temp_str[16];
                snprintf(temp_str, sizeof(temp_str), "%.1f", temperature);
                ESP_LOGI(TAG, "\t%s", temp_str);

                publishTemperature(temp_str);
            }
            else {
                ESP_LOGE(TAG, "No devices found");
                publishTemperature("");
            }

        }
        else {
            ESP_LOGE(TAG, "ow reset false");
            publishTemperature("");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void initDs18b20() {
    ESP_LOGI(TAG, "initDs18b20");

    ow_init(&ow, GPIO_DS18B20_0);

    xTaskCreate(loopDs18b20, "ds18b20", 4096, NULL, 2, NULL);
}
