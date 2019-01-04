
#include "stm32f4xx.h"
#include "button.h"
/*
#define button_not_pressed      0x00
#define button_not_pressed      0x01

void door_init(void)
{


    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;  // enable the clock to GPIOD
    __asm("dsb");                         // stall instruction pipeline, until instruction completes, as
                                          //    per Errata 2.1.13, "Delay after an RCC peripheral clock enabling"
    GPIOD->MODER = (1 << 30);             // set pin 13 to be general purpose output



}



void door_open(void)
{


       GPIOD->ODR = (1 << 15);           // Toggle the pin 
}


void door_close(void)
{


       GPIOD->ODR = (0 << 15);           // Toggle the pin 

}
*/
