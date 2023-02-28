#include "adc_control.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>

void init_adc() {
    gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1 | GPIO2 | GPIO3 | GPIO4 | GPIO5 | GPIO6);


    adc_set_clk_prescale(ADC_CCR_ADCPRE_BY2); //Testing...

    adc_power_off(ADC1);
    adc_disable_scan_mode(ADC1);

    //adc_enable_temperature_sensor();

    adc_set_single_conversion_mode(ADC1); //Testing...

    //adc_set_sample_time(ADC1, ADC_CHANNEL0, ADC_SMPR_SMP_3CYC);
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_3CYC);

    adc_power_on(ADC1); //Test if keep here or if to move to the reading part (due to the power consumption?)
}

uint16_t get_light_val(uint8_t sensor_num) {
    uint8_t channel[16];
    channel[0] = sensor_num;

    adc_set_regular_sequence(ADC1, 1, channel);
    adc_set_multi_mode(ADC_CCR_MULTI_INDEPENDENT); //Testing...

    //adc_power_on(ADC1); //Test if I shoul.....

    adc_start_conversion_regular(ADC1);

    while(!adc_eoc(ADC1));
    uint16_t reg16 = adc_read_regular(ADC1);
    return reg16;
}

#define V_25        0.76 //datasheet table 71
#define AVG_SLOPE   0.0025 //-||-

//Sampling time must not be lower than 10 micro seconds...

uint16_t get_temp() {
    uint8_t channel[16];
    channel[0] = ADC_CHANNEL_TEMP_F40;

    adc_set_regular_sequence(ADC1, 1, channel);
    adc_set_multi_mode(ADC_CCR_MULTI_INDEPENDENT); //Testing...

    //adc_power_on(ADC1); //

    adc_start_conversion_regular(ADC1);

    while(!adc_eoc(ADC1));
    uint16_t reg16 = adc_read_regular(ADC1);
    uint16_t temp = ((V_25 - reg16) / AVG_SLOPE) + 25; //Datasheet formula
    return temp;
}
