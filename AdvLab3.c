
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>

// Define enums for LED status
typedef enum {Hz1, Hz2, Hz3, OFF} LED1Mode;
typedef enum {YELLOW, CYAN, MAGENTA, DARK} LED2Mode;


// Define timer configuration structure
Timer_A_UpModeConfig timerConfig;

// Define constants for Timer_A base addresses and clock configurations
const uint32_t TIMER = TIMER_A0_BASE;
const uint32_t TIMER2 = TIMER_A1_BASE;
uint16_t clockPeriodArray[] = {37500, 18750, 9375, 37500};
uint16_t clockDividerArray[] = {40, 40, 40, 40};

// Define volatile variables for LED status
volatile LED1Mode LED1;
volatile LED2Mode LED2;

// Function prototypes
void myTimer(uint32_t timer, uint16_t clockPeriod, uint16_t clockDivider);
void myTimer2(uint32_t timer, uint16_t clockPeriod, uint16_t clockDivider);
void configIO();
void led1Change();
void led2Change();
void pushButtons();

int main(void){

    MAP_WDT_A_holdTimer();

    // Configure IO pins and LED statuses
    configIO();
    LED1 = OFF;
    LED2 = DARK;

    // Configure Timer_A0 for LED1 toggling and Timer_A1 for LED2 toggling
    myTimer(TIMER, clockPeriodArray[LED1], 40);
    myTimer2(TIMER2, 37500, 40);

    // Enable sleep on ISR exit and global interrupts
    MAP_Interrupt_enableSleepOnIsrExit();
    __enable_interrupts();

    // Enter Low Power Mode 0
    MAP_PCM_gotoLPM0();

    // Infinite loop
    while (1) {}
}


// Function to configure GPIO pins
// Function to configure GPIO pins
void configIO(void)
{
    // Set all LEDs as outputs and initialize them to low
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // Red LED1
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2); // LED2
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);


    // Configure interrupt edge select for switches
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4, GPIO_HIGH_TO_LOW_TRANSITION);

    // Clear interrupt flags for switches
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);

    // Enable interrupts for switches and register interrupt handler
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    MAP_GPIO_registerInterrupt(GPIO_PORT_P1, pushButtons);
}

// Interrupt handler for SW1/SW2 buttons
void pushButtons(void)
{
    if (MAP_GPIO_getInterruptStatus(GPIO_PORT_P1, GPIO_PIN1))
    {
        MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
        if (LED1 != OFF)
            LED1++;
        else
            LED1 = Hz1;
    }
    if (MAP_GPIO_getInterruptStatus(GPIO_PORT_P1, GPIO_PIN4))
    {
        MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
        if (LED2 != OFF)
            LED2++;
        else
            LED2 = YELLOW;
    }
}


// Interrupt handler for Timer_A0
void led1Change(void)
{
    Timer_A_clearInterruptFlag(TIMER);

    switch (LED1)
    {
    case Hz1:
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        break;
    case Hz2:
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        break;
    case Hz3:
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        break;
    default:
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        break;
    }
    myTimer(TIMER, clockPeriodArray[LED1], clockDividerArray[LED1]);
}

// Interrupt handler for Timer_A1
void led2Change(void)
{
    Timer_A_clearInterruptFlag(TIMER2);

    switch (LED2)
    {
    case YELLOW:
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1);
        break;
    case CYAN:
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1 | GPIO_PIN2);
        break;
    case MAGENTA:
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0| GPIO_PIN2);
        break;
    default:
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
        break;
    }
}

// Function to configure Timer_A0 for LED1 toggling
void myTimer(uint32_t timer, uint16_t clockPeriod, uint16_t clockDivider)
{
    timerConfig.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    timerConfig.clockSourceDivider = clockDivider;
    timerConfig.timerPeriod = clockPeriod;
    timerConfig.timerClear = TIMER_A_DO_CLEAR;

    Timer_A_stopTimer(timer);
    Timer_A_configureUpMode(timer, &timerConfig);
    Timer_A_enableInterrupt(timer);
    Timer_A_clearInterruptFlag(timer);
    Timer_A_startCounter(timer, TIMER_A_UP_MODE);
    Timer_A_registerInterrupt(timer, TIMER_A_CCRX_AND_OVERFLOW_INTERRUPT, led1Change);
}

// Function to configure Timer_A1 for LED2 toggling
void myTimer2(uint32_t timer, uint16_t clockPeriod, uint16_t clockDivider)
{
    timerConfig.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    timerConfig.clockSourceDivider = clockDivider;
    timerConfig.timerPeriod = clockPeriod;
    timerConfig.timerClear = TIMER_A_DO_CLEAR;

    Timer_A_stopTimer(timer);
    Timer_A_configureUpMode(timer, &timerConfig);
    Timer_A_enableInterrupt(timer);
    Timer_A_clearInterruptFlag(timer);
    Timer_A_startCounter(timer, TIMER_A_UP_MODE);
    Timer_A_registerInterrupt(timer, TIMER_A_CCRX_AND_OVERFLOW_INTERRUPT, led2Change);
}
