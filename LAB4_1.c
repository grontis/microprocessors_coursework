#include <msp430.h> 

int R5_SW=0, R6_LED=0, temp=0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer

    P1OUT = 0b00000000;     // mov.b    #00000000b,&P1OUT
    P1DIR = 0b11111111;     // mov.b    #11111111b,&P1DIR
    P2DIR = 0b00000000;     // mov.b    #00000000b,&P2DIR

    R5_SW = P2IN;
    R6_LED = R5_SW & (BIT3 | BIT4 | BIT5);

    while (1)
    {
        // read all switches and save them in R5_SW
        R5_SW = P2IN;


    // check for read mode
    if (R5_SW & BIT0)
          {
            R6_LED = R5_SW & (BIT3 | BIT4 | BIT5);
            P1OUT = R6_LED;            // send the pattern out
          }

    // display rotation mode
    else
      {
        // check for direction
        if (R5_SW & BIT1)
        {
            R6_LED = (R6_LED >> 1) | (R6_LED << (8-1));
        }   // rotate left

        else
        {
            R6_LED = (R6_LED << 1) | (R6_LED >> (8-1));
        }   // rotate right

        // mask any excessive bits of the pattern and send it out
        R6_LED &= 0xFF;             // help clear all bits beyound the byte so when you rotate you do not see garbage coming in
        P1OUT = R6_LED;

        // check for speed
        if (R5_SW & BIT2)
        {
            __delay_cycles( 40000);
        }  //fast
        else
        {
            __delay_cycles(100000);
        }  //slow
      }
    }
}
