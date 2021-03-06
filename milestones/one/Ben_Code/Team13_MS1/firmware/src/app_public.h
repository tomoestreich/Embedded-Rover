/* ************************************************************************** */
/** Timer ISR Callback function header file

  @Company
    Team 13

  @File Name
    tmr_callback.h

  @Summary
    Timer interrupt callback function header

  @Description
    To declare all functions for handling timer interrupt functionality
 */
/* ************************************************************************** */

#ifndef _TMR_CALLBACK_H    /* Guard against multiple inclusion */
#define _TMR_CALLBACK_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "peripheral/ports/plib_ports.h"

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */

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
void vTMR0_Callback(void);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
