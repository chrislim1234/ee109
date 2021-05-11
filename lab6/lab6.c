/********************************************
 *
 *  Name: Chris Lim
 *  Email: chrislim@usc.edu
 *  Section: TThu9:30 Lab W2pm
 *  Assignment: Lab 6 - Analog-to-digital conversion
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "lcd.h"
#include "adc.h"

void rand_init(void);

int main(void)
{

    // Initialize the LCD
    lcd_init();

    // Initialize the ADC
    adc_init();

    // Initialize the random number function
    rand_init();

    // Write splash screen
    lcd_moveto(0,0);
    lcd_stringout("Chris Lim");
    lcd_moveto(1,0);
    lcd_stringout("Lab 6 Game Start");
    _delay_ms(2000);
    lcd_writecommand(0x01);
//
//    // Find a random initial position of the 'X'
//    char buf[15];
    unsigned char r = rand();
    unsigned char num = r%16;
//
//    // Display the 'X' on the screen
    lcd_moveto(0,num);
    lcd_stringout("x");

    //counter for later
    int counter=0;

    //location intiialize for ^
    unsigned char p = 0;

    while (1) {                 // Loop forever
	// Check for button pressed
          unsigned char adc_result = adc_sample(0);
//        char buf[7];
//        snprintf(buf, 5, "%4d", adc_result);
//        lcd_moveto(0,0);
//        buf[6] = '\0';
//        lcd_stringout(buf);
        //If pressed, move 'X' to new position
        if (adc_result<5) {
            //right
            while(adc_result<5) {
                adc_result = adc_sample(0);
            }
            lcd_moveto(0,num);
            lcd_stringout(" ");
            num=(num+1)%16;
            lcd_moveto(0,num);
            lcd_stringout("x");
        }
        else if ((adc_result<=160) && (adc_result>=150)) {
            while((adc_result<=160) && (adc_result>=150)) {
                adc_result = adc_sample(0);
            }
            //left
            lcd_moveto(0,num);
            lcd_stringout(" ");
            num=(num+15)%16;
            lcd_moveto(0,num);
            lcd_stringout("x");
        }
        

        // Do a conversion of potentiometer input
        lcd_moveto(1,0);
        lcd_stringout("                ");
        unsigned char pot = adc_sample(3);
        p = (pot-pot%16)/16;

	// Move '^' to new position
        lcd_moveto(1,p);
        lcd_stringout("^");

	// Delay
        _delay_ms(10);
	// Check if '^' is aligned with 'X'
        if (p==num) {
            counter++;
        }
        else {
            counter = 0;
        }

        if (counter==50) {
            lcd_moveto(0,0);
            lcd_stringout("You have won!");
            while(1) {

            }
        }
    }

    return 0;   /* never reached */
}

void rand_init()
{
    int seed = 0;
    unsigned char i, j, x;

    // Build a 15-bit number from the LSBs of an ADC
    // conversion of the channels 1-5, 3 times each
    for (i = 0; i < 3; i++) {
	for (j = 1; j < 6; j++) {
	    x = adc_sample(j);
	    x &= 1;	// Get the LSB of the result
	    seed = (seed << 1) + x; // Build up the 15-bit result
	}
    }
    srand(seed);	// Seed the rand function
}


