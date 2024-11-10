#include <stdint.h>
#include <Adafruit_NeoPXL8.h>

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

void blink(Adafruit_NeoPXL8& leds, uint32_t colour, uint16_t period_ms, uint16_t total_length_ms) {
    uint64_t elapsed_time_ms = 0;
    bool leds_on = true;
    while (elapsed_time_ms < total_length_ms) {
        leds.fill(leds_on? colour : black);
        leds.show();
        delay(period_ms);
        elapsed_time_ms += period_ms;
        leds_on = !leds_on;
    }
}


#endif