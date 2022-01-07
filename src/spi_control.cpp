#include "spi_control.hpp"

SPIClass jtag_spi(pin_tdi, pin_tdo, pin_tck);
SPISettings jtag_spi_settings(24000000, LSBFIRST, SPI_MODE0);

void setup_jtag_spi_gpio() {
    pinMode(pin_tdi, OUTPUT);
    pinMode(pin_tms, OUTPUT);
    pinMode(pin_tck, OUTPUT);
    pinMode(pin_tdo, INPUT);
}