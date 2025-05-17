# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/main/*.*)

idf_component_register(SRCS ${app_sources}
                       INCLUDE_DIRS "../include"
                       REQUIRES esp_partition esp_netif nvs_flash driver esp-tls json mqtt ds18x20
                       PRIV_REQUIRES esp_driver_gpio esp_eth)
