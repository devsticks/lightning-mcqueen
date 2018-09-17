//********************************************************************
//*                    EEE3099S Robot Project Ver1.0                 *
//*==================================================================*
//* WRITTEN BY:    	                 		             *
//* DATE CREATED:                                                    *
//* MODIFIED:                                                        *
//*==================================================================*
//* PROGRAMMED IN: Eclipse Luna Service Release 1 (4.4.1)            *
//* DEV. BOARD:    UCT STM32 Development Board                       *
//* TARGET:	   STMicroelectronics STM32F051C6                    *
//*==================================================================*
//* DESCRIPTION:                                                     *
//*                                                                  *
//********************************************************************
// INCLUDE FILES
//====================================================================
#include "lcd_stm32f0.h"
#include "stm32f0xx.h"
//====================================================================
// SYMBOLIC CONSTANTS
//====================================================================
#define DELAY1 255
#define DELAY2 2000

#define SW1 GPIO_IDR_1

#define FAR_LEFT GPIO_IDR_6
#define LEFT GPIO_IDR_5
#define NEAR_LEFT GPIO_IDR_4

#define NEAR_RIGHT GPIO_IDR_3
#define RIGHT GPIO_IDR_15
#define FAR_RIGHT GPIO_IDR_14

#define DISTANCE1 GPIO_IDR_11
#define DISTANCE2 GPIO_IDR_10

//====================================================================
// GLOBAL VARIABLES
//====================================================================
int farLeft = 0;
//====================================================================
// FUNCTION DECLARATIONS
//====================================================================
void init_PWM(void);
void init_sensors(void);
void Delay(void);
void detection(void);
//====================================================================
// MAIN FUNCTION
//====================================================================
void main (void)
{
	init_LCD();									//Initialise lcd
	lcd_putstring("Robot Ver1.0");		//Display string on line 1
	lcd_command(LINE_TWO);					//Move cursor to line 2
	lcd_putstring("Group 21");				//Display string on line 2

	//calling initialisation functions
	init_sensors();
	init_PWM();

	//waits until button pressed to start code
	while ((GPIOA->IDR && SW1) != 0);

	//infinite loop
	while (1) {

		detection(); //calls line detection function

		if (farLeft == 1) {
			TIM2->CCR3 = 100*80;
			TIM2->CCR4 = 50*80;
		}//end if

		else {
			TIM2->CCR3 = 50*80;
			TIM2->CCR4 = 100*80;
		}//end else
		Delay();

	}//end forever
}//end main

//====================================================================
// FUNCTION DEFINITIONS
//====================================================================
void init_PWM(){

	//enable clocks
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	//Map TIM2_CH3 to pin10, TIM2_CH4 to pin11
	GPIOB->AFR[1] |= (2 << (4*(10 - 8))); // PB10_AF = AF2 (ie: map to TIM2_CH3) map pin 11 to tim1
	GPIOB->AFR[1] |= (2 << (4*(11 - 8))); // PB11_AF = AF2 (ie: map to TIM2_CH4)

	//define auto reload register
	TIM2->ARR = 8000;

	//specify PWM mode: We want mode 1
	TIM2->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1); // PWM Mode 1
	TIM2->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1); // PWM Mode 1

	//enable the OC channels
	TIM2->CCER |= TIM_CCER_CC3E; //channel 3
	TIM2->CCER |= TIM_CCER_CC4E; //channel 4

	//enable counter for timer 2
	TIM2->CR1 |= TIM_CR1_CEN;
}//end init_PWM

void init_sensors(){

	//enable clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	GPIOA->MODER &= ~GPIO_MODER_MODER4; //PA4 for IR LED 1 Input simulation
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR4_0;

	//define GPIOs to be Alternative Function for PWM
	GPIOB->MODER |= GPIO_MODER_MODER10_1;
	GPIOB->MODER |= GPIO_MODER_MODER11_1;

	//initiate button 1 and make it pull down
	GPIOA->MODER &= ~GPIO_MODER_MODER1;
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_0;

	//line sensors (GPIO inputs)
	GPIOA->MODER &= ~GPIO_MODER_MODER14_0;	//PA14
	GPIOA->MODER &= ~GPIO_MODER_MODER15_0;  //PA15
	GPIOB->MODER &= ~GPIO_MODER_MODER3_0;	//PB3
	GPIOB->MODER &= ~GPIO_MODER_MODER4_0;	//PB4
	GPIOB->MODER &= ~GPIO_MODER_MODER5_0;	//PB5
	GPIOB->MODER &= ~GPIO_MODER_MODER6_0;	//PB6

	//distance sensors (GPIO inputs)
	GPIOA->MODER &= ~GPIO_MODER_MODER11_0;	//PA11
	GPIOA->MODER &= ~GPIO_MODER_MODER10_0;  //PA10

}//end init_sensors

void Delay() {

	int i, j;

	for (i = 0; i < DELAY1; i++)
		for (j = 0; j < DELAY2; j++);
}//end Delay

void detection() {
	if ((GPIOA -> IDR & GPIO_IDR_4) == 0) {
		farLeft = 1;
	}//end if

	else
		farLeft = 0;
}
//********************************************************************
// END OF PROGRAM
//********************************************************************
