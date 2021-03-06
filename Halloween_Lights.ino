/*
 * A battery-powered "blinking" lights toy.
 *
 * ATtiny85 connections:
 *
 *         +-\/-+
 *   Vcc  1|o   |8  Vcc
 *    NC  2|    |7  LED---/\/\/---GND
 *    NC  3|    |6  LED---/\/\/---GND
 *   GND  4|    |5  LED---/\/\/---GND
 *         +----+
 *
 */
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#define wdt_enable_interrupt() do { WDTCR |= _BV(WDIE); } while (0)

uint8_t countdown = 0;
// current enabled LED
int8_t current = -1;
// minimum on time
const uint8_t min_time = 6;
// maximum on time
const uint8_t max_time = 28;
// number of LEDs
const uint8_t leds = 3;
// number of "dead" slots (no LED enabled)
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
  wdt_enable_interrupt();
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
  wdt_enable_interrupt();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}


void update_leds(void)
{
  int8_t led = random(leds + dead) - dead;

  // turn off current LED
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
