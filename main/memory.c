#include "memory.h"

#include "nvs_flash.h"
#include "esp_log.h"

static const char* TAG = "memory";

const char* namespace = "water-tank";

void initMemory() {
    ESP_LOGI(TAG, "init nvs flash");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

int readMemory(const char* key) {
    int32_t value = 0;
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open(namespace, NVS_READONLY, &my_handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return value;
    }
    ESP_ERROR_CHECK(err);

    err = nvs_get_i32(my_handle, key, &value);
    nvs_close(my_handle);
    return value;
}

void persistMemory(const char* key, int value) {
    nvs_handle_t my_handle;
    ESP_ERROR_CHECK(nvs_open(namespace, NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_i32(my_handle, key, value));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}