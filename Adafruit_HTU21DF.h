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
#define HTU21DF_READTEMP	0xE3
#define HTU21DF_READHUM		0xE5
#define HTU21DF_WRITEREG	0xE6
#define HTU21DF_READREG		0xE7
#define HTU21DF_RESET		0xFE



class Adafruit_HTU21DF {
public:
  Adafruit_HTU21DF();
  boolean begin(void);
  void reset(void);
  bool readTemperature(void) { return readRaw(HTU21DF_READTEMP, raw_t); }
  bool readHumidity(void) { return readRaw(HTU21DF_READHUM, raw_h); }
  uint16_t rawTemperature(void) { return raw_t; }
  uint16_t rawHumidity(void) { return raw_h; }
  float temperature(void);
  float humidity(void);
  float compensatedHumidity(void);

private:
  uint16_t raw_t, raw_h;

  bool readRaw(uint8_t addr, uint16_t& raw);
  uint8_t calcCRC(uint16_t data, uint8_t crc = 0);
};

