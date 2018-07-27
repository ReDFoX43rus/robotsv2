#!/bin/bash
sudo ~/esp/xtensa-esp32-elf/bin/xtensa-esp32-elf-gdb build/esp32_robots.elf -b 115200 -ex 'target remote /dev/ttyUSB1'
