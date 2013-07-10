#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

uint8_t countdown = 0;
int8_t current = -1;
const uint8_t min_time = 6;
const uint8_t max_time = 28;
const uint8_t leds = 3;
const uint8_t dead = 1;


void setup_io()
{
  for (uint8_t i = 0; i < leds; ++i)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  for (uint8_t i = leds; i < 5; ++i)
  {
    pinMode(i, INPUT);
  }
}


ISR(WDT_vect)
{
  WDTCR |= _BV(WDIE);
}

void wait()
{
  cli();
  {
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
  }
  sei();
}

void setup_wait()
{
  power_all_disable();
  wdt_enable(WDTO_250MS);
  WDTCR |= _BV(WDIE);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}


void update_leds(void)
{
  int8_t led = random(leds + dead) - dead;

  // turn off current
  if (current >= 0)
  {
    digitalWrite(current, LOW);
  }

  if (led >= 0)
  {
    if (led == current)
    {
      // blink
      wait();
    }
    // turn on led
    digitalWrite(led, HIGH);
  }
  current = led;
  countdown = random(min_time, max_time+1);
}


void setup()
{
  uint8_t seed = 0;
  for (int i = 0; i < 4; ++i)
  {
    seed += analogRead(i);
  }
  randomSeed(seed);

  setup_io();
  setup_wait();
}

void loop()
{
  wait();
  if (countdown)
  {
    countdown--;
  }
  else
  {
    update_leds();
  }
}
