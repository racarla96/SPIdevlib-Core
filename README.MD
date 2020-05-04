# Arduino SPI Library

SPIdev library for Arduino - Main SPI device class header file.

Abstracts bit and byte SPI R/W functions into a convenient class.



Based on I2Cdev library by Jeff Rowberg.

For compatibility with I2C interface. We use the similar interface but ignoring the unnecessary variables with the intention of having compatibility with sensors in I2Cdev library that have I2C and SPI interface.



2020-05-03 by Rafael Carbonell <racarla96@gmail.com>

## Idea based on:

[I2Cdev Library](https://github.com/jrowberg/i2cdevlib)

[MPU](https://github.com/bolderflight/MPU9250/blob/master/src/MPU9250.cpp)

## TODO:
- Real test with MPU6050
