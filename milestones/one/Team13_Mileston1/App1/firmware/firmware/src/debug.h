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
