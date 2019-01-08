#include "door.h"
#include "interupt.h"
#include "adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "stm32f4xx_usart.h"


void USART_Config(void)
{
// Enable clock for GPIOB
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
// Enable clock for USART1
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
// Connect PB6 to USART1_Tx
GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
// Connect PB7 to USART1_Rx
GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
// Initialization of GPIOB
GPIO_InitTypeDef GPIO_InitStruct;
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
GPIO_Init(GPIOB, &GPIO_InitStruct);
//Initialization of USART1
USART_InitTypeDef USART_InitStruct;
USART_InitStruct.USART_BaudRate = 9600;
USART_InitStruct.USART_HardwareFlowControl =
USART_HardwareFlowControl_None;
USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
USART_InitStruct.USART_Parity = USART_Parity_No;
USART_InitStruct.USART_StopBits = USART_StopBits_1;
USART_InitStruct.USART_WordLength = USART_WordLength_8b;
USART_Init(USART1, &USART_InitStruct);
//Enable USART1
USART_Cmd(USART1, ENABLE);
}
void USART_PutChar(char c)
{
    // Wait until transmit data register is empty
while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
// Send a char using USART1
USART_SendData(USART1, c);
}

void USART_PutString(char *s)
{
    // Send a string
while (*s)
{
USART_PutChar(*s++);
}
}


uint16_t USART_GetChar()
{
    // Wait until data is received
while (!USART_GetFlagStatus(USART1, USART_FLAG_RXNE));
// Read received char
return USART_ReceiveData(USART1);}


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

// Call USART1 configuration
USART_Config();

//Send "Hello, World!" to PC
USART_PutString("Hello, World!\n");

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
    uint16_t data = USART_GetChar();

    if (data == 'H')
        {
  
            Doors_Open(FrontDoor);
        }
     else if (data == 'L')
         {
  
            Doors_Close(FrontDoor);
         }
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
return 0;
}
