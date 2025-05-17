
#include "cJSON.h"
#include "mqtt.h"
#include "mqtt_topics.h"
#include "esp_log.h"

#define VERSION "1.0.0"

static const char* TAG = "ha-discovery";

static bool isPublished = false;

void addDeviceConfig(cJSON* root) {
    cJSON* device;
    device = cJSON_CreateObject();
    cJSON_AddStringToObject(device, "sw_version", VERSION);
    cJSON_AddStringToObject(device, "manufacturer", "zbl");
    cJSON_AddStringToObject(device, "identifiers", MQTT_DEVICE_ID);
    cJSON_AddStringToObject(device, "name", "Water tank");
    cJSON_AddStringToObject(device, "model", "Water tank");

    cJSON_AddItemToObject(root, "device", device);
}

void publishJson(const char* const topic, cJSON* json) {
    char* jsonString = cJSON_Print(json);

    esp_mqtt_client_enqueue(mqtt_client, topic, jsonString, 0, 0, true, true);
    cJSON_free(jsonString);
}

void publishDs18b20HaDiscovery() {
    ESP_LOGI(TAG, "publish Ds18b20HaDiscovery");
    cJSON* root;
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "Water temperature");
    cJSON_AddStringToObject(root, "availability_topic", MQTT_STATUS_TOPIC);
    cJSON_AddStringToObject(root, "device_class", "temperature");
    cJSON_AddStringToObject(root, "state_topic", MQTT_WATER_TEMP_TOPIC);
    cJSON_AddStringToObject(root, "unique_id", MQTT_WATER_TEMP_TOPIC);
    cJSON_AddStringToObject(root, "unit_of_measurement", "°C");

    addDeviceConfig(root);

    publishJson(MQTT_HA_DISCOVERY_WATER_TEMP_TOPIC, root);
    cJSON_Delete(root);
}

void publishConnectivityHaDiscovery() {
    ESP_LOGI(TAG, "publish ConnectivityHaDiscovery");
    cJSON* root;
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "Status");
    cJSON_AddStringToObject(root, "availability_topic", MQTT_STATUS_TOPIC);
    cJSON_AddStringToObject(root, "device_class", "connectivity");
    cJSON_AddStringToObject(root, "state_topic", MQTT_STATUS_TOPIC);
    cJSON_AddStringToObject(root, "unique_id", MQTT_STATUS_TOPIC);
    cJSON_AddStringToObject(root, "payload_on", ONLINE_PAYLOAD);
    cJSON_AddStringToObject(root, "payload_off", OFFLINE_PAYLOAD);

    addDeviceConfig(root);

    publishJson(MQTT_HA_DISCOVERY_CONNECTIVITY_TOPIC, root);
    cJSON_Delete(root);
}

void publishWaterLevelDiscovery() {
    ESP_LOGI(TAG, "publish WaterLevelDiscovery");
    cJSON* root;
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "Water level");
    cJSON_AddStringToObject(root, "availability_topic", MQTT_STATUS_TOPIC);
    cJSON_AddStringToObject(root, "state_topic", MQTT_WATER_LEVEL_TOPIC);
    cJSON_AddStringToObject(root, "unique_id", MQTT_WATER_LEVEL_TOPIC);
    cJSON_AddStringToObject(root, "unit_of_measurement", "%");

    addDeviceConfig(root);

    publishJson(MQTT_HA_DISCOVERY_WATER_LEVEL_TOPIC, root);
    cJSON_Delete(root);
}

void publishSensorMeasuredDistanceDiscovery() {
    ESP_LOGI(TAG, "publish SensorMeasuredDistanceDiscovery");
    cJSON* root;
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "Measured distance");
    cJSON_AddStringToObject(root, "availability_topic", MQTT_STATUS_TOPIC);
    cJSON_AddStringToObject(root, "state_topic", MQTT_SENSOR_MEASURED_DISTANCE_TOPIC);
    cJSON_AddStringToObject(root, "unique_id", MQTT_SENSOR_MEASURED_DISTANCE_TOPIC);
    cJSON_AddStringToObject(root, "entity_category", "diagnostic");
    cJSON_AddStringToObject(root, "unit_of_measurement", "cm");

    addDeviceConfig(root);

    publishJson(MQTT_HA_DISCOVERY_SENSOR_MEASURED_DISTANCE_TOPIC, root);
    cJSON_Delete(root);
}

void publishSensorHeightDiscovery() {
    ESP_LOGI(TAG, "publish SensorHeightDiscovery");
    cJSON* root;
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "Water level sensor height");
    cJSON_AddStringToObject(root, "availability_topic", MQTT_STATUS_TOPIC);
    cJSON_AddStringToObject(root, "device_class", "distance");
    cJSON_AddStringToObject(root, "state_topic", MQTT_SENSOR_HEIGHT_TOPIC);
    cJSON_AddStringToObject(root, "command_topic", MQTT_SENSOR_HEIGHT_TOPIC);
    cJSON_AddStringToObject(root, "unique_id", MQTT_SENSOR_HEIGHT_TOPIC);
    cJSON_AddStringToObject(root, "unit_of_measurement", "cm");
    cJSON_AddNumberToObject(root, "max", 200);
    cJSON_AddStringToObject(root, "entity_category", "config");
    cJSON_AddBoolToObject(root, "retain", true);

    addDeviceConfig(root);

    publishJson(MQTT_HA_DISCOVERY_SENSOR_LEVEL_TOPIC, root);
    cJSON_Delete(root);
}

void publishOverflowSensorDiscovery() {
    ESP_LOGI(TAG, "publish OverflowSensorDiscovery");
    cJSON* root;
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "Overflow sensor");
    cJSON_AddStringToObject(root, "availability_topic", MQTT_STATUS_TOPIC);
    cJSON_AddStringToObject(root, "state_topic", MQTT_OVERFLOW_SENSOR_TOPIC);
    cJSON_AddStringToObject(root, "unique_id", MQTT_OVERFLOW_SENSOR_TOPIC);

    addDeviceConfig(root);

    publishJson(MQTT_HA_DISCOVERY_OVERFLOW_SENSOR_LEVEL_TOPIC, root);
    cJSON_Delete(root);
}

void publishMaxWaterLevelDiscovery() {
    ESP_LOGI(TAG, "publish MaxWaterLevelDiscovery");
    cJSON* root;
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "Maximum water level");
    cJSON_AddStringToObject(root, "availability_topic", MQTT_STATUS_TOPIC);
    cJSON_AddStringToObject(root, "state_topic", MQTT_MAX_WATER_LEVEL_TOPIC);
    cJSON_AddStringToObject(root, "command_topic", MQTT_MAX_WATER_LEVEL_TOPIC);
    cJSON_AddStringToObject(root, "unique_id", MQTT_MAX_WATER_LEVEL_TOPIC);
    cJSON_AddStringToObject(root, "unit_of_measurement", "cm");
    cJSON_AddNumberToObject(root, "max", 200);
    cJSON_AddStringToObject(root, "entity_category", "config");
    cJSON_AddBoolToObject(root, "retain", true);

    addDeviceConfig(root);

    publishJson(MQTT_HA_DISCOVERY_MAX_WATER_LEVEL__TOPIC, root);
    cJSON_Delete(root);
}

void publishPumpHaDiscovery() {
    ESP_LOGI(TAG, "publish PumpHaDiscovery");
    cJSON* root;
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "Pump");
    cJSON_AddStringToObject(root, "availability_topic", MQTT_STATUS_TOPIC);
    cJSON_AddStringToObject(root, "icon", "mdi:water-pump");
    cJSON_AddStringToObject(root, "unique_id", MQTT_PUMP_TOPIC);
    cJSON_AddStringToObject(root, "command_topic", MQTT_PUMP_TOPIC);
    cJSON_AddStringToObject(root, "state_topic", MQTT_PUMP_TOPIC);

    addDeviceConfig(root);

    publishJson(MQTT_HA_DISCOVERY_PUMP_TOPIC, root);
    cJSON_Delete(root);
}



void publishOverflowPumpHaDiscovery() {
    ESP_LOGI(TAG, "publish OverflowPumpHaDiscovery");
    cJSON* root;
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "Overflow pump");
    cJSON_AddStringToObject(root, "availability_topic", MQTT_STATUS_TOPIC);
    cJSON_AddStringToObject(root, "icon", "mdi:water-pump");
    cJSON_AddStringToObject(root, "unique_id", MQTT_OVERFLOW_PUMP_TOPIC);
    cJSON_AddStringToObject(root, "command_topic", MQTT_OVERFLOW_PUMP_TOPIC);
    cJSON_AddStringToObject(root, "state_topic", MQTT_OVERFLOW_PUMP_TOPIC);

    addDeviceConfig(root);

    publishJson(MQTT_HA_DISCOVERY_OVERFLOW_PUMP_TOPIC, root);
    cJSON_Delete(root);
}

void publishOverflowPumpModeDiscovery() {
    ESP_LOGI(TAG, "publish OverflowPumpModeDiscovery");
    cJSON* root;
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "Overflow pump mode");
    cJSON_AddStringToObject(root, "availability_topic", MQTT_STATUS_TOPIC);
    cJSON_AddStringToObject(root, "state_topic", MQTT_OVERFLOW_PUMP_MODE_TOPIC);
    cJSON_AddStringToObject(root, "command_topic", MQTT_OVERFLOW_PUMP_MODE_TOPIC);
    cJSON_AddStringToObject(root, "unique_id", MQTT_OVERFLOW_PUMP_MODE_TOPIC);
    cJSON_AddStringToObject(root, "entity_category", "config");
    cJSON_AddBoolToObject(root, "retain", true);

    cJSON* options;
    options = cJSON_AddArrayToObject(root, "options");
    cJSON* element;
    element = cJSON_CreateString("manual");
    cJSON_AddItemToArray(options, element);
    element = cJSON_CreateString("auto");
    cJSON_AddItemToArray(options, element);

    addDeviceConfig(root);

    publishJson(MQTT_HA_DISCOVERY_OVERFLOW_PUMP_MODE_TOPIC, root);
    cJSON_Delete(root);
}

void publishHaDiscovery() {
    if (isPublished) {
        ESP_LOGI(TAG, "already published");
        return;
    }
    ESP_LOGI(TAG, "start publish ha discovery");
    publishDs18b20HaDiscovery();
    publishConnectivityHaDiscovery();
    publishWaterLevelDiscovery();
    publishSensorMeasuredDistanceDiscovery();
    publishSensorHeightDiscovery();
    publishOverflowSensorDiscovery();
    publishMaxWaterLevelDiscovery();
    publishPumpHaDiscovery();
    publishOverflowPumpHaDiscovery();
    publishOverflowPumpModeDiscovery();

    ESP_LOGI(TAG, "completed publish ha discovery");

    isPublished = true;
}
