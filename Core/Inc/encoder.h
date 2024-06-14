/*
 * encoder.h
 *
 *  Created on: Apr 24, 2024
 *      Author: Alex
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include "main.h"

#define ENCODER1_ADDR	0x36
#define ENCODER2_ADDR	0x37
#define ENCODER3_ADDR	0x38
#define ENCODER4_ADDR	0x39

#define ENCODER_DELAY	0	//delay for reads (ideally zero I hope)

#define NEOPIXEL_PIN	6
#define SWITCH_PIN		24

void initPixel(uint8_t encoderID);
void initEncoder(uint8_t encoderID);
void initPushButton(uint8_t encoderID);
void setGPIOInterrupts(uint8_t encoderID, uint32_t pins, uint8_t enabled);
uint8_t checkPushButton(uint8_t encoderID);
uint32_t getEncoderPosition(uint8_t encoderID);
void pinMode(uint8_t encoderID, uint8_t pin, uint8_t mode);
void pinModeBulk(uint8_t encoderID, uint32_t pinsa, uint32_t pinsb, uint8_t mode);
void pinModeBulkB(uint8_t encoderID, uint32_t pins, uint8_t mode);
uint8_t digitalRead(uint8_t encoderID, uint8_t pin);
uint32_t digitalReadBulk(uint8_t encoderID, uint32_t pins);
uint32_t digitalReadBulkB(uint8_t encoderID, uint32_t pins);
uint8_t getInterruptFlag(uint8_t encoderID);
void setEncoderPosition(uint8_t encoderID, uint32_t pos);
void enableEncoderInterrupt(uint8_t encoderID);
void disableEncoderInterrupt(uint8_t encoderID);
void setPixelSpeed(uint8_t encoderID, uint8_t speed);
void setBufferLength(uint8_t encoderID, uint8_t length);
void setPixelPin(uint8_t encoderID, uint8_t pin);
void setPixelColor(uint8_t encoderID, uint8_t r, uint8_t g, uint8_t b);
void setPixelColor32(uint8_t encoderID, uint32_t color);
uint32_t colorWheel(uint8_t WheelPos);

#endif /* INC_ENCODER_H_ */
