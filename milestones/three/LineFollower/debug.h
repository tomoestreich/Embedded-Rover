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
#define DLOC_TASK_WHILE 3
#define DLOC_TASK_BEFORE_QUEUE 4
#define DLOC_TASK_AFTER_QUEUE 5
#define DLOC_ISR_ENTER 6
#define DLOC_ISR_EXIT 7
#define DLOC_ISR_BEFORE_QUEUE 8
#define DLOC_ISR_AFTER_QUEUE 9
#define DLOC_FAILURE_EXIT 255
    
// Location constants for Minotaur rover
#define DLOC_MINO_WRITE_TO_MOTOR    40
#define DLOC_MINO_SEND_INTERSECTION 41
#define DLOC_MINO_RECEIVE           42
    
#define DLOC_MINO_LINE_READ         50
    
#define DLOC_MINO_LINE_INT           51
#define DLOC_MINO_ENCODER_INT        52
#define DLOC_MINO_USART              53
    
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