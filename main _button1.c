

//#define USE_STDPERIPH_DRIVER
#include "stm32f4xx.h"
#include "delay.h"
#include "stm32f4_discovery.h"
#include "timer.h"
#define PRESSED_BUTTON_NONE 0x00
#define PRESSED_BUTTON_USER 0x01


void  coreTimer(volatile uint32_t times);
uint8_t  Read_User_Button(void);
    uint8_t MyKey = PRESSED_BUTTON_NONE;
    uint8_t  MyUserKeyState = 0;
int main(void)
{




    TIMER3_Config();

    STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);

while(1)
{




// read key
//

    MyKey = Read_User_Button();
    
    if(MyKey == PRESSED_BUTTON_USER)
    {
    MyUserKeyState++;
    if(MyUserKeyState > 1)
        MyUserKeyState = 0;

    }
//    else if(Mykey == PRESSED_BUTTON_NONE)
//    {
//    }
//

    if(MyUserKeyState == 0)
    {
    STM_EVAL_LEDOff(LED3); STM_EVAL_LEDOff(LED4); STM_EVAL_LEDOff(LED5); STM_EVAL_LEDOff(LED6);
    }
    else if (MyUserKeyState == 1)
    {
    
    STM_EVAL_LEDOn(LED3); STM_EVAL_LEDOn(LED4); STM_EVAL_LEDOn(LED5); STM_EVAL_LEDOn(LED6);
    }


} //while loop finish


/*
while ( *p_int < 21 ) {
    (*p_int)++;

       ms_delay(1500);

door_init();

       ms_delay(1500);
door_open();

       ms_delay(1500);
door_close();

       ms_delay(1500);



        }

        */
} // main is finishing

uint8_t  Read_User_Button(void)
{
    
  //  uint32_t coreTimer;
   uint32_t CurrentTimerVal;
 //   User blue butten is pressed
    if(STM_EVAL_PBGetState(BUTTON_USER))
    {
    CurrentTimerVal = coreTimer(0);
    while(STM_EVAL_PBGetState(BUTTON_USER) == Bit_SET);
    if((coreTimer(CurrentTimerVal) > 100))
        return PRESSED_BUTTON_USER;
    else
        
        return PRESSED_BUTTON_NONE;




    }
    else 
    {
    
        return PRESSED_BUTTON_NONE;
    }
}

void coreTimer(volatile uint32_t times) {
uint32_t CurrentTimerVal;
CurrentTimerVal = times;
while(CurrentTimerVal){};
}
