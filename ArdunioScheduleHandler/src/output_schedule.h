#pragma once

#include "dynqueue.h"
#include "info_digitalout.h"

class output_schedule
{
public:
    // Initialize schedule with a memory limit
    output_schedule(unsigned int);

    // Run item and free memory if the delay is 0
    // Return the remaining delay of the current item or next item
    // inline void run();
    void run();

    // Get the remaining time until next write
    inline unsigned long remainingTime();

    // Cut delay by given amount
    // This is just an unsigned integer because we only have 16 bit timers.
    inline void cutTime(unsigned int);

    // Schedule an item based on the given array of bytes
    // Returns if the packet is the first in the list
    // [ PIN STATE DELAY3 DELAY2 DELAY1 DELAY0 ]
    bool addPacket(uint8_t []);
    
    // Returns true if the queue is empty
    inline bool isEmpty();

    // Calculate items remaining
    inline unsigned int itemsLeft();
private: 
    dynqueue<info_digitalout> queue;
    unsigned int memory;
};

output_schedule::output_schedule(unsigned int init_memory) 
{
    memory = init_memory;
}

void output_schedule::run()
{
    info_digitalout * write_info = queue.peek();

    if(write_info == nullptr) return;
    digitalWrite(write_info->pin, write_info->state);

    queue.pop();
}

inline unsigned long output_schedule::remainingTime()
{
    return queue.peek()->delay;
}

inline void output_schedule::cutTime(unsigned int time)
{
    queue.peek()->delay -= time;
}

bool output_schedule::addPacket(uint8_t packet[])
{
    bool first = false;
    if(queue.peek() == nullptr) first = true;

    unsigned long new_delay = packet[2]*0x01000000 + packet[3]*0x010000 + packet[4]*0x0100 + packet[5]*0x01;
    queue.push(new info_digitalout(packet[0],packet[1],new_delay));

    return first;
}

inline bool output_schedule::isEmpty()
{
    return queue.peek() == nullptr;
}

inline unsigned int output_schedule::itemsLeft()
{
    unsigned int mem_left = memory - queue.getSize();
    return (unsigned int) (mem_left / sizeof(info_digitalout));
}