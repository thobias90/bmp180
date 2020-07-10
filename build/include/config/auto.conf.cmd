deps_config := \
	/home/thobias/ESP32/esp-idf/components/app_trace/Kconfig \
	/home/thobias/ESP32/esp-idf/components/aws_iot/Kconfig \
	/home/thobias/ESP32/esp-idf/components/bt/Kconfig \
	/home/thobias/ESP32/esp-idf/components/driver/Kconfig \
	/home/thobias/ESP32/esp-idf/components/efuse/Kconfig \
	/home/thobias/ESP32/esp-idf/components/esp32/Kconfig \
	/home/thobias/ESP32/esp-idf/components/esp_adc_cal/Kconfig \
	/home/thobias/ESP32/esp-idf/components/esp_event/Kconfig \
	/home/thobias/ESP32/esp-idf/components/esp_http_client/Kconfig \
	/home/thobias/ESP32/esp-idf/components/esp_http_server/Kconfig \
	/home/thobias/ESP32/esp-idf/components/esp_https_ota/Kconfig \
	/home/thobias/ESP32/esp-idf/components/espcoredump/Kconfig \
	/home/thobias/ESP32/esp-idf/components/ethernet/Kconfig \
	/home/thobias/ESP32/esp-idf/components/fatfs/Kconfig \
	/home/thobias/ESP32/esp-idf/components/freemodbus/Kconfig \
	/home/thobias/ESP32/esp-idf/components/freertos/Kconfig \
	/home/thobias/ESP32/esp-idf/components/heap/Kconfig \
	/home/thobias/ESP32/esp-idf/components/libsodium/Kconfig \
	/home/thobias/ESP32/esp-idf/components/log/Kconfig \
	/home/thobias/ESP32/esp-idf/components/lwip/Kconfig \
	/home/thobias/ESP32/esp-idf/components/mbedtls/Kconfig \
	/home/thobias/ESP32/esp-idf/components/mdns/Kconfig \
	/home/thobias/ESP32/esp-idf/components/mqtt/Kconfig \
	/home/thobias/ESP32/esp-idf/components/nvs_flash/Kconfig \
	/home/thobias/ESP32/esp-idf/components/openssl/Kconfig \
	/home/thobias/ESP32/esp-idf/components/pthread/Kconfig \
	/home/thobias/ESP32/esp-idf/components/spi_flash/Kconfig \
	/home/thobias/ESP32/esp-idf/components/spiffs/Kconfig \
	/home/thobias/ESP32/esp-idf/components/tcpip_adapter/Kconfig \
	/home/thobias/ESP32/esp-idf/components/unity/Kconfig \
	/home/thobias/ESP32/esp-idf/components/vfs/Kconfig \
	/home/thobias/ESP32/esp-idf/components/wear_levelling/Kconfig \
	/home/thobias/ESP32/esp-idf/components/wifi_provisioning/Kconfig \
	/home/thobias/ESP32/esp-idf/components/app_update/Kconfig.projbuild \
	/home/thobias/ESP32/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/thobias/ESP32/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/thobias/ESP32/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/thobias/ESP32/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_TARGET)" "esp32"
include/config/auto.conf: FORCE
endif
ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
