#include <stdint.h>

#ifndef MISC_FUNCS
#define MISC_FUNCS

uint16_t setup_pwm_pins(int pin1, int pin2, int max_val) {
    gpio_set_function(pin1, GPIO_FUNC_PWM);
    gpio_set_function(pin2, GPIO_FUNC_PWM);
    uint16_t slice_num = pwm_gpio_to_slice_num(pin1);
    pwm_set_wrap(slice_num, max_val);
    pwm_set_enabled(slice_num, true);
    return slice_num;
}



#endif