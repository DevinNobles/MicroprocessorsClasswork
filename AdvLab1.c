#include "ti/devices/msp432p4xx/inc/msp.h"
#include "ti/devices/msp432p4xx/driverlib/driverlib.h"
#include <stdint.h>

#define RED1 BIT0
#define RED2 BIT0
#define GREEN BIT1
#define BLUE BIT2
#define S1 BIT1 // Switch 1 connected to P1.1
#define S2 BIT4 // Switch 2 connected to P1.4
#define delay1 3000000
#define delay2 1500000
#define delay3 750000

void configureSwitches();
void delayChange(uint8_t led1Mode);
void colorChange(uint8_t ledMode);

uint8_t led1Mode = 0;
uint8_t led2Mode = 0;
uint8_t button1Clicked = 0; // Flag to track button click
uint8_t button2Clicked = 0;

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop WDT

    // Set LED pins as outputs
    P1->DIR |= RED1;
    P2->DIR |= RED2 | BLUE | GREEN; // P2.0, P2.1, P2.2 set as outputs

    P1->OUT |= RED1; // Turn on the light connected to P1.0
    P2->OUT |= (RED2 | BLUE | GREEN); // Set P2.0, P2.1, P2.2 high
    __delay_cycles(delay1);

    P1->OUT &= ~RED1; // Set P1.0 low (clear bit)
    P2->OUT &= ~(RED2 | BLUE | GREEN); // Set P2.0, P2.1, P2.2 low (clear bits)
    __delay_cycles(delay1);

    P1->OUT |= RED1; // Turn on the light connected to P1.0
    P2->OUT |= (RED2 | BLUE | GREEN); // Set P2.0, P2.1, P2.2 high
    __delay_cycles(delay1);

    P1->OUT &= ~RED1; // Set P1.0 low (clear bit)
    P2->OUT &= ~(RED2 | BLUE | GREEN); // Set P2.0, P2.1, P2.2 low (clear bits)

    // Initialize switch configurations
    configureSwitches();

    while (1) {
        if (P1->IFG & S1) {
            P1->IFG &= ~S1; // Clear interrupt flag
            led1Mode++; // Change LED mode
            if (led1Mode > 3) {
                led1Mode = 0; // Reset the mode variable if it exceeds 3
            }
        }

        if (P1->IFG & S2) {
            P1->IFG &= ~S2; // Clear interrupt flag
            led2Mode++; // Change LED mode
            if (led2Mode > 3) {
                led2Mode = 0; // Reset the mode variable if it exceeds 3
            }
        }

        colorChange(led2Mode);
        delayChange(led1Mode);
    }
}

void configureSwitches() {
    // Configure switch S1 as input with pull-up resistor and interrupt sensing
    P1->DIR &= ~S1; // Set as input
    P1->REN |= S1; // Enable pull-up resistor
    P1->OUT |= S1; // Enable pull-up
    P1->IE |= S1; // Enable interrupt for S1
    P1->IES |= S1; // Set interrupt to trigger on high-to-low transition
    P1->IFG &= ~S1; // Clear any existing interrupt flags

    // Configure switch S2 as input with pull-up resistor and interrupt sensing
    P1->DIR &= ~S2; // Set as input
    P1->REN |= S2; // Enable pull-up resistor
    P1->OUT |= S2; // Enable pull-up
    P1->IE |= S2; // Enable interrupt for S2
    P1->IES |= S2; // Set interrupt to trigger on high-to-low transition
    P1->IFG &= ~S2; // Clear any existing interrupt flags
}

void delayChange(uint8_t led1Mode) {
    // Toggle RED LED1 based on the LED mode
    switch (led1Mode) {
        case 0: // OFF
            P1->OUT &= ~RED1; // Set P1.0 low (clear bit)
            break;
        case 1: // 1 Hz
            P1->OUT ^= RED1;
            __delay_cycles(delay1);
            break;
        case 2: // 2 Hz
            P1->OUT ^= RED1;
            __delay_cycles(delay2);
            break;
        case 3: // 4 Hz
            P1->OUT ^= RED1;
            __delay_cycles(delay3);
            break;
        default:
            // Handle unexpected mode value
            break;
    }
}

void colorChange(uint8_t led2Mode) {
    // Toggle LEDs based on the LED mode
    switch (led2Mode) {
        case 0: // OFF
            P2->OUT &= ~(RED2 | BLUE | GREEN); // Turn off all LEDs
            break;
        case 1: // 1 Hz
            P2->OUT &= ~(BLUE | GREEN); // Set P2.0, P2.1, P2.2 low (clear bits)
            P2->OUT |= RED2; // Set P2.0, P2.1, P2.2 high
            break;
        case 2: // 2 Hz
            P2->OUT &= ~(RED2 | BLUE); // Set P2.0, P2.1, P2.2 low (clear bits)
            P2->OUT |= GREEN; // Set P2.0, P2.1, P2.2 high
            break;
        case 3: // 4 Hz
            P2->OUT &= ~(RED2 | GREEN); // Set P2.0, P2.1, P2.2 low (clear bits)
            P2->OUT |= BLUE; // Set P2.0, P2.1, P2.2 high
            break;
        default:
            // Handle unexpected mode value
            break;
    }
}
