
// Header files
#include "uart.h"
#include "gpio.h"
#include "irq.h"
#include "sysreg.h"

// Reference to the global mode of operation variable
extern unsigned int mode;


////////////////////////////////////////////////////////////////////////////////
//
//  Function:       IRQ_handler
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function first prints out some basic information about
//                  the state of the interrupt controller, GPIO pending
//                  interrupts, and selected system registers. Determines if interrupt is
//                  coming from pin 23 or pin 24. The interrupt is cleared, 
//                  and interrupt is handled to transition to the correct mode.
//
////////////////////////////////////////////////////////////////////////////////

void IRQ_handler()
{
    unsigned int r;

    // Print out exception type
    uart_puts("\nInside IRQ exception handler:\n");

    // Print out further information about the exception    
    r = getCurrentEL();
    uart_puts("    CurrentEL is:  0x");
    uart_puthex(r);
    uart_puts("\n");

    r = getDAIF();
    uart_puts("    DAIF is:  0x");
    uart_puthex(r);
    uart_puts("\n");

    r = *IRQ_PENDING_2;
    uart_puts("    IRQ_PENDING_2 is:  0x");
    uart_puthex(r);
    uart_puts("\n");
    
    r = *GPEDS0;
    uart_puts("    GPEDS0 is:  0x");
    uart_puthex(r);
    uart_puts("\n");
    	

    // Handle GPIO interrupts in general
    if (*IRQ_PENDING_2 == 0x00100000) {
      // Handle the interrupt associated with GPIO pin 23
      if (*GPEDS0 == 0x00020000) {          // !!!!------ must update this number
	   // Clear the interrupt
	   *GPEDS0 = (0x1 << 23); 
	   // change the mode
	   mode = 0;
      }

      //Handle interrupt associated with GPIO pin 24
      if (*GPEDS0 == 0x00040000) {         // !!!------- must update this number
        //Clear the interrupt
        *GPEDS0 = (0x1 <<24);
        //change the mode
        mode = 1;
      }
    } 
    
    // Return to the IRQ exception handler stub
    return;
}