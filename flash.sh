#!/bin/bash
sudo esptool.py --chip esp32 --port /dev/ttyUSB1 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 build/bootloader/bootloader.bin 0x10000 build/esp32_robots.bin 0x8000 build/partitions.bin
#sudo minicom esp32
