/**
 *	Keil project for DS18B20 library example
 *
ces on 1wire bus

 *	Start conversion on all devices
 *	Read temperatures and display it
 *	Checks for devices with alarm flag set
 *
 *	Before you start, select your target, on the right of the "Load" button
 *
 *	@author		Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@ide		Keil uVision 5
 *	@packs		STM32F4xx Keil packs version 2.2.0 or greater required
 *	@stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 */
/* Include core modules */
#include "stm32f4xx.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_onewire.h"
#include "tm_stm32f4_usart.h"
#include "tm_stm32f4_ds18b20.h"
#include "tm_stm32f4_disco.h"
#include <stdio.h>


#include "door.h"
#include "interupt.h"
#include "adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "stm32f4xx_usart.h"

#include "tm_stm32f4_i2c.h"

/* How many sensors we are expecting on 1wire bus? */
#define EXPECTING_SENSORS     1	


uint32_t volatile light_offset, temp_offset, emty_bar_offset, water_level_offset, humidity_offset;
uint32_t volatile ConvertedValueOffset = 0;
uint32_t volatile day_time = 1;
uint32_t volatile night_time = 1;
uint32_t volatile time_mode = 1;
uint32_t volatile heating_on, heating_off, heating_mode, heating_offset, Value_Offset;
//uint16_t volatile ADC3ConvertedValue[5] = {0,0,0,0,0};


void EXTI0_IRQHandler();
void EXTI15_10_IRQHandler();


int main(void) {
	char buf[40];
	uint16_t light_value, tempvalue, devices, i, j, count, alarm_count;
	uint8_t device[EXPECTING_SENSORS][8], alarm_device[EXPECTING_SENSORS][8];
	float temps[EXPECTING_SENSORS];
	
	
    /* OneWire working struct */
	TM_OneWire_t OneWire1;
	
	/* Initialize system */
	SystemInit();
	
	/* Initialize delay */
	TM_DELAY_Init();
	
	/* Initialize OneWire on pin PD0 */
	TM_OneWire_Init(&OneWire1, GPIOC, GPIO_Pin_7);
	
	/* Initialize USART, TX: PB6, RX: PB7 */
	TM_USART_Init(USART3, TM_USART_PinsPack_1, 115200);
	

	/* Initialize I2C, SCL: PB6 and SDA: PB7 with 100kHt serial clock */
	TM_I2C_Init(I2C1, TM_I2C_PinsPack_1, 100000);

	
	/* Turn leds on */
	TM_DISCO_LedOn(LED_ALL);
	
	/* Checks for any device on 1-wire */
	count = 0;
	devices = TM_OneWire_First(&OneWire1);

	while (devices) {
		/* Increase counter */
		count++;
		
		/* Get full ROM value, 8 bytes, give location of first byte where to save */
		TM_OneWire_GetFullROM(&OneWire1, device[count - 1]);
		
		/* Get next device */
		devices = TM_OneWire_Next(&OneWire1);
	}
	
	/* If any devices on 1wire */
	if (count > 0) {
		sprintf(buf, "device on 1-wire: %d\n", count);
        TM_DISCO_LedOn(LED_GREEN);
        Delayms(1000);
        TM_DISCO_LedOff(LED_GREEN);
		TM_USART_Puts(USART1, buf);
		/* Display 64bit rom code for each device */
		for (j = 0; j < count; j++) {
			for (i = 0; i < 8; i++) {
				sprintf(buf, "0x%02X ", device[j][i]);
				TM_USART_Puts(USART3, buf);
			}
			TM_USART_Puts(USART3, "\n");
		}
	} else {
		TM_USART_Puts(USART3, "No devices on OneWire.\n\r");

        TM_DISCO_LedOff(LED_RED);
        TM_DISCO_LedOff(LED_BLUE);
	}
	
	/* Go through all connected devices and set resolution to 12bits */
	for (i = 0; i < count; i++) {
		/* Set resolution to 12bits */
		TM_DS18B20_SetResolution(&OneWire1, device[i], TM_DS18B20_Resolution_12bits);
	}
	
	/* Set high temperature alarm on device number 0, 25 degrees celcius */
	TM_DS18B20_SetAlarmHighTemperature(&OneWire1, device[0], 25);
	
	/* Disable alarm temperatures on device number 1 */
	TM_DS18B20_DisableAlarmTemperature(&OneWire1, device[1]);



adc_configure();    

   //Doors_init();
Doors_Init(FrontDoor);
Doors_Init(OutsideDoor);
  /* System init */
//SystemInit();

Configure_PA0();
Configure_PB12();

while (1) {


    
    /* Start temperature conversion on all devices on one bus */
		TM_DS18B20_StartAll(&OneWire1);
		
		/* Wait until all are done on one onewire port */
		while (!TM_DS18B20_AllDone(&OneWire1));
		
		/* Read temperature from each device separatelly */
		for (i = 0; i < count; i++) {
			/* Read temperature from ROM address and store it to temps variable */
			if (TM_DS18B20_Read(&OneWire1, device[i], &temps[i])) {
		       

                tempvalue = *temps;
                sprintf(buf,"Temp: %d temperatūra = %u\n\r",i,tempvalue); 
                /* Print temperature */
				TM_USART_Puts(USART3, buf);
		    } else {
				/* Reading error */
				TM_USART_Puts(USART3, "Reading error;\n");

			}
		}
		
		/* Reset alarm count */
		alarm_count = 0;
		
		/* Check if any device has alarm flag set */
		while (TM_DS18B20_AlarmSearch(&OneWire1)) {
			/* Store ROM of device which has alarm flag set */
			TM_OneWire_GetFullROM(&OneWire1, alarm_device[alarm_count]);
			/* Increase count */
			alarm_count++;
		}
		
		/* Format string and send over USART for debug */
		sprintf(buf, "Devices with alarm: %d\n\r", alarm_count);
		TM_USART_Puts(USART3, buf);
		
		/* Any device has alarm flag set? */
		if (alarm_count > 0) {
			/* Show rom of this devices */
			for (j = 0; j < alarm_count; j++) {
				TM_USART_Puts(USART3, "Device with alarm: ");
				for (i = 0; i < 8; i++) {
					sprintf(buf, "0x%02X ", alarm_device[j][i]);
					TM_USART_Puts(USART3, buf);
				}
				TM_USART_Puts(USART3, "\n\r    ");
			}
			TM_USART_Puts(USART3, "ALARM devices recognized!\n\r");
		}
		
	/* Print separator */
	TM_USART_Puts(USART3, "----------\n\r");
		
	/* Some delay */
	Delayms(1000);

    uint32_t volatile *day_time_pointer;
    day_time_pointer = &day_time;

    uint32_t volatile *night_time_pointer;
    night_time_pointer = &night_time;

    uint32_t volatile *time_mode_pointer;
    time_mode_pointer = &time_mode;

    uint32_t volatile *Value_Offset_pointer;
    Value_Offset_pointer = &Value_Offset;

    temp_offset = ADC3ConvertedValue[0];
    light_offset = ADC3ConvertedValue[1];
    light_value = ADC3ConvertedValue[2];
    emty_bar_offset = ADC3ConvertedValue[3];
    water_level_offset = ADC3ConvertedValue[4];
    ConvertedValueOffset = light_value + *Value_Offset_pointer;

    if (ConvertedValueOffset > light_offset) {

        *time_mode_pointer = 1;
        if (((*time_mode_pointer) == 1) && ((*day_time_pointer) == 1) )  {
            Doors_Open(FrontDoor);

			TM_USART_Puts(USART3, "It is a day!\n\r");
			TM_USART_Puts(USART3, "Doors start opening\n\r");
            *day_time_pointer = 0;
            *night_time_pointer = 1;
            *Value_Offset_pointer = 100;
        }
    } else if (ConvertedValueOffset < light_offset) {
        
             *time_mode_pointer = 0;
             if ((*time_mode_pointer == 0) && (*night_time_pointer == 1) )  {
             
			    TM_USART_Puts(USART3, "It is a night!\n\r");
			    TM_USART_Puts(USART3, "Doors start closing\n\r");
                Doors_Open(OutsideDoor);
                *time_mode_pointer = 1;
                *day_time_pointer = 1;
                *night_time_pointer = 0;
                *Value_Offset_pointer = -100;
             }

    }

    uint32_t volatile *heating_on_pointer;
    heating_on_pointer = &heating_on;

    uint32_t volatile *heating_off_pointer;
    heating_off_pointer = &heating_off;

    uint32_t volatile *heating_mode_pointer;
    heating_mode_pointer = &heating_mode;

    uint32_t volatile *heating_offset_pointer;
    heating_offset_pointer = &heating_offset;


    uint32_t volatile temp_converted_value_offset = 0;
    temp_converted_value_offset = tempvalue + *heating_offset_pointer;
    if (tempvalue > temp_offset) {

        *heating_mode_pointer = 1;
        if (((*heating_mode_pointer) == 1) && ((*heating_on_pointer) == 1) )  {
            Doors_Open(FrontDoor);

		   TM_USART_Puts(USART3, "water is cold!\n\r");
		   TM_USART_Puts(USART3, "start heating!\n\r");

            *heating_on_pointer = 0;
            *heating_off_pointer = 1;
            *heating_offset_pointer = 3;
        }
    } else if (temp_converted_value_offset < temp_offset) {
        
             *heating_mode_pointer = 0;
             if ((*heating_mode_pointer == 0) && (*heating_off_pointer == 1) )  {
                Doors_Open(OutsideDoor);

		        TM_USART_Puts(USART3, "water is warm!\n\r");
		        TM_USART_Puts(USART3, "stop heating!\n\r");

                *heating_mode_pointer = 1;
                *heating_on_pointer = 1;
                *heating_off_pointer = 0;
                *heating_offset_pointer = -3;
             }
	}
}

}
