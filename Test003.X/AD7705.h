///////////////////////////////////////////////////////////////////////////// 
////                              AD7705.C                               //// 
////                     Driver for analog device AD7705                 //// 
////                                                                     //// 
//// adc_init()                    Call after power up                   //// 
////                                                                     //// 
//// read_adc_value(channel)Read adc value from the specified channel    //// 
////                                                                     //// 
//// adc_disable()    Disables the adc conversion                   //// 
///////////////////////////////////////////////////////////////////////////// 
////        (C) Copyright 1996,2003 Custom Computer Services             //// 
//// This source code may only be used by licensed users of the CCS C    //// 
//// compiler.  This source code may only be distributed to other        //// 
//// licensed users of the CCS C compiler.  No other use, reproduction   //// 
//// or distribution is permitted without written permission.            //// 
//// Derivative programs created using this software in object code      //// 
//// form are not restricted in any way.                                 //// 
///////////////////////////////////////////////////////////////////////////// 
//// Driver routines for the AD7705 chip 
//Assuming a 2.4576 crystal ocsillator is used between MCLK IN and MCLK OUT 

// corrected P Breuil, Ecole des Mines saint-Etienne France 

#ifndef AD7705_H
#define	AD7705_H

#include <xc.h>

//connection pins to the PIC 
#define ADC_DRDY  PORTCbits.RC2 
#define ADC_DO    PORTCbits.RC3 
#define ADC_DI    PORTCbits.RC4  
#define ADC_RESET PORTCbits.RC8 
#define ADC_CS    PORTCbits.RC1 
#define ADC_CLK   PORTCbits.RC5 
//---------------------------------------------------

//Operation modes 
#define ADC_NORMAL 0x00 
#define ADC_SELF 0x40 
#define ADC_ZERO_SCALE 0x80 
#define ADC_FULL_SCALE 0xc0 

//Gain settings 
#define ADC_GAIN_1 0x00 
#define ADC_GAIN_2 0x08 
#define ADC_GAIN_4 0x10 
#define ADC_GAIN_8 0x18 
#define ADC_GAIN_16 0x20 
#define ADC_GAIN_32 0x28 
#define ADC_GAIN_64 0x30 
#define ADC_GAIN_128 0x38 

//Polar operations 
#define ADC_BIPOLAR 0x04 
#define ADC_UNIPOLAR 0x00 

//Update rates (CLK, FS1, FS0 bits in Clock Register) 
#define ADC_20          0x00 
#define ADC_25          0x01 
#define ADC_100         0x02 
#define ADC_200         0x03 
#define ADC_50          0x04 
#define ADC_60          0x05 
#define ADC_250         0x06 
#define ADC_500         0x07 

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

//Custom Define
#define HIGH  1;
#define LOW  0;

void adc_init();
void write_adc_byte(char data);
void setup_adc_device(int masterclk, int clkdiv, int rate, int mode, int gain, int polar, int buffer, int fsync);
unsigned long int read_adc_value(unsigned char ch);
void adc_disable();

#endif	/* AD7705_H */

