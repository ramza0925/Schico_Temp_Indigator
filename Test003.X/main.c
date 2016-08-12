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
    int num2 = 0;
    
    
    if(num < 0) {
        num2 = num;
        num = num * -1;
    }
    if(num>=1000){
        if(num2 < 0)
            PORTB = (fnd_character[num/1000] | FND_AP1) & DOT;
        else
            PORTB = fnd_character[num/1000] | FND_AP1;
        Delay_ms(5);
        PORTB = 0x00f0;
    }
    if(num>=100){
        PORTB = fnd_character[(num%1000)/100] | FND_AP2;
        Delay_ms(5);
        PORTB = 0x00f0;
    }
    PORTB = (fnd_character[(num%100)/10] | FND_AP3) & DOT;
    Delay_ms(5);
    PORTB = 0x00f0;
    PORTB = fnd_character[num%10] | FND_AP4;
    Delay_ms(5);
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
    PR1             = 0x7777;       //Load the Period register with the value 0xffff
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
    const char loopCountA = 12;
    const char loopCountB = 12;
    const float RTDA = 3.9083 * pow(10,-3);
    const float RTDB = -5.775 * pow(10,-7);
    const float RTDC = -4.183 * pow(10,-12);
    
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
            Delay_us(50);
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
    
    vout = (avgValue * UNIT)/GAIN;
    rt = vout/CURRENT;
    if(rt>=R0) {
        tmp_Value = Solve_second_Equation(R0*RTDB, R0*RTDA, R0-rt);
        tmp_Value = (int)(tmp_Value*10)%10000;
    }
    else {
//        tmp_Value = Solve_Fourth_Equation(RTDC, -100*R0*RTDC, R0*RTDB, R0*RTDA, R0-rt);
//        tmp_Value = (int)(tmp_Value*10)%10000;
        tmp_Value = (int)((((rt/100)-1)/0.00385f)*10.0f)%10000;
    }
    Delay_ms(400);
    
}

//Current Check
void Current_Check(){
   //TODO
}

//Current Controll
void Current_Control(){
   //TODO
}

float Solve_second_Equation(float a, float b, float c){
    double d = b*b-4*a*c;
    float result = 0;
    if(d < 0) result = 9999;
    else if(d == 0) result = (-b)/(2*a);
    else{
        result = (-b+sqrt(d))/(2*a); 
        if(result < 0) result = (-b-sqrt(d))/(2*a);
    }
    return result;
}

float Solve_Fourth_Equation(float a, float b, float c, float d, float e){
    
    float result = 0;

    
    float res1 = 0;
    float res2 = 0;
    float res3 = 0;
    float res4 = 0;
    float y1 = 0;
    float y2 = 0;
    float y3 = 0;
    float a0 = a/a;
    float b0 = b/a;
    float c0 = c/a;
    float d0 = d/a;
    float e0 = e/a;
    float f0 = 0;
    float g0 = 0;
    float h0 = 0;
    float a1 = 0;
    float b1 = 0;
    float c1 = 0;
    float d1 = 0;
    float f = 0;
    float g = 0;
    float h = 0;
    float i = 0;
    float j = 0;
    float k = 0;
    float l = 0;
    float m = 0;
    float n = 0;
    float p = 0;
    float q = 0;
    float r = 0;
    float s = 0;
    float t = 0;
    
    float o = 0;
    
    o = (pow(d,2)*pow(c,2)*pow(b,2)-4*pow(d,3)*pow(b,3)-4*pow(d,2)*pow(c,3)*a+18*pow(d,3)*a*b*c-27*pow(d,4)*pow(a,2)+256*pow(e,3)*pow(a,3))
		+e*(-4*pow(c,3)*pow(b,2)+18*d*c*pow(b,3)+16*pow(c,4)*a-80*pow(c,2)*d*b*a-6*pow(d,2)*pow(b,2)*a+144*pow(d,2)*pow(a,2)*c)+pow(e,2)*(-27*pow(b,4)
        +144*pow(b,2)*c*a-128*pow(c,2)*pow(a,2)-192*pow(a,2)*d*b);
    
    if(o < 0) result = 9999;
    else if(o >= 0){
        f0 = c0-(3*pow(b0,2)/8);
		g0 = d0+(pow(b0,3)/8)-(b0*c0/2);
		h0 = e0-(3*pow(b0,4)/256)+(pow(b0,2)*c0/16)-(b0*d0/4);

		a1 = 1;
		b1 = f0/2;
		c1 = (pow(f0,2)-4*h0)/16;
		d1 = -pow(g0,2)/64;
	
		f = (((3*c1)/a1) - (pow(b1,2)/pow(a1,2)))/3;
	    g = ((2*pow(b1,3)/pow(a1,3))-((9*b1*c1)/pow(a1,2))+(27*d1/a1)) / 27;
	    h = (pow(g,2)/4)+(pow(f,3)/27);

		
		i = sqrt((pow(g,2)/4 - h)); 
		j = pow(i,1/3);
		k = acos(-(g/(2*i)));
		l = j * (-1);
		m = cos(k/3);
		n = sqrt(3) * sin(k/3);
		p = (b1/(3*a1))*(-1);

		y1 = (2*j)*cos(k/3)-(b1/(3*a1));
		y2 = l * (m + n) + p;
		y3 = l * (m - n) + p;
        
        if((y1*y1 < y2*y2) && (y1*y1 < y3*y3)) {
			//if(y2 < 0)
            q = sqrt(y2);
			r = sqrt(y3);
		} else if((y2*y2 < y1*y1) && (y2*y2 < y3*y3)){
			q = sqrt(y1);
			r = sqrt(y3);
		} else { 
			q = sqrt(y1);
			r = sqrt(y2);
		}
    }
    
    s = -g0/(8*q*r);
	t = b0/(4*a0);

	res1 = q+r+s-t;
	res2 = q-r-s-t;
	res3 = -q+r-s-t;
	res4 = -q-r+s-t;
    if(res1 < 0) result = res1;
    else if (res2 < 0) result = res2;
    else if (res3 < 0) result = res3;
    else if (res4 < 0) result = res4;
    else result = -999;
    return result;
}
