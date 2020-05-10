#include "SPI.h"
#include "SPIdev.h"

const uint32_t SPI_LS_CLOCK = 1000000;  // 1 MHz
const uint32_t SPI_HS_CLOCK = 15000000; // 15 MHz
SPISettings settings(SPI_LS_CLOCK, MSBFIRST, SPI_MODE3);
SPIdev spidev(10, settings, MSBFIRST);

const uint8_t ACCEL_OUT = 0x3B;
uint8_t data;

void setup() {
  Serial.begin(115200);
}

void loop() {
  spidev.readByte(ACCEL_OUT, data);
  Serial.println(data);
  delay(1000);
}
