
#include "stm32F10x.h"
#include "LEDs.h"
#include "stm32f10x_gpio.h"

int LED;

//Function to initialise output pin controlling LED on dev board
void LED_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;	//Structure that holds configuration values for GPIO (General Purpose In/Out) pins
	
	//start GPIOC clock to enable GPIO Port C
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//Configure Port C Pin 9 (PC9) to drive LED (ie configure as output)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	//Choose pin
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//Choose mode (output, push-pull)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//Choose max speed (slow is fine here)
  GPIO_Init(GPIOC, &GPIO_InitStructure);	//Call the GPIO init function from the std periph lib to initialise GPIOC with these parameters
}

//Function to turn LED off
void LED_off()
{
	//Send PC9 low (using std periph lib function)
	GPIO_ResetBits(GPIOC, GPIO_Pin_9);
	//Set flag to remember current state of LED
	LED = 0;
}

void LED_on()
{
	//Send PC9 high (using std periph lib function)
	GPIO_SetBits(GPIOC, GPIO_Pin_9);
	//Set flag to remember current state of LED
	LED = 1;
}

void LED_toggle()
{
	if (LED == 1)
	{
		LED_off();
	}
	else if (LED == 0)
	{
		LED_on();
	}
}

