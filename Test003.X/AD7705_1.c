#include "AD7705_1.h"

//initialization routine 
void adc_init() 
{ 
	ADC_CS_LOW;
	//_delay_us(1);
    ADC_RST_HIGH;
	//output_low(ADC_RESET);    // Reset all ADC registers to their default state. 
	ADC_RST_LOW;
	//_delay_ms(10); 
	//output_high(ADC_RESET);
	ADC_RST_HIGH;
	//_delay_us(1); 

	// output_high(ADC_CLK);     // CLK line at high state 
	ADC_CLK_HIGH; 
	//_delay_us(10); 
	//output_high(ADC_CS);      // CS line at high state 
	ADC_CS_HIGH;
   
   setup_adc_device(ADC_MASTERCLK_ENABLE,ADC_CLKDIV_ON,ADC_20, 
                    ADC_NORMAL,ADC_GAIN_32,ADC_BIPOLAR,ADC_BUFFER_OFF,ADC_FSYNC_START); 
	//_delay_ms(1); 
} 

//setup the device parameters 
void setup_adc_device(int masterclk, int clkdiv, int rate, 
int mode, int gain, int polar, int buffer, int fsync) 
{ 
   char temp1, temp2; 

   write_adc_byte( 0x20 ); //Communications Register set to write of Clock Register 
   temp1 = masterclk | clkdiv | rate; 
   write_adc_byte( temp1 ); //Clock Register info here 

   write_adc_byte( 0x10 ); //Communications Register set to write of Setup Register 
   temp2 = mode | gain | polar | buffer | fsync; 
   write_adc_byte( temp2 ); //Setup Register info here 
} 

void write_adc_byte(char data) 
{ 
   int i; 

   //output_low(ADC_CS); 
   ADC_CS_LOW;
   // output_low(ADC_CLK); 
   ADC_CLK_LOW;	
   // data in low
   PORTC &= ~0x01;
   
   //_delay_us(1);
   
   for(i=0; i<8; i++) 
   { 
	  ADC_CLK_LOW;

	  if ((data & 0x80) == 0) PORTC &= ~0x01;
	  else PORTC |= 0x01;
	  //_delay_us(1);
	  
	  ADC_CLK_HIGH;
 	  
	  data <<= 1;
   } 
  // output_high(ADC_CS); 
	ADC_CS_HIGH;
} 

long read_adc_word() 
{ 
   int i; 
   int data = 0; 

   //output_low(ADC_CS);
   ADC_CS_LOW;
   //_delay_us(10);
   
   for(i=0; i<16; i++) 
   { 
	   data <<= 1;
      // output_low(ADC_CLK); 
	  ADC_CLK_LOW; 
      //_delay_us(10);
      // output_high(ADC_CLK); 
	  ADC_CLK_HIGH;
	  //_delay_us(1);
	  
      // shift_left(&data,2,input(ADC_DO)); 
	  if (PORTC & 0x08) data |= 0x01;
   } 
   //output_high(ADC_CS); 
   ADC_CS_HIGH;
   
   return data; 
} 

// read an adc value from the specified channel 
// ch = 0 or 1 
long read_adc_value(int ch) 
{ 
	char ad_chk_tmr = 0;
	
	long value; 	
	// Loop until data is ready
	ad_chk_tmr = 0;
	while(!(PORTC & 0x4))
	{
		//_delay_ms(1);
		//if (ad_chk_tmr++ > 100) return 0xffff;		
	}
	ad_chk_tmr = 0;
	while(PORTC & 0x4)
	{
	//	_delay_us(50);
	//	if (ad_chk_tmr++ > 100) return 0xffff;		
	}
   
	if(ch)  // ch=1 (AIN2+, AIN2-) 
		write_adc_byte(0x39);   //communications register set to read of data 
	else    // ch=0 (AIN1+, AIN1-) 
		 write_adc_byte(0x38);   //communications register set to read of data 

	value = read_adc_word(); 
    return value; 
} 
