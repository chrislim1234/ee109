/********************************************
 *
 *  Name: Chris Lim
 *  Email: chrislim@usc.edu
 *  Section: Wed 2-3:20
 *  Assignment: Project
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "ds18b20.h"
#include "project.h"
#include "lcd.h"
#include <avr/eeprom.h>
#define NUM_TONES 25
volatile unsigned char a, b;
volatile unsigned char new_state, old_state;
volatile unsigned char changed = 0;  // Flag for state change
volatile int count = 0;        // Count to display
volatile int count2 = 0;
unsigned int note_freq[NUM_TONES] =
{ 131, 139, 147, 156, 165, 176, 185, 196, 208, 220, 233, 247,
  262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523};

unsigned char note_names[NUM_TONES][4] = //here
{ "C3", "C3#","D3","D3#","E3","F3","F3#","G3","G3#","A3","A3#","B3",
    "C4", "C4#", "D4", "D4#", "E4", "F4", "F4#", "G4", "G4#", "A4", "A4#", "B4", "C5"
};
ISR(PCINT0_vect) {
    if (button==1) {
        if ((PINB&(1 << PB3))==0) {
        //if ((PINC&(1 << PC1))==0) {
            a =0;
        }
        else {
            a=1;
        }
        if ((PINB&(1 << PB4))==0) {
        //if ((PINC&(1 << PC5))==0) {
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
    }
    else if (button==2) {
        if ((PINB&(1 << PB3))==0) {
        //if ((PINC&(1 << PC1))==0) {
            a =0;
        }
        else {
            a=1;
        }
        if ((PINB&(1 << PB4))==0) {
        //if ((PINC&(1 << PC5))==0) {
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
                count2++;
            }
            else if (b) { //!a && b
                new_state = 2;
                count2--;
            }

        }
        else if (old_state == 1) {

            // Handle A and B inputs for state 1
            if (a && b) {
                new_state = 3;
                count2++;
            }
            else if (!a && !b) {
                new_state = 0;
                count2--;
            }

        }
        else if (old_state == 2) {

            // Handle A and B inputs for state 2
            if (!a && !b) {
                new_state = 0;
                count2++;
            }
            else if (a && b) {
                new_state = 3;
                count2--;
            }

        }
        else {   // old_state = 3

            // Handle A and B inputs for state 3
            if (!a && b) {
                new_state = 2;
                count2++;
            }
            else if (a && !b) {
                new_state = 1;
                count2--;
            }
        }
    }

    // If state changed, update the value of old_state,
    // and set a flag that the state has changed.
    if (new_state != old_state) {
        changed = 1;
        old_state = new_state;
    }
}


int find_note(unsigned char count) {
    int note = ((abs(count))%51)/2;
    return note;
}

unsigned char* find_note2(int n) {
    //short freq = note_freq[n];
    unsigned char* freq = note_names[n];
    return freq;
}

unsigned int find_freq(int n) {
    //short freq = note_freq[n];
    unsigned int freq = note_freq[n];
    return freq;
}



int find_temp(unsigned char count) {
    int temp = ((abs(count))%82)/2;
    return temp +60;
}

void encoder_init() {
    //initialize encoder
    DDRB&=~((1 << PB3) | (1 << PB4));
    PORTB |= ( (1 << PB3) | (1 << PB4) );
    PCICR|=(1 <<PCIE0);
    PCMSK0 |= ((1 << PCINT3 )| (1 << PCINT4 ));
    //speaker intitialize
    DDRC |= (1 << PC5);
    sei();
}

void encoder() {
    if (!b && !a) {
        old_state = 0;
    }
    else if (!b && a) {
        old_state = 1;
    }
    else if (b && !a) {
        old_state = 2;
    }
    else {
        old_state = 3;
        new_state = old_state;
    }
    if (changed & (button==1)) { // Did state change?
        changed = 0;        // Reset changed flag
        // Output count to LCD
        lcd_moveto(1,0);
        lcd_stringout("    ");
        int number = find_temp(count);
        temp = number;
        //EEPROM stuff
        unsigned char x = eeprom_read_byte((void *) 100);
        if (x!=number) {
            eeprom_update_byte((void *) 100, number);
        }
    }
    else if (changed & (button==2)) {
        changed = 0;        // Reset changed flag
        // Output count to LCD
        lcd_moveto(0,14);
        lcd_stringout("    ");
        int number = find_note(count2);
        note = find_note2(number);
        notename = number;
        freq = find_freq(notename);
        OCR0A = (int)(8000000/(freq * 256)) - 1;  //ensures buzzing frequency
        //EEPROM stuff
         unsigned char y = eeprom_read_byte((void *) 200);
        if (y!=number) {
            eeprom_update_byte((void *) 200, number);
        }
        
    }
}

