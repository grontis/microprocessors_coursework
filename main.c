#include <msp430.h> 

int R5_SW=0, R6_LED=0, temp=0, i=0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer

    P1OUT = 0b00000000;     // mov.b    #00000000b,&P1OUT
    P1DIR = 0b11111111;     // mov.b    #11111111b,&P1DIR
    P2DIR = 0b00000000;     // mov.b    #00000000b,&P2DIR

    R5_SW = P2IN;
    R6_LED = 0b00000001;

    while (1)
    {
        // read all switches and save them in R5_SW
        R5_SW = P2IN;


    // check for read mode
    if (R5_SW & BIT0) //this case involves when P2.0 is active
       {

        if(R5_SW & BIT1)// when P2.0 is active and P2.1 is also active
        {
            P1OUT = R6_LED;
            for (temp = 0; temp < 8; temp ++)
            {
                R6_LED = (R6_LED >> temp) | (R6_LED << (8-temp)); //shifts bits based on value of temp.
                R6_LED &= 0xFF;
                P1OUT = R6_LED;

                if (R5_SW & BIT2)
                    {
                        __delay_cycles(40000);
                    }  //fast
                else
                    {
                        __delay_cycles(100000);
                    }  //slow
            }
        }
        else // when P2.0 is active and P2.1 is off
        {
            P1OUT = R6_LED;
            for (temp = 0; temp < 3; temp ++)
            {
                R6_LED = (R6_LED << 1) | (R6_LED >> (8-1));
                R6_LED &= 0xFF;
                P1OUT = R6_LED;

                if (R5_SW & BIT2)
                    {
                        __delay_cycles(40000);
                    }  //fast
                else
                    {
                        __delay_cycles(100000);
                    }  //slow
            }

            for (temp = 0; temp < 7; temp ++)
            {
                R6_LED = (R6_LED >> 1) | (R6_LED << (8-1));
                R6_LED &= 0xFF;
                P1OUT = R6_LED;

                if (R5_SW & BIT2)
                    {
                        __delay_cycles(40000);
                    }  //fast
                else
                    {
                        __delay_cycles(100000);
                    }  //slow
            }
        }

       }

    else    //this case involves when P2.0 is off
    {

        if(R5_SW & BIT1)// when P2.0 is off and P2.1 is active
        {
            P1OUT = R6_LED;
            for (temp = 0; temp < 3; temp ++)
            {
                R6_LED = (R6_LED << 2) | (R6_LED >> (8-2)); //shifts bits by 2 in a circular way by adding the "lost" bits
                R6_LED &= 0xFF;
                if (R5_SW & BIT2)
                    {
                      __delay_cycles(40000);
                    }  //fast
                else
                    {
                     __delay_cycles(100000);
                    }  //slow
            }
            P1OUT = R6_LED;
          }
        else    // when P2.0 is off and P2.1 is off
        {
            P1OUT = R6_LED;
            for (temp = 0; temp < 9; temp ++)
            {
                R6_LED = 0b00000001;
                for (i = 0; i < temp; i++)
                {
                    R6_LED = (R6_LED >> 1) | (R6_LED << (8-1));
                    R6_LED &= 0xFF;
                    P1OUT = R6_LED;
                    if (R5_SW & BIT2)
                        {
                          __delay_cycles(40000);
                        }  //fast
                    else
                        {
                         __delay_cycles(100000);
                        }  //slow

                }

                if (R5_SW & BIT2)
                    {
                      __delay_cycles(40000);
                    }  //fast
                else
                    {
                     __delay_cycles(100000);
                    }  //slow
            }

        }
     }



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
