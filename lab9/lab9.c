/********************************************
 *
 *  Name: Chris Lim
 *  Email: chrislim@usc.edu
 *  Section: Wed2-3:20
 *  Assignment: Lab 9 - Serial Communications
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "lcd.h"
#include "adc.h"
#include "check9.h"

// Serial communications functions and variables
void serial_init(unsigned short);
void serial_stringout(char *);
void serial_txchar(char);

//variables
volatile unsigned char flag = 0;  // Flag for state change
volatile unsigned char count = 0;
volatile char buffer[17];

#define FOSC 16000000           // Clock frequency
#define BAUD 9600               // Baud rate used
#define MYUBRR (FOSC/16/BAUD-1) // Value for UBRR0 register

// ADC functions and variables

#define ADC_CHAN 0              // Buttons use ADC channel 0

char *messages[] = {
    "Hello           ",
    "How are you?    ",
    "Redekopp is goat",
    "0123456789012345",
    "Yes             ",
    "Uno             ",
    "GME to the moon ",
    "Doge Coin :)    ",
    "Which vaccine?  ",
    "Whats ur name?  ",
    // Add more messgages
};


int main(void) {

    // Initialize the LCD, ADC and serial modules
    lcd_init();  //lcd initialize
    adc_init();  //adc intialize
    serial_init(MYUBRR); //serial intialize
    
    // Initialize the baud rate checker
    check9_init();

    // Enable interrupts
    UCSR0B |= (1 << RXCIE0);    // Enable receiver interrupts

    sei();                      // Enable interrupts

    // Show the splash screen
    lcd_moveto(0,0);
    lcd_stringout("Chris Lim");
    lcd_moveto(1,0);
    lcd_stringout("Lab 9");
    _delay_ms(2000);
    lcd_writecommand(0x01);
    
    unsigned char counter = 0;
    char buf[17];
    buf[16] = '\0';
    lcd_moveto(0,0);
    snprintf(buf, 17, "%s", messages[counter]);
    lcd_stringout(buf);

    while (1) {                 // Loop forever

	// Get an ADC sample to read buttons
        unsigned char adc_result = adc_sample(ADC_CHAN);
        _delay_ms(100);
        
        // Up button pressed?
        if (adc_result>=40 && adc_result<=60) {
            while((adc_result>=40 && adc_result<=60)) {
                adc_result = adc_sample(ADC_CHAN);
            }
            counter++;
            lcd_moveto(0,0);
            snprintf(buf, 17, "%s", messages[counter%10]);
            lcd_stringout(buf);
        }

        // Down button pressed?
        else if (adc_result>=90 && adc_result<=110) {
            while((adc_result>=90 && adc_result<=110)) {
                adc_result = adc_sample(ADC_CHAN);
            }
            counter--;
            lcd_moveto(0,0);
            snprintf(buf, 17, "%s", messages[counter%10]);
            lcd_stringout(buf);
        }

        // Select button pressed?
        else if (adc_result>=195 && adc_result<=215) {
            while((adc_result>=195 && adc_result<=215)) {
                adc_result = adc_sample(ADC_CHAN);
            }
            serial_stringout(messages[counter%10]);
            
        }

        // Message received from remote device?
        if (flag) {
            lcd_moveto(1,0);
            buffer[16] = '\0';
            snprintf(buf, 17, "%s", buffer);
            lcd_stringout(buf);
            flag=0;
            
        }
    }
}

/* ----------------------------------------------------------------------- */

void serial_init(unsigned short ubrr_value)
{

    // Set up USART0 registers
    UBRR0 = ubrr_value;                    // Set baud rate
    UCSR0C = (3 << UCSZ00);               // Async., no parity,
                                          // 1 stop bit, 8 data bits
    UCSR0B |= (1 << TXEN0 | 1 << RXEN0);  // Enable RX and TX

    // Enable tri-state buffer
    DDRD |= (1<<3);
    PORTD &= ~(1<<3);
}

void serial_txchar(char ch)
{
    while ((UCSR0A & (1<<UDRE0)) == 0);
    UDR0 = ch;
}

void serial_stringout(char *s)
{

    // Call serial_txchar in loop to send a string
    // (Very similar to lcd_stringout in lcd.c)
    int i = 0;
    while (s[i] != '\0') {    // Loop until next charater is NULL byte
        serial_txchar(s[i]);  // Send the character
        i++;
    }
}


ISR(USART_RX_vect)
{
    char ch;

    ch = UDR0;
// Handle received character
        // Store in buffer
    buffer[count] = ch;
    count++;
    // If message complete, set flag
    if (count==16) {
        count=0;
        flag=1;
        buffer[16]='\0';
    }
}
