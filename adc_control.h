#ifndef ADC_CONTROL
#define ADC_CONTROL

#include "libs.h"

void init_adc_light();
void init_adc_temp();
uint8_t get_adc_val(uint8_t sensor_num);

#endif
