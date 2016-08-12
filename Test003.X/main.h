/* 
 * File:   main.h
 * Author: Jeong Kyuwon
 *
 * Created on 2016? 8? 3? (?), PM 6:43
 */

#ifndef MAIN_H
#define	MAIN_H

// PIC24F32KA304 Configuration Bit Settings

// 'C' source line config statements

// FBS
#pragma config BWRP = OFF               // Boot Segment Write Protect (Disabled)
#pragma config BSS = OFF                // Boot segment Protect (No boot program flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Write Protect (General segment may be written)
#pragma config GSS0 = OFF               // General Segment Code Protect (No Protection)

// FOSCSEL
#pragma config FNOSC = PRI              // Oscillator Select (Primary Oscillator (XT, HS, EC))
#pragma config SOSCSRC = DIG            // SOSC Source Type (Digital Mode for use with external source)
#pragma config LPRCSEL = HP             // LPRC Oscillator Power and Accuracy (High Power, High Accuracy Mode)
#pragma config IESO = ON               // Internal External Switch Over bit (Internal External Switchover mode disabled (Two-speed Start-up disabled))

// FOSC
#pragma config POSCMOD = EC             // Primary Oscillator Configuration bits (External clock mode selected)
#pragma config OSCIOFNC = OFF           // CLKO Enable Configuration bit (CLKO output disabled)
#pragma config POSCFREQ = MS            // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency between 100kHz and 8MHz)
#pragma config SOSCSEL = SOSCHP         // SOSC Power Selection Configuration bits (Secondary Oscillator configured for high-power operation)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Both Clock Switching and Fail-safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPS = PS256            // Watchdog Timer Postscale Select bits (1:256)
#pragma config FWPSA = PR32             // WDT Prescaler bit (WDT prescaler ratio of 1:32)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bits (WDT disabled in hardware; SWDTEN bit disabled)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard WDT selected(windowed WDT disabled))

// FPOR
#pragma config BOREN = BOR0             // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware, SBOREN bit disabled)
#pragma config LVRCFG = OFF             // Low Voltage Regulator Configuration bit (Low Voltage regulator is not available)
#pragma config PWRTEN = ON              // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Use Default SCL1/SDA1 Pins For I2C1)
#pragma config BORV = V18               // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (1.8V))
#pragma config MCLRE = ON               // MCLR Pin Enable bit (RA5 input pin disabled,MCLR pin enabled)

// FICD
#pragma config ICS = PGx1               // ICD Pin Placement Select bits (EMUC/EMUD share PGC1/PGD1)

// FDS
#pragma config DSWDTPS = DSWDTPSF       // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses Low Power RC Oscillator (LPRC))
#pragma config DSBOREN = OFF            // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR disabled in Deep Sleep)
#pragma config DSWDTEN = OFF            // Deep Sleep Watchdog Timer Enable bit (DSWDT disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <math.h>

#define FOSC        2000000                     //OSCILLATOR VALUE
#define FCY         FOSC/2         
#define TCY         1/FCY



#define FND_AP1 0x00e0                          //1000 Digit
#define FND_AP2 0x00d0                          //100 Digit
#define FND_AP3 0x00b0                          //10 Digit
#define FND_AP4 0x0070                          //1 Dight

//FND Charcters
const int fnd_character[]=
    {
        0xc000,                                 //0
        0xf900,                                 //1
        0xa400,                                 //2
        0xb000,                                 //3
        0x9900,                                 //4
        0x9200,                                 //5
        0x8200,                                 //6
        0xd800,                                 //7
        0x8000,                                 //8
        0x9000,                                 //9
        0x8800,                                 //A
        0x8300,                                 //B
        0xa700,                                 //C
        0xa100,                                 //D
        0x8600,                                 //E
        0x8e00,                                 //F
        0xc200,                                 //G
        0x8900,                                 //H
        0xcf00,                                 //I
        0xe100,                                 //J
        0x8500,                                 //K
        0xc700,                                 //L
        0xaa00,                                 //M
        0xab00,                                 //N
        0xa300,                                 //O
        0x8c00,                                 //P
        0x9800,                                 //Q
        0xaf00,                                 //R
        0x9200,                                 //S
        0x8700,                                 //T
        0xe300,                                 //U
        0x8100,                                 //V
        0x9500,                                 //W
        0xc900,                                 //X
        0x9100,                                 //Y
        0xb600,                                 //Z
    };

const int DOT = 0x7fff;                         //FND Dot

const float UNIT = 3.3f/4096;                   //Volt per Unit in ADC
const float GAIN = 1+20000/3000;                //ADC GAIN
const float CURRENT = 0.001f;                   //PT100ohm Current

//Functions
void Delay_us(unsigned char _dcnt);             //Make Delay micro seconds
void Delay_ms(unsigned int cnt);                //Make Delay milli seconds
void Init();                                    //Initialize
void OSC_Init();                                //Oscillator Intialize
void Port_Init();                               //Port Initialize
void Timer_Init();                              //Timer Initialize
void ADC_Init();                                //ADC Initialize
void Data_Init();                               //Data EEPROM Load
void Button_Check();                            //TACT Switch Check
void Temp_Check();                              //PT100 Temperature Sensor Check
void Current_Check();                           //Consumption Current Check
void Current_Control();                         //Consumption Current Control

int Solve_Rational_Poly_Equation(float rt);   //Temperature Calculator with PT100

#endif	/* MAIN_H */