// This program sets up GPIO pin 17 as an input pin, and sets it to generate
// an interrupt whenever a rising edge is detected. The pin is assumed to
// be connected to a push button switch on a breadboard. When the button is
// pushed, a 3.3V level will be applied to the pin. The pin should otherwise
// be pulled low with a pull-down resistor of 10K Ohms.

// Include files
#include "uart.h"
#include "sysreg.h"
#include "gpio.h"
#include "irq.h"


// Function prototypes
void init_GPIO23_to_risingEdgeInterrupt();
void init_GPIO24_to_fallingEdgeInterrupt();
void init_GPIO17_to_output();
void init_GPIO22_to_output();
void init_GPIO27_to_output();
void blinkLED();
void longWait();
void setLED1();
void clearLED1();
void setLED2();
void clearLED2();
void setLED3();
void clearLED3();

// Declare a global shared variable
unsigned int mode;



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       main
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function first prints out the values of some system
//                  registers for diagnostic purposes. It then initializes
//                  GPIO pin 17 to be an input pin that generates an interrupt
//                  (IRQ exception) whenever a rising edge occurs on the pin.
//                  The function then goes into an infinite loop, where the
//                  shared global variable is continually checked. If the
//                  interrupt service routine changes the shared variable,
//                  then this is detected in the loop, and the current value
//                  is printed out.
//
////////////////////////////////////////////////////////////////////////////////

void main()
{
    unsigned int r;
    unsigned int localValue;


    // Set up the UART serial port
    uart_init();

    // Query the current exception level
    r = getCurrentEL();

    // Print out the exception level
    uart_puts("Current exception level is:  0x");
    uart_puthex(r);
    uart_puts("\n");

    // Get the SPSel value
    r = getSPSel();

    // Print out the SPSel value
    uart_puts("SPSel is:  0x");
    uart_puthex(r);
    uart_puts("\n");

    // Query the current DAIF flag values
    r = getDAIF();

    // Print out the DAIF flag values
    uart_puts("Initial DAIF flags are:  0x");
    uart_puthex(r);
    uart_puts("\n");

    // Print out initial values of the Interrupt Enable Register 2
    r = *IRQ_ENABLE_IRQS_2;
    uart_puts("Initial IRQ_ENABLE_IRQS_2 is:  0x");
    uart_puthex(r);
    uart_puts("\n");

    // Print out initial values the GPREN0 register (rising edge interrupt
    // enable register)
    r = *GPREN0;
    uart_puts("Initial GPREN0 is:  0x");
    uart_puthex(r);
    uart_puts("\n");


    // Initialize the sharedValue global variable and
    // and set the local variable to be same value
    localValue = 0;
    mode = 0;

    // Set up GPIO pin #17 to input and so that it triggers
    // an interrupt when a rising edge is detected
    init_GPIO23_to_risingEdgeInterrupt();
    init_GPIO24_to_fallingEdgeInterrupt();
    init_GPIO17_to_output();
    init_GPIO22_to_output();
    init_GPIO27_to_output();

    // Enable IRQ Exceptions
    enableIRQ();

    // Query the DAIF flag values
    r = getDAIF();

    // Print out the new DAIF flag values
    uart_puts("\nNew DAIF flags are:  0x");
    uart_puthex(r);
    uart_puts("\n");

    // Print out new value of the Interrupt Enable Register 2
    r = *IRQ_ENABLE_IRQS_2;
    uart_puts("New IRQ_ENABLE_IRQS_2 is:  0x");
    uart_puthex(r);
    uart_puts("\n");

    // Print out new value of the GPREN0 register
    r = *GPREN0;
    uart_puts("New GPREN0 is:  0x");
    uart_puthex(r);
    uart_puts("\n");


    // Print out a message to the console
    uart_puts("\nRising Edge IRQ program starting.\n");

    // Loop forever, waiting for interrupts to change the shared value
    while (1) {
        // Check to see if the shared value was changed by an interrupt
	if (localValue != mode) {
	    // Update the local variable
	    localValue = mode;

	    // Print out the shared value
	    uart_puts("\nsharedValue is:  ");
	    uart_puthex(mode);
	    uart_puts("\n");
      }
        blinkLED();

        // Delay a little using a busy loop
        r = 0x0000FFFF;
    	while (r--) {
      	    asm volatile("nop");
    	}
    }
}
void setLED1(){
  register unsigned int r;
  r = (0x1 << 17);
  *GPSET0 = r;
}
void clearLED1(){
  register unsigned int r;
  r = (0x1 << 17);
  *GPCLR0 = r;
}
void setLED2(){
  register unsigned int r;
  r = (0x1 << 27);
  *GPSET0 = r;
}
void clearLED2(){
  register unsigned int r;
  r = (0x1 << 27);
  *GPCLR0 = r;
}
void setLED3(){
  register unsigned int r;
  r = (0x1 << 22);
  *GPSET0 = r;
}
void clearLED3(){
  register unsigned int r;
  r = (0x1 << 22);
  *GPCLR0 = r;
}
void blinkLED(){
  if(mode == 0){
    setLED1();
    clearLED2();
    clearLED3();
    longWait();
    longWait();
    if (mode != 0) return;

    clearLED1();
    setLED2();
    clearLED3();
    longWait();
    longWait();
    if (mode != 0) return;

    clearLED1();
    clearLED2();
    setLED3();
    longWait();
    longWait();
  }
  if(mode == 1){
    setLED3();
    clearLED2();
    clearLED1();
    longWait();
    if(mode != 1) return

    setLED2();
    clearLED3();
    clearLED1();
    longWait();
    if(mode != 1) return

    setLED1();
    clearLED3();
    clearLED1();
    longWait();
  }
}
void longWait(){
  register unsigned int r;
  r = 0x000FFFFF;
  while (r--) {
      asm volatile("nop");
    }
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       init_GPIO17_to_risingEdgeInterrupt
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function sets GPIO pin 17 to an input pin without
//                  any internal pull-up or pull-down resistors. Note that
//                  a pull-down (or pull-up) resistor must be used externally
//                  on the bread board circuit connected to the pin. Be sure
//                  that the pin high level is 3.3V (definitely NOT 5V).
//                  GPIO pin 17 is also set to trigger an interrupt on a
//                  rising edge, and GPIO interrupts are enabled on the
//                  interrupt controller.
//
////////////////////////////////////////////////////////////////////////////////

void init_GPIO23_to_risingEdgeInterrupt()
{
    register unsigned int r;


    // Get the current contents of the GPIO Function Select Register 1
    r = *GPFSEL2;

    // Clear bits 21 - 23. This is the field FSEL17, which maps to GPIO pin 17.
    // We clear the bits by ANDing with a 000 bit pattern in the field. This
    // sets the pin to be an input pin
    r &= ~(0x7 << 9);

    // Write the modified bit pattern back to the
    // GPIO Function Select Register 1
    *GPFSEL2 = r;

    // Disable the pull-up/pull-down control line for GPIO pin 17. We follow the
    // procedure outlined on page 101 of the BCM2837 ARM Peripherals manual. We
    // will pull down the pin using an external resistor connected to ground.

    // Disable internal pull-up/pull-down by setting bits 0:1
    // to 00 in the GPIO Pull-Up/Down Register
    *GPPUD = 0x0;

    // Wait 150 cycles to provide the required set-up time
    // for the control signal
    r = 150;
    while (r--) {
        asm volatile("nop");
    }

    // Write to the GPIO Pull-Up/Down Clock Register 0, using a 1 on bit 17 to
    // clock in the control signal for GPIO pin 17. Note that all other pins
    // will retain their previous state.
    *GPPUDCLK0 = (0x1 << 23);

    // Wait 150 cycles to provide the required hold time
    // for the control signal
    r = 150;
    while (r--) {
        asm volatile("nop");
    }

    // Clear all bits in the GPIO Pull-Up/Down Clock Register 0
    // in order to remove the clock
    *GPPUDCLK0 = 0;

    // Set pin 17 to so that it generates an interrupt on a rising edge.
    // We do so by setting bit 17 in the GPIO Rising Edge Detect Enable
    // Register 0 to a 1 value (p. 97 in the Broadcom manual).
    *GPREN0 = (0x1 << 23);

    // Enable the GPIO IRQS for ALL the GPIO pins by setting IRQ 52
    // GPIO_int[3] in the Interrupt Enable Register 2 to a 1 value.
    // See p. 117 in the Broadcom Peripherals Manual.
    *IRQ_ENABLE_IRQS_2 = (0x1 << 20);
}
void init_GPIO24_to_fallingEdgeInterrupt()
{
    register unsigned int r;


    // Get the current contents of the GPIO Function Select Register 1
    r = *GPFSEL2;

    // Clear bits 21 - 23. This is the field FSEL17, which maps to GPIO pin 17.
    // We clear the bits by ANDing with a 000 bit pattern in the field. This
    // sets the pin to be an input pin
    r &= ~(0x7 << 12);

    // Write the modified bit pattern back to the
    // GPIO Function Select Register 1
    *GPFSEL2 = r;

    // Disable the pull-up/pull-down control line for GPIO pin 17. We follow the
    // procedure outlined on page 101 of the BCM2837 ARM Peripherals manual. We
    // will pull down the pin using an external resistor connected to ground.

    // Disable internal pull-up/pull-down by setting bits 0:1
    // to 00 in the GPIO Pull-Up/Down Register
    *GPPUD = 0x0;

    // Wait 150 cycles to provide the required set-up time
    // for the control signal
    r = 150;
    while (r--) {
        asm volatile("nop");
    }

    // Write to the GPIO Pull-Up/Down Clock Register 0, using a 1 on bit 17 to
    // clock in the control signal for GPIO pin 17. Note that all other pins
    // will retain their previous state.
    *GPPUDCLK0 = (0x1 << 24);

    // Wait 150 cycles to provide the required hold time
    // for the control signal
    r = 150;
    while (r--) {
        asm volatile("nop");
    }

    // Clear all bits in the GPIO Pull-Up/Down Clock Register 0
    // in order to remove the clock
    *GPPUDCLK0 = 0;

    // Set pin 17 to so that it generates an interrupt on a rising edge.
    // We do so by setting bit 17 in the GPIO Rising Edge Detect Enable
    // Register 0 to a 1 value (p. 97 in the Broadcom manual).
    *GPREN0 = (0x1 << 23);

    // Enable the GPIO IRQS for ALL the GPIO pins by setting IRQ 52
    // GPIO_int[3] in the Interrupt Enable Register 2 to a 1 value.
    // See p. 117 in the Broadcom Peripherals Manual.
    *IRQ_ENABLE_IRQS_2 = (0x1 << 20);
}
void init_GPIO17_to_output(){
  register unsigned int r;


  // Get the current contents of the GPIO Function Select Register 2
  r = *GPFSEL1;

  // Clear bits 9 - 11. This is the field FSEL23, which maps to GPIO pin 23.
  // We clear the bits by ANDing with a 000 bit pattern in the field.
  r &= ~(0x7 << 21);

  // Set the field FSEL23 to 001, which sets pin 23 to an output pin.
  // We do so by ORing the bit pattern 001 into the field.
  r |= (0x1 << 21);

  // Write the modified bit pattern back to the
  // GPIO Function Select Register 2
  *GPFSEL1 = r;

  // Disable the pull-up/pull-down control line for GPIO pin 23. We follow the
  // procedure outlined on page 101 of the BCM2837 ARM Peripherals manual. The
  // internal pull-up and pull-down resistor isn't needed for an output pin.

  // Disable pull-up/pull-down by setting bits 0:1
  // to 00 in the GPIO Pull-Up/Down Register
  *GPPUD = 0x0;

  // Wait 150 cycles to provide the required set-up time
  // for the control signal
  r = 150;
  while (r--) {
    asm volatile("nop");
  }

  // Write to the GPIO Pull-Up/Down Clock Register 0, using a 1 on bit 23 to
  // clock in the control signal for GPIO pin 23. Note that all other pins
  // will retain their previous state.
  *GPPUDCLK0 = (0x1 << 17);

  // Wait 150 cycles to provide the required hold time
  // for the control signal
  r = 150;
  while (r--) {
    asm volatile("nop");
  }

  // Clear all bits in the GPIO Pull-Up/Down Clock Register 0
  // in order to remove the clock
  *GPPUDCLK0 = 0;
}
void init_GPIO22_to_output(){
  register unsigned int r;


  // Get the current contents of the GPIO Function Select Register 2
  r = *GPFSEL2;

  // Clear bits 9 - 11. This is the field FSEL23, which maps to GPIO pin 23.
  // We clear the bits by ANDing with a 000 bit pattern in the field.
  r &= ~(0x7 << 6);

  // Set the field FSEL23 to 001, which sets pin 23 to an output pin.
  // We do so by ORing the bit pattern 001 into the field.
  r |= (0x1 << 6);

  // Write the modified bit pattern back to the
  // GPIO Function Select Register 2
  *GPFSEL2 = r;

  // Disable the pull-up/pull-down control line for GPIO pin 23. We follow the
  // procedure outlined on page 101 of the BCM2837 ARM Peripherals manual. The
  // internal pull-up and pull-down resistor isn't needed for an output pin.

  // Disable pull-up/pull-down by setting bits 0:1
  // to 00 in the GPIO Pull-Up/Down Register
  *GPPUD = 0x0;

  // Wait 150 cycles to provide the required set-up time
  // for the control signal
  r = 150;
  while (r--) {
    asm volatile("nop");
  }

  // Write to the GPIO Pull-Up/Down Clock Register 0, using a 1 on bit 23 to
  // clock in the control signal for GPIO pin 23. Note that all other pins
  // will retain their previous state.
  *GPPUDCLK0 = (0x1 << 22);

  // Wait 150 cycles to provide the required hold time
  // for the control signal
  r = 150;
  while (r--) {
    asm volatile("nop");
  }

  // Clear all bits in the GPIO Pull-Up/Down Clock Register 0
  // in order to remove the clock
  *GPPUDCLK0 = 0;
}
void init_GPIO27_to_output(){
  register unsigned int r;


  // Get the current contents of the GPIO Function Select Register 2
  r = *GPFSEL2;

  // Clear bits 9 - 11. This is the field FSEL23, which maps to GPIO pin 23.
  // We clear the bits by ANDing with a 000 bit pattern in the field.
  r &= ~(0x7 << 21);

  // Set the field FSEL23 to 001, which sets pin 23 to an output pin.
  // We do so by ORing the bit pattern 001 into the field.
  r |= (0x1 << 21);

  // Write the modified bit pattern back to the
  // GPIO Function Select Register 2
  *GPFSEL2 = r;

  // Disable the pull-up/pull-down control line for GPIO pin 23. We follow the
  // procedure outlined on page 101 of the BCM2837 ARM Peripherals manual. The
  // internal pull-up and pull-down resistor isn't needed for an output pin.

  // Disable pull-up/pull-down by setting bits 0:1
  // to 00 in the GPIO Pull-Up/Down Register
  *GPPUD = 0x0;

  // Wait 150 cycles to provide the required set-up time
  // for the control signal
  r = 150;
  while (r--) {
    asm volatile("nop");
  }

  // Write to the GPIO Pull-Up/Down Clock Register 0, using a 1 on bit 23 to
  // clock in the control signal for GPIO pin 23. Note that all other pins
  // will retain their previous state.
  *GPPUDCLK0 = (0x1 << 27);

  // Wait 150 cycles to provide the required hold time
  // for the control signal
  r = 150;
  while (r--) {
    asm volatile("nop");
  }

  // Clear all bits in the GPIO Pull-Up/Down Clock Register 0
  // in order to remove the clock
  *GPPUDCLK0 = 0;
}
