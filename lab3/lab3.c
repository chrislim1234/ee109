/********************************************
*
*  Name: Chris Lim	
*  Email: chrislim@usc.edu
*  Lab section: EE109 2-3:20
*  Assignment: Lab3
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>

#define DOT_LENGTH  250         /* Define the length of a "dot" time in msec */

void dot(void);
void dash(void);
void makeOutput(char);
char checkInput(char);

int main(void)
{

    // Initialize appropriate DDR registers
	DDRD |= 0x04;
	DDRB &= 0x00;
    // Initialize the LED output to 0
    PORTD &= 0x00;
		//PORTD |= (1 << 2);
    // Enable the pull-up resistors for the 
    // 3 button inputs 
	PORTB |= 0x38;
	
	//refresh pinb
	PINB &=0x00;

    // Loop forever
    while (1) {                 
    
	//  Use if statements and the checkInput()
	//  function to determine if a button
	//  is being pressed and then output
	//  the correct dot/dash sequence by
	//  calling the dot(), dash(), and 
	//  using appropriate delay functions
	
	if (checkInput(3)!=0) {
		// shows a U which is 2 dots 1 dash
		dot();
		dot();
		dash();
	}
	else if (checkInput(4)!=0) {
	// shows a S which is 3 dots
		dot();
		dot();
		dot();
	}
	else if (checkInput(5)!=0) {
	// shows a C which is dash dot dash dot
		dash();
		dot();
		dash();
		dot();
		
	}
	//delay for character
	_delay_ms(DOT_LENGTH*3);
		
    }

    return 0;   /* never reached */
}

/*
  dot() - Makes the output LED blink a "dot".

  Write code to generate a dot by using the makeOutput function
  to turn the output ON and OFF with appropriate delays.
  Be sure you don't forget to also delay an
  appropriate time after you turn the output off
*/
void dot()
{
	//turn on light
	makeOutput(1);
	//keep light on for 250
	_delay_ms(DOT_LENGTH);
	//turn off light
	makeOutput(0);
	//delay until next symbol
	_delay_ms(DOT_LENGTH);
}

/*
  dash() - Makes the output LED blink a "dash".

  Write code to generate a dash by using the makeOutput function
  to turn the output ON and OFF with appropriate delays.
  Be sure you don't forget to also delay
  appropriate time after you turn the output off
*/
void dash()
{
	//turn on light
	makeOutput(1);
	//keep light on for 750
	_delay_ms(DOT_LENGTH*3);
	//turn off light
	makeOutput(0);
	//delay until next symbol
	_delay_ms(DOT_LENGTH);
}

/*
  makeOutput() - Changes the output bit to either a zero 
  or one, based on the input argument "value".
  
  If the argument is zero, turn the output OFF,
  otherwise turn the output ON. 
  
  Do not use any delays here.  Just use bit-wise operations
  to make the appropriate PORT bit turn on or off.
*/
void makeOutput(char value)
{
	//PD2 light is off
	if (value == 1) {
		PORTD |= (1<< 2);
	}
	//light is on
	else {
		PORTD &= ~(1<< 2);
	}
}

/*
  checkInput(bit) - Checks the state of the input bit specified by the
  "bit" argument (0-7), and returns either 0 or 1 depending on its state.
  
  Write code to use the appropriate group's PIN register and determine if  
  the specified bit (which is passed as the argument) of that group is
  pressed or not.  Think carefully about what bit value means "pressed"
  when connected to a pushbutton.
 */
char checkInput(char bit)
{
	//check PinB bit is 0 which is when button is pressed
	if ((PINB & (1<< bit))==0) {
		return 1;
	}
	//not pressed
	else {
		return 0;
	}
}
