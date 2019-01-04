
//#define USE_STDPERIPH_DRIVER
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#define ALL_LED_ON() GPIO_SetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15)
#define ALL_LED_OFF() GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15)  
#define RED_LED_ON() GPIO_SetBits(GPIOD, GPIO_Pin_14)
#define RED_LED_OFF() GPIO_ResetBits(GPIOD, GPIO_Pin_14)
#define BLUE_LED_ON() GPIO_SetBits(GPIOD, GPIO_Pin_15)                                                                                                                                                   
#define BLUE_LED_OFF() GPIO_ResetBits(GPIOD, GPIO_Pin_15)
#define YELLOW_LED_ON() GPIO_SetBits(GPIOD, GPIO_Pin_13)                                                                                                                                                   
#define YELLOW_LED_OFF() GPIO_ResetBits(GPIOD, GPIO_Pin_13)
#define GREEN_LED_ON() GPIO_SetBits(GPIOD, GPIO_Pin_12)                                                                                                                                                   
#define GREEN_LED_OFF() GPIO_ResetBits(GPIOD, GPIO_Pin_12)
#define delay 200                                                                                                                        

//#define BUTTON_READ()   GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)

//#define DOORSTATE()   GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)

#include "stm32f4xx_adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"

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
/* Handle PD0 interrupt */
void EXTI0_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
            /* Do your stuff when PD0 is changed */
            
        RED_LED_ON();
            
            /* Clear interrupt flag */
            EXTI_ClearITPendingBit(EXTI_Line0);
        }
}

/* Handle PB12 interrupt */
void EXTI15_10_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line12) != RESET) {
            /* Do your stuff when PB12 is changed */
            
        RED_LED_OFF();
            
            /* Clear interrupt flag */
            EXTI_ClearITPendingBit(EXTI_Line12);
        }
}




int ConvertedValue = 0; //Converted value readed from ADC


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



//void DoorState(void);
//void ButtonRead(void);
uint16_t delay_count=0;

//#include "core_cmInstr.h"
void SysTick_Handler(void)//1ms
{
    if (delay_count >0)
    {
        delay_count--;
    }
}
//----------------------------------------------------------
//
//-----------------------------------------------------------
void delay_ms(uint16_t delay_temp)
{
    delay_count = delay_temp;

    while (delay_count) {
        
    }

}



//
//--------------------------------------------------------------------
void LEDs_init(void)
{
    GPIO_InitTypeDef GPIO_LED_init;                                                                                                                                                                         
  
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);                                                                                                                                                   
    SysTick_Config(SystemCoreClock/1000); //1ms
    GPIO_LED_init.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_LED_init.GPIO_Mode = GPIO_Mode_OUT;                                                                                                                                                                
    GPIO_LED_init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_LED_init.GPIO_OType = GPIO_OType_PP;
    GPIO_LED_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_LED_init);
}





int main(void) {



 adc_configure();   //Start configuration

    LEDs_init();

//    SysTick_Config(SystemCoreClock/1000); //1ms
  /* System init */
     SystemInit();
            /* Configure PD0 as interrupt */
     Configure_PA0();
                /* Configure PB12 as interrupt */
     Configure_PB12();


            // GPIOD initialization as an output for orange LED (GPIOD13)
    // and blue LED (GPIOD15)






while(1)
{

    //    GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
      //  delay_ms(500);

//        GREEN_LED_ON();

        delay_ms(500);

  //      GREEN_LED_OFF();

    ConvertedValue = adc_convert();//Read the ADC converted value
//    DoorState();
//    ButtonRead();
/*   
-if doors free and doors close and  adc = day ==>> doors open
--else if free and adc = night and doors open  === >> doors close 
if temp = cold ==>> warmon
else ===>> warm off

if adc = night and flag =0 lampOn 3h and flag 1
if adc = day ==>  flag 0



*/
//    if (DOORSTATE() == 1) 
//    {
//    RED_LED_ON();
//    }
//    else if   (DOORSTATE() == 0) 
//    {
//
//    ALL_LED_ON();
//    delay_ms(500); 
//   // YELLOW_LED_OFF();
//    //for (i=0;i<2000000;i++){}//delay_ms(delay);
//    ALL_LED_OFF();
//    delay_ms(500);
//
//
//    }
}

}
//void ButtonRead(void)
//{
//
//    GPIO_InitTypeDef GPIO_But_init;                                                                                                                                                                         
//                                                                                                                                                                        
//  
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);                                                                                                                                                   
//  //  SysTick_Config(SystemCoreClock/1000); //1ms
//    GPIO_But_init.GPIO_Pin = GPIO_Pin_0;
//    GPIO_But_init.GPIO_Mode = GPIO_Mode_IN;                                                                                                                                                                
//    GPIO_But_init.GPIO_Speed = GPIO_Speed_2MHz;
//    GPIO_But_init.GPIO_OType = GPIO_OType_PP;
//    GPIO_But_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOA, &GPIO_But_init);
//}
//

//void DoorState(void)
//{
//
//    GPIO_InitTypeDef DoorState_init;                                                                                                                                                                         
//                                                                                                                                                                        
//  
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);                                                                                                                                                   
//  //  SysTick_Config(SystemCoreClock/1000); //1ms
//    DoorState_init.GPIO_Pin = GPIO_Pin_0;
//    DoorState_init.GPIO_Mode = GPIO_Mode_IN;                                                                                                                                                                
//    DoorState_init.GPIO_Speed = GPIO_Speed_2MHz;
//    DoorState_init.GPIO_OType = GPIO_OType_PP;
//    DoorState_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOA, &DoorState_init);
//}

