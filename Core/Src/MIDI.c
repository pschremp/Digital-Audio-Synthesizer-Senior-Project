/*
 * MIDI.c
 *
 * Utilizes the "usart.h" module which handles USART6 on the STM32F746NG-DISCO board.
 * Receives MIDI commands/data on USART6 (Arduino connector pins PC7/D0)
 *
 *  Created on: Mar 20, 2024
 *      Author: Alex
 */

// NOTES: STILL NEED TO ADD GATES/TRIGGERS FOR ADSR STAGE (PER NOTE/OSCILLATOR) MAX OF 4 POLYPHONY, POTENTIAL DEFINE MAX POLYPHONY TO MAKE IT MORE FLEXIBLE

#include "usart.h"
#include "MIDI.h"
#include <string.h>

#ifndef MIDI_CHANNEL
#define MIDI_CHANNEL	0xFF	//select a MIDI channel for this device to listen to. (0-15). 0xFF means listen to ALL
#endif

extern uint8_t UART6_rxBuffer[UART_BUFF_SIZE];
extern DMA_HandleTypeDef hdma_usart6_rx;

uint8_t MIDI_buffer[UART_BUFF_SIZE]; //MIDI buffer
uint8_t MIDI_buffer_index; //index of current byte (initialized to 0)
uint8_t MIDI_message_length; //total number of bytes (including status) to expect
uint8_t MIDI_max_valid; //index of last valid byte in array (stop point for MIDI_check)
uint8_t MIDI_cmd_state; //FSM parameter for MIDI check (0: status byte, 1: data 1, 2: data 2, etc.)
uint8_t MIDI_cmd_stage[MIDI_MAX_CMD_LEN]; //storage for MIDI command as bytes come in (always recentered around status byte)

void USART6_DATA_RX(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART6) {
		if (HAL_UARTEx_GetRxEventType(USART6) == HAL_UART_RXEVENT_HT) {
			rx_flag = 1; //first half ready
			rx_half = 1;
			MIDI_max_valid = Size;
			memcpy(&MIDI_buffer,&UART6_rxBuffer,Size);
		}
		else if (HAL_UARTEx_GetRxEventType(USART6) == HAL_UART_RXEVENT_TC) {
			rx_flag = 1; //second half ready
			rx_half = 2;
			MIDI_max_valid = Size;
			memcpy(&MIDI_buffer,&UART6_rxBuffer,Size);
		}
		else if (HAL_UARTEx_GetRxEventType(USART6) == HAL_UART_RXEVENT_IDLE) {
			rx_flag = 1; //receive ready (undetermined half)
			MIDI_max_valid = Size;
			memcpy(&MIDI_buffer,&UART6_rxBuffer,Size);
		}
		MIDI_check();
	}
}

void MIDI_init() {
	MIDI_buffer_index = 0;
	MIDI_message_length = UART_BUFF_SIZE;
	HAL_UART_RegisterRxEventCallback(&huart6, USART6_DATA_RX); // register the user-defined RX callback
	HAL_UARTEx_ReceiveToIdle_DMA(&huart6, UART6_rxBuffer, UART_BUFF_SIZE);
}

void MIDI_check() {
	if (rx_flag == 1) {
		// NEW DATA AVAILABLE, RUN STATE MACHINE!
		for (int i = MIDI_buffer_index; i < MIDI_max_valid; i++) {
			uint8_t new_byte = MIDI_buffer[i];
			if (new_byte == 0xFF) {
				// SYSTEM RESET: Used as a panic button. Makes all silent.
				MIDI_cmd_state = 0;
				MIDI_message_length = 0xFF; // prevent accidental parsing of a running status command after this
				MIDI_systemReset();
			}
			else if (new_byte >= 0x80) {
				//status byte
				MIDI_cmd_state = 0;
				// CHECK WHAT TYPE OF MESSAGE, TO SET CORRECT MESSAGE LENGTH
				uint8_t status_msb = new_byte >> 4;
				if ((status_msb == 0x8) || (status_msb == 0x9) || (status_msb == 0xA) || (status_msb == 0xB) || (status_msb == 0xE)) {
					MIDI_message_length = 2; // expect 2 data bytes
				}
				else if ((status_msb == 0xC) || (status_msb == 0xD)) {
					MIDI_message_length = 1; // expect 1 data byte
				}
				else if ((new_byte == 0xF1) || (new_byte == 0xF3)) {
					MIDI_message_length = 1; // expect 1 data byte
				}
				else if (new_byte == 0xF2) {
					MIDI_message_length = 2; // expect 2 data bytes
				}
				else {
					//not a supported MIDI message (probably SysEx)
					MIDI_message_length = 0xFF; // ensure the message parsing *never* occurs
				}
			}
			else {
				//data byte
				MIDI_cmd_state++; //move FSM to next position for data byte
			}
			MIDI_cmd_stage[MIDI_cmd_state] = new_byte;
			if (MIDI_cmd_state >= MIDI_message_length) {
				// Command Is Complete! (in theory)
				// Parse This Command!
				MIDI_parse();
			}
		}
		MIDI_buffer_index = MIDI_max_valid; //reset buffer index to last byte read
		if (MIDI_buffer_index >= UART_BUFF_SIZE - 1) {
			MIDI_buffer_index = 0;
			for (int i= 0; i<UART_BUFF_SIZE;i++)
			{
				MIDI_buffer[i] = 0;
			}
		}
		rx_flag = 0;
	}
}

void MIDI_parse() {
	// What type of message did we receive? (Check status byte value.)
	// Call respective callback based on message received.
	uint8_t status_msb = MIDI_cmd_stage[0] >> 4;
	uint8_t status_lsb = MIDI_cmd_stage[0] & 0xF;
	if ((MIDI_CHANNEL == 0xFF) || (status_lsb == MIDI_CHANNEL) ) {
		if (status_msb == 0x8) {
			// NOTE OFF
			MIDI_noteOff(MIDI_cmd_stage[1],MIDI_cmd_stage[2]);
			MIDI_cmd_state = 0; //reset buffer index in case of running status
		}
		else if (status_msb == 0x9) {
			// NOTE ON
			if (MIDI_cmd_stage[2] == 0) {
				// note_on velocity is 0 --> using Implicit Note Off
				MIDI_noteOff(MIDI_cmd_stage[1],0);
			}
			else {
				MIDI_noteOn(MIDI_cmd_stage[1],MIDI_cmd_stage[2]);
			}
			MIDI_cmd_state = 0; //reset buffer index in case of running status
		}
		else if (status_msb == 0xB) {
			// CONTROL CHANGE (CC)
			MIDI_CC(MIDI_cmd_stage[1],MIDI_cmd_stage[2]);
			MIDI_cmd_state = 0; //reset buffer index in case of running status
		}
		else if (status_msb == 0xE) {
			// PITCH-BEND
			MIDI_pitchBend(MIDI_cmd_stage[1],MIDI_cmd_stage[2]);
			MIDI_cmd_state = 0; //reset buffer index in case of running status
		}
		else {
			//not a supported MIDI message. ignore! :)
		}
	}
	else {
		//not the correct MIDI channel. ignore! :)
	}
}

//
//
// THE FOLLOWING ARE USER-DEFINABLE CALLBACKS

//FOR DEBUG ONLY --- REMOVE LATER
uint8_t num_noteOns = 0;
uint8_t num_noteOffs = 0;
uint8_t num_CCs = 0;
uint8_t num_pitchBends = 0;
uint8_t num_systemResets = 0;

//for some reason __weak doesn't work like it should

//__weak void MIDI_noteOn(uint8_t note_num, uint8_t velocity) {
//	// NOTE ON
//	num_noteOns++; //FOR DEBUG ONLY
//}
//
//__weak void MIDI_noteOff(uint8_t note_num, uint8_t velocity) {
//	// NOTE OFF
//	num_noteOffs++; //FOR DEBUG ONLY
//}
//
//__weak void MIDI_CC(uint8_t control_num, uint8_t value) {
//	// CONTROL CHANGE
//	num_CCs++; //FOR DEBUG ONLY
//}
//
//__weak void MIDI_pitchBend(uint8_t bend_lsb, uint8_t bend_msb) {
//	// PITCH BEND
//	num_pitchBends++; //FOR DEBUG ONLY
//}
//
//__weak void MIDI_systemReset() {
//	// TURN OFF ALL SOUND
//	num_systemResets++; //FOR DEBUG ONLY
//}
