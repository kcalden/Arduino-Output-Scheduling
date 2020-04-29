#pragma once

// InfoType for an output state. 
struct info_digitalout
{
    // Default constructor
    info_digitalout();
    // Initialize info with a pin, state, and delay
    info_digitalout(uint8_t new_pin, uint8_t new_state, unsigned long new_delay);
    
    // Copy constructor for info_digitalout
    info_digitalout(const info_digitalout& );

    // Byte packet constructor for info_digitalout
    // Converts bytes to members of this structure and store them
    // [ PIN STATE DELAY3 DELAY2 DELAY1 DELAY0 ]
    info_digitalout(uint8_t []);

    // Cut delay by given number
    void cutDelay(unsigned int);

    // Output state and the pin to output on
    uint8_t state, pin;
    
    // Delay before writing the state
    // This will correspond to the timer count
    unsigned long delay;
};

info_digitalout::info_digitalout(){ pin=0; state=0; delay=0; }

info_digitalout::info_digitalout(uint8_t new_pin, uint8_t new_state, unsigned long new_delay) 
{
    pin = new_pin;
    state = new_state;
    delay = new_delay;
};

info_digitalout::info_digitalout(const info_digitalout &new_info)
{  
    state = new_info.state;
    pin = new_info.pin;
    delay = new_info.delay;
}

info_digitalout::info_digitalout(uint8_t byte_packet[])
{
    pin = byte_packet[0];
    state = byte_packet[1];

    delay = byte_packet[2];
    for(int i = 3; i<=5;i++)
    {
        delay <<= 8;
        delay += byte_packet[i];
    }
}

void info_digitalout::cutDelay(unsigned int time)
{
    if(time >= delay)
    {
        delay = 0;
        return;
    }

    delay -= time;
}