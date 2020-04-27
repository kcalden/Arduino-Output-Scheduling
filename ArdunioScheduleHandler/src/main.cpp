#include <Arduino.h>
#include "dynqueue.h"
#include "info_digitalout.h"
#include "main_defs.h"
#include "main_init.h"
#include "main_helperfunc.h"
#include "output_schedule.h"

output_schedule write_schedule(SCHEDULE_LIMIT);

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