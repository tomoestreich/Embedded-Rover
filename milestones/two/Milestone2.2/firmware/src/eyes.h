/* ************************************************************************** */
/** Eyes Thread Header File

  @Company
    Team 13

  @File Name
    eyes.h

  @Summary
    Header file for the eyes thread and necessary sub methods

  @Description
    Function declarations for the eye thread functionality and communication
 */
/* ************************************************************************** */

#ifndef _EYES_H
#define _EYES_H

// ****** Include Files ******//
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
typedef enum
{
	/* Application's state machine's initial state. */
	EYES_STATE_INIT=0,
	EYES_STATE_RCV_DATA,
    EYES_STATE_SND_DATA,

	/* TODO: Define states used by the application state machine. */

} EYES_STATES;

//****** Eyes Thread Variables/Data ******//
typedef struct
{
    /* The application's current state */
    EYES_STATES state;

    /* TODO: Define any additional data used by the application. */

} EYES_DATA;

//****** Callback Functions ******//
	
//****** Initialization and Eyes Thread Functions ******//
void EYES_Tasks( void );

// End Eyes Thread 
#endif /* _EYES_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END
/*******************************************************************************
 End of File
 */

