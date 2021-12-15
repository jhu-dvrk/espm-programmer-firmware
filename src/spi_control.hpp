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

extern SPIClass jtag_spi;
extern SPISettings jtag_spi_settings;

void setup_jtag_spi_gpio();