#include "door.h"
#include "interupt.h"
#include "adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "stm32f4xx_usart.h"





/*
 *  Specify the port and pin used for 1-wire comms
 */
#define ONEWIRE_PIN_NUM					7
#define ONEWIRE_PIN_MASK                (7<<ONEWIRE_PIN_NUM)
#define ONEWIRE_PORT					GPIOC
#define ONEWIRE_CLK						RCC_AHB1Periph_GPIOC  


/*
 *  The following macros collapse direct accesses of the GPIO registers into
 *  single commands.  Refer to stm32f4xx_gpio.c and the STM32F4xx Reference
 *  Manual (GPIO chapter) for details.
 */
#define  ONEWIRE_INPUT_READ				ONEWIRE_PORT->IDR&ONEWIRE_PIN_MASK
#define  ONEWIRE_OUTPUT_HIGH			ONEWIRE_PORT->BSRRL=ONEWIRE_PIN_MASK
#define  ONEWIRE_OUTPUT_LOW				ONEWIRE_PORT->BSRRH=ONEWIRE_PIN_MASK
#define  ONEWIRE_CONFIG_OUTPUT			ONEWIRE_PORT->MODER|=(GPIO_Mode_OUT<<(ONEWIRE_PIN_NUM*2))
#define  ONEWIRE_CONFIG_INPUT			ONEWIRE_PORT->MODER&=~(GPIO_MODER_MODER0<<(ONEWIRE_PIN_NUM*2))


#define  READ_ROM			0x33
#define  SKIP_ROM			0xcc
#define  READ_SCRATCHPAD	0xbe
#define  CONVERT_TEMP		0x44


/*
 *  Local functions
 */
static void					PingOneWireNetwork(void);
static void					OneWire_Init(void);
static void					delay_usecs(uint32_t  usecs);
static void					SendInitialization(void);
static void					SendByte(uint8_t  val);
static uint8_t				ReadByte(void);
static void					ReportScratchpad(void);
static void					StartConversion(void);
static void					ReportTemperature(void);
static void					ReportROM(void);



uint8_t						pad[9];
uint8_t						rom[8];







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



	OneWire_Init();

USART_PutString("\n\rDallas ds1820 temp\n\r");





// Call USART1 configuration
USART_Config();

//Send "Hello, World!" to PC

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

    if (data == 'T')
        {
  
    	    PingOneWireNetwork();
    	    ReportROM();
    	    ReportScratchpad();
    	    StartConversion();
            delay_usecs(750000);
    	    ReportTemperature();
	        ReportScratchpad();
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
}




static void  PingOneWireNetwork(void)
{
	uint8_t				response;

USART_PutString("\n\rSending initialization pulses...");

	response = 0;

	ONEWIRE_CONFIG_INPUT;
//	delays(100);

	while (1)
	{
		SendInitialization();
		response = ONEWIRE_INPUT_READ;		// device pulls line low in response
		delay_usecs(420);

		if (response == 0)  break;
	}
	USART_PutString("response detected.");
}



static void  StartConversion(void)
{
	USART_PutString("\n\rSending command to start conversion.");
	SendInitialization();
	delay_usecs(100);
	SendByte(SKIP_ROM);
	SendByte(CONVERT_TEMP);
}

static void  ReportTemperature(void)
{
	uint32_t			val;
	uint32_t			t;
	uint32_t			frac;
	uint8_t				n;

	SendInitialization();
	delay_usecs(100);
	SendByte(SKIP_ROM);
	SendByte(READ_SCRATCHPAD);
	for (n=0; n<9; n++)
	{
		pad[n] = ReadByte();
	}
	val = (pad[1] * 256 + pad[0]);			// temp in 0.5 degs C
	t = val;
	val = val >> 1;							// temp in degs C
	frac = 0;
	if ((val << 1) != t)  frac = 5;			// if the roll lost a bit, allow for 0.5 deg C
	USART_PutString("\n\rTemperature is: %d.%d degrees C", val, frac);
}



static void  ReportROM(void)
{
	uint8_t					n;

	SendInitialization();
	delay_usecs(100);
	SendByte(READ_ROM);
	for (n=0; n<8; n++)
	{
		rom[n] = ReadByte();
	}
	USART_PutString("\n\rROM:  7  6  5  4  3  2  1  0");
	USART_PutString("\n\r     %02x %02x %02x %02x %02x %02x %02x %02x",
		rom[7], rom[6], rom[5], rom[4], rom[3], rom[2], rom[1], rom[0]);
}


static void  ReportScratchpad(void)
{
	uint8_t					n;

	SendInitialization();
	delay_usecs(100);
	SendByte(SKIP_ROM);
	SendByte(READ_SCRATCHPAD);
	for (n=0; n<9; n++)
	{
		pad[n] = ReadByte();
	}
	USART_PutString("\n\rScratchpad contains: %02x %02x %02x %02x %02x %02x %02x %02x %02x",
		pad[0], pad[1], pad[2], pad[3], pad[4], pad[5], pad[6], pad[7], pad[8]);
}


static void  SendInitialization(void)
{
	ONEWIRE_OUTPUT_HIGH;
	ONEWIRE_CONFIG_OUTPUT;
	delay_usecs(500);

	ONEWIRE_OUTPUT_LOW;
	delay_usecs(500);

	ONEWIRE_OUTPUT_HIGH;
	ONEWIRE_CONFIG_INPUT;
	delay_usecs(50);
}


static void  SendByte(uint8_t  val)
{
	uint8_t				n;

	for (n=0; n<8; n++)
	{
		ONEWIRE_OUTPUT_LOW;
		ONEWIRE_CONFIG_OUTPUT;
		delay_usecs(5);
		if (val & 1)  ONEWIRE_OUTPUT_HIGH;
		delay_usecs(95);
		ONEWIRE_OUTPUT_HIGH;
		delay_usecs(5);
		val = val >> 1;
	}
}


static  uint8_t  ReadByte(void)
{
	uint8_t				n;
	uint8_t				val;

	val = 0;
	for (n=0; n<8; n++)
	{
		val = val >> 1;
		ONEWIRE_OUTPUT_LOW;
		ONEWIRE_CONFIG_OUTPUT;
		delay_usecs(15);
		ONEWIRE_OUTPUT_HIGH;
		ONEWIRE_CONFIG_INPUT;
		delay_usecs(10);
		if (ONEWIRE_INPUT_READ)  val = val | 0x80;
		delay_usecs(35);
	}
	return  val;
}




/*
 *  OneWire_Init      hardware-specific configuration of 1-wire I/O
 */
static void  OneWire_Init(void)
{
	GPIO_InitTypeDef			GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(ONEWIRE_CLK, ENABLE);		// route the clocks

    GPIO_InitStructure.GPIO_Pin = ONEWIRE_PIN_MASK;				// select the pin to modify
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				// set the mode to output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			// set the I/O speed to 100 MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;				// set the output type to open-drain
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			// set the pull-up to none
    GPIO_Init(ONEWIRE_PORT, &GPIO_InitStructure);				// do the init
}

static void  delay_usecs(uint32_t  usecs)
{
	uint32_t			n;

	n = (144 * usecs) / 10;

	while (n)
	{
		n--;
	}
}

