#include "SPI.h"
#include "SPIdev.h"

const uint32_t SPI_LS_CLOCK = 1000000;  // 1 MHz
const uint32_t SPI_HS_CLOCK = 15000000; // 15 MHz
SPISettings settings(SPI_HS_CLOCK, MSBFIRST, SPI_MODE3);
SPIdev spidev(10, settings, MSBFIRST);

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:

}
