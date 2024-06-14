/*
 * Filter.h
 *
 *  Created on: May 10, 2024
 *      Author: pschr
 */

#ifndef INC_FILTER_H_
#define INC_FILTER_H_

#include "stdint.h"

void filter(int16_t in_buffer[], int16_t out_buffer[], int cutoff, float q, float lfo_buffer[],
		    float lfo_filter_mod_amount, float ADSR_buffer[], float ADSR_filter_mod_amount, float dist_amount, int filter_type,
			int offset);
void filter_init(void);

#endif /* INC_FILTER_H_ */
