#include <avr/io.h>

#include "adc.h"

void adc_init(void)
{
    // Initialize the ADC
    //Set/clear the REFS[1:0] bits in ADMUX to select the high voltage reference. (01)
    ADMUX&=~(1<<REFS1);
    ADMUX|=(1<<REFS0);
    //Set the ADLAR bit in ADMUX such that we will use 8-bit conversion results (not 10-bit). (set to 1)
    ADMUX|=(1<<ADLAR);
    //Set/clear the ADPS[2:0] bits in ADCSRA to select an appropriate prescalar value.
    ADCSRA|=((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));
    //Set the ADEN bit in ADCSRA to enable the ADC module.
    ADCSRA|=(1<<ADEN);
    //The ADSC bit (shown below in red) will be set to a one later in your code, but in the initialization of the registers is should be a zero.
    ADCSRA &= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN));
}

unsigned char adc_sample(unsigned char channel)
{
    // copy channel1 into mux bits
    //clear admux
    ADMUX&=~0x07;
    if (channel==1) {
        //set channel 1 for ADMUX mux
        ADMUX|=0x01;
    }
    else if (channel==2) {
        //set channel 1 for ADMUX mux
        ADMUX|=0x02;
    }
    else if (channel==3) {
        ADMUX|=0x03;
    }
    else if (channel==4) {
        ADMUX|=0x04;
    }
    else if (channel==5){
        ADMUX|=0x05;
    }
    else {
    }
    
    // Set ADC input mux bits to 'channel' value
    //ADMUX|=temp;
    // Convert an analog input and return the 8-bit result
    ADCSRA|=(1<<ADSC);
    while ((ADCSRA&(1<<ADSC))!=0) {
        continue;
    }
    unsigned char result = ADCH;
    return result;
}
