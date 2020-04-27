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

void setup_Serial()
{
  Serial.begin(115200);
  Serial.setTimeout(300);
}

void setup_Pins()
{
    pinMode(LED_BUILTIN, OUTPUT);
}