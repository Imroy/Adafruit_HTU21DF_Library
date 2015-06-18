/***************************************************
  This is a library for the HTU21D-F Humidity & Temp Sensor

  Designed specifically to work with the HTU21D-F sensor from Adafruit
  ----> https://www.adafruit.com/products/1899

  These displays use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <Wire.h>

#define HTU21DF_I2CADDR		0x40

enum HTU21DF_command {
  MEASTEMP		= 0xE3,
  MEASHUM		= 0xE5,
  MEASTEMP_NOHOLD	= 0xF3,
  MEASHUM_NOHOLD	= 0xF5,
  WRITEREG		= 0xE6,
  READREG		= 0xE7,
  RESET			= 0xFE,
};


class Adafruit_HTU21DF {
public:
  Adafruit_HTU21DF();
  boolean check(void);
  boolean begin(void);
  void reset(void);

  // up to 50 ms
  unsigned long measureTemperature(void) {
    measure(MEASTEMP_NOHOLD);
    return 50;
  }
  // up to 16 ms
  unsigned long measureHumidity(void) {
    measure(MEASHUM_NOHOLD);
    return 16;
  }

  bool readRawTemperature(void) { return readRaw(raw_t); }
  bool readRawHumidity(void) { return readRaw(raw_h); }

  uint16_t rawTemperature(void) const { return raw_t; }
  uint16_t rawHumidity(void) const { return raw_h; }

  float temperature(void) const;
  float humidity(void) const;
  float compensatedHumidity(void) const;

private:
  uint16_t raw_t, raw_h;
  bool error;

  uint8_t readUserReg(void);
  void measure(HTU21DF_command cmd);
  bool readRaw(uint16_t& raw);
  uint8_t calcCRC(uint16_t data, uint8_t crc = 0);
};

