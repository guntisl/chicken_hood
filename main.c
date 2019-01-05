//#define USE_STDPERIPH_DRIVER
//#include "stm32f4xx.h"
//#include "stm32f4xx_conf.h"
//#include "stm32f4xx_adc.h"
//#include "stm32f4xx_rcc.h"
//#include "stm32f4xx_exti.h"
//#include "stm32f4xx_syscfg.h"
//#include "misc.h"
#include "door.h"



/*Configure pins to be interrupts */
void Configure_PA0(void) {
    /* Set variables used */
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    /* Enable clock for GPIOA */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    /* Set pin as input */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* Tell system that you will use PA0 for EXTI_Line0 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
    
    /* PA0 is connected to EXTI_Line0 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);

    /* Add IRQ vector to NVIC */
    /* PD0 is connected to EXTI_Line0, which has EXTI0_IRQn vector */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    /* Set priority */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);
}


void Configure_PB12(void) {
    /* Set variables used */
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    /* Enable clock for GPIOB */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    /* Set pin as input */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* Tell system that you will use PB12 for EXTI_Line12 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);
    
    /* PB12 is connected to EXTI_Line12 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line12;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);

    /* Add IRQ vector to NVIC */
    /* PB12 is connected to EXTI_Line12, which has EXTI15_10_IRQn vector */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
    /* Set priority */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);
}

        


/* Set interrupt handlers */
/* Handle PA0 interrupt */
void EXTI0_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
            /* Do your stuff when PA0 is changed */
            
Doors_Close(FrontDoor);
            
            /* Clear interrupt flag */
            EXTI_ClearITPendingBit(EXTI_Line0);
        }
}

/* Handle PB12 interrupt */
void EXTI15_10_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line12) != RESET) {
            /* Do your stuff when PB12 is changed */
            
Doors_Close(OutsideDoor);
            
            /* Clear interrupt flag */
            EXTI_ClearITPendingBit(EXTI_Line12);
        }
}




int ConvertedValue = 0; //Converted value readed from ADC
int ConvertedValueOffset = 0;

int day_time = 1;
int night_time = 1;
int time_mode = 1;
int Value_Offset = 0;
void adc_configure(){
 ADC_InitTypeDef ADC_init_structure; //Structure for adc confguration
 GPIO_InitTypeDef GPIO_initStructre; //Structure for analog input pin
 //Clock configuration
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//The ADC1 is connected the APB2 peripheral bus thus we will use its clock source
  RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN,ENABLE);//Clock for the ADC port!! Do not forget about this one ;)
  //Analog pin configuration
  GPIO_initStructre.GPIO_Pin = GPIO_Pin_0;//The channel 10 is connected to PC0
  GPIO_initStructre.GPIO_Mode = GPIO_Mode_AN; //The PC0 pin is configured in analog mode
  GPIO_initStructre.GPIO_PuPd = GPIO_PuPd_NOPULL; //We don't need any pull up or pull down
 GPIO_Init(GPIOC,&GPIO_initStructre);//Affecting the port with the initialization structure configuration
 //ADC structure configuration
  ADC_DeInit();
  ADC_init_structure.ADC_DataAlign = ADC_DataAlign_Right;//data converted will be shifted to right
  ADC_init_structure.ADC_Resolution = ADC_Resolution_12b;//Input voltage is converted into a 12bit number giving a maximum value of 4096
  ADC_init_structure.ADC_ContinuousConvMode = ENABLE; //the conversion is continuous, the input data is converted more than once
   ADC_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;// conversion is synchronous with TIM1 and CC1 (actually I'm not sure about this one :/)
   ADC_init_structure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//no trigger for conversion
   ADC_init_structure.ADC_NbrOfConversion = 1;//I think this one is clear :p
ADC_init_structure.ADC_ScanConvMode = DISABLE;//The scan is configured in one channel
   ADC_Init(ADC1,&ADC_init_structure);//Initialize ADC with the previous configuration
   //Enable ADC conversion
ADC_Cmd(ADC1,ENABLE);
   //Select the channel to be read from
  ADC_RegularChannelConfig(ADC1,ADC_Channel_10,1,ADC_SampleTime_144Cycles);
   }
 int adc_convert(){
  ADC_SoftwareStartConv(ADC1);//Start the conversion
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)){};//Processing the conversion
   return ADC_GetConversionValue(ADC1); //Return the converted data
   }
//-------------------------------------------------------------------------------------



//----------------------------------------------------------
//
//-----------------------------------------------------------




int main(void) {

int *day_time_pointer;
day_time_pointer = &day_time;

int *night_time_pointer;
night_time_pointer = &night_time;

int *time_mode_pointer;
time_mode_pointer = &time_mode;

int *Value_Offset_pointer;
Value_Offset_pointer = &Value_Offset;
 adc_configure();   //Start configuration

   // LEDs_init();
    Doors_Init(FrontDoor);
    Doors_Init(OutsideDoor);
  //  SysTick_Config(SystemCoreClock/1000); //1ms
  /* System init */
     SystemInit();
            /* Configure PA0 as interrupt */
     Configure_PA0();
                /* Configure PB12 as interrupt */
     Configure_PB12();



while(1)
    {
    ConvertedValue = adc_convert();//Read the ADC converted value
    ConvertedValueOffset = ConvertedValue + *Value_Offset_pointer;
    
    if (ConvertedValueOffset > 2900)
        {

            *time_mode_pointer = 1;
        if (((*time_mode_pointer) == 1) && ((*day_time_pointer) == 1) ) 
        {
            Doors_Open(FrontDoor);
            *day_time_pointer = 0;
            *night_time_pointer = 1;
            *Value_Offset_pointer = 100;
        }
        }
    else if (ConvertedValueOffset < 2900)
        {
        
            *time_mode_pointer = 0;
        if ((*time_mode_pointer == 0) && (*night_time_pointer == 1) ) 
        {
            Doors_Open(OutsideDoor);
            *time_mode_pointer = 1;
            *day_time_pointer = 1;
            *night_time_pointer = 0;
            *Value_Offset_pointer = -100;
        }
        }
        }
    }



