 /*
    Copyright (c) 2010  Art Dahm (art@dahm.com)                  

 Permission is hereby granted, free of charge, to any
 person obtaining a copy of this software and associated
 documentation files (the "Software"), to deal in the
 Software without restriction, including without
 limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software,
 and to permit persons to whom the Software is furnished to
 do so, subject to the following conditions:

 The above copyright notice and this permission notice
 shall be included in all copies or substantial portions of
 the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
 ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*/

///////////////////////////////////////////////////////////////////////////////
//
// rgb.c
//
// A small ATtiny2313 program to control an RGB LED connected to
// pins 0-2 of PORTB
//
// Switches on pins 0 and 1 of PORTD control the behavior of the LED
// The default behavior is to slowly change colors. Alternate behavior is
// to show a single color which can be selected.
//
///////////////////////////////////////////////////////////////////////////////

#include "rgb.h"

///////////////////////////////////////////////////////////////////////////////

ISR(TIMER0_OVF_vect)	//timer 0 interrupt for the pwm core.
{
	uint8_t temp;

	static uint8_t count;	// PWM count
	static uint8_t out;		// Value to output to Port B

	count++;
	out = 0;

	// Turn pin on or off for each PWM
	for(temp = 0; temp < NUMBER_OF_PWMS; temp++) {
		if(pwms[temp] <= count) {
			 out |= _BV(temp);
		}
	}

	// Output PWMs
  PORTB = out;

	// Reset interrupt timer
	TCNT0 = 255;
}


///////////////////////////////////////////////////////////////////////////////

void initButtons(void) {
	// Set up input buttons
	DDRD = 0b00000000;
	PORTD = 0b00000011;

	pollStatePrev = BUTTONS_CURRENT;
}

///////////////////////////////////////////////////////////////////////////////

void pollButtons(void) {
	pollState = BUTTONS_CURRENT;	// Read the buttons
	pollStatePrev ^= pollState;		// Set bits that have changed to 1

	pressedButtons = pollStatePrev & pollState;		// Set bits that have been pressed to 1
	releasedButtons = pollStatePrev & ~pollState;	// Set bits that have been released to 1

	pollStatePrev = pollState;	//Save the buttons for next time
}

///////////////////////////////////////////////////////////////////////////////

void delay(uint16_t a) {

	uint16_t b;

	// Simple delay loop
	for (b=0; b<a; b++) {
			nop();
	}
}

///////////////////////////////////////////////////////////////////////////////

void setColor(void) {
	pwms[0] = (color & 0b00000001)?0x00:0xff;
	pwms[1] = (color & 0b00000010)?0x00:0xff;
	pwms[2] = (color & 0b00000100)?0x00:0xff;
}

///////////////////////////////////////////////////////////////////////////////

int main(void) {
	// declare variables
	uint8_t temp;
  uint8_t newpwms[NUMBER_OF_PWMS];	// Colors to work towards
	uint8_t done;											// Count the number of PWMs that have reached newpwms. 3 = done

	// Set up data direction register for Port B
	DDRB |= 0b00000111;

	// Set up timer registers
	TCCR0A = 0x00;
	TCCR0B = (0 << WGM02)|(0 << CS02)|(0 << CS01)|(1 << CS00);
	TIMSK |= (1 << TOIE0);

	// Set starting color
	for(temp=0; temp < NUMBER_OF_PWMS; temp++) {
		pwms[temp] = newpwms[temp] = 255;
	}

	// Initialize button registers and variables
	initButtons();

	// Start interrupts
	sei();

	// Main loop
	while(1) {
		// Color change delay. Also button debounce
		delay(speed);

		// if Changing Color mode
		if (mode == 0) {
			// initialize done variable
			done = 0;

			for(temp=0; temp < NUMBER_OF_PWMS; temp++) {
				// If this PWM has reached its new color increment done
				if (pwms[temp] == newpwms[temp]) {
					done++;
				// Move PWM towards its new color
				} else if (pwms[temp] < newpwms[temp]) {
					pwms[temp] += 1;
				} else if (pwms[temp] > newpwms[temp]) {
					pwms[temp] -= 1;
				}
			}
			
			// If all the PWMs have reached their new colors
			if (done == NUMBER_OF_PWMS) {

				// Pick a random color (other than black)
				// Each pin is either on or off without all pins being off
				done = (rand() % 6) + 1;

				// Set up PWMs for each pin
				for(temp=0; temp < NUMBER_OF_PWMS; temp++) {
						newpwms[temp] = ((done >> temp) & 0x01) * 255;
				}
			}
		}

		// Get the state of the buttons on Port D
		pollButtons();

		// If mode button is pressed, change mode
		if (releasedButtons & BUTTON_MODE) {
			mode ^= 1;
			// If solid color mode, set color
			if (mode) {
				setColor();
			}
		}

		// If change button is pressed
		if (releasedButtons & BUTTON_CHANGE){
			// Change speed that colors change
			if (mode == 0) {
				speed += 0x40;
				if (speed > 0x80) {
					speed = 0;
				}
				// Blink LED to show speed: 1 = Fast, 2 = Med, 3 = Slow
				cli();
				for (temp = (speed >> 6) + 1; temp > 0; temp--) {
					PORTB = 0x00;
					delay(BLINK_RATE);
					PORTB = 0x07;
					delay(BLINK_RATE);
				}
				PORTB = 0x00;
				delay(BLINK_DELAY);
				sei();
			// Or change color of LED
			} else {
				color += 1;
				color &= 0b00000111;
				if (!color) {
					color += 1;
				}
				setColor();
			}
		}
	}
}
