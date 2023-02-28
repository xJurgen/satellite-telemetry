#ifndef ADC_CONTROL
#define ADC_CONTROL

#include "libs.h"

void init_adc();
uint16_t get_light_val(uint8_t sensor_num);
uint16_t get_temp();

#endif
