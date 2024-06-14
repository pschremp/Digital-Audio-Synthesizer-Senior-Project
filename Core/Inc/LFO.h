/*
 * LFO.h
 *
 *  Created on: May 10, 2024
 *      Author: pschr
 */

#ifndef INC_LFO_H_
#define INC_LFO_H_

void lfo_gen(int wave_shape, float freq, float duty_cycle, float lfo_out_buffer[], int offset);
void lfo_init(void);

#endif /* INC_LFO_H_ */
