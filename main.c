#include  <msp430g2553.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "position.h"

volatile unsigned int i = 0;            // Volatile to prevent optimization. This keeps count of cycles between LED toggles

char text[] = "Green\r\n";
char buffer[100] = {};

position pos;

spi_pos master_mode = IDLE;

void serial_output(char *str, position* p) {
        while (!(IFG2 & UCB0TXIFG));
        UCB0TXBUF = p->right;

        while (!(IFG2 & UCA0TXIFG));
        UCA0TXBUF = UCB0RXBUF;

        switch(master_mode)
        {
            case RIGHT:
                UCB0TXBUF = 0x12;
                UCA0TXBUF = UCB0RXBUF;
        }
}

int read_file(FILE *ptr, char *s, FILE *ptr1) {

    char ch;
    int count = 0;

    if (ptr == NULL) {
        return -1;
    }

    do {
        ch = fgetc(ptr);
        printf("%c", ch);
        s[count] = ch;
        count++;

        // Checking if character is not EOF.
        // If it is EOF stop reading.
     } while (ch != EOF);


    fprintf(ptr1, s);
    fclose(ptr);
    return 0;
}



void initializeUART(){

    P1SEL |= BIT1 |BIT2;
    P1SEL2 |= BIT1|BIT2;

    UCA0CTL1 |= UCSWRST+UCSSEL_2;
    UCA0BR0 = 52;  //settings for 19200 baud
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_0;
    UCA0CTL1 &= ~UCSWRST;

}

void spi_init() {

    P1SEL |= BIT5 + BIT6 + BIT7;
    P1SEL2 |= BIT5 + BIT6 + BIT7;

    UCB0CTL1 |= UCSWRST;

    // Configure USCB0 for SPI
    UCB0CTL0 |= UCCKPL + UCMST+ UCSYNC + UCMSB; // Clock polarity high, master mode, synchronous mode, MSB first
    UCB0CTL1 &= ~UCMST;
    UCB0CTL1 |= UCSSEL_2;                        // Select SMCLK as the clock source

    // Set clock divider
    UCB0BR0 = BIT1;                              // SPI clock = SMCLK / 2
    UCB0BR1 = BIT0;                              // (High byte)

    // Initialize USCB0 by releasing it from reset
    UCB0CTL1 &= ~UCSWRST;

}


void button2_int() {
    P2DIR &= ~BIT0;
    P2OUT =  BIT0;
    P2REN |= BIT0;
    P2IE |= BIT0;
    P2IES |= BIT0;
    P2IFG |= BIT0;
}

void button1_init()
{
    P1DIR |= BIT0;      // P1DIR is a register that configures the direction (DIR) of a port pin as an output or an input.
    P1DIR &= ~BIT3;     // Set P1.3 SEL as Input
    P1IE |= BIT3;       // Interrupt Enable
    P1IES |= BIT3;      // Trigger from High to Low and low to high
    P1IFG |= BIT3;      // Interrupt Flag
}

void main(void) {
    WDTCTL = WDTPW + WDTHOLD; //

    initializeUART();
    spi_init();
    button2_int();
    button1_init();

    __bis_SR_register(LPM0_bits + GIE);

    while (1) {

    }
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{

    if(P1IFG & BIT3) {
        pos.right = 0xFF;
        master_mode = RIGHT;
        serial_output(text, &pos);
        P1OUT ^= BIT0;
        P1IFG &= ~BIT3;
    }

}

#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
    if (P2IFG & BIT0) {
        P1OUT ^= BIT0;
        P2IFG &= ~BIT0;

    }
}


#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCBRX_IRS(void)
{
    //UCB0I2CSA;
    while (!(IFG2 & UCB0RXIFG));
    int data = (unsigned int)UCB0RXBUF;
    P1OUT ^= BIT0;
    //int data = (unsigned int)UCB0RXBUF;
}
