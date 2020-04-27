#include <Arduino.h>
#include "dynqueue.h"
#include "info_digitalout.h"
#include "main_defs.h"
#include "output_schedule.h"

output_schedule write_schedule(SCHEDULE_LIMIT);

// Setup timer 1
void init_TIM1();

// Preload timer 1 counter with time
void set_TCNT1(unsigned int count);

// Interpret command
void interpretCMDByte(uint8_t cmd_byte);

void setup() {

  // Initialize timers
  cli(); // Disable interrupts
  init_TIM1();
  sei(); // Enable interrupts

  Serial.begin(250000);
  pinMode(LED_BUILTIN, OUTPUT);
}

uint8_t packet_buf[6];
void loop() {
  while(Serial.available())
  {
    // Read into the buffer and write it to the schedule
    switch (Serial.readBytes(packet_buf, 6))
    {
    case 6: // Output and delay packet
      write_schedule.addPacket(packet_buf);
      break;

    case 1: // Command byte
      interpretCMDByte(packet_buf[0]);
      break;

    default:
      break;
    }
  }
}

ISR(TIMER1_OVF_vect)
{
  unsigned long remaining_time = write_schedule.remainingTime();
  if(write_schedule.isEmpty()) return;

  if(remaining_time > 0)
  {
    if(remaining_time > 0xFFFF)
    {
      set_TCNT1(0xFFFF);
      write_schedule.cutTime(0xFFFF);
    }else {
      set_TCNT1((unsigned int)remaining_time);
      write_schedule.cutTime(remaining_time);
    }
    return;
  }

  write_schedule.run();
}

void init_TIM1()
{
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  
  // Set clock select in control register B (0b010 for 8 prescaler)
  TCCR1B |= (1<<CS11);

  // Enable timer overflow interrupt
  TIMSK1 |= (1<<TOIE1);
  
  // Set counter high and low register to 0
  TCNT1H = 0;
  TCNT1L = 0;
}

void set_TCNT1(unsigned int count)
{
  unsigned int preload = 0xFFFF - count;
  TCNT1L = (preload & 0xFF);
  TCNT1H = (preload & 0xFF00) >> 8;
}

void interpretCMDByte(uint8_t cmd_byte)
{
  switch(cmd_byte)
  {
    // Request items left to send
    case 0x01:
    
      unsigned int items_left = write_schedule.itemsLeft();
      if(items_left > 0)
      {
        // Write out the remaining memory left for items.
        Serial.write((uint8_t)((items_left&0xFF00)>>8));
        Serial.write((uint8_t)(items_left&0xFF));
      }

      break;
    default:
      break;
  }
}