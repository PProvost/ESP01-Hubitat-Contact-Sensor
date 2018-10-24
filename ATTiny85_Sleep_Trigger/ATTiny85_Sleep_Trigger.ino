// ATtiny85 sleep mode, wake on pin change interrupt demo
// Author: Nick Gammon
// Date: 12 October 2013

// ATMEL ATTINY 25/45/85 / ARDUINO
//
//                  +-\/-+
// Ain0 (D 5) PB5  1|    |8  Vcc
// Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1
// Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1
//            GND  4|    |5  PB0 (D 0) pwm0
//                  +----+

#include <avr/sleep.h>    // Sleep Modes
#include <avr/power.h>    // Power management

const byte LED = 3;  // pin 2
const byte SWITCH = 4; // pin 3 / PCINT4
#define PULSE_LEN 250

ISR (PCINT0_vect) 
 {
 // do something interesting here
 }
 
void setup ()
  {
  pinMode (LED, OUTPUT);
  pinMode (SWITCH, INPUT);
  digitalWrite (SWITCH, HIGH);  // internal pull-up
  
  // pin change interrupt (example for D4)
  PCMSK  |= bit (PCINT4);  // want pin D4 / pin 3
  GIFR   |= bit (PCIF);    // clear any outstanding interrupts
  GIMSK  |= bit (PCIE);    // enable pin change interrupts 
  
  }  // end of setup

void loop ()
  {
  digitalWrite (LED, HIGH);
  delay (PULSE_LEN); 
  digitalWrite (LED, LOW);
  // delay (500); 
  goToSleep ();
  }  // end of loop
  
  
void goToSleep ()
  {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0;            // turn off ADC
  power_all_disable ();  // power off ADC, Timer 0 and 1, serial interface
  sleep_enable();
  sleep_cpu();                             
  sleep_disable();   
  power_all_enable();    // power everything back on
  }  // end of goToSleep 
