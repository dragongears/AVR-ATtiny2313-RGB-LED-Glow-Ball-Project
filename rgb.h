#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define nop() asm volatile ("nop")
#define NUMBER_OF_PWMS 3		// One PWM for each pin of an RGB LED
#define BLINK_RATE 0x4fff		// Rate to blink LED to show speed change
#define BLINK_DELAY 0xcfff	// Wait time before returning to changing color after blinking

#define BUTTON_MODE 0b00000001			// Changing colors/Single color
#define BUTTON_CHANGE 0b00000010		// Speed/Color
#define BUTTONS_CURRENT PIND & 0b00000011 	// Actual state of button register


///////////////////////////////////////////////////////////////////////////////

// Global Variables
uint8_t pwms[NUMBER_OF_PWMS];	// One PWM for each pin of an RGB LED

uint8_t mode = 0;					// 0 = change, 1= single
uint8_t color = 0x01;			// Color when in Single Color mode
uint8_t speed = 0;				// Speed when in Changing Color mode

uint8_t pollState;				// The current state of the buttons after calling pollButtons()
uint8_t pressedButtons;		// Buttons whose state has changed to pressed since the last call to pollButtons()
uint8_t releasedButtons;	// Buttons whose state has changed to released since the last call to pollButtons()
uint8_t pollStatePrev;		// The previous state of the buttons before calling pollButtons()


///////////////////////////////////////////////////////////////////////////////

// Function Prototypes
int main(void);
void delay(uint16_t);	// A short delay

void initButtons(void);	// Initialize registers and variables for the hardware buttons
void pollButtons(void);	// Get the state of the hardware buttons

////////////////////////////////////////////////////////////////////////////////
