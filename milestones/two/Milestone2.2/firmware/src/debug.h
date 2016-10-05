/* ************************************************************************** */
/** Debug Routine Header File

  @Company
    Team 13

  @File Name
    debug.h

  @Summary
    Debugging library header for ECE 4534 Team 13 rover

  @Description
    Function declarations for the debugging library for general rover processes
 */
/* ************************************************************************** */

#ifndef _DEBUG_H    /* Guard against multiple inclusion */
#define _DEBUG_H


//****** Include Files ******//
#include "peripheral/ports/plib_ports.h"
#include "message.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


//****** Debug Constant and Variable Definitions ******//
// OUTPUT LOCATION CONSTANTS
#define DLOC_TASK_INIT_EYES 0
#define DLOC_STATE_INIT_EYES 1
#define DLOC_STATE_BEGIN_TX 2
#define DLOC_TASK_WHILE 3
#define DLOC_TASK_BEFORE_QUEUE 4
#define DLOC_TASK_AFTER_QUEUE 5
#define DLOC_ISR_ENTER 6
#define DLOC_ISR_EXIT 7
#define DLOC_ISR_BEFORE_QUEUE 8
#define DLOC_ISR_AFTER_QUEUE 9
#define DLOC_STATE_TX_EYE_COMPLETE 0xAA
#define DLOC_STATE_DEFAULT 0x55
#define DLOC_STATE_READ_USART 0xc3
#define DLOC_STATE_USART_DREADY 0xff
    
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// ****************************************************************************
    
// Initialize the debugger output pins
void dbgOutInit(void);
    
// Function for placing a value on pins 29-43 odds only (29LSb, 43 MSb)
void dbgOutputVal(unsigned char outVal);

// Function for placing a location value on pins 30-44 even only (30LSb, 44 MSb)
void dbgOutputLoc(unsigned char outLoc);

// Output desired location using dbgLoc, Turn on LED4 and prevent further action
void procFailure(unsigned char outLoc);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
