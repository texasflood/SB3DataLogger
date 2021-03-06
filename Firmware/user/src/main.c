

#include "stm32F10x.h"
#include "LEDs.h"
#include "UART.h"
#include "ADC.h"
#include "TIM.h"
/*#include "stm32F10x_conf.h"
#include "stm32F10x_it.h"
#include "core_cm3.h"
#include "stdint.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_adc.h"*/
//Hardware:

//LED3 is connected to PC9 (active high)
//Test circuit should have potentiometer connected to PC0 for analogue sampling
//UART TX and RX pins are PA9 and PA10 respectively (with MAX232 interface)

//Firmware:

//User files that can be modified/replaced
//LEDs.c and LEDs.h - functions to control LED on dev board
//ADC.c and ADC.h - functions to initalise ADC and take samples from it
//UART.c and UART.h - functions to initialise and use UART for serial RS232 comms with PC

//Standard includes that should not be modified
//startup_stm32f10x_md_vl.s - startup file (in assembler) that intialises the MCU before it enters main()
//core_cm3.c - definitions specific to the ARM Cortex M3 core
//system_stm32f10.c and stm32f10x.h - functions and definitions specific to the STM32F10x series

//There are also a number of Standard Peripheral Library files that should not be modified (additional SPL files can be included if needed, see "Firmware/lib" folder)

//Software:
//This firmware works with the Labwindows/CVI demonstration appllication sofwtare

//------------------------------------------------------------------------------

//Global variables
//DAC_InitTypeDef            DAC_InitStructure;
//TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);
//Command from UART
volatile int command_flag;
volatile int value;
volatile int value_received;
volatile int volume = 1;
volatile int echoGain = 0;

//ADC variables
volatile uint8_t new_data;
//Max vol variables
volatile uint8_t new_vol = 0;
volatile uint16_t max_vol = 0;
//------------------------------------------------------------------------------

//Function prototypes for functions in main.c file

void check_and_process_received_command(void);
void LED_flash(void);
void DAC_config(void);
void send_new_vol(void);
int signOfANeg = 1;
uint64_t a = 0;
uint64_t m = 1023;
uint64_t d = 1;
int DC_off;
//------------------------------------------------------------------------------

//Main function (execution starts here after startup file)
int main(void)
{
	int i;
	//Initalise LEDs
	LED_init();
	//Short delay during which we can communicate with MCU via debugger even if later user code causes error such as sleep state with no wakeup event that prevents debugger interface working
	//THIS MUST COME BEFORE ALL USER CODE TO ENSURE CHIPS CAN BE REPROGRAMMED EVEN IF THEY GET STUCK IN A SLEEP STATE LATER
	for (i = 0; i < 4000000; i++)
	{
		LED_on();
	}
	delaySwitch(0);
	distortInit(signOfANeg, a, m, d);
	fillLookup(signOfANeg, a, m, d);
	TIM2_init();
	TIM3_init();
	DAC_config();
	ADC_init();
	UART_init();
	//Main loop
	//------------------------------------------------------------------------------
	while (1)
	{
		check_and_process_received_command();
		
		if (new_vol ==1)
		{
			send_new_vol();
			new_vol = 0;
			max_vol = 0;
		}
	}
	//------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------

//Other functions

//Simple function to check for abd process one of two received commands from PC via global varoable flags
void check_and_process_received_command(void)
{
	uint16_t ADC_word;
	uint8_t byte1, byte2;
	
	//Check for a receievd (non-zero) command flag from the PC
	if (command_flag == 1)
	{
		//An ADC sample command has been received
		
		//Perform an ADC conversion
		ADC_word = ADC_perform_single_conversion();
		
		//Put the 12-bit result into 2 bytes
		byte1 = (uint8_t)((ADC_word >>8) & 0xFF);
		byte2 = (uint8_t)((ADC_word >>0) & 0xFF);
		
		//Send the message (command flag followed by two bytes with the analogue value)
		UART_send_byte(1);
		UART_send_byte(byte1);
		UART_send_byte(byte2);
		
		//Reset command flag
		command_flag = 0;
	}
	else if (command_flag == 2)
	{
		//A distortion command has been received
		
		//Check whether we have also received the value attached to the command (ie on or off)
		if (value_received == 1)
		{
			//Reset command flag
			command_flag = 0;
			
			//Set distortion based on value (on or off)
			distortionSwitch(value);
		}
	}
	else if (command_flag == 3)
	{
		if (value_received == 1)
		{
			command_flag = 0;
			TIM_DeInit(TIM3);
			DAC_SetChannel1Data(DAC_Align_12b_R, 2048);
			d = (uint64_t) value;
			if (d >= 2)
			{
				a = 30;
			}
			else
			{
				a = 0;
			}
			distortInit(signOfANeg, a, m, d);
			fillLookup(signOfANeg, a, m, d);
			TIM3_init();
		}
		
	}
	else if (command_flag == 4)
	{
		if (value_received == 1)
		{
			command_flag = 0;
			volume = value;
		}
	}
	// NEW Delay initialise
	else if (command_flag == 5)
	{
		if (value_received == 1)
		{
			command_flag = 0;
			// Deinitialise so that output buffer is refreshed
			
			TIM_DeInit(TIM3);
			DAC_SetChannel1Data(DAC_Align_12b_R, 2048);
			delaySwitch(value);
			TIM3_init();
		}
	}
	// NEW Delay set value
	else if (command_flag == 6)
	{
		if (value_received == 1)
		{
			command_flag = 0;
			TIM_DeInit(TIM3);
			DAC_SetChannel1Data(DAC_Align_12b_R, 2048);
			echoGain = value;
			delayInit();
			TIM3_init();
		}
	}
}

void DAC_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	DAC_InitTypeDef	DAC_InitStructure;
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
		
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	// DAC channel1 Configuration
	DAC_StructInit(&DAC_InitStructure);
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
		
	// Enable DAC
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

void send_new_vol(void)
{
	uint8_t byte1;
	
	byte1 = (uint8_t) ((max_vol >> 2) & 0xFF);
	
	UART_send_byte(1);
	UART_send_byte(byte1);
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif
