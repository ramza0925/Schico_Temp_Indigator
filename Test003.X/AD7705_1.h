/* 
 * File:   AD7705_1.h
 * Author: ??
 *
 * Created on 2016? 9? 7? (?), ?? 11:45
 */

#ifndef AD7705_1_H
#define	AD7705_1_H

#include <xc.h>

// AD7705.c 
//// Driver routines for the AD7705 chip 
//Assuming a 2.4576 crystal ocsillator is used between MCLK IN and MCLK OUT 

//--- samhoi - flow meter circuit define...

#define ADC_RST_LOW		(PORTC &= ~0x100)
#define ADC_RST_HIGH	(PORTC |= 0x100)
#define ADC_CS_LOW		(PORTC &= ~0x2)
#define ADC_CS_HIGH		(PORTC |= 0x2)
#define ADC_CLK_LOW		(PORTC &= ~0x20)
#define ADC_CLK_HIGH	(PORTC |= 0x20)
//---------------------------------------------------

//connection pins to the PIC 
#define ADC_DRDY  PORTCbits.RC2 
#define ADC_DO    PORTCbits.RC3 
#define ADC_DI    PORTCbits.RC4  
#define ADC_RESET PORTCbits.RC8 
#define ADC_CS    PORTCbits.RC1 
#define ADC_CLK   PORTCbits.RC5 

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
void setup_adc_device(int masterclk, int clkdiv, int rate, 
	int mode, int gain, int polar, int buffer, int fsync); 
void write_adc_byte(char data); 
long read_adc_word(); 
long read_adc_value(int ch); 

#endif	/* AD7705_1_H */

