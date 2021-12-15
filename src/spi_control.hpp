#pragma once
#include <Arduino.h>
#include "SPI.h"

const int pin_tdi = PB15;
const int pin_tms = PB11;
const int pin_tdo = PB14;
const int pin_tck = PB13;
const int pin_led_yellow = PB3;
const int pin_led_blue = PB4;
const int pin_sd_cs = PA4;

SPIClass jtag_spi(pin_tdi, pin_tdo, pin_tms);
SPISettings jtag_spi_settings(10000000, LSBFIRST, SPI_MODE0)
