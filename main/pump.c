#include "pump.h"
#include "esp_log.h"
#include "driver/gpio.h"

static const char* TAG = "pump";

#define RELAY_1_GPIO GPIO_NUM_18
#define RELAY_2_GPIO GPIO_NUM_19

#define GPIO_LEVEL_LOW 0
#define GPIO_LEVEL_HIGH 1

#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<RELAY_1_GPIO) | (1ULL<<RELAY_2_GPIO))

void initPump() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    gpio_set_level(RELAY_1_GPIO, GPIO_LEVEL_HIGH);
    gpio_set_level(RELAY_2_GPIO, GPIO_LEVEL_HIGH);
}

void turnOnRelay1() {
    gpio_set_level(RELAY_1_GPIO, GPIO_LEVEL_LOW);
    ESP_LOGI(TAG, "relay 1 on");
}

void turnOffRelay1() {
    gpio_set_level(RELAY_1_GPIO, GPIO_LEVEL_HIGH);
    ESP_LOGI(TAG, "relay 1 off");
}

void turnOnRelay2() {
    gpio_set_level(RELAY_2_GPIO, GPIO_LEVEL_LOW);
    ESP_LOGI(TAG, "relay 2 on");
}

void turnOffRelay2() {
    gpio_set_level(RELAY_2_GPIO, GPIO_LEVEL_HIGH);
    ESP_LOGI(TAG, "relay 2 off");
}