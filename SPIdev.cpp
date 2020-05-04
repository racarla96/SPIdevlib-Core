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

#include "SPIdev.h"

/** Default constructor.
 */
SPIdev::SPIdev(int8_t slavePin, SPISettings settings, uint8_t bitOrder) {
    // Slave Pin
    slave = slavePin;
    // set the slaveSelectPin as an output:
    pinMode(slave, OUTPUT);
    // initialize SPI:
    SPI.begin();
    // Settings
    settings = settings;
    dataOrder = bitOrder;
}

/** Read a single bit from an 8-bit device register.
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @return Status of read operation (true = success)
 */
int8_t SPIdev::readBit(uint8_t regAddr, uint8_t bitNum, uint8_t *data) {
    uint8_t b;
    uint8_t count = readByte(regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}

/** Read a single bit from a 16-bit device register.
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-15)
 * @param data Container for single bit value
 * @return Status of read operation (true = success)
 */
int8_t SPIdev::readBitW(uint8_t regAddr, uint8_t bitNum, uint16_t *data) {
    uint16_t b;
    uint8_t count = readWord(regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}

/** Read multiple bits from an 8-bit device register.
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @return Status of read operation (true = success)
 */
int8_t SPIdev::readBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t count, b;
    if ((count = readByte(regAddr, &b)) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        b &= mask;
        b >>= (bitStart - length + 1);
        *data = b;
    }
    return count;
}

/** Read multiple bits from a 16-bit device register.
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-15)
 * @param length Number of bits to read (not more than 16)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @return Status of read operation (1 = success, 0 = failure, -1 = timeout)
 */
int8_t SPIdev::readBitsW(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data) {
    // 1101011001101001 read byte
    // fedcba9876543210 bit numbers
    //    xxx           args: bitStart=12, length=3
    //    010           masked
    //           -> 010 shifted
    uint8_t count;
    uint16_t w;
    if ((count = readWord(regAddr, &w)) != 0) {
        uint16_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        w &= mask;
        w >>= (bitStart - length + 1);
        *data = w;
    }
    return count;
}

/** Read single byte from an 8-bit device register.
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @return Status of read operation (true = success)
 */
int8_t SPIdev::readByte(uint8_t regAddr, uint8_t *data) {
    return readBytes(regAddr, 1, data);
}

/** Read single word from a 16-bit device register.
 * @param regAddr Register regAddr to read from
 * @param data Container for word value read from device
 * @return Status of read operation (true = success)
 */
int8_t SPIdev::readWord(uint8_t regAddr, uint16_t *data) {
    return readWords(regAddr, 1, data);
}

/** Read multiple bytes from an 8-bit device register.
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @return Number of bytes read (-1 indicates failure)
 */
int8_t SPIdev::readBytes(uint8_t regAddr, uint8_t length, uint8_t *data) {

    int8_t count = 0;

    #ifdef SPIDEV_SERIAL_DEBUG
        Serial.print("SPI reading ");
        Serial.print(length, DEC);
        Serial.print(" bytes from 0x");
        Serial.print(regAddr, HEX);
        Serial.print("...");
    #endif

    SPI.beginTransaction(settings);

    // take the slave pin low to select the chip:
    digitalWrite(slave, LOW);

    SPI.transfer(regAddr | READ); // specify the starting register address
    for(; count < length; count++){
        data[count] = SPI.transfer(0x00); // read the data

        #ifdef SPIDEV_SERIAL_DEBUG
            Serial.print(data[count], HEX);
            if (count + 1 < length) Serial.print(" ");
        #endif
    }

    // take the slave pin high to de-select the chip:
    digitalWrite(slave, HIGH);

    SPI.endTransaction();

    #ifdef SPIDEV_SERIAL_DEBUG
        Serial.print(". Done (");
        Serial.print(count, DEC);
        Serial.println(" read).");
    #endif

    return count;
}

/** Read multiple words from a 16-bit device register.
 * @param regAddr First register regAddr to read from
 * @param length Number of words to read
 * @param data Buffer to store read data in
 * @return Number of words read (-1 indicates failure)
 */
int8_t SPIdev::readWords(uint8_t regAddr, uint8_t length, uint16_t *data) {

    int8_t count = 0;

    #ifdef SPIDEV_SERIAL_DEBUG
        Serial.print("SPI reading ");
        Serial.print(length, DEC);
        Serial.print(" bytes from 0x");
        Serial.print(regAddr, HEX);
        Serial.print("...");
    #endif

    SPI.beginTransaction(settings);

    // take the slave pin low to select the chip:
    digitalWrite(slave, LOW);

    // SPI subsystem uses internal buffer that breaks with large data requests
    // so if user requests more than BUFFER_LENGTH bytes, we have to do it in
    // smaller chunks instead of all at once
    bool msb = dataOrder; // starts with MSB, then LSB
    SPI.transfer(regAddr | READ); // specify the starting register address
    for(; count < length;){
        if (msb == MSBFIRST) {
            // first byte is bits 15-8 (MSb=15)
            data[count] = SPI.transfer(0x00) << 8; // read the data
            msb = LSBFIRST;
        } else {
            // second byte is bits 7-0 (LSb=0)
            data[count] |= SPI.transfer(0x00); // read the data
            #ifdef SPIDEV_SERIAL_DEBUG
                Serial.print(data[count], HEX);
                if (count + 1 < length) Serial.print(" ");
            #endif
            count++;
            msb = MSBFIRST;
        }
    }

    // take the slave pin high to de-select the chip:
    digitalWrite(slave, HIGH);

    SPI.endTransaction();

    #ifdef SPIDEV_SERIAL_DEBUG
        Serial.print(". Done (");
        Serial.print(count, DEC);
        Serial.println(" read).");
    #endif

    return count;
}

/** write a single bit in an 8-bit device register.
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
bool SPIdev::writeBit(uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    uint8_t b;
    readByte(regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return writeByte(regAddr, b);
}

/** write a single bit in a 16-bit device register.
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-15)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
bool SPIdev::writeBitW(uint8_t regAddr, uint8_t bitNum, uint16_t data) {
    uint16_t w;
    readWord(regAddr, &w);
    w = (data != 0) ? (w | (1 << bitNum)) : (w & ~(1 << bitNum));
    return writeWord(regAddr, w);
}

/** Write multiple bits in an 8-bit device register.
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
bool SPIdev::writeBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b;
    if (readByte(regAddr, &b) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
        return writeByte(regAddr, b);
    } else {
        return false;
    }
}

/** Write multiple bits in a 16-bit device register.
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-15)
 * @param length Number of bits to write (not more than 16)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
bool SPIdev::writeBitsW(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data) {
    //              010 value to write
    // fedcba9876543210 bit numbers
    //    xxx           args: bitStart=12, length=3
    // 0001110000000000 mask word
    // 1010111110010110 original value (sample)
    // 1010001110010110 original & ~mask
    // 1010101110010110 masked | value
    uint16_t w;
    if (readWord(regAddr, &w) != 0) {
        uint16_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        w &= ~(mask); // zero all important bits in existing word
        w |= data; // combine data with existing word
        return writeWord(regAddr, w);
    } else {
        return false;
    }
}

/** Write single byte to an 8-bit device register.
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
bool SPIdev::writeByte(uint8_t regAddr, uint8_t data) {
    return writeBytes(regAddr, 1, &data);
}

/** Write single word to a 16-bit device register.
 * @param regAddr Register address to write to
 * @param data New word value to write
 * @return Status of operation (true = success)
 */
bool SPIdev::writeWord(uint8_t regAddr, uint16_t data) {
    return writeWords(regAddr, 1, &data);
}

/** Write multiple bytes to an 8-bit device register.
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
bool SPIdev::writeBytes(uint8_t regAddr, uint8_t length, uint8_t* data) {

    #ifdef SPIDEV_SERIAL_DEBUG
        Serial.print("SPI writing ");
        Serial.print(length, DEC);
        Serial.print(" bytes to 0x");
        Serial.print(regAddr, HEX);
        Serial.print("...");
    #endif

    uint8_t status = 0;

    SPI.beginTransaction(settings);

    // take the slave pin low to select the chip:
    digitalWrite(slave, LOW);

    SPI.transfer(regAddr); // specify the starting register address
    for (uint8_t i = 0; i < length; i++) {
        #ifdef SPIDEV_SERIAL_DEBUG
            Serial.print(data[i], HEX);
            if (i + 1 < length) Serial.print(" ");
        #endif

        SPI.transfer((uint8_t) data[i]); // send the data
    }

    // take the slave pin high to de-select the chip:
    digitalWrite(slave, HIGH);

    SPI.endTransaction();

    #ifdef SPIDEV_SERIAL_DEBUG
        Serial.println(". Done.");
    #endif
    
    return status == 0;
}

/** Write multiple words to a 16-bit device register.
 * @param regAddr First register address to write to
 * @param length Number of words to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
bool SPIdev::writeWords(uint8_t regAddr, uint8_t length, uint16_t* data) {
    #ifdef SPIDEV_SERIAL_DEBUG
        Serial.print("SPI writing ");
        Serial.print(length, DEC);
        Serial.print(" words to 0x");
        Serial.print(regAddr, HEX);
        Serial.print("...");
    #endif
    uint8_t status = 0;

    SPI.beginTransaction(settings);

    // take the slave pin low to select the chip:
    digitalWrite(slave, LOW);

    SPI.transfer(regAddr); // specify the starting register address
    for (uint8_t i = 0; i < length; i++) {
        #ifdef SPIDEV_SERIAL_DEBUG
            Serial.print(data[i], HEX);
            if (i + 1 < length) Serial.print(" ");
        #endif
        SPI.transfer((uint8_t)(data[i] >> 8));     // send MSB
        SPI.transfer((uint8_t)data[i]);          // send LSB
    }

    // take the slave pin high to de-select the chip:
    digitalWrite(slave, HIGH);

    SPI.endTransaction();

    #ifdef SPIDEV_SERIAL_DEBUG
        Serial.println(". Done.");
    #endif

    return status == 0;
}

/** Default timeout value for read operations.
 * Set this to 0 to disable timeout detection.
 */
//uint16_t SPIdev::readTimeout = SPIDEV_DEFAULT_READ_TIMEOUT;
uint16_t SPIdev::readTimeout = 0;


/* STUBS */
int8_t SPIdev::readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout) {
    return readBit(regAddr, bitNum, data);
}
int8_t SPIdev::readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data, uint16_t timeout) {
    return readBitW(regAddr, bitNum, data);
}
int8_t SPIdev::readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout) {
    return readBits(regAddr, bitStart, length, data);
}
int8_t SPIdev::readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data, uint16_t timeout) {
    return readBitsW(regAddr, bitStart, length, data);
}
int8_t SPIdev::readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout) {
    return readByte(regAddr, data);
}
int8_t SPIdev::readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout) {
    return readWord(regAddr, data);
}
int8_t SPIdev::readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout) {
    return readBytes(regAddr, length, data);
}
int8_t SPIdev::readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout) {
    return readWords(regAddr, length, data);
}

bool SPIdev::writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    return writeBit(regAddr, bitNum, data);
}
bool SPIdev::writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data) {
    return writeBitW(regAddr, bitNum, data);
}
bool SPIdev::writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    return writeBits(regAddr, bitStart, length, data);
}
bool SPIdev::writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data) {
    return writeBitsW(regAddr, bitStart, length, data);
}
bool SPIdev::writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    return writeByte(regAddr, data);
}
bool SPIdev::writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data) {
    return writeWord(regAddr, data);
}
bool SPIdev::writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {
    return writeBytes(regAddr, length, data);
}
bool SPIdev::writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data) {
    return writeWords(regAddr, length, data);
}
