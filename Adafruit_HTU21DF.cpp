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

Adafruit_HTU21DF::Adafruit_HTU21DF() {
}


boolean Adafruit_HTU21DF::check(void) {
  readUserReg();
  return !error;
}

boolean Adafruit_HTU21DF::begin(void) {
  reset();

  uint8_t reg = readUserReg();
  if (error)
    return false;

  return (reg == 0x2); // after reset user register should be 0x2
}

void Adafruit_HTU21DF::reset(void) {
  Wire.beginTransmission(HTU21DF_I2CADDR);
  Wire.write(HTU21DF_RESET);
  Wire.endTransmission();
  delay(15);
}


uint8_t Adafruit_HTU21DF::readUserReg(void) {
  Wire.beginTransmission(HTU21DF_I2CADDR);
  Wire.write(HTU21DF_READREG);
  Wire.endTransmission();
  Wire.requestFrom(HTU21DF_I2CADDR, 1);

  int reg = Wire.read();
  error = false;
  if (reg == -1) {
    error = true;
    return 0;
  }

  return reg;
}

void Adafruit_HTU21DF::measure(uint8_t addr) {
  Wire.beginTransmission(HTU21DF_I2CADDR);
  Wire.write(addr);
  Wire.endTransmission();
}

bool Adafruit_HTU21DF::readRaw(uint16_t& raw) {
  Wire.requestFrom(HTU21DF_I2CADDR, 3);
  while (!Wire.available()) {}

  int d;
  d = Wire.read();
  if (d == -1)
    return false;
  raw = (uint8_t)d << 8;
  d = Wire.read();
  if (d == -1)
    return false;
  raw |= (uint8_t)d;

  d = Wire.read();
  if (d == -1)
    return false;
  uint8_t checksum = d;
  uint8_t crc = calcCRC(raw, checksum);
  raw &= 0xfffc;		// remove the two LSB status bits

  return (crc == 0);
}

float Adafruit_HTU21DF::temperature(void) {
  float temp = raw_t;
  temp *= 175.72;
  temp /= 65536;
  temp -= 46.85;

  return temp;
}
  
float Adafruit_HTU21DF::humidity(void) {
  float hum = raw_h;
  hum *= 125;
  hum /= 65536;
  hum -= 6;

  return hum;
}

float Adafruit_HTU21DF::compensatedHumidity(void) {
  float t = temperature();
  float h = humidity();
  return h + ((25 - t) * -0.15);
}

/* Copied from SparkFun's slightly more complete library:
   https://github.com/sparkfun/HTU21D_Breakout/blob/master/library/HTU21D_Humidity/HTU21D.cpp
*/
uint8_t Adafruit_HTU21DF::calcCRC(uint16_t data, uint8_t crc) {
  uint32_t remainder = (uint32_t)data << 8;	// Pad with 8 bits because we have to add in the check value
  remainder |= crc;				// Add on the check value

  uint32_t divsor = (uint32_t)0x988000;		// This is the polynomial shifted to farthest left of three bytes

  for (int i = 0 ; i < 16 ; i++) {		// Operate on only 16 positions of max 24. The remaining 8 are our remainder and should be zero when we're done.
    if (remainder & (uint32_t)1<<(23 - i))	// Check if there is a one in the left position
      remainder ^= divsor;

    divsor >>= 1;				//Rotate the divsor max 16 times so that we have 8 bits left of a remainder
  }

  return (uint8_t)remainder && 0xff;
}




/*********************************************************************/
