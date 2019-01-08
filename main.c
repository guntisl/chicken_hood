#include "door.h"
#include "interupt.h"
#include "adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "misc.h"


uint32_t volatile ConvertedValue = 0; //Converted value readed from ADC
uint32_t volatile ConvertedValueOffset = 0;
uint32_t volatile day_time = 1;
uint32_t volatile night_time = 1;
uint32_t volatile time_mode = 1;
uint32_t volatile Value_Offset = 0;
uint16_t volatile ADC3ConvertedValue[5] = {0,0,0,0,0};


void EXTI0_IRQHandler();
void EXTI15_10_IRQHandler();

int main(void) {

adc_configure();    
    
    


   //Doors_init();
    Doors_Init(FrontDoor);
    Doors_Init(OutsideDoor);
  /* System init */
     SystemInit();

Configure_PA0();
Configure_PB12();

while(1)
    {
  
uint32_t volatile *day_time_pointer;
day_time_pointer = &day_time;

uint32_t volatile *night_time_pointer;
night_time_pointer = &night_time;

uint32_t volatile *time_mode_pointer;
time_mode_pointer = &time_mode;

uint32_t volatile *Value_Offset_pointer;
Value_Offset_pointer = &Value_Offset;


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
