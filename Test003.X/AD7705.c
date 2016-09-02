// AD7705.c 
// Driver routines for the AD7705 chip 
// Assuming a 2.4576 crystal ocsillator is used between MCLK IN and MCLK OUT 

//--- samhoi - flow meter circuit define...

#define ADC_RST_LOW		(PORTC &= ~0x0001)
#define ADC_RST_HIGH	(PORTC |= 0x0001)
#define ADC_CS_LOW		(PORTC &= ~0x0002)
#define ADC_CS_HIGH		(PORTC |= 0x0002)
#define ADC_CLK_LOW		(PORTC &= ~0x0020)
#define ADC_CLK_HIGH	(PORTC |= 0x0020)
//---------------------------------------------------

//Operation modes (MD1, MD0 bits in Setup Register) 
#define ADC_NORMAL 0x00 
#define ADC_SELF 0x40 
#define ADC_ZERO_SCALE 0x80 
#define ADC_FULL_SCALE 0xC0 

//Gain settings (G2, G1, G0 bits in Setup Register) 
#define ADC_GAIN_1      0x00 
#define ADC_GAIN_2      0x08 
#define ADC_GAIN_4      0x10 
#define ADC_GAIN_8      0x18 
#define ADC_GAIN_16     0x20 
#define ADC_GAIN_32     0x28 
#define ADC_GAIN_64     0x30 
#define ADC_GAIN_128    0x38 

//Polar operations (B/U bit in Setup Register) 
#define ADC_BIPOLAR     0x00    
#define ADC_UNIPOLAR    0x04    

//Buffer control (BUF bit in Setup Register) 
#define ADC_BUFFER_ON   0x02    
#define ADC_BUFFER_OFF  0x00    

//Fsync (FSYNC bit in Setup Register) 
#define ADC_FSYNC_RESET 0x01 
#define ADC_FSYNC_START 0x00 

//Master clock (CLKDIS bit in Clock Register) 
#define ADC_MASTERCLK_ENABLE  0x00 
#define ADC_MASTERCLK_DISABLE 0x10 

//Clock divider (CLKDIV bit in Clock Register) 
#define ADC_CLKDIV_ON  0x08 
#define ADC_CLKDIV_OFF 0x00 

//Update rates (CLK, FS1, FS0 bits in Clock Register) 
#define ADC_20          0x00 
#define ADC_25          0x01 
#define ADC_100         0x02 
#define ADC_200         0x03 
#define ADC_50          0x04 
#define ADC_60          0x05 
#define ADC_250         0x06 
#define ADC_500         0x07 

// Function prototypes 
void adc_init(); 
void setup_adc_device(int masterclk, int clkdiv, int rate, int mode, int gain, int polar, int buffer, int fsync); 
void write_adc_byte(char data); 
long read_adc_word(); 
long read_adc_value(int ch); 

//initialization routine 
void adc_init() 
{ 
	ADC_CS_LOW;
	Delay_us(1);
    ADC_RST_HIGH;
	//output_low(ADC_RESET);    // Reset all ADC registers to their default state. 
	ADC_RST_LOW;
	Delay_ms(10); 
	//output_high(ADC_RESET);
	ADC_RST_HIGH;
	Delay_us(1); 

	// output_high(ADC_CLK);     // CLK line at high state 
	ADC_CLK_HIGH; 
	Delay_us(10); 
	//output_high(ADC_CS);      // CS line at high state 
	ADC_CS_HIGH;
   
   setup_adc_device(ADC_MASTERCLK_ENABLE,ADC_CLKDIV_ON,ADC_20, 
                    ADC_NORMAL,ADC_GAIN_32,ADC_BIPOLAR,ADC_BUFFER_OFF,ADC_FSYNC_START); 
	Delay_ms(1); 
} 

//setup the device parameters 
void setup_adc_device(int masterclk, int clkdiv, int rate, int mode, int gain, int polar, int buffer, int fsync) 
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
   
   Delay_us(1);
   
   for(i=0; i<8; i++) 
   { 
	  ADC_CLK_LOW;

	  if ((data & 0x80) == 0) PORTC &= ~0x0010;
	  else PORTC |= 0x0010;
	  Delay_us(1);
	  
	  ADC_CLK_HIGH;
 	  
	  data <<= 1;
   } 
  // output_high(ADC_CS); 
	ADC_CS_HIGH;
} 

long read_adc_word() 
{ 
   int i; 
   unsigned int data = 0; 

   //output_low(ADC_CS);
   ADC_CS_LOW;
   Delay_us(10);
   
   for(i=0; i<16; i++) 
   { 
	   data <<= 1;
      // output_low(ADC_CLK); 
	  ADC_CLK_LOW; 
      Delay_us(10);
      // output_high(ADC_CLK); 
	  ADC_CLK_HIGH;
	  Delay_us(1);
	  
      // shift_left(&data,2,input(ADC_DO)); 
	  if (PORTC & 0x02) data |= 0x01;
   } 
   //output_high(ADC_CS); 
   ADC_CS_HIGH;
   
   return data; 
} 

// read an adc value from the specified channel 
// ch = 0 or 1 
long read_adc_value(int ch) 
{ 
	unsigned char ad_chk_tmr = 0;
	
	long value; 	
	// Loop until data is ready
	ad_chk_tmr = 0;
	while(!(PORTD & 0x80))
	{
		//_delay_ms(1);
		//if (ad_chk_tmr++ > 100) return 0xffff;		
	}
	ad_chk_tmr = 0;
	while(PORTD & 0x80)
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
