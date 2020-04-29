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
  Serial.setTimeout(100);
}

void setup_Pins()
{
    pinMode(LED_BUILTIN, OUTPUT);
}