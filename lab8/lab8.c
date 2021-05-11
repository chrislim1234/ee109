/********************************************
 *
 *  Name: Chris Lim
 *  Email: chrislim@usc.edu
 *  Section: TThu9:30 Lab W2pm
 *  Assignment: Lab 8 - Digital Stopwatch
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "lcd.h"
#include "adc.h"
#include <avr/interrupt.h>
enum states { Stopped, Started, Lapped };
volatile unsigned char changed = 0;  // Flag for state change
volatile unsigned char num1 = '0';
volatile unsigned char num2 = '0';
volatile unsigned char num3 = '0';
volatile unsigned char state = Stopped;
ISR(TIMER1_COMPA_vect){
// increments every 0.25s
    if (state==Stopped) {
        //nothing
    }
    else {
        if (num3=='6') {
            num1='0';
            num2='0';
            num3='0';
        }
        else {
            if (num1=='9') {
                num1='0';
                num2++;
            }
            else {
                num1++;
            }
            if (num2==':') {
                num2='0';
                num3++;
            }
        }
        changed = 1;
    }
}

void init_timer1(unsigned short m)
{
TCCR1B |= (1 << WGM12);
TIMSK1 |= (1 << OCIE1A);
OCR1A = m;
TCCR1B |= (1 << CS12);
}

int main(void)
{
    // Initialize the LCD
    lcd_init();

    // Initialize the ADC
    adc_init();

    // Write splash screen
    lcd_moveto(0,0);
    lcd_stringout("Chris Lim");
    lcd_moveto(1,0);
    lcd_stringout("Lab 8");
    _delay_ms(2000);
    lcd_writecommand(0x01);
    
    //timer initialization
    init_timer1(6250);
    
    // Enable interrupts
    sei();
    
    //nitialize the count to 00.0 whenever the program is started.
    lcd_moveto(0,0);
    lcd_stringout("0.00              ");
    //The program should be in the "Stopped" state on start up.
    
    while(1) {
        //    Polls the ADC for button presses
        //    Updates the state of the stopwatch as needed
        unsigned char adc_result = adc_sample(0);
        //_delay_ms(100);
        if (adc_result>=40 && adc_result<=60) {
            while((adc_result>=40 && adc_result<=60)) {
                adc_result = adc_sample(0);
            }
            //top pressed
            if (state==Started) {
                state=Stopped;
            }
            else {
                state = Started;
            }
        }
        else if (adc_result>=80 && adc_result<=120) {
            while((adc_result>=80 && adc_result<=120)) {
                adc_result = adc_sample(0);
            }
            //bottom pressed
            if (state==Stopped) {
                lcd_moveto(0,0);
                lcd_stringout("00.0              ");
                num1='0';
                num2='0';
                num3='0';
            }
            else if (state==Started) {
                state=Lapped;
            }
            else {
                state = Started;
            }
        }
        
        //    Checks a flag variable to see if an update of the displayed time on the LCD is required.
        if (changed && state==Started) {
            changed = 0;        // Reset changed flag
            if (state==Lapped) {
                continue;
            }
            else {
                lcd_moveto(0,0);
                lcd_stringout("              ");
                char buf[5];
                //snprintf(buf, 5, "%d", count);
                lcd_moveto(0,0);
                buf[0] = num3;
                buf[1] = num2;
                buf[2]= '.';
                buf[3] = num1;
                buf[4] = '\0';
                lcd_stringout(buf);
            }
        }
    }
}
//while(1) {
//Task 1
//        unsigned char adc_result = adc_sample(0);
//        char buf[7];
//        snprintf(buf, 5, "%4d", adc_result);
//        lcd_moveto(0,0);
//        buf[6] = '\0';
//        lcd_stringout(buf);
        
        //increment every 0.1 second
//}
