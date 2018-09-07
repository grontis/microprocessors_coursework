#include <msp430.h> 

//Digit configuration, make sure segments h-a are connected to PORT1 pins 7-0
//also besides disigts 0-9, you have single segments abcdefg.
int LEDS[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67,0x01,0x02,04,0x08,0x10,0x20,0x40,0x80};

int switches = 0;
int leftdigit=0, rightdigit=0;
int pleftdigit=0, prightdigit=0;    //preset values
int flag=0;

int main(void)
{
//  WDTCTL = WDTPW | WDTHOLD;       // Stop watchdog timer
    BCSCTL2 |= DIVS_2;              // DIVS_0; DIVS_1; DIVS_2; DIVS_3;
    WDTCTL = WDT_MDLY_0_5;          // WDT_MDLY_32; WDT_MDLY_8; WDT_MDLY_0_5;
    IE1 |= WDTIE;

    P1OUT = 0x00;                   // port 1 out default 00
    P1DIR = 0xff;                   // port 1 all output
    P2DIR = 0x03;                   // port 2 all inputs, except BIT0 and BIT1

    __enable_interrupt();

for (;;)
{
    switches = P2IN; //if wired as active low

    // the displayed numbers while we keep multiplexing of the display relatively faster

    //check switches for 000 --> Counter resets to 00
    if (((switches & BIT5) != BIT5) && ((switches & BIT4) != BIT4) && ((switches & BIT3) != BIT3))
    {
        leftdigit=0;
        rightdigit=0;
    }

    //check switches for 001 --> right digit count up
    if (((switches & BIT5) != BIT5) && ((switches & BIT4) != BIT4) && ((switches & BIT3) == BIT3))
    {
        rightdigit++;
        if (rightdigit >=10)
        {
            rightdigit=0;
        }
    }

    //check switches for 010 --> left digit count up
    if (((switches & BIT5) != BIT5) && ((switches & BIT4) == BIT4) && ((switches & BIT3) != BIT3))
    {
        leftdigit++ ;
        if (leftdigit >=10)
        {
            leftdigit=0;
        }
    }

    //check switches for 011 --> Right and left digits both hold values (preset value)
    if (((switches & BIT5) != BIT5) && ((switches & BIT4) == BIT4) && ((switches & BIT3) == BIT3))
    {
        pleftdigit=leftdigit;
        prightdigit=rightdigit;
    }

    // SW-321 = 111: Counter cycles up from the preset value to 99
    if (((switches & BIT5) == BIT5) && ((switches & BIT4) == BIT4) && ((switches & BIT3) == BIT3))

    {
        rightdigit = prightdigit;
        leftdigit = pleftdigit;
        prightdigit++;

        if (prightdigit >= 10)
        {
            prightdigit = 0;
            pleftdigit++;
        }

        if(pleftdigit >= 10)
        {
            pleftdigit = 0;
            prightdigit = 0;
        }


    }

    // SW-321 = 100: Counter cycles down from the preset value to 00
    if (((switches & BIT5) == BIT5) && ((switches & BIT4) != BIT4) && ((switches & BIT3) != BIT3))
    {
        rightdigit = prightdigit;
        leftdigit = pleftdigit;
        prightdigit--;

        if (prightdigit < 0 && pleftdigit > 0)
        {
            prightdigit = 9;
            pleftdigit--;
        }

        if(pleftdigit < 1 && prightdigit < 0)
        {
            pleftdigit = 9;
            prightdigit = 9;
        }

    }

    //check switches for 101 --> pause counting
    if (((switches & BIT5) == BIT5) && ((switches & BIT4) != BIT4)&& ((switches & BIT3) == BIT3))
    { }

    //check switches for 110 --> 2 digit count down
    if (((switches & BIT5) == BIT5) && ((switches & BIT4) == BIT4)&& ((switches & BIT3) != BIT3))
    { }

    // this delay determins the speed of changing the number bing displayed
    __delay_cycles (250000);

} // end of for loop
} // end of main


// WDT interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt void WDT(void)
{
    //This executes every time there is a timer interrupt from WDT
    //The frequency of this interrupt controls the flickering of display
    //The right and left digits are displayed alternatively
    //Note that both digits must be turned off to avoid aliasing


    //Display code for Common-Cathode display
    P1OUT= 0;
    P2OUT=0;
    __delay_cycles (100);
    if (flag == 0)
    {
        P2OUT= BIT0;
        P1OUT= LEDS[leftdigit];
        flag=1;
    }
    else
    {
        P2OUT= BIT1;
        P1OUT= LEDS[rightdigit];
        flag=0;
    }
    __delay_cycles (100);


}
