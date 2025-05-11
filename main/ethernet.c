#include <string.h>
#include "esp_event.h"
#include "esp_eth.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_err.h"
#include "sdkconfig.h"
#include "esp_netif.h"
#include "esp_eth_driver.h"
#include "esp_eth_mac.h"
#include "esp_eth_mac_spi.h"
#include "esp_mac.h"

#define CONFIG_CONNECT_ETHERNET 1
#define CONFIG_CONNECT_WIFI n
#define CONFIG_USE_INTERNAL_ETHERNET y
#define CONFIG_ETH_MDC_GPIO 23
#define CONFIG_ETH_MDIO_GPIO 18
#define CONFIG_ETH_PHY_RST_GPIO 5
#define CONFIG_ETH_PHY_ADDR 1
#define CONFIG_ETHERNET_EMAC_TASK_STACK_SIZE 3072

#define CONFIG_USE_W5500 1
#define CONFIG_ETH_SPI_HOST 2
#define CONFIG_ETH_SPI_SCLK_GPIO 14
#define CONFIG_ETH_SPI_MOSI_GPIO 13
#define CONFIG_ETH_SPI_MISO_GPIO 12
#define CONFIG_ETH_SPI_CS_GPIO 15
#define CONFIG_ETH_SPI_CLOCK_MHZ 1 // 36
#define CONFIG_ETH_SPI_INT_GPIO 4
#define CONFIG_ETH_PHY_RST_GPIO 5
#define CONFIG_ETH_PHY_ADDR 1

#define NETIF_DESC_ETH "netif_eth"


static const char* TAG = "ethernet";
static SemaphoreHandle_t s_semph_get_ip_addrs = NULL;

static esp_netif_t* eth_start(void);
static void eth_stop(void);

/**
 * @brief Checks the netif description if it contains specified prefix.
 * All netifs created within common connect component are prefixed with the module TAG,
 * so it returns true if the specified netif is owned by this module
 */
bool is_our_netif(const char* prefix, esp_netif_t* netif)
{
    return strncmp(prefix, esp_netif_get_desc(netif), strlen(prefix) - 1) == 0;
}

static bool netif_desc_matches_with(esp_netif_t* netif, void* ctx)
{
    return strcmp(ctx, esp_netif_get_desc(netif)) == 0;
}

esp_netif_t* get_netif_from_desc(const char* desc)
{
    return esp_netif_find_if(netif_desc_matches_with, (void*)desc);
}


/** Event handler for Ethernet events */

static void eth_on_got_ip(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data)
{
    ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
    if (!is_our_netif(NETIF_DESC_ETH, event->esp_netif)) {
        return;
    }
    ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR, esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));
    xSemaphoreGive(s_semph_get_ip_addrs);
}

static void eth_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_id == ETHERNET_EVENT_DISCONNECTED) {
        ESP_LOGI("ETH", "Ethernet disconnected");
    }
    else if (event_id == ETHERNET_EVENT_CONNECTED) {
        ESP_LOGW("ETH", "Ethernet connected");
    }
}

static esp_eth_handle_t s_eth_handle = NULL;
static esp_eth_mac_t* s_mac = NULL;
static esp_eth_phy_t* s_phy = NULL;
static esp_eth_netif_glue_handle_t s_eth_glue = NULL;

static esp_netif_t* eth_start(void)
{
    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();
    // Warning: the interface desc is used in tests to capture actual connection details (IP, gw, mask)
    esp_netif_config.if_desc = NETIF_DESC_ETH;
    esp_netif_config.route_prio = 64;
    esp_netif_config_t netif_config = {
        .base = &esp_netif_config,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH
    };
    esp_netif_t* netif = esp_netif_new(&netif_config);
    assert(netif);

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    mac_config.rx_task_stack_size = CONFIG_ETHERNET_EMAC_TASK_STACK_SIZE;
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = CONFIG_ETH_PHY_ADDR;
    phy_config.reset_gpio_num = CONFIG_ETH_PHY_RST_GPIO;

    gpio_install_isr_service(0);
    spi_bus_config_t buscfg = {
        .miso_io_num = CONFIG_ETH_SPI_MISO_GPIO,
        .mosi_io_num = CONFIG_ETH_SPI_MOSI_GPIO,
        .sclk_io_num = CONFIG_ETH_SPI_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(CONFIG_ETH_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
    spi_device_interface_config_t spi_devcfg = {
        .mode = 0,
        .clock_speed_hz = CONFIG_ETH_SPI_CLOCK_MHZ * 1000 * 1000,
        .spics_io_num = CONFIG_ETH_SPI_CS_GPIO,
        .queue_size = 20
    };

    /* w5500 ethernet driver is based on spi driver */
    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(CONFIG_ETH_SPI_HOST, &spi_devcfg);
    w5500_config.int_gpio_num = CONFIG_ETH_SPI_INT_GPIO;
    s_mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);
    s_phy = esp_eth_phy_new_w5500(&phy_config);

    // Install Ethernet driver
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(s_mac, s_phy);
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &s_eth_handle));

    /* The SPI Ethernet module might doesn't have a burned factory MAC address, we cat to set it manually.
       We set the ESP_MAC_ETH mac address as the default, if you want to use ESP_MAC_EFUSE_CUSTOM mac address, please enable the
       configuration: `ESP_MAC_USE_CUSTOM_MAC_AS_BASE_MAC`
    */
    uint8_t eth_mac[6] = { 0 };
    ESP_ERROR_CHECK(esp_read_mac(eth_mac, ESP_MAC_ETH));
    ESP_ERROR_CHECK(esp_eth_ioctl(s_eth_handle, ETH_CMD_S_MAC_ADDR, eth_mac));

    // combine driver with netif
    s_eth_glue = esp_eth_new_netif_glue(s_eth_handle);
    esp_netif_attach(netif, s_eth_glue);

    // Register user defined event handers
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &eth_on_got_ip, NULL));
    esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &eth_event_handler, NULL);
    esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_CONNECTED, &eth_event_handler, NULL);

    esp_eth_start(s_eth_handle);
    return netif;
}

static void eth_stop(void)
{
    esp_netif_t* eth_netif = get_netif_from_desc(NETIF_DESC_ETH);
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, &eth_on_got_ip));

    ESP_ERROR_CHECK(esp_eth_stop(s_eth_handle));
    ESP_ERROR_CHECK(esp_eth_del_netif_glue(s_eth_glue));
    ESP_ERROR_CHECK(esp_eth_driver_uninstall(s_eth_handle));
    s_eth_handle = NULL;
    ESP_ERROR_CHECK(s_phy->del(s_phy));
    ESP_ERROR_CHECK(s_mac->del(s_mac));

    esp_netif_destroy(eth_netif);
}

esp_eth_handle_t get_eth_handle(void)
{
    return s_eth_handle;
}

/* tear down connection, release resources */
void shutdown(void)
{
    if (s_semph_get_ip_addrs == NULL) {
        return;
    }
    vSemaphoreDelete(s_semph_get_ip_addrs);
    s_semph_get_ip_addrs = NULL;

    eth_stop();
}

esp_err_t connect(void)
{
    s_semph_get_ip_addrs = xSemaphoreCreateBinary();
    if (s_semph_get_ip_addrs == NULL) {
        return ESP_ERR_NO_MEM;
    }

    eth_start();
    ESP_LOGI(TAG, "Waiting for IP(s).");

    xSemaphoreTake(s_semph_get_ip_addrs, portMAX_DELAY);

    return ESP_OK;
}

static esp_err_t print_all_ips_tcpip(void* ctx)
{
    const char* prefix = ctx;
    // iterate over active interfaces, and print out IPs of "our" netifs
    esp_netif_t* netif = NULL;
    while ((netif = esp_netif_next_unsafe(netif)) != NULL) {
        if (is_our_netif(prefix, netif)) {
            ESP_LOGI(TAG, "Connected to %s", esp_netif_get_desc(netif));
            esp_netif_ip_info_t ip;
            ESP_ERROR_CHECK(esp_netif_get_ip_info(netif, &ip));

            ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",", IP2STR(&ip.ip));
        }
    }
    return ESP_OK;
}

void print_all_netif_ips(const char* prefix)
{
    // Print all IPs in TCPIP context to avoid potential races of removing/adding netifs when iterating over the list
    esp_netif_tcpip_exec(print_all_ips_tcpip, (void*)prefix);
}

esp_err_t ethernet_connect(void)
{
    if (connect() != ESP_OK) {
        return ESP_FAIL;
    }
    ESP_ERROR_CHECK(esp_register_shutdown_handler(&shutdown));

    print_all_netif_ips(NETIF_DESC_ETH);

    return ESP_OK;
}


