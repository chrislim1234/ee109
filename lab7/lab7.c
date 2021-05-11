/********************************************
 *
 *  Name:
 *  Email:
 *  Section:
 *  Assignment: Lab 7 - Rotary Encoder
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "lcd.h"
volatile unsigned char a, b;
volatile unsigned char new_state, old_state;
volatile unsigned char changed = 0;  // Flag for state change
volatile int count = 0;        // Count to display
ISR(PCINT1_vect) {
    if ((PINC&(1 << PC1))==0) {
        a =0;
    }
    else {
        a=1;
    }
    if ((PINC&(1 << PC5))==0) {
        b =0;
    }
    else {
        b=1;
    }
    // For each state, examine the two input bits to see if state
    // has changed, and if so set "new_state" to the new state,
    // and adjust the count value.
    if (old_state == 0) {

        // Handle A and B inputs for state 0
        if (a) { //a && !b
            new_state = 1;
            count++;
        }
        else if (b) { //!a && b
            new_state = 2;
            count--;
        }

    }
    else if (old_state == 1) {

        // Handle A and B inputs for state 1
        if (a && b) {
            new_state = 3;
            count++;
        }
        else if (!a && !b) {
            new_state = 0;
            count--;
        }

    }
    else if (old_state == 2) {

        // Handle A and B inputs for state 2
        if (!a && !b) {
            new_state = 0;
            count++;
        }
        else if (a && b) {
            new_state = 3;
            count--;
        }

    }
    else {   // old_state = 3

        // Handle A and B inputs for state 3
        if (!a && b) {
            new_state = 2;
            count++;
        }
        else if (a && !b) {
            new_state = 1;
            count--;
        }
    }

    // If state changed, update the value of old_state,
    // and set a flag that the state has changed.
    if (new_state != old_state) {
        changed = 1;
        old_state = new_state;
    }
}

void variable_delay_us(int);
void play_note(unsigned short);

// Frequencies for natural notes from middle C (C4)
// up one octave to C5.
unsigned short frequency[8] =
    { 262, 294, 330, 349, 392, 440, 494, 523 };

int main(void) {
    // Initialize DDR and PORT registers and LCD
    lcd_init();
    DDRB |= (1 << PB4);
    DDRC&=~(1 << PC1) | (1 << PC5);
    PORTC |= (1 << PC1) | (1 << PC5);
    PCICR|=(1 <<PCIE1);
    PCMSK1 |= ((1 << PCINT9)| (1 << PCINT13));
    sei();

    // Write a spash screen to the LCD
    lcd_moveto(0,0);
    lcd_stringout("Chris Lim");
    lcd_moveto(1,0);
    lcd_stringout("Lab 7 ");
    _delay_ms(2000);
    lcd_writecommand(0x01);
    
    while (1) {                 // Loop forever
            // Read the A and B inputs to determine the intial state.
        // In the state number, B is the MSB and A is the LSB.
        // Warning: Do NOT read A and B separately.  You should read BOTH inputs
        // at the same time, then determine the A and B values from that value.
        if (!b && !a)
        old_state = 0;
        else if (!b && a)
        old_state = 1;
        else if (b && !a)
        old_state = 2;
        else
        old_state = 3;
        new_state = old_state;
        
        if (changed) { // Did state change?
            changed = 0;        // Reset changed flag
            
            // Output count to LCD
            lcd_moveto(0,0);
            lcd_stringout("              ");
            char buf[7];
            snprintf(buf, 5, "%d", count);
            lcd_moveto(0,0);
            buf[6] = '\0';
            lcd_stringout(buf);
            // Do we play a note?
            if ((count % 8) == 0) {
            // Determine which note (0-7) to play
                int note = ((abs(count))%64) / 8;
            // Find the frequency of the note
                short freq = frequency[note];
            // Call play_note and pass it the frequency
                play_note(freq);
            }
        }
    }
}

/*
  Play a tone at the frequency specified for one second
*/
void play_note(unsigned short freq)
{
    unsigned long period;

    period = 1000000 / freq;      // Period of note in microseconds

    while (freq--) {
	// Make PB4 high
        PORTB |= (1 << PB4);
	// Use variable_delay_us to delay for half the period
        variable_delay_us(period/2);
	// Make PB4 low
        PORTB &= ~(1 << PB4);
	// Delay for half the period again
        variable_delay_us(period/2);
    }
}

/*
    variable_delay_us - Delay a variable number of microseconds
*/
void variable_delay_us(int delay)
{
    int i = (delay + 5) / 10;

    while (i--)
        _delay_us(10);
}
