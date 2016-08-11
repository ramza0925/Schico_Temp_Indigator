/*
 * File:   main.c
 * Author: Jeong Kyuwon
 *
 * Created on 2016 8 3
 */

#include "main.h"
                            
float tmp_Value;                            //Temperature Value
//float curr_Value;                         //current Value

void __attribute__((__interrupt__, __shadow__)) _T1Interrupt(void){
    int num = (int)tmp_Value;
    
    PORTB = fnd_character[num/1000] | FND_AP1;
    Delay_ms(7);
    PORTB = 0x00f0;
    PORTB = fnd_character[(num%1000)/100] | FND_AP2;
    Delay_ms(7);
    PORTB = 0x00f0;
    PORTB = (fnd_character[(num%100)/10] | FND_AP3) & DOT;
    Delay_ms(7);
    PORTB = 0x00f0;
    PORTB = fnd_character[num%10] | FND_AP4;
    Delay_ms(7);
    PORTB = 0x00f0;
    
    IFS0bits.T1IF = 0;              //Reset Timer1 interrupt flag and Return from ISR
}


/*****************************************************************************
 * MAIN
 *****************************************************************************/
int main(void) {
    
    Init();
    while(1){
        Button_Check();     //TBD
        Temp_Check();
        Current_Check();
        Current_Control();
        //FND_Display((int)tmp_Value);
    }
    return 0;
}
/******************************************************************************
 * End MAIN
 *****************************************************************************/


//Make Delay us
void Delay_us(unsigned char _dcnt) {
    while(--_dcnt !=0);
}

//Make Delay ms
void Delay_ms(unsigned int cnt){
    unsigned char i;
    do{
        i = 1;
        do{
            Delay_us(250);
        }while(--i);
    }while(--cnt);
}

//Initialize
void Init(){
    OSC_Init();
    Port_Init();
    Timer_Init();
    ADC_Init(); 
}

//OSCillator Initialize
void OSC_Init(){
    //COSC(12~14):  010
    //NOSC(8~10):   010
    //CLKLOCK(7):   1
    //LOCK(5):      0
    //CF(3):        0
    //SOSCDRV(2):   0
    //SOSCEN(1):    0
    //OSWEN(0):     0
    OSCCONbits.COSC     = 2;        //Primary OSC Use
    OSCCONbits.NOSC     = 2;        //Primary OSC Use
    OSCCONbits.CLKLOCK  = 1;        //Clock and PPL Selections are Lock
    
}

//Port Initalize
void Port_Init(){
    //PORT Initialize
    TRISA = 0x0F03;                 //0b111100000011
    PORTA = 0x0000;
    TRISB = 0x0000;
    PORTB = 0x0000;
    TRISC = 0x0000;
    PORTC = 0x0000;
}

//Timer1 Initerize
void Timer_Init(){
    T1CONbits.TSIDL = 0;            //Stop in Idle mode
    T1CONbits.TCS   = 0;            //Inner Clock use
    T1CONbits.TCKPS = 0;            //1:1 Prescale
    TMR1            = 0x0000;       //Clear contents of the timer register
    PR1             = 0xaaaa;       //Load the Period register with the value 0xffff
    IPC0bits.T1IP   = 0x01;         //Setup Timer1 interrupt for desired priority level(1)
    IFS0bits.T1IF   = 0;            //Clear the Timer1 interrupt status flag
    IEC0bits.T1IE   = 1;            //Enable Timer1 Intrrupts
    T1CONbits.TON   = 1;            //Timer1 Start
    
}

//ADC Initialize
void ADC_Init(){
    //AD1CON Register Setting
    AD1CON1bits.SAMP    = 0;        //ADC Sample Holding
    AD1CON1bits.ASAM    = 1;        //ADC sampling start immeditely convert
    AD1CON1bits.SSRC    = 7;        //Sample Clock(Internal counter ends sampleing and starts conversion-auto-convert)
    AD1CON1bits.FORM    = 0;        //Data Output Format(Absolute decimal result, unsigned, reght-justified)
    AD1CON1bits.MODE12  = 1;        //Sampling Mode(12-bit A/D operation)
    
    //AD1CON2 Register Setting
    AD1CON2bits.BUFM    = 0;        //Buffer Fill Mode(ADCBUF0 Starts filling)
    AD1CON2bits.SMPI    = 7;        //Interrupt Sampel Rate Selecte bits(8th)
    AD1CON2bits.NVCFG   = 0;        //Converter Negative Voltage Reference Configuration bits(Vss)
    AD1CON2bits.PVCFG   = 0;        //Coverter Positive Voltage Reference Configuration bits(Vdd)
    
    //AD1CON3 register Setting
    AD1CON3bits.ADCS    = 63;        //A/D Conversion Time Select bits(64*Tcy 0x3F)
    AD1CON3bits.SAMC    = 0;        //Auto-Sample Time Select bits
    AD1CON3bits.ADRC    = 0;        //A/D Conversion Clock Source bit(System Clock)
    
    //AD1CHS0 Register Setting
    AD1CHS              = 0;        //A/D Sample Select Register Setting(default)
    
    //AD1CSSL Register Setting
    AD1CSSL             = 0x0003;   //A/D Input Selection Bits(AN0, AN1)
    
    //ADC1 Convert Start
    AD1CON1bits.ADON    = 1;        //ADC Covert Start
}

//Button Check
void Button_Check(){
   //TODO TBD
}

//Temperature Check
void Temp_Check(){
    float temp, vout, rt;
    
    AD1CHS = 0x0001;
    Delay_us(10);
    temp = ADC1BUF1;
    vout = (temp * UNIT)/GAIN;
    rt = vout/CURRENT;
    tmp_Value = (int)((((rt/100)-1)/0.00385f)*10.0f)%10000;
    Delay_ms(300);
    
}

//Current Check
void Current_Check(){
   //TODO
}

//Current Controll
void Current_Control(){
   //TODO
}

//FND Display
//void FND_Display(int num){
//    PORTB = fnd_character[num/1000] | FND_AP1;
//    Delay_ms(1);
//    PORTB = 0x00f0;
//    PORTB = fnd_character[(num%1000)/100] | FND_AP2;
//    Delay_ms(1);
//    PORTB = 0x00f0;
//    PORTB = (fnd_character[(num%100)/10] | FND_AP3) & DOT;
//    Delay_ms(1);
//    PORTB = 0x00f0;
//    PORTB = fnd_character[num%10] | FND_AP4;
//    Delay_ms(1);
//    PORTB = 0x00f0; 
//}
