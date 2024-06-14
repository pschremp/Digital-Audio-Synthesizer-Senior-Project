/*
 * MIDI.h
 *
 *  Created on: Mar 20, 2024
 *      Author: Alex
 */

#ifndef INC_MIDI_H_
#define INC_MIDI_H_


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "usart.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define MIDI_MAX_CMD_LEN	8
extern uint8_t MIDI_buffer[UART_BUFF_SIZE]; //MIDI buffer
extern uint8_t MIDI_buffer_index; //index of current byte (initialized to 0)
extern uint8_t MIDI_message_length; //total number of bytes (including status) to expect
/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */
void MIDI_init();
void MIDI_check();
void MIDI_parse();
__weak void MIDI_noteOn(uint8_t, uint8_t);
__weak void MIDI_noteOff(uint8_t, uint8_t);
__weak void MIDI_CC(uint8_t, uint8_t);
__weak void MIDI_pitchBend(uint8_t, uint8_t);
__weak void MIDI_systemReset();
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif


#endif /* INC_MIDI_H_ */
