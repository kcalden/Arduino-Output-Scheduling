#include <Arduino.h>
// #include "dynqueue.h"
#include "info_digitalout.h"
#include "main_defs.h"
#include "main_init.h"
// #include "output_schedule.h"
#include "staticqueue.h"

// output_schedule write_schedule(SCHEDULE_LIMIT);
StaticQueue<info_digitalout, SCHEDULE_LIMIT> write_schedule;

// Interpret command
void interpretCMDByte(uint8_t cmd_byte);

void setup() {
  // Initialize timers
  cli(); // Disable interrupts
  init_TIM1();
  sei(); // Enable interrupts

  setup_Serial();
  setup_Pins();
}

// Eight byte packet buffer
uint8_t packet_buf[9];

void loop() {
  while(Serial.available())
  {
    // Read into the buffer and write it to the schedule
    switch (Serial.readBytes(packet_buf, 6))
    {
      case 1: // Command byte
        interpretCMDByte(packet_buf[0]);
        break;
      case 6: // Output and delay packet
        info_digitalout new_info = packet_buf;

        // If the buffer was empty, immediately trigger the interrupt
        if(write_schedule.empty()) set_TCNT1(0);
        write_schedule.push(new_info);
        break;
    }
  }
}

// Interpret a command
void interpretCMDByte(uint8_t cmd_byte)
{
  switch(cmd_byte)
  {
    // Request items left to send
    case 0x01:
      unsigned int items_left = write_schedule.itemsLeft();
      Serial.write((uint8_t)((items_left&0xFF00)>>8));
      Serial.write((uint8_t)(items_left&0xFF));
      break;
  }
}

ISR(TIMER1_OVF_vect)
{
  // If the schedule is empty don't do anything 
  if(write_schedule.empty()) return;
  
  info_digitalout & current_write = write_schedule.peek();
  unsigned long remaining_time = current_write.delay;

  // Cut down the remaining time and wait
  if(remaining_time > 0)
  {
    if(remaining_time > 0xFFFF)
    {
      set_TCNT1(0xFFFF);
      current_write.cutDelay(0xFFFF);
    }else {
      set_TCNT1((unsigned int)remaining_time);
      current_write.cutDelay(remaining_time);
    }
    return;
  }

  // Write the state out and advance the queue
  digitalWrite(LED_BUILTIN, current_write.state);
  write_schedule.pop();
}