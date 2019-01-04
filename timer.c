/**
  ******************************************************************************
  * @file    BoardTestProject/timer.c 
  * @author  Ibrahim Ozturk -- www.ozturkibrahim.com
  */ 
#include "includes.h"

__IO	uint16_t CCR1_Val = 13600;		/* 136 --> 5us */

int counter;
void TIMER3_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//uint16_t PrescalerValue = 0;

	/* TIM3 Clock Enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	/* Enable TIM3 Global Interrupt */	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
	
	/* Time Base Configuration */
	/*	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 28000000) -1;*/
	
	//TIM_TimeBaseStructure.TIM_Period = 1360;
	TIM_TimeBaseStructure.TIM_Period =  100 - 1; // 1 MHz down to 10 KHz (0.1 ms)
	//TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_Prescaler =  84 - 1; // Down to 1 MHz (adjust per your clock)
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* Prescaler Configuration */
	//TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);
	
	/* Timer 3 Mode configuration: Channel 1 */
	//TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Active;
	//TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	//TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	//TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	//TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);	
	//TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	/* TIM Interrupts ENABLE*/
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	/*Enable TIM3 Counter */
	TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void)
{
	counter++;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update | TIM_IT_CC1); /* BOTH BITS! */			
		/* procedure for interrupt at phase change*/
	}
	else
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
		/* procedure for interrupt delivering data output */
	}
}
