# ESP32 Robots (& IoT) Framework

This framework is on develop state right now

### Purposes:
* Make different kinds of robots (and RC models such as quadrocopters)
* IoT projects
* Different kinds of embedded systems

### Main idea:
* User have to once flash firmware to ESP32 board
* Next step is to create program using RuC or lua
* Upload program then execute

## Getting started with development
1. Follow getting started guide from [esp-idf](http://esp-idf.readthedocs.io/en/latest/get-started/index.html)
2. Install and configure `minicom` for Linux or `PuTTY` for Windows
* Serial Device `/dev/ttyUSB0` for Linux
* Hardware flow control: `no`
* Software flow control: `no`
* Save your configuration as `esp32`
3. Install `nodejs`
4. Download `esp-idf` using submodules
* `git submodule init`
* `git submodule update`
5. Try to `make` the project
6. Use `./flash.sh` to flash firmware for the first time. Next ones you can use wifi OTA updates if your firmware configured
7. Congratulations! Now you can develop this Framework
