// This program sets up GPIO pins 17, 27, 22 as output pins, 
// and pins 23 and 24 to input pins (buttons)
// generates interrupts when a button is pushed on the breadboard.
// Button A will put the program into mode 0 (state 1)
// Button B will put the program into mode 1 (state 2)

// Include files
#include "uart.h"
#include "sysreg.h"
#include "gpio.h"
#include "irq.h"


// Function prototypes
void init_pins();
void change_light(int mode);
void smallWait();
void stateOne();
void stateTwo();
void stateThree();

// Declare a global mode of operation
unsigned int mode;
unsigned int state;



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       main
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    Initialize global mode value, enable all the pins,
//                  then infinitely loop through the LED's and change
//                  mode depending on interrupts.
//
////////////////////////////////////////////////////////////////////////////////

void main()
{
    unsigned int r;
    unsigned int localValue;

    // Set up the UART serial port
    uart_init();

    // Initialize the mode global variable and
    // and set the local variable to be same value
    localValue = mode = 0;
    state = 1;
    
    // Setup pins to be inputs and outputs
    init_pins();
    
    // Enable IRQ Exceptions
    enableIRQ();

    // Loop forever, waiting for interrupts to change the shared value
    while (1) {
    // Check to see if the mode was changed by an interrupt
	if (localValue != mode) {
	    // Update the local variable
	    localValue = mode;
        }
	    // change which light emits
        change_light(mode);

        // Delay a little using a busy loop
       longWait();
    }
}


////////////////////////////////////////////////////////////////////////////////
//
//  Function:       change_light
//
//  Arguments:      mode - 0 for lights to go in order 1,2,3 .. 1 for lights to 
//                  go in order 3,2,1
//
//  Returns:        void
//
//  Description:    If mode = 0, will change next light to emit, all others off
//                  If mode = 1, change the previous light to emit, all others off
//
////////////////////////////////////////////////////////////////////////////////

void change_light(int mode){
    if (mode == 0){
        longWait();     // make it take longer to iterate if it is in mode 0
        if(state == 1){
            //if mode = 0, and LED1 is on, turn LED1 off, and turn LED2 on
            stateTwo();
        } else if(state == 2){
             //if mode = 0, and LED2 is on, turn LED2 off, and turn LED3 on
            stateThree();
        } else if(state == 3){
            //if mode = 0, and LED3 is on, turn LED3 off, and turn LED1 on
            stateOne();
        }
    } else if(mode == 1) {
        if(state == 1){
            //if mode = 1, and LED1 is on, turn LED1 off, and turn LED3 on
            stateThree();
        } else if(state == 2){
            //if mode = 1, and LED2 is on, turn LED2 off, and turn LED1 on
            stateOne(state == 3);
        } else if(){
             //if mode = 1, and LED3 is on, turn LED3 off, and turn LED2 on
            stateTwo();
        }
    }
}


// A sequence of state changing functions for the LED lights
void stateOne(){
    state = 1;
    register unsigned int r;
    r = (0x1 << 17);    //light up LED connected to pin 17
    *GPSET0 = r;
    r = (0x1 << 22);
    *GPCLR0 = r;
    r = (0x1 << 27);
    *GPCLR0 = r;
}
void stateTwo(){
    state = 2;
    r = (0x1 << 17);    
    *GPCLR0 = r;
    r = (0x1 << 22);
    *GPCLR0 = r;
    r = (0x1 << 27);    //light up LED connected to pin 27
    *GPSET0 = r;
}
void stateThree(){
    state = 3;
     r = (0x1 << 17);    
    *GPCLR0 = r;
    r = (0x1 << 22);    //light up LED connected to pin 27
    *GPSET0 = r;
    r = (0x1 << 27); 
    *GPCLR0 = r;
}

////////////////////////////////////////////////////////////////////////////////
//
//  Function:       init_pins
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    Set pins 17, 27, 22 to output
//                  Set pins 23, 24 to input
//
////////////////////////////////////////////////////////////////////////////////

void init_pins()
{
    register unsigned int r;
    register unsigned int s;
    
    
    // Get the current contents of the GPIO Function Select Register 1
    r = *GPFSEL1;
    // Get contents of GPFSEL2
    s = *GPFSEL2;

    //clear all bits corresponding to our input/output pins
    r &= ~(0x7 << 21);      // pin 17
    s &= ~(0x7 << 6);       // pin 22
    s &= ~(0x7 << 9);       // pin 23
    s &= ~(0x7 << 12);      // pin 24
    s &= ~(0x7 << 21);      // pin 27

    //set appropriate pins to output (others are already 000 which is input)
    r |= (0x1 << 21);       // pin 17 to output
    s |= (0x1 << 6);        // pin 22 to output
    s |= (0x1 << 21);       // pin 27 to output

    // Write the modified bit patterns back to the registers
    *GPFSEL1 = r;
    *GPFSEL2 = s;

    // Disable internal pull-up/pull-down by setting bits 0:1
    // to 00 in the GPIO Pull-Up/Down Register 
    *GPPUD = 0x0;

    // Wait 150 cycles to provide the required set-up time 
    smallWait();
    // Write to the GPIO Pull-Up/Down Clock Register 0
    *GPPUDCLK0 = (0x1 << 17);
    // Wait 150 cycles to provide the required hold time
    smallWait();
    // Repeat process for each other pin.
    *GPPUDCLK0 = (0x1 << 22);
    smallWait();
    *GPPUDCLK0 = (0x1 << 23);
    smallWait();
    *GPPUDCLK0 = (0x1 << 24);
    smallWait();
    *GPPUDCLK0 = (0x1 << 27);
    smallWait();

    // Clear all bits in the GPIO Pull-Up/Down Clock Register 0
    *GPPUDCLK0 = 0;
    
    // Set pin 23 to falling edge triggered interrupt
    *GPFEN0 = (0x1 << 23);
    // Set pin 24 to rising edge triggered interrupt
    *GPREN0 = (0x1 << 24);
    
    // Enable the GPIO IRQS for ALL the GPIO pins 
    *IRQ_ENABLE_IRQS_2 = (0x1 << 20);
}


// a small waiting subroutine 
void smallWait(){
    r = 150;
    while (r--) {
        asm volatile("nop");
    }
}

// a longer waiting subroutine
void longWait(){
    r = 0x0005FFFF;
    while (r--) {
        asm volatile("nop");
    }
}
