#include "ti/devices/msp432p4xx/inc/msp.h"
#include "ti/devices/msp432p4xx/driverlib/driverlib.h"
#include <stdint.h>

#define RED1  BIT0
#define RED2  BIT0
#define GREEN BIT1
#define BLUE  BIT2
#define S1    BIT1 // Switch 1 connected to P1.1
#define S2    BIT4 // Switch 2 connected to P1.4
#define delay1 3000000
#define delay2 1500000
#define delay3 750000
//#define delay3 375000

void myTimeDelay(uint32_t delay);
void configureSwitches();
void delayChange(uint8_t led1Mode);
void colorChange(uint8_t led2Mode, uint8_t led1Mode);
void mrDelay();

uint8_t led1Mode = 0;
uint32_t led1Delay = 0;
uint8_t led2Mode = 0;
uint8_t button1Clicked = 0; // Flag to track button click
uint8_t button2Clicked = 0;

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop WDT

    // Set LED pins as outputs
    P1->DIR |= RED1;
    P2->DIR |= RED2 | BLUE | GREEN;         // P2.0, P2.1, P2.2 set as outputs

    // Initialize LED states
    P1->OUT &= ~RED1;
    P2->OUT &= ~(RED2 | BLUE | GREEN);

    // Initialize switch configurations
    configureSwitches();

    while(1) {
        // Handle switch S1
        if (P1->IFG & S1) {
            P1->IFG &= ~S1; // Clear interrupt flag
            led1Mode++; // Change LED mode for LED1
            led1Delay++;
            if (led1Mode > 3) {
                led1Mode = 0; // Reset LED mode variable if it exceeds 3
                led1Delay = 0;
            }
        }

        // Handle switch S2
        if (P1->IFG & S2) {
            P1->IFG &= ~S2; // Clear interrupt flag
            led2Mode++; // Change LED mode for LED2
            if (led2Mode > 3) {
                led2Mode = 0; // Reset LED mode variable if it exceeds 3
            }
        }

        delayChange(led1Mode);

        colorChange(led2Mode, led1Mode);
    }
}

void configureSwitches() {
    // Configure switch S1 as input with pull-up resistor and interrupt sensing
    P1->DIR &= ~S1; // Set as input
    P1->REN |= S1;  // Enable pull-up resistor
    P1->OUT |= S1;  // Enable pull-up
    P1->IE |= S1;   // Enable interrupt for S1
    P1->IES |= S1;  // Set interrupt to trigger on high-to-low transition
    P1->IFG &= ~S1; // Clear any existing interrupt flags

    // Configure switch S2 as input with pull-up resistor and interrupt sensing
    P1->DIR &= ~S2; // Set as input
    P1->REN |= S2;  // Enable pull-up resistor
    P1->OUT |= S2;  // Enable pull-up
    P1->IE |= S2;   // Enable interrupt for S2
    P1->IES |= S2;  // Set interrupt to trigger on high-to-low transition
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
            mrDelay();
            break;
        case 2: // 2 Hz
            P1->OUT ^= RED1;
            mrDelay();
            break;
        case 3: // 4 Hz
            P1->OUT ^= RED1;
            mrDelay();
            break;
        default:
            // Handle unexpected mode value
            break;
    }
}
void colorChange(uint8_t led2Mode, uint8_t led1Mode) {
    // Toggle LEDs based on the LED mode

    //if (led1Mode == 0) {  // If LED1 is off
        // Turn on LED2 with the corresponding color
        switch (led2Mode) {
            case 0: // OFF
                P2->OUT &= ~(RED2 | BLUE | GREEN); // Turn off all LEDs
                break;
            case 1: // Yellow
                P2->OUT ^= RED2 | GREEN;
                P2->OUT &= ~BLUE;
                mrDelay();

                break;
            case 2: // Cyan
                P2->OUT ^= GREEN | BLUE;
                P2->OUT &= ~RED2;
                mrDelay();

                break;
            case 3: // Magenta
                P2->OUT ^= RED2 | BLUE;
                P2->OUT &= ~GREEN;
                mrDelay();

                break;
            default:
                // Handle unexpected mode value
                break;
        }
    }


void mrDelay() {
    // Determine delay based on led1Delay
    switch (led1Delay) {
        case 0: // OFF
//            myTimeDelay(); // Very long delay
            break;
        case 1: // 1 Hz
            myTimeDelay(delay1);
            break;
        case 2: // 2 Hz
            myTimeDelay(delay2);
            break;
        case 3: // 4 Hz
            myTimeDelay(delay3);
            break;
        default:
            // Handle unexpected mode value
            break;
    }
}

void myTimeDelay(uint32_t delay){
    const uint32_t COUNTFLAG = BIT(16);
    MAP_SysTick_setPeriod(delay); // Set Counter Value
    MAP_SysTick_enableModule(); // Turn ON Counter
    // Poll COUNTFLAG until it is ONE
        while((SysTick->CTRL & COUNTFLAG) == 0);
            MAP_SysTick_disableModule(); //Turn OFF Counter and return
}
