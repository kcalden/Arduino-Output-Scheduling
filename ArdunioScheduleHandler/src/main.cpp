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
void interpretCMDPacket(uint8_t packet[]);

// Compute checksum
uint8_t checksum(uint8_t packet[], unsigned int length);

void setup() {
  // Initialize timers
  cli(); // Disable interrupts
  init_TIM1();
  sei(); // Enable interrupts

  setup_Serial();
  setup_Pins();
}

// Eight byte packet buffer
uint8_t packet_buf[8], response_buffer[8];
void loop() {
  while(Serial.available())
  {
    // Clear response buffer
    for(int i = 0;i<8;i++) response_buffer[i] = 0;

    // Read packet
    Serial.readBytes(packet_buf, 8);

    // First bit is always the checksum
    response_buffer[0] = checksum(packet_buf, 8);

    // Interpret packet
    // TODO: Send response byte
    switch (packet_buf[0])
    {
      case 0x01: // Command
        interpretCMDPacket(packet_buf);
        break;
      case 0x02: // Output and delay packet
        info_digitalout new_info(packet_buf,1);
        // Check if the write schedule is full then push
        if(!write_schedule.full()) {
          // If the buffer was empty, immediately trigger the interrupt
          bool was_empty = write_schedule.empty();
          write_schedule.push(new_info);
          if(was_empty) set_TCNT1(1);
          Serial.write(response_buffer,8);
        }else{
          // Write back fault if the queue is full
          response_buffer[1] = 0x01;
          Serial.write(response_buffer,8);
        }
        break;
      default:
        response_buffer[0] = checksum(packet_buf, 8);
        Serial.write(response_buffer, 8);
        break;
    }
  }
}

// Interpret a command
void interpretCMDPacket(uint8_t packet[])
{
  switch(packet[1])
  {
    case 0x01: // Memory remaining on the schedule
      unsigned int items_left = write_schedule.itemsLeft();
      response_buffer[1] = (uint8_t)((items_left&0xFF00)>>8);
      response_buffer[2] = (uint8_t)(items_left&0xFF);
      Serial.write(response_buffer,8);
      break;
  }
}

uint8_t checksum(uint8_t packet[], unsigned int length)
{
  unsigned int sum = 0;

  for(int i = 0; i<length; i++)
  {
    sum += packet[i];
  }

  return (uint8_t) (sum & 0xFF);
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