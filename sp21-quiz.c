#include <avr/io.h>
#include <util/delay.h>       // allows for _delay_ms() function
#define ALL_LEDS ((1 << 2) | (1<<1) | (1<<0))
int main()
{	/* ---- ADD ANY NECESSARY INIT CODE HERE ---- */
	int ledbit = 0;   // Which led to turn on next for case 3...can be used or deleted
	//make led's output on ddr
	DDRD|=0x07;
	//pull up resistors activated for d6 and d7
	PORTD|=0xc0;



	while(1) {
		char btns = PIND; 

		// Case 1: check if both pressed both pressed
		if(  (btns & 0xc0)== 0 ){
			//turn all lights on for 1 second
			PORTD|=0x07;
			_delay_ms(1000);

		}
		// Case 2: check if only A is pressed
		else if( (btns & (1 << 7)) == 0 ){  // fine to also check ((btns & (1<<6)) != 0)
			//turn all lights off for 500 ms
			PORTD&=~(0x07);
			_delay_ms(500);

		}
		// Case 3
		else {
			//when ledbit gets to three, we bring it down to 0
			if (ledbit>=3) {
				ledbit=0;
			}
			//turn on and off led
			PORTD|=(1<<ledbit);
			_delay_ms(250);
			PORTD&=~(1<<ledbit);
			
			//increase counter
			ledbit++;
			
		}
	} /* end while */
	return 0;  
} /* end main */
