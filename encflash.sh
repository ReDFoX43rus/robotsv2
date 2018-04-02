#!/bin/bash
3rd-party/esp-idf/components/esptool_py/esptool/espsecure.py encrypt_flash_data --keyfile flash_encryption_key.bin --address 0x10000 -o build/esp32_robots-encrypted.bin build/esp32_robots.bin
sudo esptool.py --port /dev/ttyUSB0 --baud 115200 write_flash 0x10000 build/esp32_robots-encrypted.bin
sudo minicom esp32
