
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
    // Handle GPIO interrupts in general
    if (*IRQ_PENDING_2 == 0x00100000) {
      uart_puts("tick");
        // Handle the interrupt associated with GPIO pin 23
        if (*GPEDS0 == (0x1 << 23)) {
            // Clear the interrupt
            *GPEDS0 = (0x1 << 23);
            // change the mode
            mode = 0;
        }

        //Handle interrupt associated with GPIO pin 24
        if (*GPEDS0 == (0x1 << 24)) {
            //Clear the interrupt
            *GPEDS0 = (0x1 << 24);
            //change the mode
            mode = 1;
        }
    }

    // Return to the IRQ exception handler stub
    return;
}
