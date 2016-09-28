/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

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

//****** Include Files ******//
#include "app.h"
#include "task_eyes.h"
#include "debug.h"
#include "message.h"

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

APP_DATA appData;
QueueHandle_t eyesQueue;
QueueHandle_t rxQueue;
int sensorData[21] = {0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf,
                    0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0x1};
int sensorDataI;
bool checkReceived;
int SEEN = 0;
data_msg EYES_MSG;
unsigned char msg[4];
data_msg RX_MSG;

/* TODO:  Add any necessary callback functions.
*/

//****** Application Initialization Function ******//
void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    EYES_MSG = buildMsg(THESEUS_EYES, MINOTAUR_CONTROL, 0, 0x0);
    appData.state = APP_STATE_INIT;
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_TIMER_2);
    
    eyesQueue = xQueueCreate(10, sizeof(int));
    rxQueue = xQueueCreate(20, sizeof(char));
    sensorDataI = 0; 
    
    // Start TMR0 and Initialize the debugger
    DRV_TMR0_Start();
    dbgOutInit();
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    unsigned char readbuf[255];
    
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:{
            bool appInitialized = true;
            if (appInitialized){
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            dbgOutputLoc(0x40);
            
            if(uxQueueMessagesWaiting(eyesQueue)){
                SEEN = 0x18;
                if(roverHasBeenSpotted(eyesQueue)){
                    SEEN = 0x81;
                }
                EYES_MSG.data = SEEN;
                packMsg(EYES_MSG, msg);
                writeMessage(msg);
                EYES_MSG.seq++;
                appData.state = WAIT_FOR_CONFIRMATION;
            }
            break;
        }

        case WAIT_FOR_CONFIRMATION:
        {           
            dbgOutputLoc(0x80);
            
            // Read the message when it becomes available and update debugger
            if(xQueueReceive(rxQueue, &readbuf, portMAX_DELAY) == pdTRUE){
                RX_MSG = unpackMsg(readbuf);
                appData.state = APP_STATE_SERVICE_TASKS;
                if (RX_MSG.data == 0x18){
                    dbgOutputVal(0x1);
                }
                else{
                    dbgOutputVal(0x2);
                }
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }
        

        /* The default state should never be executed. */
        default:
        {
            break;
        }
    }
} 

/*******************************************************************************
 End of File
 */
