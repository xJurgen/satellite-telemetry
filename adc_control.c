#include "adc_control.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>

void init_adc() {
    gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1 | GPIO2 | GPIO3 | GPIO4 | GPIO5 | GPIO6);


    adc_set_clk_prescale(ADC_CCR_ADCPRE_BY2);

    adc_power_off(ADC1);
    adc_disable_scan_mode(ADC1);

    adc_set_single_conversion_mode(ADC1);
    adc_enable_temperature_sensor();

    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_3CYC);

    adc_set_multi_mode(ADC_CCR_MULTI_INDEPENDENT);

    adc_power_on(ADC1); //Test if keep here or if to move to the reading part (due to the higher power consumption?)

    /* Wait for ADC starting up. */
    for (int i = 0; i < 800000; i++)    /* Wait a bit. */
        __asm__("nop");

    //adc_reset_calibration(ADC1);
    //adc_calibrate(ADC1);

}

uint16_t get_light_val(uint8_t sensor_num) {
    uint8_t channel[16];
    channel[0] = sensor_num;

    adc_set_regular_sequence(ADC1, 1, channel);

    adc_start_conversion_regular(ADC1);

    while(!adc_eoc(ADC1));
    uint16_t reg16 = adc_read_regular(ADC1);
    return reg16;
    //return reg16 / 4096 * 3.3;
}

#define V_25        0.76 //datasheet table 71
#define AVG_SLOPE   0.0025 //-||-
#define VSENSE      3.3/4096 //3.3 over 4096 samples

//Sampling time must not be lower than 10 micro seconds...

uint16_t get_temp() {
    uint8_t channel[16];
    channel[0] = ADC_CHANNEL_TEMP_F42;

    adc_set_regular_sequence(ADC1, 1, channel);

    adc_start_conversion_regular(ADC1);

    while(!adc_eoc(ADC1));
    uint16_t reg16 = adc_read_regular(ADC1);
    //return V_25 - reg16;
    uint16_t temp = ((V_25 - (reg16*VSENSE)) / AVG_SLOPE) + 25; //Datasheet formula
    return temp;
}
