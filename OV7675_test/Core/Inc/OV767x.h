// SPDX-License-Identifier: GPL-2.0-only
/*
 * This file is part of the Arduino_OX767X library.
 * Copyright (c) 2020 Arduino SA. All rights reserved.
 */
#include <stdint.h>
#include "stm32h7xx_hal.h"

#ifndef _OV767X_H_
#define _OV767X_H_

/*
 * Pin assignments: OV7670 -> STM32
* VSYNC PB2
* HREF  PC0
* PCLK   PA3
* XCLK  PC9
* D0    PF0
* D1    PF1
* D2    PF2
* D3    PF3
* D4    PF4
* D5    PF5
* D6    PF6
* D7    PF7
*/

#define VSYNC_PORT GPIOB->IDR
#define HREF_PORT GPIOC->IDR
#define PCLK_PORT GPIOA->IDR
#define DPINS_PORT GPIOF->IDR

enum
{
  YUV422 = 0,
  RGB444 = 1,
  RGB565 = 2,
  // SBGGR8 = 3
  GRAYSCALE = 4
};

enum
{
  VGA = 0,  // 640x480
  CIF = 1,  // 352x240
  QVGA = 2, // 320x240
  QCIF = 3,  // 176x144
  QQVGA = 4,  // 160x120
};

class OV767X
{
public:
  OV767X();
  virtual ~OV767X();

  int begin(int resolution, int format, int fps); // Supported FPS: 1, 5, 10, 15, 30
  void end();

  // must be called after Camera.begin():
  int width() const;
  int height() const;
  int bitsPerPixel() const;
  int bytesPerPixel() const;

  void readFrame(void* buffer);

  void testPattern(int pattern = 2);
  void noTestPattern();

  void setSaturation(int saturation); // 0 - 255
  void setHue(int hue); // -180 - 180
  void setBrightness(int brightness); // 0 - 255
  void setContrast(int contrast); // 0 - 127
  void horizontalFlip();
  void noHorizontalFlip();
  void verticalFlip();
  void noVerticalFlip();
  void setGain(int gain); // 0 - 255
  void autoGain();
  void setExposure(int exposure); // 0 - 65535
  void autoExposure();

private:
  int _width;
  int _height;
  int _bytesPerPixel;
  bool _grayscale;

  void* _ov7670;

  // volatile uint32_t* _vsyncPort;
  uint32_t _vsyncMask;
  // volatile uint32_t* _hrefPort;
  uint32_t _hrefMask;
  // volatile uint32_t* _pclkPort;
  uint32_t _pclkMask;

  int _saturation;
  int _hue;
};

extern OV767X Camera;

#endif
