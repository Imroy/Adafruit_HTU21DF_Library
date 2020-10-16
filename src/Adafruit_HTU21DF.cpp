/***************************************************
  This is a library for the HTU21DF Humidity & Temp Sensor

  Designed specifically to work with the HTU21DF sensor from Adafruit
  ----> https://www.adafruit.com/products/1899

  These displays use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_HTU21DF.h"

bool Adafruit_HTU21DF::check(void) {
  readUserReg();
  return !_error;
}

bool Adafruit_HTU21DF::begin(void) {
  reset();

  uint8_t reg = readUserReg();
  if (_error)
    return false;

  return (reg == static_cast<uint8_t>(Register_mask::Disable_OTP));
}

void Adafruit_HTU21DF::reset(void) {
  Wire.beginTransmission(HTU21DF_I2CADDR);
  Wire.write(static_cast<uint8_t>(Command::Reset));
  Wire.endTransmission();
  delay(15);
}


uint8_t Adafruit_HTU21DF::readUserReg(void) {
  Wire.beginTransmission(HTU21DF_I2CADDR);
  Wire.write(static_cast<uint8_t>(Command::ReadRegister));
  Wire.endTransmission();

  _error = false;
  if (Wire.requestFrom(HTU21DF_I2CADDR, 1) < 1) {
    _error = true;
    return 0;
  }

  int reg = Wire.read();
  if (reg == -1) {
    _error = true;
    return 0;
  }

  return reg;
}

void Adafruit_HTU21DF::writeUserReg(uint8_t value) {
  Wire.beginTransmission(HTU21DF_I2CADDR);
  Wire.write(static_cast<uint8_t>(Command::WriteRegister));
  Wire.write(value);
  Wire.endTransmission();
}

void Adafruit_HTU21DF::measure(Adafruit_HTU21DF::Command cmd) {
  Wire.beginTransmission(HTU21DF_I2CADDR);
  Wire.write(static_cast<uint8_t>(cmd));
  Wire.endTransmission();
}

uint16_t Adafruit_HTU21DF::readRaw(void) {
  if (Wire.requestFrom(HTU21DF_I2CADDR, 3) < 3) {
    _error = true;
    return 0;
  }

  while (!Wire.available()) {}

  _error = false;

  int high = Wire.read();
  if (high == -1) {
    _error = true;
    return 0;
  }

  int low = Wire.read();
  if (low == -1) {
    _error = true;
    return 0;
  }

  int checksum = Wire.read();
  if (checksum == -1) {
    _error = true;
    return 0;
  }

  uint16_t raw = static_cast<uint16_t>(high & 0xff) | (low & 0xff);
  uint8_t crc = calcCRC(raw, checksum);
  raw &= 0xfffc;		// remove the two LSB status bits
  if (crc != 0) {
    _error = true;
    return 0;
  }

  return raw;
}

bool Adafruit_HTU21DF::readRawTemperature(void) {
  uint16_t d = readRaw();
  if (_error) return false;

  _raw_t = d;
  return true;
}

bool Adafruit_HTU21DF::readRawHumidity(void) {
  uint16_t d = readRaw();
  if (_error) return false;

  _raw_h = d;
  return true;
}

/* Originally copied from SparkFun's slightly more complete library:
   https://github.com/sparkfun/HTU21D_Breakout/blob/master/Libraries/Arduino/src/SparkFunHTU21D.cpp
*/
uint8_t Adafruit_HTU21DF::calcCRC(uint16_t data, uint8_t crc) {
  uint32_t remainder = static_cast<uint32_t>(data) << 8;	// Pad with 8 bits because we have to add in the check value
  remainder |= crc;				// Add on the check value

  uint32_t divisor = 0x988000L;			// This is the polynomial shifted to farthest left of three bytes

  for (int i = 0 ; i < 16 ; i++) {		// Operate on only 16 positions of max 24. The remaining 8 are our remainder and should be zero when we're done.
    if (remainder & 1L << (23 - i))		// Check if there is a one in the left position
      remainder ^= divisor;

    divisor >>= 1;				// Rotate the divisor max 16 times so that we have 8 bits left of a remainder
  }

  return remainder & 0xff;
}
