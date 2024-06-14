/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MIDI.h"
#include <string.h>
#define ARM_MATH_CM7
#include "stm32746g_discovery_audio.h"
#include "stm32746g_discovery.h"
#include "math.h"
#include "stdlib.h"
#include "ADSR.h"
#include "LFO.h"
#include "Filter.h"
#include "Mixer.h"
#include "Osc.h"
#include "stm32746g_discovery_lcd.h"
#include <stdio.h>
#include "Screen.h"
#include "seesaw.h"
#include "encoder.h"
#include "ui_engine.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define __FPU_PRESENT	1U	// YES WE HAVE AN FPU
#define SAMPLE_BUFFER_SIZE 2048
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */
void mtof_init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


//Buffer Admin
//////////////////////////////////////////////////////////////////////////////
int16_t audio_out_sample_buffer[SAMPLE_BUFFER_SIZE];
int16_t osc1_out_buffer[SAMPLE_BUFFER_SIZE];
int16_t osc2_out_buffer[SAMPLE_BUFFER_SIZE];
int16_t osc3_out_buffer[SAMPLE_BUFFER_SIZE];
int16_t osc4_out_buffer[SAMPLE_BUFFER_SIZE];
int16_t mix_out_buffer[SAMPLE_BUFFER_SIZE];
int read1_complete = 0;
int read2_complete = 0;
int pause = 0;
int offset = 0;
int loop_count = 0;
//////////////////////////////////////////////////////////////////////////////

//Osc Admin
////////////////////////////////////////////////////////////////////////////
extern enum wave_shape{SQU, SAW, TRI, SIN, NOI};
////////////////////////////////////////////////////////////////////////////

//LFO Admin
////////////////////////////////////////////////////////////////////////////
float lfo_out_buffer[SAMPLE_BUFFER_SIZE];
////////////////////////////////////////////////////////////////////////////

//Filter Admin
////////////////////////////////////////////////////////////////////////////
extern enum filter_type{LPF, BPF, HPF};
////////////////////////////////////////////////////////////////////////////

//ADSR Admin
////////////////////////////////////////////////////////////////////////////
float ADSR_out_buffer[SAMPLE_BUFFER_SIZE];
float ADSR_out_buffer2[SAMPLE_BUFFER_SIZE];
float ADSR_out_buffer3[SAMPLE_BUFFER_SIZE];
float ADSR_out_buffer4[SAMPLE_BUFFER_SIZE];
////////////////////////////////////////////////////////////////////////////

//MIDI Admin
////////////////////////////////////////////////////////////////////////////
int note_value;
int note_velocity;
int CC_num;
int CC_value;
int pitchbend;
int note_freq = 0;

int available_osc[] = {1, 1, 1, 1};
int active_notes[] = {0, 0, 0, 0};
int mtof_lut[127];
int ui_loop_counter = 0;
int ui_loop_index = 7;

////////////////////////////////////////////////////////////////////////////



//Osc Inputs
////////////////////////////////////////////////////////////////////////////
int input_freq_osc1 = 0;
int input_freq_osc2 = 0;
int input_freq_osc3 = 0;
int input_freq_osc4 = 0;
int wave_shape = SAW;
float duty_cycle = 0.5;
////////////////////////////////////////////////////////////////////////////

//LFO Inputs
////////////////////////////////////////////////////////////////////////////
float input_freq_lfo = 20;
int lfo_step_size = 0;
int lfo_wave_shape = SQU;
float lfo_pitch_mod_amount = 0;
float lfo_amp_mod_amount = 0;
float lfo_filter_mod_amount = 0;
////////////////////////////////////////////////////////////////////////////

//Filter Inputs
////////////////////////////////////////////////////////////////////////////
int cutoff = 20000;
int cutoff_step_size = 0;
float q = 1;
float dist_amount = 2.20;
int filter_type = LPF;
////////////////////////////////////////////////////////////////////////////

//ADSR Inputs
////////////////////////////////////////////////////////////////////////////
int attack = 1;
int decay = 1;
float sustain = 0.5;
int release = 1;
float ADSR_pitch_mod_amount = 0;
float ADSR_amp_mod_amount = 1;
float ADSR_filter_mod_amount = 0;
////////////////////////////////////////////////////////////////////////////




/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */
  //__HAL_RCC_TIM6_CLK_DISABLE(); //for debug only
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART6_UART_Init();
  MX_LTDC_Init();
  MX_DMA2D_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  // Initialize User Defined Modules
  MIDI_init();
  lfo_init();
  filter_init();
  osc_init();
  ADSR_init();
  mtof_init();
  UI_Engine_Init();
  LCD_init();
  update_LCD();


  // Initialize Audio Codec
  BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, 100, AUDIO_FREQUENCY_48K);
  BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);

  // // Calculate new audio sample values to fill the firrst half of the audio output buffer
  if (input_freq_osc1 != 0)  //osc for 1st voice
  {
    osc_gen1(wave_shape, input_freq_osc1, duty_cycle, osc1_out_buffer, lfo_out_buffer, lfo_pitch_mod_amount, ADSR_out_buffer, ADSR_pitch_mod_amount, offset);
  }
  if (input_freq_osc2 != 0)  //osc for 2st voice
  {
  	osc_gen2(wave_shape, input_freq_osc2, duty_cycle, osc2_out_buffer, lfo_out_buffer, lfo_pitch_mod_amount, ADSR_out_buffer2, ADSR_pitch_mod_amount, offset);
  }
  if (input_freq_osc3 != 0)  //osc for 3st voice
  {
  	osc_gen3(wave_shape, input_freq_osc3, duty_cycle, osc3_out_buffer, lfo_out_buffer, lfo_pitch_mod_amount, ADSR_out_buffer3, ADSR_pitch_mod_amount, offset);
  }
  if (input_freq_osc4 != 0)  //osc for 4st voice
  {
  	osc_gen4(wave_shape, input_freq_osc4, duty_cycle, osc4_out_buffer, lfo_out_buffer, lfo_pitch_mod_amount, ADSR_out_buffer4, ADSR_pitch_mod_amount, offset);
  }
  lfo_gen(lfo_wave_shape, input_freq_lfo, duty_cycle, lfo_out_buffer, offset);
  ADSR(attack, decay, sustain, release, ADSR_out_buffer, offset);  //envelope for 1st voice
  ADSR2(attack, decay, sustain, release, ADSR_out_buffer2, offset);  //envelope for 2st voice
  ADSR3(attack, decay, sustain, release, ADSR_out_buffer3, offset);  //envelope for 3st voice
  ADSR4(attack, decay, sustain, release, ADSR_out_buffer4, offset);  //envelope for 4st voice
  sum_osc(osc1_out_buffer, osc2_out_buffer, osc3_out_buffer, osc4_out_buffer, mix_out_buffer, ADSR_out_buffer, ADSR_out_buffer2,
  		  ADSR_out_buffer3, ADSR_out_buffer4, ADSR_amp_mod_amount, lfo_out_buffer, lfo_amp_mod_amount, offset);
  filter(mix_out_buffer, audio_out_sample_buffer, cutoff, q, lfo_out_buffer, lfo_filter_mod_amount, ADSR_out_buffer,
  		 ADSR_filter_mod_amount, dist_amount, filter_type, offset);

  // Start play back of audio output buffer in audio codec
  BSP_AUDIO_OUT_Play(audio_out_sample_buffer, SAMPLE_BUFFER_SIZE*2);
  __enable_irq();
  offset = SAMPLE_BUFFER_SIZE/2;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	while(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_3)==1)
	{
		UI_Engine_Handle();
	}

	read1_complete = 0;
	read2_complete = 0;

	// Calculate new audio sample values and fill the buffer
	if (input_freq_osc1 != 0)  //osc for 1st voice
    {
	    osc_gen1(wave_shape, input_freq_osc1, duty_cycle, osc1_out_buffer, lfo_out_buffer, lfo_pitch_mod_amount, ADSR_out_buffer, ADSR_pitch_mod_amount, offset);
    }
    if (input_freq_osc2 != 0) //osc for 2st voice
    {
	    osc_gen2(wave_shape, input_freq_osc2, duty_cycle, osc2_out_buffer, lfo_out_buffer, lfo_pitch_mod_amount, ADSR_out_buffer2, ADSR_pitch_mod_amount, offset);
    }
    if (input_freq_osc3 != 0)  //osc for 3st voice
    {
	    osc_gen3(wave_shape, input_freq_osc3, duty_cycle, osc3_out_buffer, lfo_out_buffer, lfo_pitch_mod_amount, ADSR_out_buffer3, ADSR_pitch_mod_amount, offset);
    }
    if (input_freq_osc4 != 0)  //osc for 4st voice
    {
	    osc_gen4(wave_shape, input_freq_osc4, duty_cycle, osc4_out_buffer, lfo_out_buffer, lfo_pitch_mod_amount, ADSR_out_buffer4, ADSR_pitch_mod_amount, offset);
    }

    lfo_gen(lfo_wave_shape, input_freq_lfo, duty_cycle, lfo_out_buffer, offset);
    ADSR(attack, decay, sustain, release, ADSR_out_buffer, offset);  //envelope for 1st voice
    ADSR2(attack, decay, sustain, release, ADSR_out_buffer2, offset);  //envelope for 2st voice
    ADSR3(attack, decay, sustain, release, ADSR_out_buffer3, offset);  //envelope for 3st voice
    ADSR4(attack, decay, sustain, release, ADSR_out_buffer4, offset);  //envelope for 4st voice

    sum_osc(osc1_out_buffer, osc2_out_buffer, osc3_out_buffer, osc4_out_buffer, mix_out_buffer, ADSR_out_buffer,
	  	    ADSR_out_buffer2, ADSR_out_buffer3, ADSR_out_buffer4, ADSR_amp_mod_amount, lfo_out_buffer, lfo_amp_mod_amount, offset);

    filter(mix_out_buffer, audio_out_sample_buffer, cutoff, q, lfo_out_buffer, lfo_filter_mod_amount, ADSR_out_buffer,
		   ADSR_filter_mod_amount, dist_amount, filter_type, offset);

    // After buffer half is filled wait for audio play back to complete
    pause = 1;
    while (pause == 1)
    {
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_SAI2;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
  PeriphClkInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

// Triggers when reading audio out buffer is half complete and fully complete
// Signals writing to switch to the half of the buffer that is not being read from
void DMA2_Stream4_IRQHandler(void)
{
	if (DMA2 -> HISR & DMA_HISR_HTIF4)
	{
		offset = 0;
		pause = 0;
		read1_complete = 1;
		read2_complete = 0;
		DMA2 -> HIFCR |= DMA_HIFCR_CHTIF4;
	}
	if (DMA2 -> HISR & DMA_HISR_TCIF4)
	{
		offset = SAMPLE_BUFFER_SIZE/2;
		pause = 0;
		read1_complete = 0;
		read2_complete = 1;
		DMA2 -> HIFCR |= DMA_HIFCR_CTCIF4;
	}
}

// Triggers when a MIDI Note On message is received
void MIDI_noteOn(uint8_t note_num, uint8_t velocity) {
	note_value = note_num & 0x7F;
	note_freq = mtof_lut[note_value];
	note_velocity = velocity & 0x7F;

	// Assigns the new note to an osc
	if (available_osc[0] == 1)
	{
		input_freq_osc1 = note_freq;
		active_notes[0] = note_value;
		trigger = 1;
		available_osc[0] = 0;
	}
	else if (available_osc[1] == 1)
	{
		input_freq_osc2 = note_freq;
		active_notes[1] = note_value;
		trigger2 = 1;
		available_osc[1] = 0;
	}
	else if (available_osc[2] == 1)
	{
		input_freq_osc3 = note_freq;
		active_notes[2] = note_value;
		trigger3 = 1;
		available_osc[2] = 0;
	}
	else if (available_osc[3] == 1)
	{
		input_freq_osc4 = note_freq;
		active_notes[3] = note_value;
		trigger4 = 1;
		available_osc[3] = 0;
	}
}

// Triggers when a MIDI Note Off message is received
void MIDI_noteOff(uint8_t note_num, uint8_t velocity) {
	note_value = note_num & 0x7F;
	note_velocity = velocity & 0x7F;

	// Finds which osc the note corresponds to and then sets the ADSR to release state
	if (active_notes[0] == note_value)
	{
		available_osc[0] = 1;
		active_notes[0] = 0;
		anti_trigger = 1;
	}
	else if (active_notes[1] == note_value)
	{
		available_osc[1] = 1;
		active_notes[1] = 0;
		anti_trigger2 = 1;
	}
	else if (active_notes[2] == note_value)
	{
		available_osc[2] = 1;
		active_notes[2] = 0;
		anti_trigger3 = 1;
	}
	else if (active_notes[3] == note_value)
	{
		available_osc[3] = 1;
		active_notes[3] = 0;
		anti_trigger4 = 1;
	}

}

// Triggers when a MIDI CC message is received
void MIDI_CC(uint8_t control_num, uint8_t value) {
	CC_num = control_num;
	CC_value = value;
}

// Triggers when a MIDI Pitch Bend message is received
void MIDI_pitchBend(uint8_t bend_lsb, uint8_t bend_msb) {
	pitchbend = (bend_lsb & 0x7F) | ((bend_msb & 0x7F) < 7);
}

// Triggers when a MIDI System Reset message is received
void MIDI_systemReset() {
	note_value = 0;
	note_velocity = 0;
	CC_num = 0;
	CC_value = 0;
	pitchbend = 0;
}

// Creates a look up table for MIDI note to frequency transforms
void mtof_init(void)
{
	for (int i = 0; i < 128; i++)
	{
		mtof_lut[i] = 440*pow(2, ((float)(i-69)/12.0));
	}
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
