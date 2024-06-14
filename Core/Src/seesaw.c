/*
 * seesaw.c
 *
 * 	Adafruit Seesaw Protocol Handler
 *
 *  Created on: Apr 21, 2024
 *      Author: Alex Elliott
 */

#include "seesaw.h"
#include "i2c.h"

#define MAX_I2C_BUFF_SIZE	32	//max buffer size!! includes 2 bytes for module ID purposes

void seesaw_write(uint16_t i2c_addr, uint8_t moduleBase, uint8_t moduleFunction, uint8_t buff[], uint8_t num)
{
//	Wire.beginTransmission((uint8_t)_i2caddr);
//	Wire.write((uint8_t)moduleBase); //module base register address
//	Wire.write((uint8_t)moduleFunction); //module function register address
//	Wire.write((uint8_t *)buff, num); //data bytes
//	Wire.endTransmission();

	uint8_t data_to_send[MAX_I2C_BUFF_SIZE] = {moduleBase,moduleFunction,*buff};
	//HAL_I2C_Master_Transmit(&hi2c1,i2c_addr<<1,data_to_send,2+num,200); //Transmit Module ID + Data Bytes: 200ms timeout

	HAL_StatusTypeDef status = HAL_OK;

	uint16_t regAddr = (moduleBase << 8) + moduleFunction;
	status = HAL_I2C_Mem_Write(&hi2c1,i2c_addr<<1,regAddr,I2C_MEMADD_SIZE_16BIT,buff,num,100);
	if(status != HAL_OK)
    {
        // Error handling, for example re-initialization of the I2C peripheral
		status = HAL_I2C_Mem_Write(&hi2c1,i2c_addr<<1,regAddr,I2C_MEMADD_SIZE_16BIT,buff,num,100);
    }
}

void seesaw_read(uint16_t i2c_addr, uint8_t moduleBase, uint8_t moduleFunction, uint8_t *buff, uint8_t num)
{
//  Wire.beginTransmission((uint8_t)_i2caddr);
//  Wire.write((uint8_t)moduleBase); //module base register address
//  Wire.write((uint8_t)moduleFunction); //module function register address
//  Wire.endTransmission();
//
//  delayMicroseconds(delay); // may not be needed
//
//  Wire.requestFrom((uint8_t)_i2caddr, num);
//
//  for(int i=0; i<num; i++){
//    buff[i] = Wire.read();
//  }

	uint8_t modbase_func[2] = {moduleBase,moduleFunction};
	//HAL_I2C_Master_Transmit(&hi2c1,i2c_addr<<1,modbase_func,2,100); //Transmit Module ID: 100ms timeout
	//HAL_Delay(delay); //very brief delay, may not be needed at all
	//HAL_I2C_Master_Receive(&hi2c1,i2c_addr<<1,buff,num,200); //Receive Data and Place in Buff: 200ms timeout


	HAL_StatusTypeDef status = HAL_OK;

	uint16_t regAddr = (moduleBase << 8) + moduleFunction;
	status = HAL_I2C_Mem_Read(&hi2c1, (i2c_addr<<1), regAddr, I2C_MEMADD_SIZE_16BIT, buff, num, 100);
	//status = HAL_I2C_Mem_Read(&hi2c1, (i2c_addr<<1) | 0x01, regAddr, I2C_MEMADD_SIZE_16BIT, buff, num, 100);

    /* Check the communication status */
    if(status != HAL_OK)
    {
    	status = HAL_I2C_Mem_Read(&hi2c1, (i2c_addr<<1), regAddr, I2C_MEMADD_SIZE_16BIT, buff, num, 100);
    }
}

