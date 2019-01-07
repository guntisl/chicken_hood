
#include "pointers.h"

int starting_values(void){



uint8_t day_time = 1U;
uint8_t night_time = 1U;
uint8_t time_mode = 1U;
uint8_t Value_Offset = 0U;

uint8_t ConvertedValue = 0U; //Converted value readed from ADC
uint8_t ConvertedValueOffset = 0U;
}

void pointers_init(void) {


int *day_time_pointer;
day_time_pointer = &day_time;

int *night_time_pointer;
night_time_pointer = &night_time;

int *time_mode_pointer;
time_mode_pointer = &time_mode;

int *Value_Offset_pointer;
Value_Offset_pointer = &Value_Offset;

}
