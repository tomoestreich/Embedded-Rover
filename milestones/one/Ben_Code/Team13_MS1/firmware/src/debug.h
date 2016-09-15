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


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "peripheral/ports/plib_ports.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */
// OUTPUT LOCATION CONSTANTS
#define DLOC_TASK_INIT 0
#define DLOC_STATE_INIT 1
#define DLOC_STATE_RUN 2
    
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
    
// Output the provided value to pins 70-77 with LSb being 70
void dbgOutputVal(unsigned char outVal);

// Output the provided location to pins 78-85 with LSb being 78
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
