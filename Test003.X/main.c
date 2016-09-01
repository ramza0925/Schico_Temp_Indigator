/*
 * File:   main.c
 * Author: Jeong Kyuwon
 *
 * Created on 2016 8 3
 */

#include "main.h"
                            
volatile int tmp_Value = 0;                            //Temperature Value
volatile int rrtd_Value = 0;
volatile int adc_Value = 0;
char* message ="";
unsigned char length = 0;

float voltage_Total = 0.0f;

//float curr_Value;                                 //current Value

unsigned char minusFlag = 0;
unsigned int APs[4] = {FND_AP1, FND_AP2, FND_AP3, FND_AP4}; 

//TIMER1 Interrupt
void __attribute__((__interrupt__, __shadow__ ,auto_psv)) _T1Interrupt(void){
    if(myMode == NORMAL_VIEW && dpMode == NUM_MODE){
        FND_Number_Display(tmp_Value, 2);
    }
    else if(myMode == RRTD_VIEW && dpMode == NUM_MODE){
        FND_Number_Display(rrtd_Value,2);
    }
    else if(myMode == SETTING && dpMode == NUM_MODE){
        FND_Number_Display(adc_Value, 0);
    }
    else
        FND_String_Display(message, length);
    IFS0bits.T1IF = 0;              //Reset Timer1 interrupt flag and Return from ISR
}

/*****************************************************************************
 * MAIN
 *****************************************************************************/
int main(void) {
    voltage_Total = 3.3f;
    myMode = NORMAL_VIEW;
    dpMode = NUM_MODE;
    Init();
    
    while(1){
        Button_Check();     
        Temp_Check();
    }
    return 0;
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

//EEPROM Data Load
void Data_Init(){
    
}

//Button Check
void Button_Check(){
    if(MODE == 0){
        Delay_ms(100);
        switch(myMode){
            case NORMAL_VIEW:
                dpMode = STR_MODE;
                Set_Message("SET",3);
                Delay_ms(1000);
                myMode = SETTING;
                dpMode = NUM_MODE;
                break;
            case SETTING:
                dpMode = STR_MODE;
                Set_Message("NOT",3);
                Delay_ms(500);
                Set_Message("SAVE",4);
                Delay_ms(1000);
                Set_Message("RES",3);
                Delay_ms(1000);
                myMode = RRTD_VIEW;
                dpMode = NUM_MODE;
                break;
            case RRTD_VIEW:
                dpMode = STR_MODE;
                Set_Message("NOR",3);
                Delay_ms(1000);
                myMode = NORMAL_VIEW;
                dpMode = NUM_MODE;
                break;
        }
    }
    if(ENTER == 0){
        switch(myMode){
            case NORMAL_VIEW:
                break;
            case SETTING:
                voltage_Total = (100.0f*RESOLUTION*CURRENT*GAIN)/Get_ADC(0x0001, ADC1BUF1);
                dpMode = STR_MODE;
                Set_Message("SAVE",4);
                Delay_ms(500);
                Set_Message("OK",2);
                Delay_ms(1000);
                Set_Message("RES",3);
                Delay_ms(1000);
                myMode = RRTD_VIEW;
                dpMode = NUM_MODE;
                break;
            case RRTD_VIEW:
                break;
        }
    }
}

//Temperature Check
void Temp_Check(){
    float vout, rt, adc;
    
    adc = Get_ADC(0x0001, ADC1BUF1);
    adc_Value = adc_Value+(0.00004f*pow(adc_Value,2)+0.0612f*adc_Value+5.2644f);
    
    vout = (adc * voltage_Total/RESOLUTION)/GAIN;
    
    rt = vout/CURRENT;
    
    switch(myMode){
        case RRTD_VIEW:
            rrtd_Value = (int)(rt*10)%10000;
            break;
        case NORMAL_VIEW:
            tmp_Value = Solve_Rational_Poly_Equation(rt);
            break;
        case SETTING:
            adc_Value = (int)adc%10000;
            break;
    }
    Delay_ms(300);
    
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

//Calculate Temperature
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
    
    if(result > 0) minusFlag = 0;
    else minusFlag = 1;
    
    return (int)(result*10)%10000;
}

//FND Display Characteristic
//Characters will only enter capital letters.
void FND_String_Display(char* string, unsigned char length){
    unsigned char i;
    
    for(i=0; i < length; i++){
        if(string[length-1-i] > 64)
            PORTB = fnd_character[string[length-1-i]-55] | APs[3-i];
        else
            PORTB = fnd_character[string[length-1-i]-48] | APs[3-i];
        Delay_ms(4);
        PORTB = 0xfff0;
    }
    Delay_ms(1);
}

void FND_Number_Display(int num, int dot){    
    if(num < 0) {
        num = num * -1;
        PORTB = MINUS | FND_AP1;
        Delay_ms(4);
        PORTB = 0xfff0;
    }
    if(num>=1000){
        if(dot == 4)
            PORTB = (fnd_character[num/1000] | FND_AP1) & DOT;
        else
            PORTB = fnd_character[num/1000] | FND_AP1;
        Delay_ms(4);
        PORTB = 0xfff0;
    }
    if(num>=100){
        if(dot == 3)
            PORTB = (fnd_character[(num%1000)/100] | FND_AP2) & DOT;
        else
            PORTB = fnd_character[(num%1000)/100] | FND_AP2;
        Delay_ms(4);
        PORTB = 0xfff0;
    }
    if(dot ==2 )
        PORTB = (fnd_character[(num%100)/10] | FND_AP3) & DOT;
    else
        PORTB = (fnd_character[(num%100)/10] | FND_AP3);
    Delay_ms(4);
    PORTB = 0xfff0;
    if(dot == 1)
        PORTB = (fnd_character[num%10] | FND_AP4) & DOT;
    else
        PORTB = fnd_character[num%10] | FND_AP4;
    Delay_ms(4);
    PORTB = 0xfff0;
    Delay_ms(1);
}

void Set_Message(char* msg, unsigned char leng){
    message = msg;
    length = leng;
}

float Get_ADC(unsigned int ch, unsigned int buffer){
    const char loopCountA = 12;
    const char loopCountB = 12;
    
    int loopA, loopB;                               //Loop Variable
    float temp, maxV, minV, avgValue;           
    float temp_arrayB[loopCountB];
    float temp_arrayA[loopCountA];
    
    //Channel Selection
    AD1CHS = ch;
    Delay_us(50);
    
    avgValue = 0;
    for(loopA = 0 ; loopA < loopCountA ; loopA++){
        temp = 0;
        for(loopB = 0 ; loopB < loopCountB ; loopB++){
            while(!AD1CON1bits.DONE);
            temp_arrayB[loopB] = buffer;
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
    return avgValue;
}
