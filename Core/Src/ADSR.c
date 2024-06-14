/*
 * ADSR.c
 *
 *
 *	Creates 4 independent envelopes to control the amplitudes of the 4 osc
 *  Created on: May 8, 2024
 *      Author: Philip
 */

#include "ADSR.h"

#define SAMPLE_BUFFER_SIZE 2048

int trigger = 0;
int trigger2 = 0;
int trigger3 = 0;
int trigger4 = 0;
int anti_trigger = 0;
int anti_trigger2 = 0;
int anti_trigger3 = 0;
int anti_trigger4 = 0;
float ADSR_sample_value = 0;
float ADSR_sample_value2 = 0;
float ADSR_sample_value3 = 0;
float ADSR_sample_value4 = 0;
enum ADSR_state{ATTACK, DECAY, SUSTAIN, RELEASE};
int ADSR_state = RELEASE;
int ADSR_state2 = RELEASE;
int ADSR_state3 = RELEASE;
int ADSR_state4 = RELEASE;
int num_release_samples = 0;
int num_release_samples2 = 0;
int num_release_samples3 = 0;
int num_release_samples4 = 0;

// Initializes sample values and states for all envelopes
void ADSR_init(void)
{
	trigger = 0;
	trigger2 = 0;
	trigger3 = 0;
	trigger4 = 0;
	anti_trigger = 0;
	anti_trigger2 = 0;
	anti_trigger3 = 0;
	anti_trigger4 = 0;
	ADSR_sample_value = 0;
	ADSR_sample_value2 = 0;
	ADSR_sample_value3 = 0;
	ADSR_sample_value4 = 0;
	ADSR_state = RELEASE;
	ADSR_state2 = RELEASE;
	ADSR_state3 = RELEASE;
	ADSR_state4 = RELEASE;
	num_release_samples = 0;
	num_release_samples2 = 0;
	num_release_samples3 = 0;
	num_release_samples4 = 0;
}


// Calculates new sample values for the envelope
void ADSR(int attack, int decay, float sustain, int release, float ADSR_out_buffer[], int offset)
{
	int num_attack_samples = 48 * attack;
	int num_decay_samples = 48 * decay;
	int num_release_samples_static = 48 * release;

	float attack_incr = (1.0/(float)num_attack_samples);
	float decay_incr = ((1.0-sustain)/(float)num_decay_samples);
	float release_incr = 0;

	int loop_count = 0;

	if (ADSR_state == RELEASE &&  num_release_samples != 0)
	{
		release_incr = ADSR_sample_value/num_release_samples;
	}

	for (int i = 0; i<SAMPLE_BUFFER_SIZE/2; i += 2)  //fills 1/2 of buffer
	{
		if (loop_count == 0)  //only calculates new sample value every 10 samples to help with computation speed
		{
			if (trigger == 1)
			{
				ADSR_state = ATTACK;
				trigger = 0;
			}
			else if (anti_trigger == 1)
			{
				ADSR_state = RELEASE;
				num_release_samples = num_release_samples_static;
				release_incr = (ADSR_sample_value/num_release_samples);
				anti_trigger = 0;
			}
			switch (ADSR_state)
			{
			case ATTACK:  //increments envelope sample value from 0 to 1
				ADSR_out_buffer[i+ offset] = ADSR_sample_value;
				ADSR_sample_value += attack_incr;
				if (ADSR_sample_value > 1.0)
				{
					ADSR_sample_value = 1.0;
					ADSR_state = DECAY;
				}
				break;

			case DECAY:  //decrements envelope sample value from 1 to sustain level
				ADSR_out_buffer[i+ offset] = ADSR_sample_value;
				ADSR_sample_value -= decay_incr;
				if (ADSR_sample_value < sustain)
				{
					ADSR_sample_value = sustain;
					ADSR_state = SUSTAIN;
				}
				break;

			case SUSTAIN:  //holds current envelope sample value
				ADSR_out_buffer[i+ offset] = ADSR_sample_value;
				break;

			case RELEASE:  //decrements envelope sample value from sustain level to 0
				ADSR_out_buffer[i+ offset] = ADSR_sample_value;
				ADSR_sample_value -= release_incr;
				if (num_release_samples > 0)
				{
					num_release_samples--;
				}
				if (ADSR_sample_value < 0)
				{
					ADSR_sample_value = 0;
				}
				break;
			}
		}
		else if(loop_count == 10)
		{
			loop_count = 0;
		}
		else
		{
			loop_count++;
			ADSR_out_buffer[i+ offset] = ADSR_sample_value;
		}
	}
}

void ADSR2(int attack, int decay, float sustain, int release, float ADSR_out_buffer2[], int offset)
{
	int num_attack_samples = 48 * attack;
	int num_decay_samples = 48 * decay;
	int num_release_samples_static = 48 * release;

	float attack_incr = 1.0/(float)num_attack_samples;
	float decay_incr = (1.0-sustain)/(float)num_decay_samples;
	float release_incr = 0;

	int loop_count = 0;

	if (ADSR_state2 == RELEASE &&  num_release_samples2 != 0)
	{
		release_incr = ADSR_sample_value2/num_release_samples2;
	}

	for (int i = 0; i<SAMPLE_BUFFER_SIZE/2; i += 2)
	{
		if (loop_count == 0)
		{
			if (trigger2 == 1)
			{
				ADSR_state2 = ATTACK;
				trigger2 = 0;
			}
			else if (anti_trigger2 == 1)
			{
				ADSR_state2 = RELEASE;
				num_release_samples2 = num_release_samples_static;
				release_incr = ADSR_sample_value2/num_release_samples2;
				anti_trigger2 = 0;
			}
			switch (ADSR_state2)
			{
			case ATTACK:
				ADSR_out_buffer2[i+ offset] = ADSR_sample_value2;
				ADSR_sample_value2 += attack_incr;
				if (ADSR_sample_value2 > 1.0)
				{
					ADSR_sample_value2 = 1.0;
					ADSR_state2 = DECAY;
				}
				break;

			case DECAY:
				ADSR_out_buffer2[i+ offset] = ADSR_sample_value2;
				ADSR_sample_value2 -= decay_incr;
				if (ADSR_sample_value2 < sustain)
				{
					ADSR_sample_value2 = sustain;
					ADSR_state2 = SUSTAIN;
				}
				break;

			case SUSTAIN:
				ADSR_out_buffer2[i+ offset] = ADSR_sample_value2;
				break;

			case RELEASE:
				ADSR_out_buffer2[i+ offset] = ADSR_sample_value2;
				ADSR_sample_value2 -= release_incr;
				if (num_release_samples > 0)
				{
					num_release_samples2--;
				}
				if (ADSR_sample_value2 < 0)
				{
					ADSR_sample_value2 = 0;
				}
				break;
			}
		}
		else if (loop_count == 10)
		{
			loop_count = 0;
		}
		else
		{
			loop_count++;
			ADSR_out_buffer2[i+ offset] = ADSR_sample_value2;
		}
	}
}

void ADSR3(int attack, int decay, float sustain, int release, float ADSR_out_buffer3[], int offset)
{
	int num_attack_samples = 48 * attack;
	int num_decay_samples = 48 * decay;
	int num_release_samples_static = 48 * release;

	float attack_incr = 1.0/(float)num_attack_samples;
	float decay_incr = (1.0-sustain)/(float)num_decay_samples;
	float release_incr = 0;

	int loop_count = 0;

	if (ADSR_state3 == RELEASE &&  num_release_samples3 != 0)
	{
		release_incr = ADSR_sample_value3/num_release_samples3;
	}

	for (int i = 0; i<SAMPLE_BUFFER_SIZE/2; i += 2)
	{
		if (loop_count == 0)
		{
			if (trigger3 == 1)
			{
				ADSR_state3 = ATTACK;
				trigger3 = 0;
			}
			else if (anti_trigger3 == 1)
			{
				ADSR_state3 = RELEASE;
				num_release_samples3 = num_release_samples_static;
				release_incr = ADSR_sample_value3/num_release_samples3;
				anti_trigger3 = 0;
			}
			switch (ADSR_state3)
			{
			case ATTACK:
				ADSR_out_buffer3[i+ offset] = ADSR_sample_value3;
				ADSR_sample_value3 += attack_incr;
				if (ADSR_sample_value3 > 1.0)
				{
					ADSR_sample_value3 = 1.0;
					ADSR_state3 = DECAY;
				}
				break;

			case DECAY:
				ADSR_out_buffer3[i+ offset] = ADSR_sample_value3;
				ADSR_sample_value3 -= decay_incr;
				if (ADSR_sample_value3 < sustain)
				{
					ADSR_sample_value3 = sustain;
					ADSR_state3 = SUSTAIN;
				}
				break;

			case SUSTAIN:
				ADSR_out_buffer3[i+ offset] = ADSR_sample_value3;
				break;

			case RELEASE:
				ADSR_out_buffer3[i+ offset] = ADSR_sample_value3;
				ADSR_sample_value3 -= release_incr;
				if (num_release_samples > 0)
				{
					num_release_samples3--;
				}
				if (ADSR_sample_value3 < 0)
				{
					ADSR_sample_value3 = 0;
				}
				break;
			}
		}
		else if (loop_count == 10)
		{
			loop_count = 0;
		}
		else
		{
			loop_count++;
			ADSR_out_buffer3[i+ offset] = ADSR_sample_value3;
		}
	}
}

void ADSR4(int attack, int decay, float sustain, int release, float ADSR_out_buffer4[], int offset)
{
	int num_attack_samples = 48 * attack;
	int num_decay_samples = 48 * decay;
	int num_release_samples_static = 48 * release;

	float attack_incr = 1.0/(float)num_attack_samples;
	float decay_incr = (1.0-sustain)/(float)num_decay_samples;
	float release_incr = 0;

	int loop_count = 0;

	if (ADSR_state4 == RELEASE &&  num_release_samples4 != 0)
	{
		release_incr = ADSR_sample_value4/num_release_samples4;
	}

	for (int i = 0; i<SAMPLE_BUFFER_SIZE/2; i += 2)
	{
		if (loop_count == 0)
		{
			if (trigger4 == 1)
			{
				ADSR_state4 = ATTACK;
				trigger4 = 0;
			}
			else if (anti_trigger4 == 1)
			{
				ADSR_state4 = RELEASE;
				num_release_samples4 = num_release_samples_static;
				release_incr = ADSR_sample_value4/num_release_samples4;
				anti_trigger4 = 0;
			}
			switch (ADSR_state4)
			{
			case ATTACK:
				ADSR_out_buffer4[i+ offset] = ADSR_sample_value4;
				ADSR_sample_value4 += attack_incr;
				if (ADSR_sample_value4 > 1.0)
				{
					ADSR_sample_value4 = 1.0;
					ADSR_state4 = DECAY;
				}
				break;

			case DECAY:
				ADSR_out_buffer4[i+ offset] = ADSR_sample_value4;
				ADSR_sample_value4 -= decay_incr;
				if (ADSR_sample_value4 < sustain)
				{
					ADSR_sample_value4 = sustain;
					ADSR_state4 = SUSTAIN;
				}
				break;

			case SUSTAIN:
				ADSR_out_buffer4[i+ offset] = ADSR_sample_value4;
				break;

			case RELEASE:
				ADSR_out_buffer4[i+ offset] = ADSR_sample_value4;
				ADSR_sample_value4 -= release_incr;
				if (num_release_samples > 0)
				{
					num_release_samples4--;
				}
				if (ADSR_sample_value4 < 0)
				{
					ADSR_sample_value4 = 0;
				}
				break;
			}
		}
		else if (loop_count == 10)
		{
			loop_count = 0;
		}
		else
		{
			loop_count++;
			ADSR_out_buffer4[i+ offset] = ADSR_sample_value4;
		}
	}
}
