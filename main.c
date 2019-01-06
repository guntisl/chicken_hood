#include "door.h"
#include "interupt.h"
#include "adc.h"
 

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



