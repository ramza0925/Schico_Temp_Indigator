#include "AD7705.h"

void adc_init() { 
    ADC_RESET = 0;
    ADC_CLK = 1;
    ADC_CS = 1;
    ADC_RESET = 1;
    ADC_CS = 0;
    
    setup_adc_device(ADC_MASTERCLK_ENABLE,ADC_CLKDIV_ON,ADC_20, ADC_NORMAL,ADC_GAIN_8,ADC_BIPOLAR,ADC_BUFFER_OFF,ADC_FSYNC_START);
} 

//setup the device paramaters(mode, gainsetting, polar operation and output rate) 
void setup_adc_device(int masterclk, int clkdiv, int rate, int mode, int gain, int polar, int buffer, int fsync) { 
   char temp1, temp2; 

   write_adc_byte(0x20); //Communications Register set to write of Clock Register 
   temp1 = masterclk | clkdiv | rate; 
   write_adc_byte(temp1); //Clock Register info here 

   write_adc_byte(0x10); //Communications Register set to write of Setup Register 
   temp2 = mode | gain | polar | buffer | fsync; 
   write_adc_byte(temp2); //Setup Register info here 
} 

//initialization routine 
void write_adc_byte(char data){ 
   char i;
   
   ADC_CS = 0;
   for(i=1;i<=8;++i) { 
       ADC_CLK = 0;
       if ((data & 0x80) == 0) ADC_DI = 0;
       else ADC_DI = 1;
       ADC_CLK = 1;
       data <<= 1;
   } 
   ADC_CS = HIGH; 
} 

long int read_adc_word(){ 
   unsigned char i; 
   long data; 

   ADC_CS = 0; 
   for(i=1;i<=16;++i) { 
       data <<= 1;
       ADC_CLK = 0;
       ADC_CLK = 1;  
       if (ADC_DO) data |= 0x01;
   } 
   ADC_CS = 1; 
   return data; 
} 

//read an adc  value from the specified channel 
unsigned long int read_adc_value(unsigned char ch){
    long int value; 
    while(ADC_DRDY);
    
    if(ch)
        write_adc_byte(0x39);//communications register set to read of data ch1    
    else  
        write_adc_byte(0x38);//communications register set to read of data 
    
    value = read_adc_word();  
    
    while (!ADC_DRDY); // but this line has no utility! 

    return value; 
}

//disable the a/d conversion 
void adc_disable(){ 
    write_adc_byte(0x20);//Communications Register set to write of clock register 
    write_adc_byte(0x10);//Clock Register info here 
}

//Make Delay us
void delay_10us(unsigned char _dcnt) {
    register unsigned char i;
    for(i = 0; i<_dcnt; i++){   //4cycle
        Nop();                  //2cycle
        Nop();                  //2cycle
        //Nop();
    }
    
}

//Make Delay ms
void delay_ms(unsigned int cnt){
    do{
        delay_10us(100);
    }while(--cnt);
}