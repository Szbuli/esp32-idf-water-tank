#include "overflow.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "memory.h"
#include "mqtt_topics.h"
#include "mqtt.h"
#include "pump.h"

static const char* TAG = "overflow";

#define OVERFLOW_GPIO GPIO_NUM_2

#define GPIO_LEVEL_LOW 0
#define GPIO_LEVEL_HIGH 1

#define GPIO_OUTPUT_PIN_SEL  (1ULL<<OVERFLOW_GPIO)

bool autoMode = false;
int buttonState = GPIO_LEVEL_LOW;
int counter = 100;
int delayCounter = 0;

const char* OVERFLOW_PUMP_MODE_KEY = "of-pump-mode";

bool isOverflowPumpAutoModeEnabled() {
    int mode = readMemory(OVERFLOW_PUMP_MODE_KEY);
    ESP_LOGV(TAG, "isOverflowPumpAutoModeEnabled: %d", mode);
    return mode != 0;
}

void turnOnOverFlowPump() {
    ESP_LOGI(TAG, "AUTO MODE - OVERFLOW PUNP ON");
    esp_mqtt_client_enqueue(mqtt_client, MQTT_OVERFLOW_PUMP_TOPIC, ON_PAYLOAD, 0, 0, 0, true);
    turnOnRelay2();
}

void turnOffOverFlowPump() {
    ESP_LOGI(TAG, "AUTO MODE - OVERFLOW PUNP OFF");
    esp_mqtt_client_enqueue(mqtt_client, MQTT_OVERFLOW_PUMP_TOPIC, OFF_PAYLOAD, 0, 0, 0, true);
    turnOffRelay2();
}

void updateOverflowPumpIfNeeded(bool overflowSensorActive) {
    if (autoMode && delayCounter > 5) {
        if (overflowSensorActive) {
            turnOnOverFlowPump();
        }
        else {
            turnOffOverFlowPump();
        }
        delayCounter = 0;
    }
    delayCounter++;
}

void readOverflowSensor() {
    int newButtonState = gpio_get_level(OVERFLOW_GPIO);
    ;
    if (buttonState != newButtonState || counter == 100) {
        if (newButtonState == GPIO_LEVEL_LOW) {
            ESP_LOGI(TAG, "overflow sensor ON");
            updateOverflowPumpIfNeeded(true);
            esp_mqtt_client_enqueue(mqtt_client, MQTT_OVERFLOW_SENSOR_TOPIC, ON_PAYLOAD, 0, 0, 0, true);
        }
        else {
            ESP_LOGI(TAG, "overflow sensor OFF");
            updateOverflowPumpIfNeeded(false);
            esp_mqtt_client_enqueue(mqtt_client, MQTT_OVERFLOW_SENSOR_TOPIC, OFF_PAYLOAD, 0, 0, 0, true);
        }
        counter = 0;
    }

    buttonState = newButtonState;
    counter++;
}

void loopOverflow() {
    while (1) {
        readOverflowSensor();

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void initOverflow() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    autoMode = isOverflowPumpAutoModeEnabled();

    ESP_LOGI(TAG, "overflow auto mode: %d", autoMode);

    xTaskCreate(loopOverflow, "overflow", 4096, NULL, 2, NULL);
}

void persistOverflowPumpAutoMode(bool enabled) {
    persistMemory(OVERFLOW_PUMP_MODE_KEY, enabled);
    autoMode = enabled;
}

