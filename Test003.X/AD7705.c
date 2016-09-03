#include "AD7705.h"


void adc_init() { 
   ADC_RESET = LOW; 
   ADC_CLK = HIGH;
   ADC_CS = HIGH;
   ADC_RESET = HIGH;
   setup_adc_device(ADC_MASTERCLK_ENABLE,ADC_CLKDIV_ON,ADC_20, ADC_NORMAL,ADC_GAIN_32,ADC_BIPOLAR,ADC_BUFFER_OFF,ADC_FSYNC_START); 
} 

//setup the device paramaters(mode, gainsetting, polar operation and output rate) 
void setup_adc_device(int masterclk, int clkdiv, int rate, int mode, int gain, int polar, int buffer, int fsync) { 
   char temp1, temp2; 

   write_adc_byte( 0x20 ); //Communications Register set to write of Clock Register 
   temp1 = masterclk | clkdiv | rate; 
   write_adc_byte( temp1 ); //Clock Register info here 

   write_adc_byte( 0x10 ); //Communications Register set to write of Setup Register 
   temp2 = mode | gain | polar | buffer | fsync; 
   write_adc_byte( temp2 ); //Setup Register info here 
} 

//initialization routine 
void write_adc_byte(char data){ 
   char i;
   
   ADC_CS = LOW;
   for(i=1;i<=8;++i) { 
       ADC_CLK = LOW;
       if ((data & 0x80) == 0) ADC_DI = 1;
       else ADC_DI = 0;
       ADC_CLK = HIGH;
       data <<= 1;
   } 
   ADC_CS = HIGH; 
} 

long int read_adc_word(){ 
   unsigned char i; 
   long data; 

   ADC_CS = LOW; 
   for(i=1;i<=16;++i) { 
      ADC_CLK = LOW; 
      ADC_CLK = HIGH; 
      // TODO shift_left(&data,2,input(ADC_DO)); 
      if (ADC_DO) data |= 0x01;
   } 
   ADC_CS = HIGH; 
   return data; 
} 

//read an adc  value from the specified channel 
unsigned long int read_adc_value(unsigned char ch){
    long int value; 
    while(!ADC_DRDY);
    
    if(ch) 
        // write_adc_byte(0x38);//communications register set to read of data register of channel 1  False! 
        write_adc_byte(0x39);//communications register set to read of data ch1    
    else 
        // write_adc_byte(0x39);//communications register set to read of data register of channel 0  false! 
        write_adc_byte(0x38);//communications register set to read of data 
    value=read_adc_word();  
    
    while (ADC_DRDY); // but this line has no utility! 
    
    return value; 
}

//disable the a/d conversion 
void adc_disable(){ 
    write_adc_byte(0x20);//Communications Register set to write of clock register 
    write_adc_byte(0x10);//Clock Register info here 
}