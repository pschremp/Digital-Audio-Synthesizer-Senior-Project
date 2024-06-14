/*
 * Osc.h
 *
 *  Created on: May 10, 2024
 *      Author: pschr
 */

#ifndef INC_OSC_H_
#define INC_OSC_H_

#include "stdint.h"

void osc_gen1(int wave_shape, int freq, float duty_cycle, int16_t osc_out_buffer[], float lfo_buffer[], float lfo_pitch_mod_amount, float ADSR_buffer[], float ADSR_pitch_mod_amount, int offset);
void osc_gen2(int wave_shape, int freq, float duty_cycle, int16_t osc_out_buffer[], float lfo_buffer[], float lfo_pitch_mod_amount, float ADSR_buffer[], float ADSR_pitch_mod_amount, int offset);
void osc_gen3(int wave_shape, int freq, float duty_cycle, int16_t osc_out_buffer[], float lfo_buffer[], float lfo_pitch_mod_amount, float ADSR_buffer[], float ADSR_pitch_mod_amount, int offset);
void osc_gen4(int wave_shape, int freq, float duty_cycle, int16_t osc_out_buffer[], float lfo_buffer[], float lfo_pitch_mod_amount, float ADSR_buffer[], float ADSR_pitch_mod_amount, int offset);
void osc_init(void);

#endif /* INC_OSC_H_ */
