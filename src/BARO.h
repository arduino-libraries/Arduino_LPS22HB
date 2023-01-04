/*
  This file is part of the Arduino_LPS22HB library.
  Copyright (c) 2019 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _BARO_H_
#define _BARO_H_

#include <Arduino.h>
#include <Wire.h>

enum {
  PSI,
  MILLIBAR,
  KILOPASCAL
};

enum {
  RATE_ONE_SHOT = 0,
  RATE_1_HZ = 1,
  RATE_10_HZ = 2,
  RATE_25_HZ = 3,
  RATE_50_HZ = 4,
  RATE_75_HZ = 5,
};

class LPS22HBClass {
public:
  LPS22HBClass(TwoWire& wire);

  int begin();
  void end();

  void setOutputRate(int rate);

  float readPressure(int units = KILOPASCAL);
  float readTemperature(void);
  float readAltitude(void);
private:
  int i2cRead(uint8_t reg);
  int i2cWrite(uint8_t reg, uint8_t val);

private:
  TwoWire* _wire;
  bool _initialized;
  int _rate;
};

extern LPS22HBClass BARO;

#endif
