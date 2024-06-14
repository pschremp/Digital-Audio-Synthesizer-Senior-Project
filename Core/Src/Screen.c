/*
 * Screen.c
 *
 *	Displays synthesis parameters on integrated display
 *  Created on: May 29, 2024
 *      Author: pschr
 */

#include "stm32746g_discovery_lcd.h"
#include <stdio.h>
#include "main.h"

extern enum state ui_state;

//Osc Inputs
////////////////////////////////////////////////////////////////////////////

extern int wave_shape;
extern float duty_cycle;
////////////////////////////////////////////////////////////////////////////

//LFO Inputs
////////////////////////////////////////////////////////////////////////////
extern float input_freq_lfo;
extern int lfo_step_size;
extern int lfo_wave_shape;
extern float lfo_pitch_mod_amount;
extern float lfo_amp_mod_amount;
extern float lfo_filter_mod_amount;
////////////////////////////////////////////////////////////////////////////

//Filter Inputs
////////////////////////////////////////////////////////////////////////////
extern int cutoff;
extern int cutoff_step_size;
extern float q;
extern float dist_amount;
extern int filter_type;
////////////////////////////////////////////////////////////////////////////

//ADSR Inputs
////////////////////////////////////////////////////////////////////////////
extern int attack;
extern int decay;
extern float sustain;
extern int release;
extern float ADSR_pitch_mod_amount;
extern float ADSR_amp_mod_amount;
extern float ADSR_filter_mod_amount;
////////////////////////////////////////////////////////////////////////////

// Initializes LCD screen
void LCD_init(void)
{
	BSP_LCD_Init();

	/* LCD Initialization */
	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS+(BSP_LCD_GetXSize()*BSP_LCD_GetYSize()*4));
	BSP_LCD_SelectLayer(0);

	/* Enable the LCD */
	BSP_LCD_DisplayOn();
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SelectLayer(1);
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font24);
}

char wave_shape_char[5][50] = {"SQU", "SAW", "TRI", "SIN", "NOI"};
char duty_cycle_char[50];

char LFO_frequency_char[50];
char lfo_step_size_char[50];

char lfo_pitch_mod_amount_char[50];
char lfo_amp_mod_amount_char[50];
char lfo_filter_mod_amount_char[50];

char attack_char[50];
char decay_char[50];
char sustain_char[50];
char release_char[50];

char ADSR_pitch_mod_amount_char[50];
char ADSR_amp_mod_amount_char[50];
char ADSR_filter_mod_amount_char[50];

char cutoff_char[50];
char cutoff_step_size_char[50];
char q_char[50];
char dist_amount_char[50];
char filter_type_char[3][50] = {"LPF", "BPF", "HPF"};

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

// Updates what is displayed on the LCD screen
void update_LCD(void)
{
	BSP_LCD_SelectLayer(1);
	BSP_LCD_Clear(LCD_COLOR_WHITE);

	switch (ui_state)
	{

		case INIT:  //in INIT state display nothing
			break;
		case START:  //in START state display nothing
			break;

		case OSC:  //in OSC state display OSC parameters
			BSP_LCD_DisplayStringAt(0, 30*0, (uint8_t*)"Oscillator Parameters", LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*2, (uint8_t*)"Wave Shape:", LEFT_MODE);
			BSP_LCD_DisplayStringAt(300, 30*2, (uint8_t*)wave_shape_char[wave_shape], LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*3, (uint8_t*)"Duty Cycle:", LEFT_MODE);
			sprintf(duty_cycle_char, "%.02f", duty_cycle);
			BSP_LCD_DisplayStringAt(300, 30*3, (uint8_t*)duty_cycle_char, LEFT_MODE);

			break;

		case LFO:  //in LFO state display LFO parameters
			BSP_LCD_DisplayStringAt(0, 30*0, (uint8_t*)"LFO Parameters", LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*2, (uint8_t*)"Frequency:", LEFT_MODE);
			sprintf(LFO_frequency_char, "%.02f", input_freq_lfo);
			BSP_LCD_DisplayStringAt(300, 30*2, (uint8_t*)LFO_frequency_char, LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*3, (uint8_t*)"Step Size:", LEFT_MODE);
			sprintf(lfo_step_size_char, "%i",lfo_step_size);
			BSP_LCD_DisplayStringAt(300, 30*3, (uint8_t*)lfo_step_size_char, LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*4, (uint8_t*)"Wave Shape:", LEFT_MODE);
			BSP_LCD_DisplayStringAt(300, 30*4, (uint8_t*)wave_shape_char[lfo_wave_shape], LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*5, (uint8_t*)"Duty Cycle:", LEFT_MODE);
			sprintf(duty_cycle_char, "%.02f", duty_cycle);
			BSP_LCD_DisplayStringAt(300, 30*5, (uint8_t*)duty_cycle_char, LEFT_MODE);

			break;

		case LFO_MOD:  //in LFO_MOD state display LFO_MOD parameters
			BSP_LCD_DisplayStringAt(0, 30*0, (uint8_t*)"LFO Modulation Destinations", LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*2, (uint8_t*)"Pitch:", LEFT_MODE);
			sprintf(lfo_pitch_mod_amount_char, "%.02f", lfo_pitch_mod_amount);
			BSP_LCD_DisplayStringAt(300, 30*2, (uint8_t*)lfo_pitch_mod_amount_char, LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*3, (uint8_t*)"Filter:", LEFT_MODE);
			sprintf(lfo_filter_mod_amount_char, "%.02f", lfo_filter_mod_amount);
			BSP_LCD_DisplayStringAt(300, 30*3, (uint8_t*)lfo_filter_mod_amount_char, LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*4, (uint8_t*)"Amp:", LEFT_MODE);
			sprintf(lfo_amp_mod_amount_char, "%.02f", lfo_amp_mod_amount);
			BSP_LCD_DisplayStringAt(300, 30*4, (uint8_t*)lfo_amp_mod_amount_char, LEFT_MODE);

			break;

		case ADSR:  //in ADSR state display ADSR parameters
			BSP_LCD_DisplayStringAt(0, 30*0, (uint8_t*)"Envelope Parameters", LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*2, (uint8_t*)"Attack:", LEFT_MODE);
			sprintf(attack_char, "%i", attack);
			BSP_LCD_DisplayStringAt(300, 30*2, (uint8_t*)attack_char, LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*3, (uint8_t*)"Decay:", LEFT_MODE);
			sprintf(decay_char, "%i", decay);
			BSP_LCD_DisplayStringAt(300, 30*3, (uint8_t*)decay_char, LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*4, (uint8_t*)"Sustain:", LEFT_MODE);
			sprintf(sustain_char, "%.02f", sustain);
			BSP_LCD_DisplayStringAt(300, 30*4, (uint8_t*)sustain_char, LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*5, (uint8_t*)"Release:", LEFT_MODE);
			sprintf(release_char, "%i", release);
			BSP_LCD_DisplayStringAt(300, 30*5, (uint8_t*)release_char, LEFT_MODE);

			break;

		case ADSR_MOD:  //in ADSR_MOD state display ADSR_MOD parameters
			BSP_LCD_DisplayStringAt(0, 30*0, (uint8_t*)"ADSR Modulation Destinations", LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*2, (uint8_t*)"Pitch:", LEFT_MODE);
			sprintf(ADSR_pitch_mod_amount_char, "%.02f", ADSR_pitch_mod_amount);
			BSP_LCD_DisplayStringAt(300, 30*2, (uint8_t*)ADSR_pitch_mod_amount_char, LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*3, (uint8_t*)"Filter:", LEFT_MODE);
			sprintf(ADSR_filter_mod_amount_char, "%.02f", ADSR_filter_mod_amount);
			BSP_LCD_DisplayStringAt(300, 30*3, (uint8_t*)ADSR_filter_mod_amount_char, LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*4, (uint8_t*)"Amp:", LEFT_MODE);
			sprintf(ADSR_amp_mod_amount_char, "%.02f", ADSR_amp_mod_amount);
			BSP_LCD_DisplayStringAt(300, 30*4, (uint8_t*)ADSR_amp_mod_amount_char, LEFT_MODE);

			break;

		case FILTER: //in FILTER state display FILTER parameters
			BSP_LCD_DisplayStringAt(0, 30*0, (uint8_t*)"Filter Parameters", LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*2, (uint8_t*)"Cutoff:", LEFT_MODE);
			sprintf(cutoff_char, "%i", cutoff);
			BSP_LCD_DisplayStringAt(300, 30*2, (uint8_t*)cutoff_char, LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*3, (uint8_t*)"Step Size:", LEFT_MODE);
			sprintf(cutoff_step_size_char, "%i", cutoff_step_size);
			BSP_LCD_DisplayStringAt(300, 30*3, (uint8_t*)cutoff_step_size_char, LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*4, (uint8_t*)"Q:", LEFT_MODE);
			sprintf(q_char, "%.02f", q);
			BSP_LCD_DisplayStringAt(300, 30*4, (uint8_t*)q_char, LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*5, (uint8_t*)"Filter Type:", LEFT_MODE);
			//sprintf(sustain_char, "%.02f", filter);
			BSP_LCD_DisplayStringAt(300, 30*5, (uint8_t*)filter_type_char[filter_type], LEFT_MODE);

			break;

		case FX:  //in FX state display FX parameters
			BSP_LCD_DisplayStringAt(0, 30*0, (uint8_t*)"Effect Parameters", LEFT_MODE);

			BSP_LCD_DisplayStringAt(0, 30*2, (uint8_t*)"Distortion:", LEFT_MODE);
			sprintf(dist_amount_char, "%.02f", dist_amount);
			BSP_LCD_DisplayStringAt(300, 30*2, (uint8_t*)dist_amount_char, LEFT_MODE);
			break;
	}
}

