

#include "stm32F10x.h"
#include "LEDs.h"
#include "UART.h"
#include "ADC.h"

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

//Command from UART
volatile int command_flag;
volatile int value;
volatile int value_received;

//ADC variables
volatile uint8_t new_data;

//------------------------------------------------------------------------------

//Function prototypes for functions in main.c file

void check_and_process_received_command(void);
void LED_flash(void);

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
	
	LED_off();
	
	//Initialise UART for serial comms with PC
	//UART_init();
	
	//Initialise ADC
	//ADC_init();
	
	//Main loop
	//------------------------------------------------------------------------------
	while (1)
	{
		//check_and_process_received_command();
		LED_flash();
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
		//An LED command has been received
		
		//Check whether we have also received the value attached to the command (ie on or off)
		if (value_received == 1)
		{
			//Reset command flag
			command_flag = 0;
			
			//Set LED based on value (on or off)
			if (value == 0)
			{
				LED_off();
			}
			else
			{
				LED_on();
			}
		}
	}
}

void LED_flash(void)
{
	int i;
	LED_on();
	for(i = 0; i < 1000000; i++);
	LED_off();
	for(i = 0; i < 1000000; i++);
}
