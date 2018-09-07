#include <msp430.h> 

//Digit configuration, make sure segments h-a are connected to PORT1 pins 7-0
//also besides disigts 0-9, you have single segments abcdefg.
int LEDS[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67,0x01,0x02,04,0x08,0x10,0x20,0x40,0x80};

int s_count = 0; //counter for timer0
int leftdigit=0, rightdigit=0; //display variables for 7seg number
int flag=0; //flag for 7seg left&right display
int tempflag = 0; //flag to show temp if first reading
int zflag = 0; //flag for z number display

//analog temp variables from sensor
int value=0, i=0 ;
int temp = 0;
int ADCReading [1];
//float type used for calculations
float celsius = 0.0f;
float fahrenheit= 0.0f;

//PWM variables
int period = 0x0FFF; //period of the PWM
float D = 0.0f;   //duty cycle, max value 1

// Function Prototypes
void ConfigureAdc(void);

//MAIN program
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;          // Stop watchdog timer

    TACCR0 = 62500;
    TA1CCR0 = 10; //this small TA1CCR0 value allows for rapid interrupts
    TACCTL0 = CCIE;
    TA1CCTL0 = CCIE;
    TACTL = MC_3|ID_3|TASSEL_2|TACLR; //SMCLK (1Mhz), freq div by 8, updown mode
    TA1CTL = MC_3|ID_3|TASSEL_1|TACLR; //ACLK (12khz), freq div by 8, updown mode
    //equation for period with up/down mode is:
    // PERIOD = (2*TACCR0)/fclk
    // PERIOD(TIMER0) = (2*TACCR0)/(1Mhz/8)
    // PERIOD(TIMER0) = (2*TACCR0)/(12khz/8)

    P1OUT = 0x00;                   // port 1 out default 00
    P1DIR = 0xff;                   // port 1 all output
    P2DIR = 0x07;                   // port 2 all inputs, except BIT0 and BIT1 BIT2
    P2OUT = 0x00;
    P2SEL |= BIT2;                  //bit2 for P2.2 LED output

    P2IE = BIT3;//Enable interrupt from port1
    P2IES = BIT3;//Interrupt edge select from high to low
    P2IFG = 0x00;//Clear the interrupt flag

    ConfigureAdc(); //set up ADC registers

    __enable_interrupt();

    LPM3;
} // end of main

void ConfigureAdc(void) //sets up initial ADC settings
{
   ADC10CTL1 = INCH_7 | CONSEQ_0;             // P1.7 used for analog input
   ADC10CTL0 = ADC10SHT_2 | MSC | ADC10ON;
   while (ADC10CTL1 & BUSY);                  // wait while busy
   ADC10DTC1 = 0x01;                          // 1 conversion
   ADC10AE0 |= BIT7;                          // ADC10 option select, P1.7
}

//this timer is used to read analog temperature values,
//and then averages them before converting to celsius(then f) every 5 sec
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void){

    if(zflag == 1) //z flag remains set for 10sec
    {
        s_count++; //second counter

        if (s_count == 10) //about 10 seconds
        {
            zflag = 0;
            s_count = 0;
        }
    }
    else if (zflag == 0)//z flag is off, temperature is calculated every sec, averaged every 5sec
    {
        s_count++; //counter for each second
        ADC10CTL0 &= ~ENC;
        while (ADC10CTL1 & BUSY);                 //Wait while ADC is busy
        ADC10SA = (unsigned)&ADCReading[0];       //RAM Address of ADC Data, must be reset every conversion
        ADC10CTL0 |= (ENC | ADC10SC);             //Start ADC Conversion
        while (ADC10CTL1 & BUSY);                 //Wait while ADC is busy
        temp += ADCReading[0];
        if(s_count == 5) //after 5 seconds
        {
            temp = temp/5; //average of 5 temp readings

            //these 3 lines of code are to convert raw value to celsius (using MCP9700)
            celsius = temp;
            celsius = (celsius * 3.66) / 1023;
            celsius = ((celsius - 0.5) * 100) - 7.4; //the minus 7.4 is to calculate for calibration

            //convert to fahrenheit
            fahrenheit = (celsius * 1.8) + 32;

            temp = fahrenheit; //this sets the float fahrenheit value as an int in temp

            if (temp < 76) //temp less than 76f
            {
                D = 0.0f;
            }

            if (temp > 86) //greater than 86f
            {
                D = 0.95f;
            }

            switch(temp) //switch statement for setting D values, 76f-86f
            {
            case 76: D = 0.1f;
                break;
            case 77: D = 0.25f;
                break;
            case 78: D = 0.2f;
                break;
            case 79: D = 0.3f;
                break;
            case 80: D = 0.4f;
                break;
            case 81: D = 0.5f;
                break;
            case 82: D = 0.6f;
                break;
            case 83: D = 0.7f;
                break;
            case 84: D = 0.8f;
                break;
            case 85: D = 0.9f;
                break;
            case 86: D = 0.95f;
                break;
            }

            //pwm
            TA1CCR0 = period-1; // PWM Period
            TA1CCR1 = period*D; // CCR1 PWM duty cycle
            TA1CCTL1 = OUTMOD_7;  // CCR1 reset/set
            TA1CTL = TASSEL_2|MC_1; // SMCLK, up mode

            //this next part of code splits up the digits into left and right variables.
            rightdigit = temp % 10; //get right digit
            temp /= 10; //shift one digit
            leftdigit = temp; //set left digit
            tempflag = 1; //flag is set to display reading.

            s_count = 0; //reset second counter
         }
    }
}

//this timer is used to output to 7segment display,
#pragma vector = TIMER1_A0_VECTOR
__interrupt void TA1_ISR(void){
    //Display code for Common-Cathode display
    P1OUT= 0;
    P2OUT=0;

    if (zflag == 1) //display last two digit of znumber
    {
        __delay_cycles (100);
        if (flag == 0) //display left digit as 2
        {
            leftdigit = 2;
            P2OUT= BIT0;
            P1OUT= LEDS[leftdigit];
            flag=1;
        }
        else // display right digit as 3
        {
            rightdigit = 3;
            P2OUT= BIT1;
            P1OUT= LEDS[rightdigit];
            flag=0;
        }
        __delay_cycles (100);
    }
    else if (tempflag == 1) //the temperature value is now displayed
    {
        __delay_cycles (100);
        if (flag == 0) //display left digit
        {
            P2OUT= BIT0;
            P1OUT= LEDS[leftdigit];
            flag=1;
        }
        else //display right digit
        {
            P2OUT= BIT1;
            P1OUT= LEDS[rightdigit];
            flag=0;
        }
        __delay_cycles (100);
    }
}

//ISR for P2 interrupt from the switch
#pragma vector=PORT2_VECTOR
//define the interrupt vector
__interrupt void PORT2_ISR(void){
// Interrupt Service Routine for Port 2
 zflag = 1; //flag for znumber display
 LPM3_EXIT; //exit LPM3
 P2IFG = 0x00;//Clear the interrupt flag
}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    __bic_SR_register_on_exit(CPUOFF);
}
