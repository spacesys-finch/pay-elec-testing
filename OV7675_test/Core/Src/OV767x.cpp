// SPDX-License-Identifier: GPL-2.0-only
/*
 * This file is part of the Arduino_OX767X library.
 * Copyright (c) 2020 Arduino SA. All rights reserved.
 */


#include "OV767X.h"

extern "C" {
  // defined in utility/ov7670.c:
  struct ov7670_fract {
    uint32_t numerator;
    uint32_t denominator;
  };

  void* ov7670_alloc();
  void ov7670_free(void*);

  int ov7670_reset(void*, uint32_t val);
  int ov7670_detect(void*);
  void ov7670_configure(void*, int devtype, int format, int wsize, int clock_speed, int pll_bypass, int pclk_hb_disable);
  int ov7670_s_power(void*, int on);
  int ov7675_set_framerate(void*, struct ov7670_fract *tpf);

  int ov7670_s_sat_hue(void*, int sat, int hue);
  int ov7670_s_brightness(void*, int value);
  int ov7670_s_contrast(void*, int value);
  int ov7670_s_hflip(void*, int value);
  int ov7670_s_vflip(void*, int value);
  int ov7670_s_gain(void*, int value);
  int ov7670_s_autogain(void*, int value);
  int ov7670_s_exp(void*, int value);
  int ov7670_s_autoexp(void*, int value);
  int ov7670_s_test_pattern(void*, int value);
};

OV767X::OV767X() :
  _ov7670(NULL),
  _saturation(128),
  _hue(0){}


OV767X::~OV767X()
{
  if (_ov7670) {
    ov7670_free(_ov7670);
  }
}

int OV767X::begin(int resolution, int format, int fps)
{
  switch (resolution) {
    case VGA:
      _width = 640;
      _height = 480;
      break;

//    case CIF:
//      _width = 352;
//      _height = 240;
//      break;

    case QVGA:
      _width = 320;
      _height = 240;
      break;

//    case QCIF:
//      _width = 176;
//      _height = 144;
//      break;

    case QQVGA:
      _width = 160;
      _height = 120;
      break;

    default:
      return 0;
  }

  _grayscale = false;
  switch (format) {
    case YUV422:
    case RGB444:
    case RGB565:
      _bytesPerPixel = 2;
      break;

    case GRAYSCALE:
      format = YUV422;    // We use YUV422 but discard U and V bytes
      _bytesPerPixel = 2; // 2 input bytes per pixel of which 1 is discarded
      _grayscale = true;
      break;

    default:
      return 0;
  }

// The only frame rates which work on the Nano 33 BLE are 1 and 5 FPS
 // if (fps != 1 && fps != 5)
    //return 0;

  _ov7670 = ov7670_alloc();
  if (!_ov7670) {
    end();

    return 0;
  }

  // Configure GPIO pins
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // pixel input (D0-D7)
  // __HAL_RCC_GPIOF_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  // vsync
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  // href
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // pclk
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  _vsyncMask = 1<<15;
  _hrefMask = 1<<0;
  _pclkMask = 1<<3;

  HAL_Delay(1000);

  if (ov7670_detect(_ov7670)) {
    end();

    return 0;
  }

  ov7670_configure(_ov7670, 0 /*OV7670 = 0, OV7675 = 1*/, format, resolution, 16 /* MHz */,
                    0 /*pll bypass*/, 1 /* pclk_hb_disable */);

  if (ov7670_s_power(_ov7670, 1)) {
    end();

    return 0;
  }

  struct ov7670_fract tpf;

  tpf.numerator = 1;
  tpf.denominator = fps;

  ov7675_set_framerate(_ov7670, &tpf);

  return 1;
}

void OV767X::end()
{
	// turn off xclk
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  if (_ov7670) {
    ov7670_free(_ov7670);
  }
}

int OV767X::width() const
{
  return _width;
}

int OV767X::height() const
{
  return _height;
}

int OV767X::bitsPerPixel() const
{
  return _bytesPerPixel * 8;
}

int OV767X::bytesPerPixel() const
{
  return _bytesPerPixel;
}

void OV767X::readFrame(void* buffer)
{
	__disable_irq();

  uint8_t* b = (uint8_t*)buffer;
  int bytesPerRow = _width * _bytesPerPixel;

  // Falling edge indicates start of frame

  while ((VSYNC_PORT & _vsyncMask) == 0); // wait for HIGH
  while ((VSYNC_PORT & _vsyncMask) != 0); // wait for LOW

  for (int i = 0; i < _height; i++) {
  // rising edge indicates start of line
    while ((HREF_PORT & _hrefMask) == 0); // wait for HIGH

    for (int j = 0; j < bytesPerRow; j++) {
      // rising edges clock each data byte
      while ((PCLK_PORT & _pclkMask) != 0); // wait for LOW

      uint32_t in = DPINS_PORT; // read all bits in parallel

      in &= 0xff; // isolate the 8 LSBs

      if (!(j & 1) || !_grayscale) {
        *b++ = in;
      }
      while ((PCLK_PORT & _pclkMask) == 0); // wait for HIGH
    }
    while ((HREF_PORT & _hrefMask) != 0); // wait for LOW
  }

  __enable_irq();
}

void OV767X::testPattern(int pattern)
{
  ov7670_s_test_pattern(_ov7670, pattern);
}

void OV767X::noTestPattern()
{
  ov7670_s_test_pattern(_ov7670, 0);
}

void OV767X::setSaturation(int saturation)
{
  _saturation = saturation;

  ov7670_s_sat_hue(_ov7670, _saturation, _hue);
}

void OV767X::setHue(int hue)
{
  _hue = hue;

   ov7670_s_sat_hue(_ov7670, _saturation, _hue);
}

void OV767X::setBrightness(int brightness)
{
  ov7670_s_brightness(_ov7670, brightness);
}

void OV767X::setContrast(int contrast)
{
  ov7670_s_contrast(_ov7670, contrast);
}

void OV767X::horizontalFlip()
{
  ov7670_s_hflip(_ov7670, 1);
}

void OV767X::noHorizontalFlip()
{
  ov7670_s_hflip(_ov7670, 0);
}

void OV767X::verticalFlip()
{
  ov7670_s_vflip(_ov7670, 1);
}

void OV767X::noVerticalFlip()
{
  ov7670_s_vflip(_ov7670, 0);
}

void OV767X::setGain(int gain)
{
  ov7670_s_gain(_ov7670, gain);
}

void OV767X::autoGain()
{
  ov7670_s_autogain(_ov7670, 1);
}

void OV767X::setExposure(int exposure)
{
  ov7670_s_exp(_ov7670, exposure);
}

void OV767X::autoExposure()
{
  ov7670_s_autoexp(_ov7670, 0 /* V4L2_EXPOSURE_AUTO */);
}

OV767X Camera;
