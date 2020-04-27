#pragma once

#include "Arduino.h"
// InfoType for an output state. 

struct info_digitalout
{
    info_digitalout(uint8_t new_pin, uint8_t new_state, unsigned long new_delay);

    // Output state and the pin to output on
    uint8_t state, pin;
    
    // Delay before writing the state
    // This will correspond to the timer
    // Currently the timer has its prescaler as 8 (500 nanoseconds per count)
    // Max delay using an unsigned long is 35.7913941 minutes
    unsigned long delay;
};

info_digitalout::info_digitalout(uint8_t new_pin, uint8_t new_state, unsigned long new_delay) 
{
    pin = new_pin;
    state = new_state;
    delay = new_delay;
};