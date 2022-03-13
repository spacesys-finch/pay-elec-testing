// SPDX-License-Identifier: GPL-2.0-only
/*
 * This file is part of the Arduino_OX767X library.
 * Copyright (c) 2020 Arduino SA. All rights reserved.
 */
#include "stm32h7xx_hal.h"
#include <cstdio>

#ifndef OV760_DEBUG
#define OV760_DEBUG
#endif

extern I2C_HandleTypeDef hi2c1;

extern "C" {
  void msleep(unsigned long ms)
  {
    HAL_Delay(ms);
  }

  int arduino_i2c_read(unsigned short address, unsigned char reg, unsigned char *value)
  {
#ifdef OV760_DEBUG
    printf("stm32_i2c_read: address = %#06x, reg = %#06x", address, reg);
#endif
    HAL_StatusTypeDef ret;
    uint8_t buf[1];

//    Wire.beginTransmission(address);
//    Wire.write(reg);
    ret = HAL_I2C_Master_Transmit(&hi2c1, address, (uint8_t *const)&reg, 1, HAL_MAX_DELAY);

    //if (Wire.endTransmission() != 0) {
      if ( ret != HAL_OK ) {
#ifdef OV760_DEBUG
    printf("\r\n");
    //printf("Error Tx\r\n");
#endif
      return -1;
    }

      ret = HAL_I2C_Master_Receive(&hi2c1, address, buf, 1, HAL_MAX_DELAY);
    //if (Wire.requestFrom(address, 1) != 1) {
      if (ret != HAL_OK){
#ifdef OV760_DEBUG
      printf("\r\n");
#endif
      return -1;
    }

//    *value = Wire.read();
  *value = buf[0];

#ifdef OV760_DEBUG
    printf(", value = %#06x\r\n", *value);
#endif

    return 0;
  }

  int arduino_i2c_write(unsigned short address, unsigned char reg, unsigned char value)
  {
#ifdef OV760_DEBUG
    printf("stm32_i2c_write: address = %#06x, reg = %#06x, value = %#06x\r\n", address, reg, value);
#endif

//    Wire.beginTransmission(address);
//    Wire.write(reg);
//    Wire.write(value);

    HAL_StatusTypeDef ret;
    uint8_t buf[2];

    buf[0] = reg;
    buf[1] = value;

    ret = HAL_I2C_Master_Transmit(&hi2c1, address, buf, 2, HAL_MAX_DELAY);

    // if (Wire.endTransmission() != 0) {
    if(ret != HAL_OK){
      return -1;
    }

    return 0;
  }
};
