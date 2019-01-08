#include "door.h"
#include "interupt.h"
//#include "adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "stm32f4_discovery.h"
/* Set interrupt handlers */
/* Handle PA0 interrupt */
void EXTI0_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
            /* Do your stuff when PA0 is changed */
            
        Doors_Close(OutsideDoor);
            
            /* Clear interrupt flag */
            EXTI_ClearITPendingBit(EXTI_Line0);
        }
}

/* Handle PB12 interrupt */
void EXTI15_10_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line12) != RESET) {
            /* Do your stuff when PB12 is changed */
            
        Doors_Close(FrontDoor);
            
            /* Clear interrupt flag */
            EXTI_ClearITPendingBit(EXTI_Line12);
        }
}




uint32_t volatile ConvertedValue = 0; //Converted value readed from ADC
uint32_t volatile ConvertedValueOffset = 0;



uint32_t volatile day_time = 1;
uint32_t volatile night_time = 1;
uint32_t volatile time_mode = 1;
uint32_t volatile Value_Offset = 0;
//-------------------------------------------------------------------------------------



//----------------------------------------------------------
//
//-----------------------------------------------------------
uint16_t ADC3ConvertedValue[5] = {0,0,0,0,0};


int main(void) {


ADC_InitTypeDef       ADC_InitStruct;
ADC_CommonInitTypeDef ADC_CommonInitStruct;
DMA_InitTypeDef       DMA_InitStruct;
GPIO_InitTypeDef      GPIO_InitStruct;
/* Enable ADC3, DMA2 and GPIO clocks ****************************************/
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);//ADC3 is connected to the APB2 peripheral bus
/* DMA2 Stream0 channel0 configuration **************************************/
DMA_InitStruct.DMA_Channel = DMA_Channel_2;
DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;//ADC3's data register
DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)&ADC3ConvertedValue;
DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
DMA_InitStruct.DMA_BufferSize = 5;
DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//Reads 16 bit values
DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//Stores 16 bit values
DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
DMA_InitStruct.DMA_Priority = DMA_Priority_High;
DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
DMA_Init(DMA2_Stream0, &DMA_InitStruct);
DMA_Cmd(DMA2_Stream0, ENABLE);
/* Configure GPIO pins ******************************************************/
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;// PC0, PC1, PC2, PC3
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;//The pins are configured in analog mode
GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;//We don't need any pull up or pull down
GPIO_Init(GPIOC, &GPIO_InitStruct);//Initialize GPIOC pins with the configuration
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//PA1
GPIO_Init(GPIOA, &GPIO_InitStruct);//Initialize GPIOA pins with the configuration
/* ADC Common Init **********************************************************/
ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
ADC_CommonInit(&ADC_CommonInitStruct);
/* ADC3 Init ****************************************************************/
ADC_DeInit();
ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;//Input voltage is converted into a 12bit int (max 4095)
ADC_InitStruct.ADC_ScanConvMode = ENABLE;//The scan is configured in multiple channels
ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//Continuous conversion: input signal is sampled more than once
ADC_InitStruct.ADC_ExternalTrigConv = 0;
ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//Data converted will be shifted to right
ADC_InitStruct.ADC_NbrOfConversion = 5;
ADC_Init(ADC3, &ADC_InitStruct);//Initialize ADC with the configuration
/* Select the channels to be read from **************************************/
ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 1, ADC_SampleTime_144Cycles);//PC0
ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 2, ADC_SampleTime_144Cycles);//PC1
ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 3, ADC_SampleTime_144Cycles);//PC2
ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 4, ADC_SampleTime_144Cycles);//PC3
ADC_RegularChannelConfig(ADC3, ADC_Channel_1,  5, ADC_SampleTime_144Cycles);//PA1
/* Enable DMA request after last transfer (Single-ADC mode) */
ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);

/* Enable ADC3 DMA */
ADC_DMACmd(ADC3, ENABLE);

/* Enable ADC3 */
ADC_Cmd(ADC3, ENABLE);
/* Start ADC3 Software Conversion */
ADC_SoftwareStartConv(ADC3);
uint32_t volatile *day_time_pointer;
day_time_pointer = &day_time;

uint32_t volatile *night_time_pointer;
night_time_pointer = &night_time;

uint32_t volatile *time_mode_pointer;
time_mode_pointer = &time_mode;

uint32_t volatile *Value_Offset_pointer;
Value_Offset_pointer = &Value_Offset;


   //Doors_init();
    Doors_Init(FrontDoor);
    Doors_Init(OutsideDoor);
  //  SysTick_Config(SystemCoreClock/1000); //1ms
  /* System init */
     SystemInit();

Configure_PA0();
Configure_PB12();

while(1)
    {
   

    ConvertedValue = ADC3ConvertedValue[1];
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



