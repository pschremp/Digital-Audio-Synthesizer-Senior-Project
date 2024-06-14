/*
 * ui_engine.c
 *
 *  Created on: May 16, 2024
 *      Author: Alex
 */

#include "seesaw.h"
#include "encoder.h"
#include "main.h"
#include "i2c.h"
#include "ui_engine.h"
#include <math.h>
#include "Screen.h"

//Osc Inputs
extern int wave_shape;
extern float duty_cycle;

//LFO Inputs
extern float input_freq_lfo;
extern int lfo_step_size;
extern int lfo_wave_shape;
extern float lfo_pitch_mod_amount;
extern float lfo_amp_mod_amount;
extern float lfo_filter_mod_amount;

//Filter Inputs
extern int cutoff;
extern int cutoff_step_size;
extern float q;
extern float dist_amount;
extern int filter_type;

//ADSR Inputs
extern int attack;
extern int decay;
extern float sustain;
extern int release;
extern float ADSR_pitch_mod_amount;
extern float ADSR_amp_mod_amount;
extern float ADSR_filter_mod_amount;

int dummy_int = 0;
float dummy_float = 0.0;

enum state {
	INIT,
	START,
	OSC,
	FILTER,
	ADSR,
	ADSR_MOD,
	LFO,
	LFO_MOD,
	FX
};

enum state ui_state = INIT;

uint8_t  buttons[4] = {0,0,0,0};
uint8_t  prev_buttons[4] = {0,0,0,0};
uint32_t startpos[4] = {0,0,0,0};
uint32_t initparamval[4] = {0,0,0,0};
int32_t enc_delta[4] = {0,0,0,0};

float param_max[7][4] =  {{4,0.99,0,0},
							{20000,4,10,2},
							{4000,4000,1,4000},
							{1,1,1,0},
							{20000,5,4,0.99},
							{1,1,1,0},
							{10,0,0,0}};

float param_min[7][4] =  {{0,0.01,0,0},
							{0,0,0,0},
							{1,1,0,1},
							{0,0,0,0},
							{0.01,0,0,0.01},
							{0,0,0,0},
							{0,0,0,0}};

float param_step[7][4] =  {{1,0.01,0,0},
							{1,1,0.1,1},
							{1,1,0.01,1},
							{0.01,0.01,0.01,0},
							{0.01,1,1,0.01},
							{0.01,0.01,0.01,0},
							{0.1,0,0,0}};

int* param_pointers_int[7][4] = {{&wave_shape,&dummy_int,&dummy_int,&dummy_int},
								{&cutoff,&cutoff_step_size,&dummy_int,&filter_type},
								{&attack,&decay,&dummy_int,&release},
								{&dummy_int,&dummy_int,&dummy_int,&dummy_int},
								{&dummy_int,&lfo_step_size,&lfo_wave_shape,&dummy_int},
								{&dummy_int,&dummy_int,&dummy_int,&dummy_int},
								{&dummy_int,&dummy_int,&dummy_int,&dummy_int}};

float* param_pointers_float[7][4] = {{&dummy_float,&duty_cycle,&dummy_float,&dummy_float},
								{&dummy_float,&dummy_float,&q,&dummy_float},
								{&dummy_float,&dummy_float,&sustain,&dummy_float},
								{&ADSR_pitch_mod_amount,&ADSR_amp_mod_amount,&ADSR_filter_mod_amount,&dummy_float},
								{&input_freq_lfo,&dummy_float,&dummy_float,&duty_cycle},
								{&lfo_pitch_mod_amount,&lfo_amp_mod_amount,&lfo_filter_mod_amount,&dummy_float},
								{&dist_amount,&dummy_float,&dummy_float,&dummy_float}};

void UI_Engine_Init() {
	for (uint8_t id = 0; id < 4; id++) {
		initEncoder(id);
		initPixel(id);
		initPushButton(id);
		enableEncoderInterrupt(id);
		setPixelColor(id,0,0,0);
		startpos[id] = getEncoderPosition(id);
	}
	ui_state = START;
}

void getEncoderInitPos() {
	for (int i = 0; i < 4; i++) {
		startpos[i] = getEncoderPosition(i);
	}
}

void debounceCheckButtons() {
//	buttons[0] = checkPushButton(0);
//	buttons[1] = checkPushButton(1);
//	buttons[2] = checkPushButton(2);
//	buttons[3] = checkPushButton(3);

	for (int i = 0; i < 4; i++) {
		uint8_t button_state = checkPushButton(i);
		if (prev_buttons[i] != button_state) {
			buttons[i] = button_state;
			prev_buttons[i] = button_state;
		}
		else {
			buttons[i] = 0;
		}
	}
}

void getEncoderDelta() {
	for (int i = 0; i < 4; i++) {
		uint32_t pos = getEncoderPosition(i);
		enc_delta[i] = pos - startpos[i];
		startpos[i] = pos;
	}
}

void updateParamSteps() {
	// update lfo_step_size
	param_step[4][0] = pow(10,lfo_step_size-2);
	// update cutoff_step_size
	param_step[1][0] = pow(10,cutoff_step_size);
}

void UI_Engine_Handle() {
	getEncoderDelta(); //check encoder positions and determine how much each has changed
	debounceCheckButtons(); //updates the buttons[] array with debounced button states
	updateParamSteps(); // update cutoff step size and lfo freq step sizes in array

	// EACH STATE SHALL HAVE AN LED COLOR SETTING, A SET OF BUTTON CHECKS TO SWITCH STATES, AND A BUNCH OF ENCODER READS
	if (ui_state >= 2) {
		for (int i = 0; i < 4; i++) {
			if (enc_delta[i] != 0) {
				*(param_pointers_int[ui_state-2][i]) = *(param_pointers_int[ui_state-2][i]) + (enc_delta[i] * ((int) param_step[ui_state-2][i]));
				if (*(param_pointers_int[ui_state-2][i]) >= (int) param_max[ui_state-2][i]) {
					*(param_pointers_int[ui_state-2][i]) = (int) param_max[ui_state-2][i];
				}
				else if (*(param_pointers_int[ui_state-2][i]) <= (int) param_min[ui_state-2][i]) {
					*(param_pointers_int[ui_state-2][i]) = (int) param_min[ui_state-2][i];
				}
				*(param_pointers_float[ui_state-2][i]) = *(param_pointers_float[ui_state-2][i]) + ((float) enc_delta[i] * param_step[ui_state-2][i]);
				if (*(param_pointers_float[ui_state-2][i]) >= param_max[ui_state-2][i]) {
					*(param_pointers_float[ui_state-2][i]) = param_max[ui_state-2][i];
				}
				else if (*(param_pointers_float[ui_state-2][i]) <= param_min[ui_state-2][i]) {
					*(param_pointers_float[ui_state-2][i]) = param_min[ui_state-2][i];
				}
				update_LCD();
			}
		}
	}

	switch (ui_state) {
	case INIT:
		//should never get here in theory
		UI_Engine_Init();
		break;
	case START:
		setPixelColor(3,255,255,255);
		if (checkPushButton(3)) {
			ui_state = OSC;
			getEncoderInitPos();
			update_LCD();
		}
		break;
	case OSC:
		setPixelColor(0,255,0,0);
		setPixelColor(1,255,0,0);
		setPixelColor(2,255,0,0);
		setPixelColor(3,255,0,0);
		if (buttons[0]) {
			ui_state = FX;
			getEncoderInitPos();
			update_LCD();
		}
		else if (buttons[1]) {
			ui_state = FILTER;
			getEncoderInitPos();
			update_LCD();
		}
		break;
	case FILTER:
		setPixelColor(0,255,255,0);
		setPixelColor(1,255,255,0);
		setPixelColor(2,255,255,0);
		setPixelColor(3,255,255,0);
		if (buttons[0]) {
			ui_state = OSC;
			getEncoderInitPos();
			update_LCD();
		}
		else if (buttons[1]) {
			ui_state = ADSR;
			getEncoderInitPos();
			update_LCD();
		}
		break;
	case ADSR:
		setPixelColor(0,0,255,0);
		setPixelColor(1,0,255,0);
		setPixelColor(2,0,255,0);
		setPixelColor(3,0,255,0);
		if (buttons[0]) {
			ui_state = FILTER;
			getEncoderInitPos();
			update_LCD();
		}
		else if (buttons[1]) {
			ui_state = ADSR_MOD;
			getEncoderInitPos();
			update_LCD();
		}
		break;
	case ADSR_MOD:
		setPixelColor(0,0,255,255);
		setPixelColor(1,0,255,255);
		setPixelColor(2,0,255,255);
		setPixelColor(3,0,255,255);
		if (buttons[0]) {
			ui_state = ADSR;
			getEncoderInitPos();
			update_LCD();
		}
		else if (buttons[1]) {
			ui_state = LFO;
			getEncoderInitPos();
			update_LCD();
		}
		break;
	case LFO:
		setPixelColor(0,0,0,255);
		setPixelColor(1,0,0,255);
		setPixelColor(2,0,0,255);
		setPixelColor(3,0,0,255);
		if (buttons[0]) {
			ui_state = ADSR_MOD;
			getEncoderInitPos();
			update_LCD();
		}
		else if (buttons[1]) {
			ui_state = LFO_MOD;
			getEncoderInitPos();
			update_LCD();
		}
		break;
	case LFO_MOD:
		setPixelColor(0,255,0,255);
		setPixelColor(1,255,0,255);
		setPixelColor(2,255,0,255);
		setPixelColor(3,255,0,255);
		if (buttons[0]) {
			ui_state = LFO;
			getEncoderInitPos();
			update_LCD();
		}
		else if (buttons[1]) {
			ui_state = FX;
			getEncoderInitPos();
			update_LCD();
		}
		break;
	case FX:
		setPixelColor(0,255,255,255);
		setPixelColor(1,255,255,255);
		setPixelColor(2,255,255,255);
		setPixelColor(3,255,255,255);
		if (buttons[0]) {
			ui_state = LFO_MOD;
			getEncoderInitPos();
			update_LCD();
		}
		else if (buttons[1]) {
			ui_state = OSC;
			getEncoderInitPos();
			update_LCD();
		}
		break;
	}
}
