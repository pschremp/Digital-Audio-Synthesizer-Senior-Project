/*
 * ADSR.h
 *
 *  Created on: May 8, 2024
 *      Author: pschr
 */

#ifndef INC_ADSR_H_
#define INC_ADSR_H_


void ADSR_init(void);
void ADSR(int attack, int decay, float sustain, int release, float ADSR_out_buffer[], int offset);
void ADSR2(int attack, int decay, float sustain, int release, float ADSR_out_buffer[], int offset);
void ADSR3(int attack, int decay, float sustain, int release, float ADSR_out_buffer[], int offset);
void ADSR4(int attack, int decay, float sustain, int release, float ADSR_out_buffer[], int offset);

extern int trigger;
extern int trigger2;
extern int trigger3;
extern int trigger4;
extern int anti_trigger;
extern int anti_trigger2;
extern int anti_trigger3;
extern int anti_trigger4;

#endif /* INC_ADSR_H_ */
