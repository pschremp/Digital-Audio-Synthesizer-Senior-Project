/*
 * encoder.c
 *
 *  Created on: Apr 24, 2024
 *      Author: Alex
 */

#include "seesaw.h"
#include "encoder.h"
#include "main.h"
#include "i2c.h"

uint8_t encoderAddr[4] = {ENCODER1_ADDR,ENCODER2_ADDR,ENCODER3_ADDR,ENCODER4_ADDR};

void initPushButton(uint8_t encoderID) {
	// use a pin for the built in encoder switch
	//ss.pinMode(SS_SWITCH, INPUT_PULLUP);
	//ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH, 1); //enable interrupt for push button as well!

	//seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_, &hwID, 1);
	pinMode(encoderID, SWITCH_PIN, INPUT_PULLUP);
	setGPIOInterrupts(encoderID, (uint32_t)1 << SWITCH_PIN, 1);

}

void initPixel(uint8_t encoderID) {
	setPixelSpeed(encoderID,1);
	setBufferLength(encoderID,3);
	setPixelPin(encoderID,NEOPIXEL_PIN);
	setPixelColor(encoderID,0,0,0);
}

void initEncoder(uint8_t encoderID) {
	uint8_t hwID = 0;
	while (hwID != 0x55) {
		seesaw_read(encoderAddr[encoderID],MODBASE_STATUS, SEESAW_STATUS_HW_ID, &hwID, 1);
	}
}

void setGPIOInterrupts(uint8_t encoderID, uint32_t pins, uint8_t enabled) {
	uint8_t cmd[] = {(uint8_t)(pins >> 24), (uint8_t)(pins >> 16),
	                 (uint8_t)(pins >> 8),  (uint8_t) pins};
	if (enabled & 0x1) {
		seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_INTENSET, cmd, 4);
	}
	else {
		seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_INTENCLR, cmd, 4);
	}
}

void pinMode(uint8_t encoderID, uint8_t pin, uint8_t mode) {
	if (pin >= 32)
	    pinModeBulk(encoderID, 0, 1ul << (pin - 32), mode);
	else
	    pinModeBulkB(encoderID, 1ul << pin, mode);
}

void pinModeBulk(uint8_t encoderID, uint32_t pinsa, uint32_t pinsb, uint8_t mode) {
	uint8_t cmd[] = {(uint8_t)(pinsa >> 24), (uint8_t)(pinsa >> 16),
	                 (uint8_t)(pinsa >> 8),  (uint8_t)pinsa,
	                 (uint8_t)(pinsb >> 24), (uint8_t)(pinsb >> 16),
	                 (uint8_t)(pinsb >> 8),  (uint8_t)pinsb};
	switch (mode) {
	case OUTPUT:
	    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_DIRSET_BULK, cmd, 8);
	    break;
	case INPUT:
	    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_DIRCLR_BULK, cmd, 8);
	    break;
	case INPUT_PULLUP:
	    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_DIRCLR_BULK, cmd, 8);
	    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_PULLENSET, cmd, 8);
	    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_BULK_SET, cmd, 8);
	    break;
	case INPUT_PULLDOWN:
	    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_DIRCLR_BULK, cmd, 8);
	    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_PULLENSET, cmd, 8);
	    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_BULK_CLR, cmd, 8);
	    break;
	}
}

void pinModeBulkB(uint8_t encoderID, uint32_t pins, uint8_t mode) {
  uint8_t cmd[] = {(uint8_t)(pins >> 24), (uint8_t)(pins >> 16),
                   (uint8_t)(pins >> 8),  (uint8_t)pins};
  switch (mode) {
  case OUTPUT:
    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_DIRSET_BULK, cmd, 4);
    break;
  case INPUT:
    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
    break;
  case INPUT_PULLUP:
    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_PULLENSET, cmd, 4);
    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_BULK_SET, cmd, 4);
    break;
  case INPUT_PULLDOWN:
    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_PULLENSET, cmd, 4);
    seesaw_write(encoderAddr[encoderID],MODBASE_GPIO, SEESAW_GPIO_BULK_CLR, cmd, 4);
    break;
  }
}

uint8_t checkPushButton(uint8_t encoderID) {
	uint8_t state = digitalRead(encoderID, SWITCH_PIN);
	if (!state)
		return 1;
	else
		return 0;
}

uint8_t getInterruptFlag(uint8_t encoderID) {
	uint8_t buf[4];
	seesaw_read(encoderAddr[encoderID], MODBASE_GPIO, SEESAW_GPIO_INTFLAG, buf, 4);
	for (uint8_t i; i < 4; i++) {
		if (buf[i] != 0) {
			return 1;
		}
	}
	seesaw_read(encoderAddr[encoderID], MODBASE_GPIO, SEESAW_GPIO_INTFLAG, buf, 4);
	for (uint8_t i; i < 4; i++) {
		if (buf[i] != 0) {
			return 1;
		}
	}
	seesaw_read(encoderAddr[encoderID], MODBASE_GPIO, SEESAW_GPIO_INTFLAG, buf, 4);
	for (uint8_t i; i < 4; i++) {
		if (buf[i] != 0) {
			return 1;
		}
	}
	return 0;
}

uint8_t digitalRead(uint8_t encoderID, uint8_t pin) {
	if (pin >= 32)
	  return digitalReadBulkB(encoderID, (1ul << (pin - 32))) != 0;
	else
	  return digitalReadBulk(encoderID, (1ul << pin)) != 0;
}

uint32_t digitalReadBulk(uint8_t encoderID, uint32_t pins) {
  uint8_t buf[4];
  seesaw_read(encoderAddr[encoderID], MODBASE_GPIO, SEESAW_GPIO_BULK, buf, 4);
  seesaw_read(encoderAddr[encoderID], MODBASE_GPIO, SEESAW_GPIO_BULK, buf, 4);
  seesaw_read(encoderAddr[encoderID], MODBASE_GPIO, SEESAW_GPIO_BULK, buf, 4);
  uint32_t ret = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
                 ((uint32_t)buf[2] << 8)  |  (uint32_t)buf[3];
  return ret & pins;
}

uint32_t digitalReadBulkB(uint8_t encoderID, uint32_t pins) {
  uint8_t buf[8];
  seesaw_read(encoderAddr[encoderID], MODBASE_GPIO, SEESAW_GPIO_BULK, buf, 8);
  seesaw_read(encoderAddr[encoderID], MODBASE_GPIO, SEESAW_GPIO_BULK, buf, 8);
  seesaw_read(encoderAddr[encoderID], MODBASE_GPIO, SEESAW_GPIO_BULK, buf, 8);
  uint32_t ret = ((uint32_t)buf[4] << 24) | ((uint32_t)buf[5] << 16) |
                 ((uint32_t)buf[6] << 8) | (uint32_t)buf[7];
  return ret & pins;
}

uint32_t getEncoderPosition(uint8_t encoderID) {
	uint8_t buff[4];
	seesaw_read(encoderAddr[encoderID],MODBASE_ENCODER,SEESAW_ENCODER_POSITION,buff,4);
	seesaw_read(encoderAddr[encoderID],MODBASE_ENCODER,SEESAW_ENCODER_POSITION,buff,4);
	seesaw_read(encoderAddr[encoderID],MODBASE_ENCODER,SEESAW_ENCODER_POSITION,buff,4);
	uint32_t ret = ((uint32_t)buff[0] << 24) | ((uint32_t)buff[1] << 16) |
	               ((uint32_t)buff[2] << 8)  | (uint32_t)buff[3];
	return ret;
}

void setEncoderPosition(uint8_t encoderID, uint32_t pos) {
	uint8_t buff[] = {(uint8_t)(pos >> 24), (uint8_t)(pos >> 16),
	        		  (uint8_t)(pos >> 8),  (uint8_t)(pos & 0xFF)};
	seesaw_write(encoderAddr[encoderID],MODBASE_ENCODER,SEESAW_ENCODER_POSITION,buff,4);
}

void enableEncoderInterrupt(uint8_t encoderID) {
	uint8_t data = 0x01;
	seesaw_write(encoderAddr[encoderID],MODBASE_ENCODER,SEESAW_ENCODER_INTENSET,&data,1);
}

void disableEncoderInterrupt(uint8_t encoderID) {
	uint8_t data = 0x01;
	seesaw_write(encoderAddr[encoderID],MODBASE_ENCODER,SEESAW_ENCODER_INTENCLR,&data,1);
}

// NEOPIXELS

void setPixelSpeed(uint8_t encoderID, uint8_t speed) {
	seesaw_write(encoderAddr[encoderID],MODBASE_PIXEL,SEESAW_NEOPIXEL_SPEED,&speed,1);
	// 1 = 800kHz
	// 0 = 400kHz
}

void setBufferLength(uint8_t encoderID, uint8_t length) {
	uint8_t message[2] = {length >> 8, length & 0xFF};
	seesaw_write(encoderAddr[encoderID],MODBASE_PIXEL,SEESAW_NEOPIXEL_BUF_LENGTH,message,2);
}

void setPixelPin(uint8_t encoderID, uint8_t pin) {
	seesaw_write(encoderAddr[encoderID],MODBASE_PIXEL,SEESAW_NEOPIXEL_PIN,&pin,1);
}

void setPixelColor(uint8_t encoderID, uint8_t r, uint8_t g, uint8_t b) {
	uint8_t message[5] = {0x00,0x00,g,r,b};
	//uint8_t data = NULL;
	seesaw_write(encoderAddr[encoderID],MODBASE_PIXEL,SEESAW_NEOPIXEL_BUF,message,5);
//	seesaw_write(encoderAddr[encoderID],MODBASE_PIXEL,SEESAW_NEOPIXEL_SHOW,&data,0);
	uint8_t regAddr[] = {MODBASE_PIXEL, SEESAW_NEOPIXEL_SHOW};
	HAL_I2C_Master_Transmit(&hi2c1, (encoderAddr[encoderID]<<1), regAddr, 2, 100);
}

void setPixelColor32(uint8_t encoderID, uint32_t color) {
	uint8_t r = color & 0xFF;
	uint8_t g = (color >> 8) & 0xFF;
	uint8_t b = (color >> 16) & 0xFF;
	setPixelColor(encoderID,r,g,b);
}

uint32_t colorWheel(uint8_t WheelPos) {
	// COPIED FROM ADAFRUIT --> returns 32 bits: xxbbggrr
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return (((255-WheelPos*3) & 0xFF) | (0 << 8) | ((WheelPos * 3) << 16));
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return ((0 & 0xFF) | ((WheelPos*3) << 8) | ((255-WheelPos * 3) << 16));
  }
  WheelPos -= 170;
  return (((WheelPos*3) & 0xFF) | ((255-WheelPos * 3 << 8)) | (0 << 16));
}
