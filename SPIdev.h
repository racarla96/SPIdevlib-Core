// SPIdev library collection - Main SPI device class header file
// Abstracts bit and byte SPI R/W functions into a convenient class
// Based on I2Cdev library interface by Jeff Rowberg 
// 2020-05-03 by Rafael Carbonell <racarla96@gmail.com>
//
// Changelog:
//      2020-05-?? - initial release

/* ============================================
SPIdev device library code 

MIT License

Copyright (c) 2020 racarla96

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===============================================
*/

#ifndef _SPIDEV_H_
#define _SPIDEV_H_

// -----------------------------------------------------------------------------
// Arduino-style "Serial.print" debug constant (uncomment to enable)
// -----------------------------------------------------------------------------
//#define SPIDEV_SERIAL_DEBUG

#include "Arduino.h"
#include <SPI.h> 

// Arduino SPI implementation doesn't support transfer timeout at least 
// 1000ms default read timeout (modify with "SPIdev::readTimeout = [ms];")
//#define SPIDEV_DEFAULT_READ_TIMEOUT     1000

#define READ 0B10000000
//#define WRITE 0B00000000 // Write is implicit

class SPIdev {
    public:
        static uint8_t slave;
        static uint8_t dataOrder;
        static SPISettings settings;

        SPIdev(int8_t slavePin, SPISettings settings, uint8_t bitOrder);

        static int8_t readBit(uint8_t regAddr, uint8_t bitNum, uint8_t *data);
        static int8_t readBitW(uint8_t regAddr, uint8_t bitNum, uint16_t *data);
        static int8_t readBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
        static int8_t readBitsW(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data);
        static int8_t readByte(uint8_t regAddr, uint8_t *data);
        static int8_t readWord(uint8_t regAddr, uint16_t *data);
        static int8_t readBytes(uint8_t regAddr, uint8_t length, uint8_t *data);
        static int8_t readWords(uint8_t regAddr, uint8_t length, uint16_t *data);

        static bool writeBit(uint8_t regAddr, uint8_t bitNum, uint8_t data);
        static bool writeBitW(uint8_t regAddr, uint8_t bitNum, uint16_t data);
        static bool writeBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
        static bool writeBitsW(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
        static bool writeByte(uint8_t regAddr, uint8_t data);
        static bool writeWord(uint8_t regAddr, uint16_t data);
        static bool writeBytes(uint8_t regAddr, uint8_t length, uint8_t *data);
        static bool writeWords(uint8_t regAddr, uint8_t length, uint16_t *data);

        /*
            For compatibility with I2C interface
            We use the similar interface but ignoring the unnecessary variables
            with the intention of having compatibility with sensors in I2Cdev library 
            that have I2C and SPI interface
            https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/I2Cdev/I2Cdev.h
            https://github.com/bolderflight/MPU9250/blob/master/src/MPU9250.cpp
        */
        static uint16_t readTimeout;
        
        static int8_t readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout=SPIdev::readTimeout);
        static int8_t readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data, uint16_t timeout=SPIdev::readTimeout);
        static int8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout=SPIdev::readTimeout);
        static int8_t readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data, uint16_t timeout=SPIdev::readTimeout);
        static int8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout=SPIdev::readTimeout);
        static int8_t readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout=SPIdev::readTimeout);
        static int8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout=SPIdev::readTimeout);
        static int8_t readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout=SPIdev::readTimeout);

        static bool writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
        static bool writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
        static bool writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
        static bool writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
        static bool writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
        static bool writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
        static bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
        static bool writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);

};

#endif
