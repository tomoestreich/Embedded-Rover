/* ************************************************************************** */
/** Control Thread Header File

  @Company
    Team 13

  @File Name
    control.h

  @Summary
    Header file for the control thread and necessary sub methods

  @Description
    Function declarations for the control thread functionality and communication
 */
/* ************************************************************************** */

#ifndef _CONTROL_H
#define _CONTROL_H

//****** Include Files ******//
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "message.h"
#include "debug.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END 

//****** Eyes Thread Typedefs ******//
// Control Application States
typedef enum
{
	/* Application's state machine's initial state. */
	CONTROL_STATE_INIT=0,
	CONTROL_STATE_SERVICE_TASKS,

	/* TODO: Define states used by the application state machine. */

} CONTROL_STATES;


//****** Eyes Thread Data and Variables ******//
typedef struct
{
    /* The application's current state */
    CONTROL_STATES state;

    /* TODO: Define any additional data used by the application. */

} CONTROL_DATA;

//****** Callback Functions ******//
// Control Thread Initializer -- No Inputs, No outputs, Starts necessary
// subcomponents and methods
void CONTROL_Initialize ( void );

// Main Control thread function for carrying out desired operations
void CONTROL_Tasks( void );

#endif /* _CONTROL_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

