/**
 * @file main.c
 * @brief Decodes a Morse code input (from a button) and sends
 * the corresponding ASCII code via UART
 *
 * @author Dusan Ilic
 * @date 2021
 */

/*=====================================================================*/
/* Includes */
/*=====================================================================*/

#include <msp430.h> 
#include <stdint.h>
#include "defines.h"
#include "morse.h"

/*=====================================================================*/
/* Defines */
/*=====================================================================*/

/**
 * @brief Debounce Compare value
 *
 * Timer A0 is clocked by ACLK (32,768 Hz).
 * This value is determines button debounce period.
 */
#define TIM_DEBOUNCE_CMP        (655)   // ~20ms

/**
 * @brief Base time period unit
 *
 * Timer A0 is clocked by ACLK (32,768 Hz).
 */
#define TIM_UNIT_PERIOD         (328)   // ~10ms

/**
 * @brief Number of base units required for button press
 * to be considered long
 */
#define LONG_PRESS_UNITS        (30)    // ~300ms

/**
 * @brief Number of base units for a break
 * between two characters
 */
#define BREAK_UNITS             (50)    // ~500ms

/**
 * @brief UCBRx for 19200 baud rate with 1,048,576Hz clock
 */
#define BR19200_UCBR            (54)

/**
 * @brief UCBRSx for 19200 baud rate with 1,048,576Hz clock
 */
#define BR19200_UCBRS           (UCBRS_5)

/*=====================================================================*/
/* Declarations */
/*=====================================================================*/

/**
 * @brief Timer A0 Initialization
 *
 * ACLK is the clock source.
 * Capture/compare interrupts are enabled.
 * The timer is used for button debouncing.
 */
static inline void TA0_Init(void);

/**
 * @brief Timer A1 Initialization
 *
 * ACLK is the clock source.
 * Capture/compare interrupts are enabled.
 * The timer is used for measuring nutton state durations
 */
static inline void TA1_Init(void);

/**
 * @brief USCI_A1 initialization
 *
 * Initializes Universal Serial Communication Interface in UART Mode
 * Selects 19200, 8N1 UART Mode
 */
static inline void USCI_A1_Init(void);

/**
 * @brief IOP Initialization
 *
 * Initializes P2.1 (Button S1) as an input with falling
 *  edge interrupts enabled.
 * Initializes P1.0 (LED LED1) as an output.
 * Initializes P4.7 (LED LED2) as an output.
 * Initializes P1.2 (LED LD3) as an output.
 */
static inline void IOP_Init(void);

/*=====================================================================*/
/* Variables */
/*=====================================================================*/

// Statuses and flags
unsigned int press_duration_checking_in_progress = 0;
unsigned int dash_dot_count = 0;
unsigned int ready_to_decode = 0;

// Characters and codes
char input;
char code[MAX_CODE_LENGTH+1];

/*=====================================================================*/
/* Main */
/*=====================================================================*/

/**
 * @brief Main function
 *
 * Initializes all the necessary peripherals, and then waits
 *  for a flag to be set to transmit the decoded character via UART
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    IOP_Init();                             // Initialize IO Ports
    TA0_Init();                             // Initialize TA0
    TA1_Init();                             // Initialize TA1
    USCI_A1_Init();                         // Initialize USCI_A1 in UART Mode

    __enable_interrupt();                   // Enable interrupts


    char decoded;                           // Holds the value of the last
                                            // decoded character

    while (1) {
        // Keeping the LED1 on while the button is pressed
        if ((P2IN & BIT1) == 0) BIT_SET(P1OUT, BIT0);
        else BIT_CLEAR(P1OUT, BIT0);

        // Keeping the LD3 off while the button is not pressed
        if ((P2IN & BIT1) != 0) BIT_CLEAR(P1OUT, BIT2);

        // If the flag was set, decode the input and send
        // the decoded output via UART
        if (ready_to_decode != 0)
        {
            ready_to_decode = 0;            // Reseting the flag
            decoded = morse_to_ascii(code); // Decoding the input
            UCA1TXBUF = decoded;            // Sending the result
                                            // via UART
        }
    }
}

/*=====================================================================*/
/* Definitions */
/*=====================================================================*/

static inline void TA0_Init(void)
{
    // Selecting the clock source - ACLK
    BIT_CLEAR(TA0CTL, TASSEL0 | TASSEL1);
    BIT_SET(TA0CTL, TASSEL__ACLK);

    // Configuring capture/compare registers
    TA0CCR0 = TIM_DEBOUNCE_CMP;

    // Enabling Capture/Compare interrupts
    BIT_SET(TA0CCTL0, CCIE);
}

static inline void TA1_Init(void)
{
    // Selecting the clock source - ACLK
    BIT_CLEAR(TA1CTL, TASSEL0 | TASSEL1);
    BIT_SET(TA1CTL, TASSEL__ACLK);

    // Configuring capture/compare registers
    TA1CCR0 = TIM_UNIT_PERIOD;

    // Enabling Capture/Compare interrupts
    BIT_SET(TA1CCTL0, CCIE);
}


static inline void USCI_A1_Init(void)
{
    // Set the software reset bit before configuring
    BIT_SET(UCA1CTL1, UCSWRST);

    // USCI Registers Initialization
    UCA1CTL0 = 0;                       // 8N1 Mode
    BIT_SET(UCA1CTL1, UCSSEL__SMCLK);   // SMCLK as clock source
    UCA1BRW = BR19200_UCBR;             // Setting baud rate
    BIT_SET(UCA1MCTL, BR19200_UCBRS);

    // Configure port secondary functions
    // P4.4 - UCA1TXD
    // P4.5 - UCA1RXD
    BIT_SET(P4SEL, (BIT4 | BIT5));

    // Release the software reset
    BIT_CLEAR(UCA1CTL1, UCSWRST);
}

static inline void IOP_Init(void)
{
    // Button S1 - P2.1 Configuration
    BIT_CLEAR(P2DIR, BIT1);             // P2.1 Input
    BIT_SET(P2REN, BIT1);               // Enabling pull-up/pull-down resistor
    BIT_SET(P2OUT, BIT1);               // Selecting pull-up
    BIT_SET(P2IES, BIT1);               // Selecting falling edge
    BIT_SET(P2IE, BIT1);                // Enabling interrupts

    // LED LED1 - P1.0 Configuration
    BIT_CLEAR(P1OUT, BIT0);             // Setting P1.0 to 0
    BIT_SET(P1DIR, BIT0);               // P1.0 Output

    // LED LED2 - P1.2 Configuration
    BIT_SET(P4OUT, BIT7);               // Setting P4.7 to 1
    BIT_SET(P4DIR, BIT7);               // P4.7 Output

    // LED LD3 - P1.2 Configuration
    BIT_CLEAR(P1OUT, BIT2);             // Setting P1.2 to 0
    BIT_SET(P1DIR, BIT2);               // P1.2 Output
}

/*=====================================================================*/
/* Interrupt Service Routines */
/*=====================================================================*/

/**
 * @brief PORT2 Interrupt Service Routine
 *
 * If falling edge occurred on button S1, starts the timer for debouncing
 */
void __attribute__ ((interrupt(PORT2_VECTOR))) PORT2_ISR (void)
{
    if ((P2IFG & BIT1) != 0)            // Button S1 event
    {
        BIT_CLEAR(P2IE, BIT1);          // Disabling interrupts on P1.4
        BIT_CLEAR(P2IFG, BIT1);         // Clearing the flag
        BIT_SET(TA0CTL, MC__UP);        // Starting TA0 in up mode
    }
}
/**
 * @brief Timer A0 CCR0 Interrupt service routine
 */
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) TA0CCR0_ISR (void)
{

}

/**
 * @brief Timer A1 CCR0 Interrupt service routine
 */
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TA1CCR0_ISR (void)
{

}
