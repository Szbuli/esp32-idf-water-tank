#include "esp_event.h"
#include "mqtt_client.h"
#include "credentials.h"
#include "esp_log.h"
#include "mqtt_topics.h"
#include "overflow.h"
#include "pump.h"
#include "a02yyuw.h"
#include "ha_discovery.h"

#define BROKER_URL "mqtts://mqtt.zbl.app"

static const char* TAG = "mqtt";

esp_mqtt_client_handle_t mqtt_client;

void mqttEventData(esp_mqtt_event_handle_t event) {
    char* topic = strndup(event->topic, event->topic_len);
    topic[event->topic_len] = '\0';

    char* payload = strndup(event->data, event->data_len);
    payload[event->data_len] = '\0';

    ESP_LOGV(TAG, "mqtt message arrived, topic: %s", topic);
    ESP_LOGV(TAG, "mqtt payload: %s", payload);

    if (strcmp(topic, MQTT_PUMP_TOPIC) == 0) {
        if ((char)payload[1] == 'N') {
            turnOnRelay1();
        }
        else {
            turnOffRelay1();
        }
    }
    else if (strcmp(topic, MQTT_OVERFLOW_PUMP_TOPIC) == 0) {
        if (!isOverflowPumpAutoModeEnabled()) {
            if ((char)payload[1] == 'N') {
                turnOnRelay2();
            }
            else {
                turnOffRelay2();
            }
        } else {
            ESP_LOGI(TAG, "overflow in auto mode, ignoring overflow pump change");
        }
    }
    else if (strcmp(topic, MQTT_SENSOR_HEIGHT_TOPIC) == 0) {
        persistSensorHeight(atoi(payload));
    }
    else if (strcmp(topic, MQTT_MAX_WATER_LEVEL_TOPIC) == 0) {
        persistMaxWaterLevel(atoi(payload));
    }
    else if (strcmp(topic, MQTT_OVERFLOW_PUMP_MODE_TOPIC) == 0) {
        persistOverflowPumpAutoMode((char)payload[0] == 'a');
    }
    else {
        ESP_LOGW(TAG, "topic not matched %s", topic);
    }
}


void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

        esp_mqtt_client_enqueue(mqtt_client, MQTT_STATUS_TOPIC, ONLINE_PAYLOAD, 0, 0, true, true);

        esp_mqtt_client_subscribe(mqtt_client, MQTT_PUMP_TOPIC, 0);
        esp_mqtt_client_subscribe(mqtt_client, MQTT_OVERFLOW_PUMP_TOPIC, 0);
        esp_mqtt_client_subscribe(mqtt_client, MQTT_SENSOR_HEIGHT_TOPIC, 0);
        esp_mqtt_client_subscribe(mqtt_client, MQTT_MAX_WATER_LEVEL_TOPIC, 0);
        esp_mqtt_client_subscribe(mqtt_client, MQTT_OVERFLOW_PUMP_MODE_TOPIC, 0);

        publishHaDiscovery();
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGV(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGV(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGV(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        mqttEventData(event);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGE(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGE(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGE(TAG, "Last captured errno : %d (%s)", event->error_handle->esp_transport_sock_errno,
                strerror(event->error_handle->esp_transport_sock_errno));
        }
        else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGE(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        }
        else {
            ESP_LOGE(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start()
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address.uri = BROKER_URL,
            .verification.certificate = MQTT_CA_CERT,
        },
        .session = {
            .keepalive = 60,
            .last_will = {
                .topic = MQTT_STATUS_TOPIC,
                .msg = OFFLINE_PAYLOAD,
                .qos = 0,
                .retain = true,
            },
        },
        .credentials.username = MQTT_USERNAME,
            .credentials.authentication.password = MQTT_PASSWORD,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}