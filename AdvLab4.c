
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>

// Define LED colors
#define RED    (BIT0)
#define GREEN  (BIT1)
#define BLUE   (BIT2)

// Define bumper switch pins
#define BMP0   (BIT0)
#define BMP1   (BIT2)
#define BMP2   (BIT3)
#define BMP3   (BIT5)
#define BMP4   (BIT6)
#define BMP5   (BIT7)

// Wheel Definitions
// Define motor control pins
#define ELB    (BIT2)
#define DIRL   (BIT4)
#define nSLPL  (BIT7)
#define PWML   (BIT7)

#define ERB    (BIT0)
#define DIRR   (BIT5)
#define nSLPR  (BIT6)
#define PWMR   (BIT6)

//Timer Definitions
 #define PERIOD 1000
 #define DUTY  23 // Range 0 to 100
 #define CLOCKDIVIDER TIMER_A_CLOCKSOURCE_DIVIDER_48
 #define LEFT TIMER_A_CAPTURECOMPARE_REGISTER_4
 #define RIGHT TIMER_A_CAPTURECOMPARE_REGISTER_3

//Global VAriables
typedef enum {GO, REV, GOR, GOL, STOP, COAST, SPINL, SPINR, REVR, REVL} MotorState;
MotorState motorState = STOP;
Timer_A_PWMConfig  timerPWMConfig;

// Function prototypes
void configIO();
void readBumperSwitches();
void configPWMTimer(uint16_t, uint16_t, uint16_t, uint16_t);

void main(void){
    MAP_WDT_A_holdTimer();

    configIO();
    configPWMTimer(PERIOD, CLOCKDIVIDER, DUTY, LEFT);
    configPWMTimer(PERIOD, CLOCKDIVIDER, DUTY, RIGHT);

        P1->OUT |= RED; // Red LED1
        P2->OUT |= (RED | GREEN | BLUE); // LED2
        P8->OUT |= (BIT0 | BIT5 | BIT6 | BIT7); // robot LEDs
        __delay_cycles(3000000);

        P1->OUT ^= RED; // Red LED1
        P2->OUT ^= (RED | GREEN | BLUE); // LED2
        P8->OUT ^= (BIT0 | BIT5 | BIT6 | BIT7); // robot LEDs
        __delay_cycles(3000000);
        P1->OUT ^= RED; // Red LED1
        P2->OUT ^= (RED | GREEN | BLUE); // LED2
        P8->OUT ^= (BIT0 | BIT5 | BIT6 | BIT7); // robot LEDs
        __delay_cycles(3000000);
        P1->OUT ^= RED; // Red LED1
        P2->OUT ^= (RED | GREEN | BLUE); // LED2
        P8->OUT ^= (BIT0 | BIT5 | BIT6 | BIT7); // robot LEDs
        __delay_cycles(3000000);


    while (1)
    {


        readBumperSwitches();



       if (motorState == GO) {
            P5->OUT |= (ELB | ERB); // ENABLE
            P5->OUT &= ~(DIRR | DIRL); // DIRECTION
            P2->OUT |= (PWMR | PWML); // PWM
            P3->OUT |= (nSLPR | nSLPL); // Sleep
        }
        else if (motorState == REV) {
            P5->OUT |= (ELB | ERB); // ENABLE
            P5->OUT |= (DIRR | DIRL); // DIRECTION
            P2->OUT |= (PWMR | PWML); // PWM
            P3->OUT |= (nSLPR | nSLPL); // Sleep
        }
        else if (motorState == STOP) {
                   P5->OUT &= ~(ELB | ERB); // ENABLE
                   P3->OUT |= (nSLPR | nSLPL); // Sleep
                   P5->OUT |= (DIRR | DIRL); // DIRECTION
                   P2->OUT &= ~(PWMR | PWML); // PWM
        }
        else if (motorState == COAST) {
            P3->OUT |= (nSLPR | nSLPL); // Sleep
        }
        else if (motorState == GOL) {
            P5->OUT |= (ELB); // ENABLE
            P5->OUT &= ~(DIRL); // DIRECTION
            P2->OUT |= (PWML); // PWM
            P3->OUT |= (nSLPL); // Sleep

            P5->OUT &= ~(ERB); // ENABLE
            P3->OUT |= (nSLPR); // Sleep
        }
        else if (motorState == GOR) {
            P5->OUT |= (ERB); // ENABLE
            P5->OUT &= ~(DIRR); // DIRECTION
            P2->OUT |= (PWMR); // PWM
            P3->OUT |= (nSLPR); // Sleep

            P5->OUT &= ~(ELB); // ENABLE
            P3->OUT |= (nSLPL); // Sleep
        }
        else if (motorState == SPINR) {
            // LW GO
            P5->OUT |= (ELB); // ENABLE
            P5->OUT &= ~(DIRL); // DIRECTION
            P2->OUT |= (PWML); // PWM
            P3->OUT |= (nSLPL); // Sleep
            // RW REV
            P5->OUT |= (ERB); // ENABLE
            P5->OUT |= (DIRR); // DIRECTION
            P2->OUT |= (PWMR); // PWM
            P3->OUT |= (nSLPR); // Sleep
        }
        else if (motorState == SPINL) {
            // RW GO
            P5->OUT |= (ERB); // ENABLE
            P5->OUT &= ~(DIRR); // DIRECTION
            P2->OUT |= (PWMR); // PWM
            P3->OUT |= (nSLPR); // Sleep
            // LW REV
            P5->OUT |= (ELB); // ENABLE
            P5->OUT |= (DIRL); // DIRECTION
            P2->OUT |= (PWML); // PWM
            P3->OUT |= (nSLPL); // Sleep
        }
        else if (motorState == REVR) {
            P5->OUT |= (ERB); // ENABLE
            P5->OUT |= (DIRR); // DIRECTION
            P2->OUT |= (PWMR); // PWM
            P3->OUT |= (nSLPR); // Sleep
        }
        else {
             motorState = STOP;
        }



    }
}

// Function to configure GPIO pins
void configIO(){
          // Configure output LEDs
          P1->DIR |= RED; // Red LED1
          P2->DIR |= (RED | GREEN | BLUE); // LED2
          P8->DIR |= (BIT0 | BIT5 | BIT6 | BIT7); // robot LEDs

          // Configure bumper switches as inputs with pull-up resistors
          P4->DIR &= ~(BMP0 | BMP1 | BMP2 | BMP3 | BMP4 | BMP5);
          P4->REN |= (BMP0 | BMP1 | BMP2 | BMP3 | BMP4 | BMP5);
          P4->OUT |= (BMP0 | BMP1 | BMP2 | BMP3 | BMP4 | BMP5);

          // Configure Wheels
          P5->DIR |= ( ELB | ERB | DIRR | DIRL ); //ENABLE & DIRECTION
          P2->DIR |= (PWMR | PWML); //PWM
          P3->DIR |= (nSLPR | nSLPL); //Sleep

          // Set Everything Low
          P1->OUT &= ~RED; // LED1
          P2->OUT &= ~(RED | GREEN | BLUE); // LED2
          P8->OUT &= ~(BIT0 | BIT5 | BIT6 | BIT7); //Robot LEDs
          P5->OUT &= ~(ELB | ERB | DIRR | DIRL); //ENABLE & DIRECTION
          P2->OUT &= ~(PWML | PWMR); //PWM
          P3->OUT &= ~(nSLPL | nSLPR); //Sleep
      }



// Function to read the value of the bumper switches and toggle LEDs accordingly
void readBumperSwitches(){

         if((GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN7)==0) && (GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN0)==0)){
            P1->OUT |= RED;
            P2->OUT |= (RED);
            P2->OUT &= ~(BLUE | GREEN);
            motorState = GOR;
         }else if(GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN7)==0){ //BMP5
            P1->OUT |= RED;
            P2->OUT &= ~(BLUE | GREEN);
            P2->OUT |= RED;
            //P8->OUT |= (BIT0 | BIT5 | BIT6 | BIT7);
            motorState = GOL;
        }else if(GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN6)==0){ //BMP4
            P1->OUT |= RED;
            P2->OUT &= ~(RED | BLUE);
            P2->OUT |= GREEN;
            //P8->OUT |= (BIT0 | BIT5 | BIT6 | BIT7);
            motorState = STOP;

        }else if(GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN5)==0){ //BMP3
            P1->OUT |= RED;
            P2->OUT &= ~(RED | GREEN);
            P2->OUT |= BLUE;
            //P8->OUT |= (BIT0 | BIT5 | BIT6 | BIT7);
            motorState = STOP;

        }else if(GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN3)==0){ //BMP2
            P1->OUT &= ~RED;
            P2->OUT &= ~(RED | GREEN);
            P2->OUT |= BLUE;
           // P8->OUT &= ~(BIT0 | BIT5 | BIT6 | BIT7);
            motorState = SPINR;

        }else if(GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN2)==0){ //BMP1
            P1->OUT &= ~RED;
            P2->OUT &= ~(BLUE | RED);
            P2->OUT |= GREEN;
           // P8->OUT &= ~(BIT0 | BIT5 | BIT6 | BIT7);
            motorState = STOP;

        }else if(GPIO_getInputPinValue(GPIO_PORT_P4,GPIO_PIN0)==0){ //BMP0
            P1->OUT &= ~RED;
            P2->OUT &= ~(BLUE | GREEN);
            P2->OUT |= RED;
            //P8->OUT &= ~(BIT0 | BIT5 | BIT6 | BIT7);
            motorState = STOP;
        }else{
            P2->OUT &= ~(RED | BLUE | GREEN);
            P8->OUT &= ~(BIT0 | BIT5 | BIT6 | BIT7);
            motorState = STOP;

        }


}

void configPWMTimer(uint16_t clockPeriod, uint16_t clockDivider, uint16_t duty, uint16_t channel)
{
    const uint32_t TIMER=TIMER_A0_BASE;
    uint16_t dutyCycle = duty*clockPeriod/100;
    timerPWMConfig.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    timerPWMConfig.clockSourceDivider = clockDivider;
    timerPWMConfig.timerPeriod = clockPeriod;
    timerPWMConfig.compareOutputMode = TIMER_A_OUTPUTMODE_TOGGLE_SET;
    timerPWMConfig.compareRegister = channel;
    timerPWMConfig.dutyCycle = dutyCycle;
    MAP_Timer_A_generatePWM(TIMER, &timerPWMConfig);
}





