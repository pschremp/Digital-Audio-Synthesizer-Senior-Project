/*
 * seesaw.h
 *
 *	Adafruit Seesaw Protocol Handler
 *
 *  Created on: Apr 21, 2024
 *      Author: Alex Elliott
 */

#ifndef INC_SEESAW_H_
#define INC_SEESAW_H_

#include "main.h"

// MODULE BASE REGISTER ADDRESSES
#define MODBASE_STATUS	0x00
#define MODBASE_GPIO	0x01
#define MODBASE_SERCOM0	0x02
#define MODBASE_SERCOM1	0x03
#define MODBASE_SERCOM2	0x04
#define MODBASE_SERCOM3	0x05
#define MODBASE_SERCOM4	0x06
#define MODBASE_SERCOM5	0x07
#define MODBASE_PWM		0x08
#define MODBASE_ADC		0x09
#define MODBASE_DAC		0x0A
#define MODBASE_INT		0x0B
#define MODBASE_DAP		0x0C
#define MODBASE_EEPROM	0x0D
#define MODBASE_PIXEL	0x0E
#define MODBASE_TOUCH	0x0F
#define MODBASE_KEYPAD	0x10
#define MODBASE_ENCODER	0x11

// ENCODER FUNCTIONS
#define SEESAW_ENCODER_STATUS		0x00
#define SEESAW_ENCODER_INTENSET		0x10
#define SEESAW_ENCODER_INTENCLR		0x20
#define SEESAW_ENCODER_POSITION		0x30
#define SEESAW_ENCODER_DELTA		0x40

// STATUS FUNCTIONS
#define SEESAW_STATUS_HW_ID 		0x01
#define SEESAW_STATUS_VERSION		0x02
#define SEESAW_STATUS_OPTIONS		0x03
#define SEESAW_STATUS_TEMP			0x04
#define SEESAW_STATUS_SWRST			0x7F

// NEOPIXEL FUNCTIONS
#define SEESAW_NEOPIXEL_STATUS		0x00
#define SEESAW_NEOPIXEL_PIN			0x01
#define SEESAW_NEOPIXEL_SPEED		0x02
#define SEESAW_NEOPIXEL_BUF_LENGTH	0x03
#define SEESAW_NEOPIXEL_BUF			0x04
#define SEESAW_NEOPIXEL_SHOW		0x05

// GPIO FUNCTIONS
#define SEESAW_GPIO_DIRSET_BULK		0x02
#define SEESAW_GPIO_DIRCLR_BULK		0x03
#define SEESAW_GPIO_BULK			0x04
#define SEESAW_GPIO_BULK_SET		0x05
#define SEESAW_GPIO_BULK_CLR		0x06
#define SEESAW_GPIO_BULK_TOGGLE		0x07
#define SEESAW_GPIO_INTENSET		0x08
#define SEESAW_GPIO_INTENCLR		0x09
#define SEESAW_GPIO_INTFLAG			0x0A
#define SEESAW_GPIO_PULLENSET		0x0B
#define SEESAW_GPIO_PULLENCLR		0x0C

// PIN DEFINITIONS
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define INPUT_PULLDOWN 0x3

void seesaw_write(uint16_t i2c_addr, uint8_t moduleBase, uint8_t moduleFunction, uint8_t *buff, uint8_t num);
void seesaw_read(uint16_t i2c_addr, uint8_t moduleBase, uint8_t moduleFunction, uint8_t *buff, uint8_t num);

#endif /* INC_SEESAW_H_ */
