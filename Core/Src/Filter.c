/*
 * Filter.c
 *
 *	Creates a single stage bi-quad filter that can be modulated by the LFO and ADSR
 *  Created on: May 10, 2024
 *      Author: Philip
 */

#include "Filter.h"
#include "stdint.h"
#include "math.h"
#include "lin_to_exp_lut.h"

#define SAMPLE_BUFFER_SIZE 2048

float w = 0;
float alpha = 0;

float b0 = 0;
float b1 = 0;
float b2 = 0;

float a0 = 0;
float a1 = 0;
float a2 = 0;

float yy = 0;
float yy1 = 0;
float yy2 = 0;
float xx = 0;
float xx1 = 0;
float xx2 = 0;
float dist_out = 0;
enum filter_type{LPF, BPF, HPF};

float sin_lut[6283];
float cos_lut[6283];
extern const float lin_to_exp_lut[1344*20];

// Creates look up tables for Sin and Cos functions to be used when calculating new filter coefficients
void filter_init(void)
{
	for (int i=0; i < 6283; i++)
	{
		sin_lut[i] = sinf(2*3.14159*i/6283);
	 	cos_lut[i] = cosf(2*3.14159*i/6283);
	}
}

// Calculates filtered output
void filter(int16_t in_buffer[], int16_t out_buffer[], int cutoff, float q, float lfo_buffer[],
		    float lfo_filter_mod_amount, float ADSR_buffer[], float ADSR_filter_mod_amount, float dist_amount, int filter_type,
			int offset)
{
	int freq_add_to_base = 0;
	int loop_count = 0;

	//adds to cutoff frequency to allow for modulation from LFO and ADSR
	freq_add_to_base = lin_to_exp_lut[(int)(lfo_buffer[0] * lfo_filter_mod_amount * 1343.0 * 20)] - 20;
	freq_add_to_base += lin_to_exp_lut[(int)(ADSR_buffer[0] * ADSR_filter_mod_amount * 1343.0 * 20)] - 20;

	//vales that will later be used to calculate filter coefficients
	w = 2*3.14156*(cutoff+freq_add_to_base)/48000;
	alpha = sinf(w)/(2*q);

	//depending of filter type calculates initial filter coefficients
	switch(filter_type)
	{
		case LPF:
			b0 = (1-cos_lut[(int)(w*1000)])/2;
			b1 = 1-cos_lut[(int)(w*1000)];
			b2 = b0;

			a0 = 1 + alpha;
			a1 = -2*cos_lut[(int)(w*1000)];
			a2 = 1 - alpha;
			break;

		case BPF:
			b0 = alpha;
			b1 = 0;
			b2 = -1*alpha;

			a0 = 1 + alpha;
			a1 = -2*cos_lut[(int)(w*1000)];
			a2 = 1 - alpha;
			break;

		case HPF:
			b0 = (1+cos_lut[(int)(w*1000)])/2;
			b1 = -1*(1+cos_lut[(int)(w*1000)]);
			b2 = b0;

			a0 = 1 + alpha;
			a1 = -2*cos_lut[(int)(w*1000)];
			a2 = 1 - alpha;
			break;
	}

	for (int i = 0; i < SAMPLE_BUFFER_SIZE/2; i+=2)  //fill 1/2 of buffer
	{
		if (loop_count == 0)  //only calculates new filter coefficients every 10 samples to reduce number of computations
		{
			if ((lfo_filter_mod_amount != 0) | (ADSR_filter_mod_amount != 0))
			{
				//adds to cutoff frequency to allow for modulation from LFO and ADSR
				freq_add_to_base = lin_to_exp_lut[(int)(lfo_buffer[i] * lfo_filter_mod_amount * 1343.0 * 20)] - 20;
				freq_add_to_base += lin_to_exp_lut[(int)(ADSR_buffer[i] * ADSR_filter_mod_amount * 1343.0 * 20)] - 20;

				w = 2*3.14156*(cutoff+freq_add_to_base)/48000;
				alpha = sin_lut[(int)((w)/(2*q)*1000)];

				//depending of filter type calculates filter coefficients
				switch(filter_type)
				{
					case LPF:
						b0 = (1-cos_lut[(int)(w*1000)])/2;
						b1 = 1-cos_lut[(int)(w*1000)];
						b2 = b0;

						a0 = 1 + alpha;
						a1 = -2*cos_lut[(int)(w*1000)];
						a2 = 1 - alpha;
						break;

					case BPF:
						b0 = alpha;
						b1 = 0;
						b2 = -1*alpha;

						a0 = 1 + alpha;
						a1 = -2*cos_lut[(int)(w*1000)];
						a2 = 1 - alpha;
						break;

					case HPF:
						b0 = (1+cos_lut[(int)(w*1000)])/2;
						b1 = -1*(1+cos_lut[(int)(w*1000)]);
						b2 = b0;

						a0 = 1 + alpha;
						a1 = -2*cos_lut[(int)(w*1000)];
						a2 = 1 - alpha;
						break;
				}

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

		//actual filtering of input samples to output buffer
		xx = in_buffer[i + offset];
		yy = (b0/a0)*xx + (b1/a0)*xx1 + (b2/a0)*xx2 - (a1/a0)*yy1 - (a2/a0)*yy2;
		xx2 = xx1;
		xx1 = xx;
		yy2 = yy1;
		yy1 = yy;
		dist_out = yy*dist_amount;

		//for high Q filters ensures sample values saturate rather than wrap
		if (dist_out > 32767)
		{
			dist_out = 32767;
		}
		else if(dist_out < -32767 )
		{
			dist_out = -32767;
		}
		out_buffer[i + offset] = dist_out;
	}
}
