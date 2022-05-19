/*
 * i2c_bitbang.c
 *
 *  Created on: Feb 14, 2022
 *      Author: yongd
 */

#include "i2c_bitbang.h"

// GPIO toggle frequency for SCL seems to max out at about 70kHz regardless of how short the I2C_BIT_DELAY is
#define I2C_BIT_DELAY 		5 // microseconds -> 1/I2C_FREQ = I2C_BIT_DELAY, usually I2C_FREQ = 100kHz
#define I2C_PACKET_DELAY 	20 // microseconds

/*
 * I2C2 default initialization
 * PF0	I2C2_SDA	n/a	n/a	Alternate Function Open Drain	No pull-up and no pull-down	Low	n/a		false
 * PF1	I2C2_SCL	n/a	n/a	Alternate Function Open Drain	No pull-up and no pull-down	Low	n/a		false

 * user LEDs, see section 6.6.1 of UM2407
 * user LED1 = PB0 green
 * user LED2 = PE1 yellow
 * user LED3 = PB14 red
 */


// initialize the GPIO pins
// see page 30 of dm00392525 HAL library
// also see https://github.com/tobajer/i2cbitbang/blob/master/i2cbitbang_driver.cpp
void i2c_bitbang_init(){
	dwt_init(); // init timer

	// initialize value
	i2c_started = 0;

	// initialize structs to hold GPIO pin info
	sda.bank = GPIOA;
	sda.pin = GPIO_PIN_3;

	scl.bank = GPIOC;
	scl.pin = GPIO_PIN_0;


	// clock
	// HAL structure for init typedef, required to initializing GPIO pins
	GPIO_InitTypeDef scl_init;
	scl_init.Pin = scl.pin;
	scl_init.Mode = GPIO_MODE_OUTPUT_OD;
	scl_init.Pull = GPIO_PULLUP; // default to 1
	scl_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	// finally init
	HAL_GPIO_Init(scl.bank, &scl_init);


	// data line
	GPIO_InitTypeDef sda_init;
	sda_init.Pin = sda.pin;
	sda_init.Mode = GPIO_MODE_OUTPUT_OD;
	sda_init.Pull = GPIO_PULLUP;
	sda_init.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(sda.bank, &sda_init); // init

	// set both to high
	set_sda();
	set_scl();
}



// ========= delay functions ==========
void dwt_init(void){
	// black magic to setup
	// https://deepbluembedded.com/stm32-delay-microsecond-millisecond-utility-dwt-delay-timer-delay/
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;	// enable TRC
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;			// enable clock cycle counter
	DWT->CYCCNT = 0;								// reset clock cycle counter value
}

void delay_us(uint32_t us){
	uint32_t curDwt = DWT->CYCCNT;
	if (us > 0)
	{
		uint32_t tp = us * (HAL_RCC_GetHCLKFreq()/1000000UL);
		while ( (DWT->CYCCNT - curDwt) < tp);
	}
}

// ============= generic read/write =============
// generic read
uint8_t read_pin(gpio_t gpio){
	return HAL_GPIO_ReadPin(gpio.bank, gpio.pin);
}

// generic set
void set_pin(gpio_t gpio){
	HAL_GPIO_WritePin(gpio.bank, gpio.pin, GPIO_PIN_SET);
}

// generic reset
void clear_pin(gpio_t gpio){
	HAL_GPIO_WritePin(gpio.bank, gpio.pin, GPIO_PIN_RESET);
}


// ===== SDA and SCL read/writes ===========
void set_sda(void){
	set_pin(sda);
}

void set_scl(void){
	set_pin(scl);
}

void clear_sda(void){
	clear_pin(sda);
}

void clear_scl(void){
	clear_pin(scl);
}

uint8_t read_sda(void){
	return read_pin(sda);
}

uint8_t read_scl(void){
	return read_pin(scl);
}


// ======= start/stop conditions ==========
void i2c_send_start(void){
	// SDA transitions HIGH -> LOW when SCL is HIGH
	// assume starting SCL is LOW (by default, start of transmission)
	clear_sda();
	delay_us(I2C_BIT_DELAY);
	clear_scl();
	i2c_started = 1;
}


void i2c_send_stop(void){
	// SCL rises when SDA is low, then SDA rises
	// assume starting SCL is LOW (finished transmission)
	clear_sda();
	delay_us(I2C_BIT_DELAY);
	set_scl();
	delay_us(I2C_BIT_DELAY);
	set_sda();
}



// ======= read/write to i2c bus ========
uint8_t i2c_read_bit(void){
	uint8_t bit;

	set_sda();		// let target drive SDA
	delay_us(I2C_BIT_DELAY);
	set_scl();		// indicate slave can write to SDA
	delay_us(I2C_BIT_DELAY);
	bit = read_sda();
	clear_scl();	// clear SCL in preparation for next operation

	return bit;
}

void i2c_write_bit(uint8_t bit){
	// set SDA, assume SCL is low
	if (bit & 0x01){
		set_sda();
	}
	else{
		clear_sda();
	}
	delay_us(I2C_BIT_DELAY);
	set_scl();		// tell slave SDA is ready
	delay_us(I2C_BIT_DELAY);

	clear_scl(); 	// clear SCL in preparation for next operation
}

uint8_t i2c_read_byte(void){
	uint8_t bit;
	uint8_t byte = 0;

	for (bit = 0; bit < 8; bit++){
		byte = (byte << 1) | i2c_read_bit();
	}

	i2c_write_bit(0x0); // write 9th bit as don't care
	set_sda();

	return byte;
}

void i2c_write_byte(uint8_t byte){
	uint8_t bit;

	// MSB first
	for (bit = 0; bit < 8; bit++){
		i2c_write_bit( (byte & 0x80) != 0);
		byte = byte << 1;
	}

	i2c_write_bit(0x0); // write 9th bit as don't care
}



void read_reg(uint8_t slaveAddress, uint8_t reg, uint8_t *pData){
	i2c_send_start();

	delay_us(I2C_BIT_DELAY);

	i2c_write_byte(slaveAddress & 0x0FE); // 7 bits
	delay_us(I2C_PACKET_DELAY);
	i2c_write_byte(reg);
	delay_us(I2C_PACKET_DELAY);
	i2c_send_stop();

	delay_us(100);

	i2c_write_byte(slaveAddress & 0x0FE);
	delay_us(I2C_PACKET_DELAY);
	*pData = i2c_read_byte();
	i2c_send_stop();
}

void write_reg(uint8_t slaveAddress, uint8_t reg, uint8_t *pData){
	i2c_send_start();

	delay_us(I2C_PACKET_DELAY);

	i2c_write_byte(slaveAddress & 0x0FE); // 7 bits
	delay_us(I2C_PACKET_DELAY);
	i2c_write_byte(reg);
	delay_us(I2C_PACKET_DELAY);
	i2c_write_byte(*pData);
	i2c_send_stop();

	delay_us(100);

	i2c_write_byte(slaveAddress & 0x0FE);
	delay_us(I2C_PACKET_DELAY);
	*pData = i2c_read_byte();
	i2c_send_stop();
}



 // ============ OTHER STUFF ============

// ============== set modes ===============
// set output push-pull
void set_mode_output_pp(gpio_t gpio){
	GPIO_InitTypeDef tmp; // HAL structure for init typedef, required to initializing GPIO pins
	tmp.Pin = gpio.pin; // set pin
	tmp.Mode = GPIO_MODE_OUTPUT_PP; // open-drain as to not damage devices using push-pull
	tmp.Mode = GPIO_NOPULL;
	tmp.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(gpio.bank, &tmp);
}


// set output open drain
void set_mode_output_od(gpio_t gpio){
	GPIO_InitTypeDef tmp;
	tmp.Pin = gpio.pin;
	tmp.Mode = GPIO_MODE_OUTPUT_OD;
	tmp.Pull = GPIO_PULLUP;
	tmp.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(gpio.bank, &tmp);
}


// set pin to input mode
void set_mode_input(gpio_t gpio){
	GPIO_InitTypeDef tmp;
	tmp.Pin = gpio.pin;
	tmp.Mode = GPIO_MODE_INPUT;
	tmp.Pull = GPIO_PULLUP; // if not in use, default value to 1

	HAL_GPIO_Init(gpio.bank, &tmp);
}
