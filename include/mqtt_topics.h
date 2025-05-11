#pragma once

#include "credentials.h"

#define MQTT_STATUS_TOPIC "smarthome/" MQTT_DEVICE_ID "/status"
#define MQTT_PUMP_TOPIC "smarthome/" MQTT_DEVICE_ID "/pump/set"
#define MQTT_OVERFLOW_PUMP_TOPIC "smarthome/" MQTT_DEVICE_ID "/overflow-pump/set"
#define MQTT_OVERFLOW_PUMP_MODE_TOPIC "smarthome/" MQTT_DEVICE_ID "/overflow-pump/mode"
#define MQTT_WATER_LEVEL_TOPIC "smarthome/" MQTT_DEVICE_ID "/water-level"
#define MQTT_SENSOR_MEASURED_DISTANCE_TOPIC "smarthome/" MQTT_DEVICE_ID "/measured-distance"
#define MQTT_WATER_TEMP_TOPIC "smarthome/" MQTT_DEVICE_ID "/water/temperature"
#define MQTT_SENSOR_HEIGHT_TOPIC "smarthome/" MQTT_DEVICE_ID "/water-level/sensor-height"
#define MQTT_OVERFLOW_SENSOR_TOPIC "smarthome/" MQTT_DEVICE_ID "/water-level/overflow"
#define MQTT_MAX_WATER_LEVEL_TOPIC "smarthome/" MQTT_DEVICE_ID "/water-level/max-water-level"

#define MQTT_HA_DISCOVERY_WATER_TEMP_TOPIC "homeassistant/sensor/" MQTT_DEVICE_ID "/water-temperature/config"
#define MQTT_HA_DISCOVERY_PUMP_TOPIC "homeassistant/switch/" MQTT_DEVICE_ID "/pump/config"
#define MQTT_HA_DISCOVERY_OVERFLOW_PUMP_TOPIC "homeassistant/switch/" MQTT_DEVICE_ID "/overflow-pump/config"
#define MQTT_HA_DISCOVERY_OVERFLOW_PUMP_MODE_TOPIC "homeassistant/select/" MQTT_DEVICE_ID "/overflow-pump-mode/config"
#define MQTT_HA_DISCOVERY_WATER_LEVEL_TOPIC "homeassistant/sensor/" MQTT_DEVICE_ID "/water-level/config"
#define MQTT_HA_DISCOVERY_SENSOR_MEASURED_DISTANCE_TOPIC "homeassistant/sensor/" MQTT_DEVICE_ID "/measured-distance/config"
#define MQTT_HA_DISCOVERY_CONNECTIVITY_TOPIC "homeassistant/binary_sensor/" MQTT_DEVICE_ID "/status/config"
#define MQTT_HA_DISCOVERY_SENSOR_LEVEL_TOPIC "homeassistant/number/" MQTT_DEVICE_ID "/water-level-sensor-height/config"
#define MQTT_HA_DISCOVERY_OVERFLOW_SENSOR_LEVEL_TOPIC "homeassistant/binary_sensor/" MQTT_DEVICE_ID "/overflow-sensor/config"
#define MQTT_HA_DISCOVERY_MAX_WATER_LEVEL__TOPIC "homeassistant/number/" MQTT_DEVICE_ID "/max-water-level/config"

#define ONLINE_PAYLOAD "online"
#define OFFLINE_PAYLOAD "offline"

#define ON_PAYLOAD "ON"
#define OFF_PAYLOAD "OFF"