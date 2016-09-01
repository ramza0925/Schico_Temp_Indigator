/*
 * File:   main.c
 * Author: Jeong Kyuwon
 *
 * Created on 2016 8 3
 */

#include "main.h"
                            
volatile int tmp_Value ;                            //Temperature Value
//float curr_Value;                         //current Value
int correction_Value; 

unsigned char SW1cnt, SW2cnt, SW3cnt, SW4cnt;


//TIMER1 Interrupt
void __attribute__((__interrupt__, __shadow__ ,auto_psv)) _T1Interrupt(void){
    int num = tmp_Value;
    
    if(num < 0) {
        num = num * -1;
        PORTB = MINUS | FND_AP1;
        Delay_ms(4);
        PORTB = 0xfff0;
    }
    if(num>=1000){
        PORTB = fnd_character[num/1000] | FND_AP1;
        Delay_ms(4);
        PORTB = 0xfff0;
    }
    if(num>=100){
        PORTB = fnd_character[(num%1000)/100] | FND_AP2;
        Delay_ms(4);
        PORTB = 0xfff0;
    }
    PORTB = (fnd_character[(num%100)/10] | FND_AP3) & DOT;
    Delay_ms(4);
    PORTB = 0xfff0;
    PORTB = fnd_character[num%10] | FND_AP4;
    Delay_ms(4);
    PORTB = 0xfff0;
    Delay_ms(1);
    
    IFS0bits.T1IF = 0;              //Reset Timer1 interrupt flag and Return from ISR
}


/*****************************************************************************
 * MAIN
 *****************************************************************************/
int main(void) {
    tmp_Value = 0;
    Init();
    myMode = NORMAL_VIEW;
    
    while(1){
        Button_Check();     //TBD
        Temp_Check();
        //Current_Check();
        //Current_Control();
    }
    return 0;
}

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
    Data_Init();
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
    TRISA = 0x0F0F;                 //0b0000111100001111
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
    PR1             = 0x8888;       //Load the Period register with the value 0xffff
    IPC0bits.T1IP   = 0x07;         //Setup Timer1 interrupt for desired priority level(1)
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
    AD1CON3bits.ADCS    = 63;       //A/D Conversion Time Select bits(64*Tcy 0x3F)
    AD1CON3bits.SAMC    = 0;        //Auto-Sample Time Select bits
    AD1CON3bits.ADRC    = 0;        //A/D Conversion Clock Source bit(System Clock)
    
    //AD1CHS0 Register Setting
    AD1CHS              = 0;        //A/D Sample Select Register Setting(default)
    
    //AD1CSSL Register Setting
    AD1CSSL             = 0x0003;   //A/D Input Selection Bits(AN0, AN1)
    
    //ADC1 Convert Start
    AD1CON1bits.ADON    = 1;        //ADC Covert Start
}

void Data_Init(){
    
}

//Button Check
void Button_Check(){
    if(SW1 == 0) SW1cnt++;
    if(SW2 == 0) SW2cnt++;
    if(SW3 == 0) SW3cnt++;
    if(SW4 == 0) SW4cnt++;
    
    if(SW1cnt > 5){
        SW1cnt = 0;
        myMode = ADC_VIEW;
    }
    if(SW2cnt > 5){
        SW2cnt = 0;
        myMode = RRTD_VIEW;
    }
    if(SW3cnt > 5){
        SW3cnt = 0;
        myMode = NORMAL_VIEW;
    }
    if(SW4cnt > 5){
        SW4cnt = 0;
        myMode = VOUT_VIEW;
    }
}

//Temperature Check
void Temp_Check(){
    const char loopCountA = 12;
    const char loopCountB = 12;
    
    int loopA, loopB;                               //Loop Variable
    float temp, vout, rt, maxV, minV, avgValue;           
    float temp_arrayB[loopCountB];
    float temp_arrayA[loopCountA];
    
    //Channel Selection
    AD1CHS = 0x0001;
    Delay_us(50);
    
    avgValue = 0;
    for(loopA = 0 ; loopA < loopCountA ; loopA++){
        temp = 0;
        for(loopB = 0 ; loopB < loopCountB ; loopB++){
            while(!AD1CON1bits.DONE);
            temp_arrayB[loopB] = ADC1BUF1;
            temp += temp_arrayB[loopB];
            Delay_us(5);
        }
        
        maxV = temp_arrayB[0];
        minV = temp_arrayB[0];
        for(loopB = 0; loopB < loopCountB ; loopB++){
            if(maxV<temp_arrayB[loopB]) {
                maxV = temp_arrayB[loopB];
            }
            if(minV>temp_arrayB[loopB]) {
                minV = temp_arrayB[loopB];
            }
        }
        //?? ? ??
        temp_arrayA[loopA] = (temp-maxV-minV)/(loopCountB-2);
        avgValue += temp_arrayA[loopA];
    }
    
    maxV = temp_arrayA[0];
    minV = temp_arrayA[0];
    for(loopA = 0 ; loopA < loopCountA ; loopA++){
        if(maxV<temp_arrayA[loopA]) {
            maxV = temp_arrayA[loopA];
        }
        if(minV>temp_arrayA[loopA]) {
            minV = temp_arrayA[loopA];
        }
    }
    avgValue = (avgValue-maxV-minV)/(loopCountA-2);
    avgValue = avgValue+(0.00004f*pow(avgValue,2)+0.0612f*avgValue+5.2644f);
    
    vout = avgValue * UNIT;
    rt = vout/CURRENT;
    //rt = 10000.0f*(vout/(3.3f-vout));
    
    switch(myMode){
        case ADC_VIEW:
            tmp_Value = avgValue;
            break;
        case RRTD_VIEW:
            tmp_Value = rt;
            break;
        case NORMAL_VIEW:
            tmp_Value = Solve_Rational_Poly_Equation(rt);
            break;
        case VOUT_VIEW:
            tmp_Value = vout*10;
            break;
    }
    //Delay_ms(700);
    
}

//Current Check
void Current_Check(){
   //TODO
}

//Current Controll
void Current_Control(){
   //TODO
}

int Solve_Rational_Poly_Equation(float rt){
    float result = 0;
    
    const float C0 = -245.19f;
    const float C1 = 2.5293f;
    const float C2 = -0.066046f;
    const float C3 = 4.0422f * pow(10,-3);
    const float C4 = -2.0697f * pow(10,-6);
    const float C5 = -0.025422f;
    const float C6 = 1.6883f * pow(10,-3);
    const float C7 = -1.3601f * pow(10,-6);
    
    result = C0 + ((C1*rt+C2*pow(rt,2)+C3*pow(rt,3)+C4*pow(rt,4))/(1+C5*rt+C6*pow(rt,2)+C7*pow(rt,3)));
    
    return (int)(result*10)%10000;
}

unsigned int eeRead(unsigned int address){
    while(NVMCONbits.WR);       // wait for any last write to complete
    TBLPAG = 0x7f;              // eprom is a t 0x7ffe00
    return __builtin_tblrdl(0xFE00+address);
}
 
void eeWrite(unsigned int address, unsigned int data){
    while(NVMCONbits.WR);       // wait for last write to complete
    NVMCON = 0x4058;            // Set up NVMCON to erase one word of data EEPROM
    TBLPAG = 0x7f;              // eprom is a t 0x7ffe00
    __builtin_tblwtl(0xFE00+address, 0xffff);
    asm volatile ("disi #5");
    __builtin_write_NVM();       // Issue Unlock Sequence & Start Write Cycle 
 
    while(NVMCONbits.WR);       // wait for errase to complete
    NVMCON = 0x4004;            // Set up NVMCON to write one word of data EEPROM
    __builtin_tblwtl(0xFE00+address, data);   // Write EEPROM data to write latch
    asm volatile ("disi #5");
    __builtin_write_NVM();     
}
