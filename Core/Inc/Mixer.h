/*
 * Mixer.h
 *
 *  Created on: May 10, 2024
 *      Author: pschr
 */

#ifndef INC_MIXER_H_
#define INC_MIXER_H_

#include "stdint.h"

void sum_osc(int16_t osc1[], int16_t osc2[], int16_t osc3[], int16_t osc4[], int16_t mix_out_buffer[],
			 float ADSR_out_buffer[], float ADSR_out_buffer2[], float ADSR_out_buffer3[], float ADSR_out_buffer4[],
			 float ADSR_amp_mod_amount, float lfo_buffer[], float lfo_amp_mod_amount, int offset);

#endif /* INC_MIXER_H_ */
