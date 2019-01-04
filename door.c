
//#include "stm32f4xx.h"
#include "door.h"


GPIO_TypeDef* GPIO_PORT[DOORn] = {FrontDoor_GPIO_PORT, BuckDoor_GPIO_PORT, WindowDoor_GPIO_PORT,
                                 OutsideDoor_GPIO_PORT};
const uint16_t GPIO_PIN[DOORn] = {FrontDoor_PIN, BuckDoor_PIN, WindowDoor_PIN,
                                 OutsideDoor_PIN};
const uint32_t GPIO_CLK[DOORn] = {FrontDoor_GPIO_CLK, BuckDoor_GPIO_CLK, OutsideDoor_GPIO_CLK,
                                 WindowDoor_GPIO_CLK};

/**
  * @brief Configure Doors connected GPIO .
  * @param  Doors: Specifies the Doors to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg FrontDoors
  *     @arg BuckDoors
  *     @arg OutsideDoors
  *     @arg WindowDoors
  * @retval None

*/
void Doors_Init(Door_TypeDef Door)
{

    GPIO_InitTypeDef Doors_InitStructure;

    /* Enable the Doors GPIO  Clock */
    RCC_AHB1PeriphClockCmd(GPIO_CLK[Door], ENABLE);
    Doors_InitStructure.GPIO_Pin = GPIO_PIN[Door];
    Doors_InitStructure.GPIO_OType = GPIO_OType_PP;
    Doors_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    Doors_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_PORT[Door], &Doors_InitStructure);
}


/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED4
  *     @arg LED3
  *     @arg LED5
  *     @arg LED6  
  * @retval None

*/



void Doors_Open(Door_TypeDef Door)
{
    

  GPIO_PORT[Door]->BSRRL = GPIO_PIN[Door];
  
}



void Doors_Close(Door_TypeDef Door)
{
  GPIO_PORT[Door]->BSRRH = GPIO_PIN[Door];  
}








/*
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

}*/
