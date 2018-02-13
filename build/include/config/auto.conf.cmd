deps_config := \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/app_trace/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/aws_iot/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/bt/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/esp32/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/ethernet/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/fatfs/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/freertos/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/heap/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/libsodium/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/log/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/lwip/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/mbedtls/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/openssl/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/pthread/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/spi_flash/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/spiffs/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/tcpip_adapter/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/wear_levelling/Kconfig \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/liberaid/git/robotsv2/3rd-party/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
