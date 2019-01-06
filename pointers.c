
#include "pointers.h"

int starting_values(void){



int day_time = 1;
int night_time = 1;
int time_mode = 1;
int Value_Offset = 0;

int ConvertedValue = 0; //Converted value readed from ADC
int ConvertedValueOffset = 0;
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
