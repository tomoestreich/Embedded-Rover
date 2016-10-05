/* ************************************************************************** */
/**  Eyes Thread Source File

  @Company
    Team 13

  @File Name
    eyes.c

  @Summary
    Source code and function definitions for the eye sight process thread

  @Description
    Function definitions for the carrying out of eye thread processs & methods
 */
/* ************************************************************************** */

//****** Include Files ******//
#include "eyes.h"
#include "control.h"

//****** External Global Variables ******//
extern QueueHandle_t ControlQueue;

//****** Global Eyes Thread Variables ******//
EYES_DATA eyesData;
int sensorSimIter;
int prevSimIter;
unsigned char seen;
unsigned char notSeen;
unsigned char sensorBuffer;
int sensorSimData[21] = {0x1, 0x3, 0x2, 0xa, 0xf, 0xf, 0x8, 0x6, 0x4, 0xc, 0xd,
        0xe, 0xa, 0x9, 0x7, 0x1, 0xf, 0xf, 0x0, 0x0, 0x7};
QueueHandle_t EyesQueue;

//****** Eyes Callback Functions ******//

//****** Local Implementation Functions ******//
//****** Check to see if sensors say Rover is seen ******//
bool roverSpotted(int data){
    if(data != 0){ 
        switch(getNumHits(data)){
        // For testing, only 1/2 sensors signifies seen
        case 1:
            return true;
            break;
        case 2:
            return true;
            break;
        default:
            return false;
            break;
        }
    // If no data return false
    }else{
        return false;
    }
}

//****** Function for checking number of hits ******//
int getNumHits(int data){
    int hits = 0;
    int i;
    
    // check each bits for 1/0 and return sum
    for(i=0; i < 4; i++){
        hits = hits + (data >> i) % 2;
    }
    return hits;
}

//****** Initialize Eyes Thread ******//
void EYES_Initialize ( void )
{
    dbgOutputLoc(DLOC_TASK_INIT_EYES);
    
    /* Place the App state machine in its initial state. */
    eyesData.state = EYES_STATE_INIT;
    sensorSimIter = 0;
    prevSimIter = 0;
    
    // Create Eye thread queue
    EyesQueue = xQueueCreate(10, sizeof(int));
    
    // Start the Timer as needed
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_TIMER_2);
    DRV_TMR0_Start();
    
    // Set up thread libraries 
    dbgOutInit();
}


//****** Main Eyes Thread Function ******//
void EYES_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( eyesData.state )
    {
        /* Application's initial state. */
        case EYES_STATE_INIT:
        {
            sensorBuffer = 0x0;
            seen = 0x1;
            notSeen = 0x0;
            bool appInitialized = true;
            dbgOutputLoc(DLOC_STATE_INIT_EYES);
            
            // If ready move to processing state
            if (appInitialized)
            {
                eyesData.state = EYES_STATE_RCV_DATA;
            }
            break;
        }

        // State for waiting to receive sensor data from the timer int/ADC
        case EYES_STATE_RCV_DATA:
        {
            // Display current sensor readings for DEBUGGING PURPOSES
            if (sensorSimIter != prevSimIter){
                dbgOutputVal(0xf & sensorSimData[sensorSimIter]);
            }
            prevSimIter = sensorSimIter;
            
            // Check to see if there is a new value in the buffer
            if(uxQueueMessagesWaiting(EyesQueue)){
                int data;
                
                // Determine what to send based on seen status
                data = xQueueReceive(EyesQueue, &data, 0);
                // Send status to the ControlQueue
                if(roverSpotted(data)){                   
                    sensorBuffer = seen;
                }
                else{
                    sensorBuffer = notSeen;
                }
                eyesData.state = EYES_STATE_SND_DATA;
            }
            break;
        }

        // State for sending packed data to wifly for transmitting
        case EYES_STATE_SND_DATA:
        {
            xQueueSendToBack(ControlQueue, &sensorBuffer, 0);
            eyesData.state = EYES_STATE_RCV_DATA;
            sensorBuffer = 0x0;
            break;
        }

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
