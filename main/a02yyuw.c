#include "driver/uart.h"
#include "esp_log.h"
#include "esp_err.h"

#include "a02yyuw.h"
#include "memory.h"
#include "mqtt_topics.h"
#include "mqtt.h"
#include "math.h"

#define BUF_SIZE (1024)

static const char* TAG = "a02yyuw";

int distance;

int maxWaterLevel = 0;
int sensorHeight = 0;

const char* SENSOR_HEIGHT_KEY = "sensor-height";
const char* MAX_WATER_LEVEL_KEY = "max-water-level";

bool _checkSum(unsigned char data[]) {
    return ((data[0] + data[1] + data[2]) & 0x00FF) == data[3];
}

int measure() {
    unsigned char data[4] = {};
    int i = 0;
    unsigned int meassuredDistance;

    uart_flush(A02YYUW_UART_PORT);

    i = 0;

    while (i < 4) {
        uart_read_bytes(A02YYUW_UART_PORT, &data[i], 1, 20 / portTICK_PERIOD_MS);

        i++;

        if (data[0] != SERIAL_HEAD_DATA) {
            i = 0;
        }
    }

    if (i != 4) {
        return DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_ERROR_SERIAL;
    }

    if (!_checkSum(data)) {
        return DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_ERROR_CHECK_SUM;
    }

    meassuredDistance = ((data[1] << 8) + data[2]);

    ESP_LOGI(TAG, "dist: %d", meassuredDistance);

    if (meassuredDistance < MIN_DISTANCE) {
        return DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_ERROR_MIN_LIMIT;
    }

    if (meassuredDistance > MAX_DISTANCE) {
        return DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_ERROR_MAX_LIMIT;
    }

    distance = meassuredDistance;

    return DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_OK;
}

void loopA02yyuw() {
    while (1) {
        int measurementStatus;
        int retryCount = 0;

        do {
            measurementStatus = measure();

            if (measurementStatus == DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_OK) {
                int distanceCm = distance / 10;
                ESP_LOGI(TAG, "distance: %d cm", distanceCm);

                ESP_LOGI(TAG, "sensorHeight: %d cm", sensorHeight);
                ESP_LOGI(TAG, "maxWaterLevel: %d cm", maxWaterLevel);
                int waterLevelInPercentage = round(((sensorHeight - distanceCm) * 100) / (float)maxWaterLevel);

                ESP_LOGI(TAG, "waterLevelInPercentage: %d", waterLevelInPercentage);
                char waterLevelStr[10];
                snprintf(waterLevelStr, sizeof(waterLevelStr), "%d", waterLevelInPercentage);
                esp_mqtt_client_enqueue(mqtt_client, MQTT_WATER_LEVEL_TOPIC, waterLevelStr, 0, 0, 0, true);
                char distanceStr[12];
                snprintf(distanceStr, sizeof(distanceStr), "%d", distanceCm);
                esp_mqtt_client_enqueue(mqtt_client, MQTT_SENSOR_MEASURED_DISTANCE_TOPIC, distanceStr, 0, 0, 0, true);
            }
            else {
                ESP_LOGI(TAG, "measurementStatus: %d", measurementStatus);
            }
        } while (measurementStatus != DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_OK && retryCount++ < 4);

        int result = measure();
        ESP_LOGI(TAG, "result code for measure: %d", result);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void initA02yyuw() {
    uart_config_t uart_config = {
        .baud_rate = A02YYUW_SERIAL_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(A02YYUW_UART_PORT, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(A02YYUW_UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(A02YYUW_UART_PORT, A02YYUW_SERIAL_PIN_TX, A02YYUW_SERIAL_PIN_RX, -1, -1));

    maxWaterLevel = readMemory(MAX_WATER_LEVEL_KEY);
    sensorHeight = readMemory(SENSOR_HEIGHT_KEY);

    ESP_LOGI(TAG, "maxWaterLevel: %d", maxWaterLevel);
    ESP_LOGI(TAG, "sensorHeight: %d", sensorHeight);

    xTaskCreate(loopA02yyuw, "a02yyuw", 4096, NULL, 2, NULL);
}

void persistSensorHeight(int newSensorHeight) {
    ESP_LOGI(TAG, "persistSensorHeight: %d", newSensorHeight);
    persistMemory(SENSOR_HEIGHT_KEY, newSensorHeight);
    sensorHeight = newSensorHeight;
}

void persistMaxWaterLevel(int newMaxWaterLevel) {
    ESP_LOGI(TAG, "persistMaxWaterLevel: %d", newMaxWaterLevel);
    persistMemory(MAX_WATER_LEVEL_KEY, newMaxWaterLevel);
    maxWaterLevel = newMaxWaterLevel;
}




