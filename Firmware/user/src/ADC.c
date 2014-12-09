
#include "stm32F10x.h"
#include "ADC.h"
#include "UART.h"
#include "LEDs.h"
#include "TIM.h"


#define ADC1_DR_Address    ((uint32_t)0x4001244C)

extern __IO uint32_t ADC_DualConvertedValueTab[16];
extern volatile uint8_t new_data;

// ADC Buffer Definitions
#define ADC_BUFFER_SIZE_2 100

uint16_t ADC_results_2[ADC_BUFFER_SIZE_2];
int ADC_count_2 = 0;
int volCounter = 0;
volatile uint16_t ADCVal = 0;

//Private function prototypes
void ADC_Config(void);
void ADC_RCC_Config(void);
void ADC_GPIO_Config(void);
void ADC_NVIC_Config(void);
void clearVol(void);
uint16_t getVol(void);

extern volatile uint16_t max_vol;
static uint16_t lookup[1024];
uint64_t A;
uint64_t B1;
uint64_t B2;
uint64_t C;
uint64_t D;
uint64_t xL;
int distortionOn = 0;
//uint64_t E = 384858382;
//uint64_t F = 3489025723049;

void distortInit(int signOfANeg, uint64_t a, uint64_t m, uint64_t d);
void fillLookup(int signOfANeg, uint64_t a, uint64_t m, uint64_t d);
void distortionSwitch(int distortionSet);
// NEW Shelving constants
int shelvingHighOn = 0;
static int a_high[3];
static int b_high[3];
static int y[2];
static int x[2];
int array_position;
int output;
//------------------------------------------------------------------------

//Initialise ADC for independent sampling on demand
void ADC_init(void)
{
	//Enable required clocks
	ADC_RCC_Config();
	
	//Configure required pin as analogue input
	ADC_GPIO_Config();
	
	//Configure the ADC itself
	ADC_Config();
	
	//Configure NVIC and ADC interrupt to trigger on End of Conversion (EOC)
	ADC_NVIC_Config();
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
}

uint16_t ADC_perform_single_conversion(void)
{
	uint16_t ADC_word;
	
	//Clear end of conversion flag
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	
	//Start conversion
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	//Wait for conversion to complete
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	
	//Get value
	ADC_word = ADC_GetConversionValue(ADC1);
	
	return ADC_word;
}

//------------------------------------------------------------------------

void ADC_Config(void)
{
	int i;
	ADC_InitTypeDef ADC_InitStructure;	//Structure to hold ADC config parameters
	
	//Set ADC config parameters
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//Independent mode - ADC1 and ADC2 operate independently
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//Only perform a single conversion (don't scan a range of input pins)
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//Sample only on external signal, not continuously
  //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//Don't enable any automatic external sampling trigger
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;	//**NEW - enable external trigger from timer 3 update**
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//Align 12-bit samples to the right of their destination word in memory
  ADC_InitStructure.ADC_NbrOfChannel = 1;	//One conversion channel (scan mode would use multiple channels)
  ADC_Init(ADC1, &ADC_InitStructure);
  
	//**NEW - ENABLE EXTERNAL TRIGGERING*
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	
	//ADC1 regular channels configuration (just one channel)
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_1Cycles5);    
	
	//power up ADC
	ADC_Cmd(ADC1, ENABLE);
	
	//wait until ADC is ready
	for (i=0; i<10000; i++);
}

void ADC_RCC_Config(void)
{
	//Set ADC clock divider
	RCC_ADCCLKConfig(RCC_PCLK2_Div2);
	
	//Enable DMA periph clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//Enable ADC and GPIOC periph clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
}

void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Set PC0 as analogue input
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void ADC1_IRQHandler(void)
{
	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	
	//ADC_results_2[ADC_count_2++] = ADC_GetConversionValue(ADC1);
	ADCVal = ADC_GetConversionValue(ADC1);
	if ((ADCVal >> 2) > max_vol)
	{
		max_vol = (ADCVal >> 2);
	}
	if (distortionOn == 1)
	{
		ADCVal = (ADCVal >> 2);
		ADCVal = lookup[ADCVal];
		ADCVal = ADCVal << 2;
	}
	/*E = 56786*F -89/E + A;
	F = 3532/F + 2354*E;
	E = 56786*F -89/E + A;
	F = 332/F + 2354*E;
	E = 5786*F -892/E + A;
	F = 353/F + 2354*E;
	E = 5676*F -249/E - A;
	F = 3532/F + 23534*E;
	E = 56786*F -89/E + A*8573;
	F = 3532/F + 2354*E;
	F = 353/F + 2354*E;
	E = 5676*F -249/E - A;*/
	
	if (shelvingHighOn == 1)
	{
		if(array_position == 0)
		{
			// If x[1],y[1] hold the previous(delay 1) value
			output = b_high[0]*ADCVal + b_high[1]*x[1] + b_high[2]*x[0] - a_high[1]*y[1] - a_high[2]*y[0];
			y[0] = output;
			x[0] = ADCVal;
			array_position = 1;
		}
		else
		{
			// If x[0],y[0] hold the previous(delay 1) value
			output = b_high[0]*ADCVal + b_high[1]*x[0] + b_high[2]*x[1] - a_high[1]*y[0] - a_high[2]*y[1];
			y[1] = output;
			x[1] = ADCVal;
			array_position = 0;
		}

		// Scale the output
		output = (output/3270) ;
	}
	if (output > 4095)
	{
		output = 4095;
	}
	if (output < 0)
	{
		output = 0;
	}
	
	DAC_SetChannel1Data(DAC_Align_12b_R, output);
	
	/*if (ADC_count_2 > ADC_BUFFER_SIZE_2)
	{
		ADC_count_2 = 0;
	}*/
	
	//LED_toggle();
}

void ADC_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

void distortInit(int signOfANeg, uint64_t a, uint64_t m, uint64_t d)
{
	B1 = 6*d*m*m*(-1+d*d);
	if (signOfANeg == 1)
	{
		A = (m*m*m)*(d*d*d-3*d+2) - 6*a*d*(m*m)*(-1+d*d) + 4*(a*a)*(d*d*d)*(-2*a+3*m);
		B2 = 24*a*d*d*d*(m - a);
		C = 12*d*d*d*(m - 2*a);
	}
	else
	{
		A = (m*m*m)*(d*d*d-3*d+2) + 6*a*d*(m*m)*(-1+d*d) + 4*(a*a)*(d*d*d)*(2*a+3*m);
		B2 = 24*a*d*d*d*(m + a);
		C = 12*d*d*d*(m + 2*a);
	}

	D = 8*d*d*d;
}

void fillLookup(int signOfANeg, uint64_t a, uint64_t m, uint64_t d)
{
	uint64_t y;
	uint64_t squared;
	uint64_t xL;
	
	for(xL = 0; xL < m + 1; xL++)
	{
		squared = xL*xL;
		if (signOfANeg == 0)
   {
       y = (A - B1*xL - B2*xL + C*squared - D*xL*squared);
       y = y/(4*m*m);
       //printf("y = %llu, x = %llu\n",y,xL);
       if(xL >= a + m/2 + m/(2*d))
       {
           lookup[xL] = (uint16_t)m;
       }
       else if(xL <= a + m/2 - m/(2*d))
       {
           lookup[xL] = 0;
       }
			 else
			 {
					if (y > m) {y = m;}
					lookup[xL] = (uint16_t)y;
			 }
   }
   else
   {
       y = (A - B1*xL + B2*xL + C*squared - D*xL*squared);
       y = y/(4*m*m);
       //printf("y = %llu, x = %llu\n",y,xL);
       if(xL >= -a + m/2 + m/(2*d))
       {
           lookup[xL] = (uint16_t)m;
       }
       else if(xL <= -a + m/2 - m/(2*d))
       {
           lookup[xL] = 0;
       }
			 else
			 {
					if (y > m) {y = m;}
					lookup[xL] = (uint16_t)y;
			 }
   }
 }
}

void distortionSwitch(int distortionSet)
{
	distortionOn = distortionSet;
}

uint16_t getVol(void)
{
	return max_vol;
}

void clearVol(void)
{
	max_vol = 0;
}
// NEW
void shelvingHighSwitch(int shelvingSet)
{
shelvingHighOn = shelvingSet;
}

// NEW
void shelvingHighInitialise(int coefficient1, int coefficient2, int coefficient3, int coefficient4, int coefficient5)
{
int i;
b_high[0] = coefficient1;
b_high[1] = coefficient2;
b_high[2] = coefficient3;
a_high[1] = coefficient4;
a_high[2] = coefficient5;

for(i=0; i<2; i++)
{
x[i] = 0;
y[i] = 0;
}

array_position = 0;
}
