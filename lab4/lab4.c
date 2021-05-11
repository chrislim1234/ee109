/********************************************
*
*  Name:
*  Email:
*  Section:
*  Assignment: Lab 4 - Up/Down counter on LCD display
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"

enum states { UP, DOWN };

int main(void) {

    // Setup DDR and PORT bits for the 2 input buttons as necessary
    PORTC|=((1<<PC2)|(1<<PC4));
    // Initialize the LCD
    lcd_init();
    unsigned char counter_value = 0;
    unsigned char count_direction = UP;
    lcd_writecommand(1);
    int counter = 0;

    // Use a state machine approach to organize your code
    //   - Declare and initialize a variable to track what state you
    //     are in by assigning the values UP or DOWN to that variable.


    while (1) {               // Loop forever

        // Use an outer if statement to select what state you are in
        // Then inside each 'if' or 'else' block, perform the desired
        // output operations and then sample the inputs to update
        // the state for the next iteration
        
        //check button for state
        if ((PINC&(1<<PC2))==0) {
            count_direction=UP;
        }
        if ((PINC&(1<<PC4))==0) {
            count_direction=DOWN;
        }
        //every 500ms we make change to display
        if (counter%10==0) {
            //numbers go up
            if (count_direction==UP) {
                //reloop if it is greater than 9
                if (counter_value<9) {
                    counter_value++;
                }
                else {
                    counter_value=0;
                }
                //clear first letter and write number to data
                lcd_writecommand(2);
                lcd_writedata(counter_value + '0');
            }
            //numbers go down
            else {
                //reloop if it is smaller than 0
                if (counter_value>0) {
                    counter_value--;
                }
                else {
                    counter_value=9;
                }
                //clear first letter and write number to data
                lcd_writecommand(2);
                lcd_writedata(counter_value + '0');
            }
        }

        // Delay before we go to the next iteration of the loop
        _delay_ms(50);
        counter++;

    }

    return 0;   /* never reached */
}

