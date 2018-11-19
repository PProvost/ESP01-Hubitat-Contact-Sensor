// ATtiny85 wake on pin change
//
// Will wake when INTERRUPT_PIN changes, sending a HIGH
// pulse on OUTPUT_SIGNAL_PIN for the number of milliseconds
// specified by PULSE_LEN.
//
// Derived from Nick Gammon's article "ATtiny85 sleep mode and wake on pin change"
// http://www.gammon.com.au/forum/?id=11488&reply=9#reply9
//
// ATMEL ATTINY 25/45/85 / ARDUINO PINOUT
//                 +-\/-+
// A0 - D5 - PB5  1|    |8  Vcc
// A3 - D3 - PB3  2|    |7  PB2 - D2 - A1
// A2 - D4 - PB4  3|    |6  PB1 - D1 - PWM1
//           GND  4|    |5  PB0 - D0 - PWM0
//                 +----+

#include <avr/sleep.h>    // Sleep Modes
#include <avr/power.h>    // Power management

// PIN ASSIGNMENTS
const byte OUTPUT_SIGNAL_PIN = 3; // D3/PB3/pin 2
const byte INTERRUPT_PIN = 4; 		// D4/PCINT4/pin 3

// Length (ms) of the output HIGH signal
#define PULSE_LEN 500

// Ununsed, but required for ISR wakeup
ISR (PCINT0_vect) 
{
}

void setup ()
{
	pinMode (OUTPUT_SIGNAL_PIN, OUTPUT);
	pinMode (INTERRUPT_PIN, INPUT);
	digitalWrite (INTERRUPT_PIN, HIGH);  // internal pull-up

	// pin change interrupt (example for D4)
	PCMSK  |= bit (PCINT4);  // monitored pin
	GIFR   |= bit (PCIF);    // clear any outstanding interrupts
	GIMSK  |= bit (PCIE);    // enable pin change interrupts 
}

void loop ()
{
	digitalWrite (OUTPUT_SIGNAL_PIN, HIGH);
	delay (PULSE_LEN); 
	digitalWrite (OUTPUT_SIGNAL_PIN, LOW);
	goToSleep ();
}

void goToSleep ()
{
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	ADCSRA = 0;            // turn off ADC
	power_all_disable ();  // power off ADC, Timer 0 and 1, serial interface
	sleep_enable();
	sleep_cpu();                             
	sleep_disable();   
	power_all_enable();    // power everything back on
}
