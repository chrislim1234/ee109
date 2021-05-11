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
#include <avr/eeprom.h>
#include "lcd.h"
#include "ds18b20.h"
#include "encoder.h"
#include "project.h"
#define NUM_TONES 25
volatile char state = 3;
char paststate = 3;
volatile char counter = '0';
int temp = 80;
int notename = 0;
int timer = 0;
unsigned char* note;
unsigned int freq;
volatile unsigned char button = 0 ;
volatile unsigned char alarm_buzzer = 0; //if alarm should buzz, set to 1


ISR(PCINT1_vect) {
    if ((PINC & (1<<PC1))==0) {
        lcd_moveto(1,3);
        lcd_stringout("<");
        lcd_moveto(1,13);
        lcd_stringout(" ");
        button = 1;
    }
    else if ((PINC & (1<<PC2))==0) {
        lcd_moveto(1,13);
        lcd_stringout(">");
        lcd_moveto(1,3);
        lcd_stringout(" ");
        button = 2;
    }
}

ISR(TIMER0_COMPA_vect){
    if (alarm_buzzer) {
            //invert PC5
        PINC |= (1<<PC5);
        timer++;
        if(timer > freq*2){
            alarm_buzzer = 0;
            timer = 0;
        }
    }
}

ISR(TIMER1_COMPA_vect){
    if ((state==1) & (counter%5==0)) {
//        lcd_moveto(1,6);
//        lcd_stringout("1");
        if ( (PORTD & (1<< PD3)) ) { //=1
            PORTD &= ~(1<< PD3);
        }
        else {
            PORTD |= (1<< PD3);
        }
    }
    else if (state==2) {
//        lcd_moveto(1,6);
//        lcd_stringout("2");
            //turn on LED
            PORTD |= (1<< PD3);
    }
    counter++;
}

void init_timer1(unsigned short m)
{
TCCR1B |= (1 << WGM12);
TIMSK1 |= (1 << OCIE1A);
OCR1A = m;
TCCR1B |= (1 << CS12);
}

void init_timer0(unsigned short m)
{
TCCR0A |= (1 << WGM01);
TIMSK0 |= (1 << OCIE0A);
OCR0A = m;
//TCCR0B |= ( (1 << CS02) | (1<<CS00) );
TCCR0B |= (1<<CS02);
}



//char checkInput(char);
//void init_timer0(unsigned short);
int main(void) {
    // Initialize LCD
    lcd_init();
    
    //initialize buttons
    DDRC &= 0x00;
    PORTC |= ( (1<<PC1) | (1<<PC2) );
    
    //initialize LED
    DDRD |= ( (1<<PD2) | (1<<PD3) );
    PORTD &=0x00;

    //encoder and buzzer initialize
    encoder_init();

    //timer1 initialization
    init_timer1(6250);
    
    // Enable interrupts
    PCICR|=(1 <<PCIE1);
    PCMSK1 |= ((1 << PCINT9)| (1 << PCINT10));
    sei();
    
    
    // Write a spash screen to the LCD
    lcd_moveto(0,0);
    lcd_stringout("Chris Lim");
    lcd_moveto(1,0);
    lcd_stringout("Project ");
    _delay_ms(2000);
    lcd_writecommand(0x01);
    
    //initialize temperature sensor
    unsigned char t[2];
    
    //EEPROM
    unsigned char x = eeprom_read_byte((void *) 100);
    unsigned char y = eeprom_read_byte((void *) 200);
    if ((x>=0x3C) && (x<=0x50)) {
        temp = x;
    }

    if ((y>=0x00) && (y<= 0x19)) {
        notename = y;
        note = note_names[notename];
    }
    
    //timer0 init
    int f = (int)(8000000/(note_freq[notename] * 256)) - 1;
    freq =note_freq[notename];
    init_timer0(f);
    
    
    
    if (ds_init() == 0) {    // Initialize the DS18B20
         // Sensor not responding
    }
    while (1) {                 // Loop forever
        if (ds_temp(t)) {    // True if conversion complete
            /*
              Process the values returned in t[0]
              and t[1] to find the temperature.
            */
            //1st row
            unsigned int newnum = 0x0000;
            char but[7];
            newnum |= t[1]<<8;
            newnum |= t[0];
            //multiply 9/5
            newnum = newnum * 9;
            newnum = newnum / 5;
            newnum = newnum + 32*16;
            unsigned int decimal;
            decimal = newnum - ((newnum>>4)<<4);
            decimal = decimal * 10/16;
            //add 32
            //back to decimal round
            but[6] = '\0';
            snprintf(but, 8, "%d.%d ", newnum/16, decimal);
            if (newnum<1600) {
                lcd_moveto(0,0);
                lcd_stringout(but);
            }
            
            /*
              Process whether temp or note and use encoder
            */
            //testing
//            char buf[4];
//            buf[3] = '\0';
            encoder();
//            lcd_moveto(1,6);
//            lcd_stringout("   ");
//            snprintf(buf,4, "%d", freq);
//            lcd_moveto(1,6);
//            lcd_stringout(buf);
            
            char temperature[4];
            temperature[3] = '\0';
            snprintf(temperature,4, "%d", temp);
            lcd_moveto(1,0);
            lcd_stringout(temperature);
            
            char buff[5];
            buff[4]= '\0';
            snprintf(buff,4, "%s", note); //problem is here
            lcd_moveto(0,13);
            lcd_stringout(buff);
            
            /*printing if hot warm or cool*/
            int difference = newnum -temp*16;
            if (difference>=640) {
                //nothing
                lcd_moveto(0,6);
                lcd_stringout("LOAD");
            }
            else if (difference>=32) {
                state = 2;
                if (paststate!=2 || alarm_buzzer==1) {
                    alarm_buzzer = 1;
                }
                else {
                    alarm_buzzer = 0;
                }
                PORTD &= ~(1<< PD2);
                lcd_moveto(0,6);
                lcd_stringout("HOT ");
                paststate = 2;
            }
            else if (difference>=0) {
                alarm_buzzer = 0;
                state = 1;
                PORTD &= ~(1<< PD2);
                lcd_moveto(0,6);
                lcd_stringout("WARM");
                paststate = 1;
            }
            else {
                alarm_buzzer = 0;
                state = 0;
                PORTD |= (1<< PD2);
                PORTD &= ~(1<< PD3);
                lcd_moveto(0,6);
                lcd_stringout("COOL");
                paststate = 0;
            }
            ds_convert();   // Start next conversion
        }
    }
}
