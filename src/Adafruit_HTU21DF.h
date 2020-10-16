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
#pragma once

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <Wire.h>

#define HTU21DF_I2CADDR		0x40

class Adafruit_HTU21DF {
 private:
  uint16_t _raw_t, _raw_h;
  bool _error;

  enum class Command : uint8_t {
    MeasureTemperature		= 0xe3,
    MeasureHumidity		= 0xe5,
    WriteRegister		= 0xe6,
    ReadRegister		= 0xe7,

    MeasureTemperature_nohold	= 0xf3,
    MeasureHumidity_nohold	= 0xf5,
    Reset			= 0xfe,
  };

  enum class Register_mask : uint8_t {
    Measurement_resolution	= 0x81,
    Disable_OTP			= 0x02,
    Enable_heater		= 0x04,

    End_of_battery		= 0x40,
  };

  enum class Measurement_resolution : uint8_t {
    RH12_T14			= 0x00,
    RH08_T12			= 0x01,
    RH10_T13			= 0x80,
    RH11_T11			= 0x81,
  };

  uint8_t readUserReg(void);
  void writeUserReg(uint8_t value);
  void measure(Command cmd);
  uint16_t readRaw(void);
  uint8_t calcCRC(uint16_t data, uint8_t crc = 0);

 public:
  Adafruit_HTU21DF() {}
  bool check(void);
  bool begin(void);
  void reset(void);

  // up to 50 ms
  unsigned long measureTemperature(void) {
    measure(Command::MeasureTemperature_nohold);
    return 50;
  }
  // up to 16 ms
  unsigned long measureHumidity(void) {
    measure(Command::MeasureHumidity_nohold);
    return 16;
  }

  bool readRawTemperature(void);
  bool readRawHumidity(void);

  uint16_t rawTemperature(void) const { return _raw_t; }
  uint16_t rawHumidity(void) const { return _raw_h; }

  float temperature(void) const { return ((static_cast<float>(_raw_t) * 175.72) / 65536) - 46.85; }
  float humidity(void) const { return ((static_cast<float>(_raw_h) * 125) / 65536) - 6; }
  float compensatedHumidity(void) const { return humidity() + ((25 - temperature()) * -0.15); }

}; // class Adafruit_HTU21DF

