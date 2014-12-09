#include "stm32F10x.h"
#include "LEDS.h"
#include "TIM.h"
#include "ADC.h"


int ADC_count = 0;
extern volatile uint8_t new_vol;
extern volatile uint16_t max_vol;

void NVIC_Configuration(void);

void TIM2_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	//Enable peripheral clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	//Init timer
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 80;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	NVIC_Configuration();
	
	//Clear update flag
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	
	//Enable interrupt on update
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	//Start timer
	TIM_Cmd(TIM2,ENABLE);
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		max_vol = getVol();
		
		new_vol = 1;
		
		/*ADC_results[ADC_count++] = ADC_perform_single_conversion();
		
		if (ADC_count > ADC_BUFFER_SIZE)
		{
			ADC_count = 0;
		}*/
	}
}

void TIM3_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	//Enable peripheral clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	//Init timer
  TIM_TimeBaseStructure.TIM_Period = 1; 
  TIM_TimeBaseStructure.TIM_Prescaler = 545;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	NVIC_Configuration();
	
	//Clear update flag
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	
	// Enter master mode with update event as trigger output (directly set register value)
	TIM3->CR2 = (1<<5);
	
	//Start timer
	TIM_Cmd(TIM3,ENABLE);
}

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}
