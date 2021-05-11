/********************************************
*
*  Name: Chris Lim
*  Email: chrislim@usc.edu
*  Section: EE 109 9:30 TTh
*  Assignment: Lab 5 - Combination Lock
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "lcd.h"

enum states { S0, S1, S2, S3, S4 };

int main(void) {

    /* Call lcd_init to set up the LCD */
    lcd_init();

    /* Call lcd_stringout to print out your name */
    lcd_moveto(0,3);
    lcd_stringout("Chris Lim");
    /* Use snprintf to create a string with your birthdate */
    char birthdate = 24;
    char birthmonth = 11;
    char year1 = 20;
    char year2 = 00;
    char buf[12];
    snprintf(buf, 15, "%d/%d/%02d%02d", birthmonth, birthdate, year1, year2);
    buf[11] = '\0';
    /* Use lcd_moveto to start at an appropriate column
       in the bottom row to appear centered */
    lcd_moveto(1,3);
    /* Use lcd_stringout to print the birthdate string */
    lcd_stringout(buf);
    /* Delay 1 second */
    _delay_ms(1000);
    //clear and setup pins and variables
    lcd_writecommand(0x01);
    PORTC|=((1<<PC2)|(1<<PC4));
    char state = S0;
    unsigned char counter = 0;
    
    while(1)
    {
// You may assume the user will NOT press both buttons at once (though an interesting exercise would be to try to detect that occurrence, output some kind of error message to the LCD, and return to state 0 so that the user can not breach the lock by holding both buttons).
        
        //if counter hits 55 means that 3 seconds has passed without any button pressed
        if (counter==55) {
            state=S0;
            counter = 0;
        }
        //initial output that code to reverts back to whenever nothing is pressed
        if (state==S0) {
            lcd_moveto(0,0);
//            char bu[12];
//            snprintf(bu, 15, "%d", state);
//            lcd_stringout(bu);
            lcd_stringout("Entered:    ");
            lcd_moveto(1,0);
            lcd_stringout("Status:Locked   ");
            state=S0;
        }
        //  Read the two input buttons and determine if either is pressed
        //button A
        if ((PINC&(1<<PC2))==0) {
            counter = 0;
            //debouncing
            while((PINC&(1<<PC2))==0) {
                continue;
            }
            //state 1
            if (state==S1) {
                state = S1;
            }
            //state 4
            else if (state==S4) {
                state = S0;
            }
            //state 0, 2 and 3
            else {
                if (state==S3) {
                    lcd_moveto(1,7);
                    lcd_stringout("Unlocked  ");
                    counter = 0;
                    lcd_moveto(0,8+state);
                    lcd_stringout("A");
                    state = S4;
                }
                else {
                    lcd_moveto(0,8+state);
                    lcd_stringout("A");
                    if (state==S1) {
                        state=S2;
                    }
                    else if (state==S0) {
                        state=S1;
                        counter = 0;
                    }
                    else if (state==S2) {
                        state=S3;
                    }
                }
            }
        }
        //button B
        else if ((PINC&(1<<PC4))==0) {
            counter = 0;
            //debouncing
            while((PINC&(1<<PC4))==0) {
                continue;
            }
            //state 1 or 3
            if ((state==S1) || (state==S3) ) {
                if (state==S1) {
                    lcd_moveto(0,8+state);
                    lcd_stringout("B");
                }
                else {
                    lcd_moveto(0,7+state);
                    lcd_stringout(" ");
                    lcd_moveto(0,7+state);
                }
                state=S2;
            }
            //state 4
            else if (state==S4) {
                state=S0;
            }
            //state 0 or state 2
            else {
                state = S0;
            }

        }
        //If neither button is pressed just continue to the next iteration of the while loop and check again.
        else {
            //counter to iterate
            counter++;
        }
        
        //state stuff
        _delay_ms(50);
        //  Put a delay of some small amount at the end of the loop so you know how long (roughly) that it takes to go around the loop. Your program can then calculate how long it has been since the last button was pressed by counting how many times it has gone around the main loop since a button was pressed. Once 3 seconds has passed, it should return to state S0 and the display should reflect this.
    }

    return 0;   /* never reached */
}
