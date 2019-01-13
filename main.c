#include "door.h"
#include "interupt.h"
#include "adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "stm32f4xx_usart.h"

//#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_onewire.h"
#include "tm_stm32f4_usart.h"
#include <stdio.h>

#include "delay.h"





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

ms_delay(500);
	char buf[100];
	uint8_t devices, i, j, count, device[2][8];
	/* OneWire working structure */
	TM_OneWire_t OneWire1;
	
	/* Initialize system */
	SystemInit();
	
	/* Initialize delay */
//	TM_DELAY_Init();
	
	/* Initialize OneWire1 instance on pin PD0 */
	TM_OneWire_Init(&OneWire1, GPIOC, GPIO_Pin_7);
	
	/* Initialize USART, TX: PB6, RX: PB7 */
	TM_USART_Init(USART1, TM_USART_PinsPack_2, 115200);
	
	/* Check for any device on 1-wire */
	devices = TM_OneWire_First(&OneWire1);
	count = 0;
	while (devices) {
		/* Increase count variable */
		count++;
		
		/* Get full 8-bytes rom address */
		TM_OneWire_GetFullROM(&OneWire1, device[count - 1]);
		
		/* Check for new device */
		devices = TM_OneWire_Next(&OneWire1);
	}
	

	/* If any devices on 1-wire */
	if (count > 0) {
		sprintf(buf, "Devices found on 1-wire instance: %d\n", count);
		TM_USART_Puts(USART1, buf);
		
		/* Display 64bit rom code */
		for (j = 0; j < count; j++) {
			for (i = 0; i < 8; i++) {
				sprintf(buf, "0x%02X ", device[j][i]);
				TM_USART_Puts(USART1, buf);
			}
			TM_USART_Puts(USART1, "\n");
		}
	} else {
		/* Nothing on OneWire */
		TM_USART_Puts(USART1, "No devices on OneWire.\n\n");
	}

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




