#include <msp430.h>

void setupSPI_Slave() {
    // Set UCSWRST (USCI Software Reset)
    UCB0CTL1 = UCSWRST;
    // Set USCI_B0 to slave mode, 3-pin SPI, synchronous mode
    UCB0CTL0 = UCSYNC + UCCKPL + UCMSB;
    // Clear UCSWRST to release USCI_B0 for operation
    UCB0CTL1 &= ~UCSWRST;
    // Enable USCI_B0 RX interrupt
    IE2 |= UCB0RXIE;
}

int main(void) {
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    P1SEL |= BIT5 + BIT6 + BIT7; // Set P1.5, P1.6, P1.7 as SPI pins
    P1SEL2 |= BIT5 + BIT6 + BIT7;

    setupSPI_Slave();

    __bis_SR_register(GIE); // Enable global interrupts

    while (1) {
        __bis_SR_register(LPM0_bits + GIE);
    }
}

// USCI_B0 Data ISR
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
    if (IFG2 & UCB0RXIFG) {
        unsigned char receivedData = UCB0RXBUF; // Read received data
        // Do something with the received data
        __bic_SR_register_on_exit(LPM0_bits); // Exit low-power mode
    }
}
