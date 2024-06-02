#include  <msp430g2553.h>
volatile unsigned int i = 0;            // Volatile to prevent optimization. This keeps count of cycles between LED toggles

char text[] = "Green\r\n";

void serial_output(char *str) {
    //while (*str != 0) {

        while (!(IFG2 & UCB0TXIFG));
        UCB0TXBUF = 0x69;

        while (!(IFG2 & UCA0TXIFG));
        UCA0TXBUF = UCB0RXBUF;
    //}
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

    //UC0IE |= UCB0RXIE;
}


void button2_int() {
    P2DIR &= ~BIT0;
    P2OUT =  0x01;
    P2REN |= 0x01;
    P2IE |= 0x01;
    P2IES |= 0x01;
    P2IFG |= 0x01;
}

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;

    initializeUART();
    spi_init();
    button2_int();

    P1DIR |= BIT0;      // P1DIR is a register that configures the direction (DIR) of a port pin as an output or an input.

    P1DIR &= (~BIT3);     // Set P1.3 SEL as Input
    P1IE |= BIT3;         // Interrupt Enable
    P1IES |= BIT3;        // Trigger from High to Low and low to high
    P1IFG |= BIT3;        // Interrupt Flag

    serial_output(text);

    __bis_SR_register(LPM0_bits + GIE);

    while (1) {

    }
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{

    if(P1IFG & BIT3) {
        serial_output(text);
        P1OUT ^= BIT0;
        P1IFG &= ~BIT3; // Clear the Interrupt Flag
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
}

