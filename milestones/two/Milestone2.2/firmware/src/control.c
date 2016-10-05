/* ************************************************************************** */
/**  Control Thread Source File

  @Company
    Team 13

  @File Name
    control.c

  @Summary
    Source code and function definitions for the control process thread

  @Description
    Function definitions for the carrying out of control process methods
 */
/* ************************************************************************** */


//****** Include Files ******//
#include "control.h"

//****** External Variable Declarations ******//

//****** Global Variable Declarations ******//
CONTROL_DATA controlData;
QueueHandle_t ControlQueue;

//****** Callback Functions ******//

//****** Local Functions ******//

//****** Control Init and Control Function Definitions ******//
// Initialize functions
void CONTROL_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    controlData.state = CONTROL_STATE_INIT;

    // Create the controls queue -- acts as WiFly receive queue
    ControlQueue = xQueueCreate(20, sizeof(dbg_msg_buffer));
    
    // initialize start necessary peripheral modules and methods
}

// Control Thread Main Function
void CONTROL_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( controlData.state )
    {
        /* Application's initial state. */
        case CONTROL_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
                dbgOutputLoc(0xcc);
                controlData.state = CONTROL_STATE_SERVICE_TASKS;
            }
            break;
        }

        case CONTROL_STATE_SERVICE_TASKS:
        {
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
