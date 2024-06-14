/*
 * Mixer.c
 *
 *	Adds together the four independent voices of the synth engine
 *  Created on: May 10, 2024
 *      Author: pschr
 */

#include "Mixer.h"
#include "stdint.h"

#define SAMPLE_BUFFER_SIZE 2048

// Adds together the 4 voices of the synth engine
void sum_osc(int16_t osc1[], int16_t osc2[], int16_t osc3[], int16_t osc4[], int16_t mix_out_buffer[],
			 float ADSR_buffer[],float ADSR_buffer2[], float ADSR_buffer3[], float ADSR_buffer4[], float ADSR_amp_mod_amount,
			 float lfo_buffer[], float lfo_amp_mod_amount, int offset)
{
	int16_t osc1_sample_value = 0;
	int16_t osc2_sample_value = 0;
	int16_t osc3_sample_value = 0;
	int16_t osc4_sample_value = 0;
	float ADSR_scaling1 = 1;
	float ADSR_scaling2 = 1;
	float ADSR_scaling3 = 1;
	float ADSR_scaling4 = 1;
	float lfo_scaling = 1;
	int loop_count = 0;

	for(int i = 0; i < SAMPLE_BUFFER_SIZE/2; i+=2)  //fills 1/2 of buffer
	{
		if (loop_count == 0)  //only calculat new ADSR scaling values every 10 samples to reduce computational intensity
		{
			if ((lfo_amp_mod_amount != 0) | (ADSR_amp_mod_amount != 0))
			{
				//uses ADSR sample value and ADSR mod amount to calculate a value to scale the osc by
				ADSR_scaling1 = 1.0-((1.0-ADSR_buffer[i])*ADSR_amp_mod_amount);
				ADSR_scaling2 = 1.0-((1.0-ADSR_buffer2[i])*ADSR_amp_mod_amount);
				ADSR_scaling3 = 1.0-((1.0-ADSR_buffer3[i])*ADSR_amp_mod_amount);
				ADSR_scaling4 = 1.0-((1.0-ADSR_buffer4[i])*ADSR_amp_mod_amount);
				lfo_scaling = 1.0-((1.0-lfo_buffer[i])*lfo_amp_mod_amount);
			}
			loop_count++;
		}
		else if (loop_count == 10)
		{
			loop_count = 0;
		}
		else
		{
			loop_count++;
		}
		//scales osc by corresponding ADSR value
		if (osc1[i+offset] != 0)
		{
			osc1_sample_value = osc1[i+offset]*ADSR_scaling1;
		}
		if (osc2[i+offset] != 0)
		{
			osc2_sample_value = osc2[i+offset]*ADSR_scaling2;
		}
		if (osc3[i+offset] != 0)
		{
			osc3_sample_value = osc3[i+offset]*ADSR_scaling3;
		}
		if (osc4[i+offset] != 0)
		{
			osc4_sample_value = osc4[i+offset]*ADSR_scaling4;
		}
		//adds together the 4 voices and scales to LFO scaling value
		mix_out_buffer[i + offset] = (float)(osc1_sample_value + osc2_sample_value + osc3_sample_value + osc4_sample_value) * lfo_scaling;
	}
}
