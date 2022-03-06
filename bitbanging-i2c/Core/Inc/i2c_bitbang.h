/*
 * i2c_bitbang.h
 *
 *  Created on: Feb 14, 2022
 *      Author: yongd
 */

#ifndef INC_I2C_BITBANG_H_
#define INC_I2C_BITBANG_H_

#include "stm32h7xx_hal.h"

// hold GPIO info
// ex. HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // green LED
//                         ^bank B,  ^pin 0
typedef struct{
	GPIO_TypeDef* bank;
	uint16_t pin;
} gpio_t;

// global
uint8_t i2c_started;

// declare GPIO structs, to be defined in C file
gpio_t sda;
gpio_t scl;


// init
void i2c_bitbang_init(void);

// delay functions
void dwt_init(void); // data watchpoint trigger
void delay_us(uint32_t us);

// generic read/write to pins
uint8_t read_pin(gpio_t gpio);
void set_pin(gpio_t gpio);
void clear_pin(gpio_t gpio);

// specific read/write to pins
void set_sda(void);
void set_scl(void);
void clear_scl(void);
void clear_sda(void);
uint8_t read_sda(void);
uint8_t read_scl(void);

// start/stop conditions
void i2c_sent_start(void);
void i2c_send_stop(void);


// read/write to i2c bus
uint8_t i2c_read_bit(void);
void i2c_write_bit(uint8_t);
uint8_t i2c_read_byte(void);
void i2c_write_byte(uint8_t);

void read_reg(uint8_t slaveAddress, uint8_t reg, uint8_t *pData);
void write_reg(uint8_t slaveAddress, uint8_t reg, uint8_t *pData);

#endif /* INC_I2C_BITBANG_H_ */
