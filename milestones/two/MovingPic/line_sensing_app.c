/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    line_sensing_app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "line_sensing_app.h"
#include "message.h"
#include "debug.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

LINE_SENSING_APP_DATA line_sensing_appData;

extern QueueHandle_t motorQueue;
QueueHandle_t lineQueue;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


bool writeToMotorQueue(LineSituation sit){
    unsigned char buffer[4];
    
    dbgOutputVal(sit);
    data_msg tx_message = buildMsg(0, 0, 0, sit);
    packMsg(tx_message, buffer);
    
    return xQueueSendToBack(motorQueue, buffer, portMAX_DELAY); 
}

LineSituation readFromLineQueue(){
    LineSituation sit;
    char temp;
    xQueueReceive(lineQueue, &temp, portMAX_DELAY);
    
    switch(temp){
    case 0x0:
        sit = deadend;
        break;
    case 0xff:
        sit = four_way;
        break;
    case 0xf0:
        sit = three_way_left;
        break;
    case 0x0f:
        sit = three_way_right;
        break;
    }
    
    return sit;
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void LINE_SENSING_APP_Initialize ( void )

  Remarks:
    See prototype in line_sensing_app.h.
 */
void LINE_SENSING_APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    line_sensing_appData.state = LINE_SENSING_APP_STATE_INIT;

    lineQueue = xQueueCreate(30, 1);
}


/******************************************************************************
  Function:
    void LINE_SENSING_APP_Tasks ( void )

  Remarks:
    See prototype in line_sensing_app.h.
 */
void LINE_SENSING_APP_Tasks ( void )
{
    LineSituation sit;
    /* Check the application's current state. */
    switch ( line_sensing_appData.state )
    {
        /* Application's initial state. */
        case LINE_SENSING_APP_STATE_INIT:
        {
            bool appInitialized = true;
            if (appInitialized)
            {
                line_sensing_appData.state = LINE_SENSING_APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case LINE_SENSING_APP_STATE_SERVICE_TASKS:
        {
            sit = readFromLineQueue();
            dbgOutputVal(DLOC_MINO_LINE_READ);
            
            writeToMotorQueue(sit);
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
