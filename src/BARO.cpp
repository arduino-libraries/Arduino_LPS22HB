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

#include <Wire.h>

#include "BARO.h"

#define LPS22HB_ADDRESS  0x5C

#define LPS22HB_WHO_AM_I_REG        0x0f
#define LPS22HB_CTRL2_REG           0x11
#define LPS22HB_STATUS_REG          0x27
#define LPS22HB_PRESS_OUT_XL_REG    0x28
#define LPS22HB_PRESS_OUT_L_REG     0x29
#define LPS22HB_PRESS_OUT_H_REG     0x2a

// Interrupts
#define LPS22HN_CTRL3_REG             0x12      // [INT H/L] [Push-pull / open-dra] [F_FSS5] [F_FTH] [F_OVR] [DRDY] [INT_S1] [INT_S0]
#define LPS22HB_INTERRUPT_CFG_REG     0x0b    //  b3 Int-generation-enable b1 Int-Low-press b0 INT-high-press
#define LPS22HB_INTERRUPT_THOLD_L_REG 0x0C
#define LPS22HB_INTERRUPT_THOLD_H_REG 0x0D
#define LPS22HB_INTERRUPT_SOURCE_REG  0x25    // [Boot_Status] -- [Interrupt active][Int low][Int high]


LPS22HBClass::LPS22HBClass(TwoWire& wire) :
  _wire(&wire)
{
}

int LPS22HBClass::begin()
{
  _wire->begin();

  if (i2cRead(LPS22HB_WHO_AM_I_REG) != 0xb1) {
    end();
    return 0;
  }
  softwareReset();
  return 1;
}

void LPS22HBClass::end()
{
  _wire->end();
}

float LPS22HBClass::readPressure(int units)
{
  // trigger one shot
  i2cWrite(LPS22HB_CTRL2_REG, 0x01);

  // wait for completion
  while ((i2cRead(LPS22HB_STATUS_REG) & 0x02) == 0) {
    yield();
  }

  float reading = (i2cRead(LPS22HB_PRESS_OUT_XL_REG) |
          (i2cRead(LPS22HB_PRESS_OUT_L_REG) << 8) | 
          (i2cRead(LPS22HB_PRESS_OUT_H_REG) << 16)) / 40960.0;

  if (units == MILLIBAR) { // 1 kPa = 10 millibar
    return reading * 10;
  } else if (units == PSI) {  // 1 kPa = 0.145038 PSI
    return reading * 0.145038;
  } else {
    return reading;
  }
}

void LPS22HBClass::setOpenDrain()
{
  uint8_t ctrl3_reg = (i2cRead(LPS22HN_CTRL3_REG) & 0b10111111) | 0b1 << 6;  
  i2cWrite(LPS22HN_CTRL3_REG, ctrl3_reg);
}

void LPS22HBClass::setPushPull()
{
  uint8_t ctrl3_reg = i2cRead(LPS22HN_CTRL3_REG) & 0b10111111;  
  i2cWrite(LPS22HN_CTRL3_REG, ctrl3_reg);
}

void LPS22HBClass::setActiveHigh()
{
  uint8_t ctrl3_reg = i2cRead(LPS22HN_CTRL3_REG) & 0b1111111 ;  
  i2cWrite(LPS22HN_CTRL3_REG, ctrl3_reg);
}

void LPS22HBClass::setActiveLow()
{
  uint8_t ctrl3_reg = (i2cRead(LPS22HN_CTRL3_REG) & 0b1111111) | 0b1 << 7;  
  i2cWrite(LPS22HN_CTRL3_REG, ctrl3_reg);
}

void LPS22HBClass::enableInterruptPin()
{
  uint8_t interrupt_cfg = (i2cRead(LPS22HB_INTERRUPT_CFG_REG) & 0b11110111) | 0b1 <<3;  // Enable DIFF_EN
  i2cWrite(LPS22HB_INTERRUPT_CFG_REG, interrupt_cfg);
  uint8_t INT_DRDY_reg = (i2cRead(0x12)  & 0b1111110) | 0b1;   // INT_S
  i2cWrite(0x12, INT_DRDY_reg);
  Serial.println(interrupt_cfg);
}

void LPS22HBClass::disableInterruptPin()
{
  uint8_t interrupt_cfg = (i2cRead(LPS22HB_INTERRUPT_CFG_REG) & 0b11110111);  // Enable DIFF_EN
  i2cWrite(LPS22HB_INTERRUPT_CFG_REG, interrupt_cfg);
  uint8_t INT_DRDY_reg = (i2cRead(0x12)  & 0b1111101);  // INT_S
  i2cWrite(0x12, INT_DRDY_reg);
}

void LPS22HBClass::setThreshold(uint16_t newThold)
{
  i2cWrite(LPS22HB_INTERRUPT_THOLD_L_REG , uint8_t(newThold));
  i2cWrite(LPS22HB_INTERRUPT_THOLD_H_REG , uint8_t(newThold>>8));
}

void LPS22HBClass::enableHighPressureInterrupt()
{
  uint8_t interrupt_cfg = (i2cRead(0x0b) & 0b1111110) | 0b1;  // Enable PHE
  i2cWrite(0x0b, interrupt_cfg);
}

void LPS22HBClass::disableHighPressureInterrupt()
{
  uint8_t interrupt_cfg = (i2cRead(0x0b) & 0b1111110);  // Disable PHE
  uint8_t INT_DRDY_reg = (i2cRead(0x12)  & 0b1111110);   // INT_S
  i2cWrite(0x0b, interrupt_cfg);
  i2cWrite(0x12, INT_DRDY_reg);
}

void LPS22HBClass::enableLowPressureInterrupt()
{
  uint8_t interrupt_cfg = (i2cRead(0x0b) & 0b1111101) | 0b1 <1;  // Enable PLE
  uint8_t INT_DRDY_reg = (i2cRead(0x12)  & 0b1111101) | 0b1 <1;  // INT_S
  i2cWrite(0x0b, interrupt_cfg);
  i2cWrite(0x12, INT_DRDY_reg);
}

void LPS22HBClass::disableLowPressureInterrupt()
{
  uint8_t interrupt_cfg = (i2cRead(0x0b) & 0b1111101);  // Disable PLE
  i2cWrite(0x0b, interrupt_cfg);
}

bool LPS22HBClass::interrupt()
{
  return i2cRead(LPS22HB_INTERRUPT_SOURCE_REG) & 0b1 << 2;
}

bool LPS22HBClass::pressureInterrupt()
{
  uint8_t reg = i2cRead(LPS22HB_INTERRUPT_SOURCE_REG);
  return (reg & 0b1<0) | (reg & 0b1<1) ;
}

bool LPS22HBClass::HighPressureInterrupt()
{
  return i2cRead(LPS22HB_INTERRUPT_SOURCE_REG) & 0b1 << 0;
}

bool LPS22HBClass::LowPresureInterrupt()
{
  return i2cRead(LPS22HB_INTERRUPT_SOURCE_REG) & 0b1 << 1;
}

void LPS22HBClass::softwareReset()
{
  i2cWrite(LPS22HB_CTRL2_REG, 0b100);

  while(i2cRead(LPS22HB_CTRL2_REG) & 0b1 << 2){
    delay(10);
  }  
}

int LPS22HBClass::i2cRead(uint8_t reg)
{
  _wire->beginTransmission(LPS22HB_ADDRESS);
  _wire->write(reg);
  if (_wire->endTransmission(false) != 0) {
    return -1;
  }

  if (_wire->requestFrom(LPS22HB_ADDRESS, 1) != 1) {
    return -1;
  }
  
  return _wire->read();
}

int LPS22HBClass::i2cWrite(uint8_t reg, uint8_t val)
{
  _wire->beginTransmission(LPS22HB_ADDRESS);
  _wire->write(reg);
  _wire->write(val);
  if (_wire->endTransmission() != 0) {
    return 0;
  }

  return 1;
}

#ifdef ARDUINO_ARDUINO_NANO33BLE
LPS22HBClass BARO(Wire1);
#else
LPS22HBClass BARO(Wire);
#endif