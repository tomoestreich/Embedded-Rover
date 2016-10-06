/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    motor_app.c

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

#include "motor_app.h"
#include "debug.h"
#include "line_sensing_app.h"
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

MOTOR_APP_DATA motor_appData;

QueueHandle_t motorQueue;
uint64_t sequence_number;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

void receiveFromMotorQueue(unsigned char * buf){
    xQueueReceive(motorQueue, &buf, portMAX_DELAY);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void MOTOR_APP_Initialize ( void )

  Remarks:
    See prototype in motor_app.h.
 */

void MOTOR_APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    motor_appData.state = MOTOR_APP_STATE_INIT;

    motorQueue = xQueueCreate(40, 4);
    
    if(motorQueue == 0){
        motor_appData.state = -1;
    }
    
    dbgOutInit();
    
    sequence_number = 0;
    
    SYS_INT_SourceEnable(INT_SOURCE_USART_1_RECEIVE);
    
    DRV_TMR0_Start();
    DRV_TMR1_Start();
}


/******************************************************************************
  Function:
    void MOTOR_APP_Tasks ( void )

  Remarks:
    See prototype in motor_app.h.
 */

void MOTOR_APP_Tasks ( void )
{
    unsigned char buffer[4];
    data_msg motor_message, tx_message;
    /* Check the application's current state. */
    switch ( motor_appData.state )
    {
        /* Application's initial state. */
        case MOTOR_APP_STATE_INIT:
        {
            bool appInitialized = true;
       
            if (appInitialized)
            {
                motor_appData.state = MOTOR_APP_STATE_RECEIVE;
            }
            break;
        }

        case MOTOR_APP_STATE_WRITE_TO_MOTOR:
        {
            dbgOutputLoc(DLOC_MINO_WRITE_TO_MOTOR);
            // TODO write to motor somehow
            motor_appData.state = MOTOR_APP_STATE_RECEIVE;
            break;
        }
        
        case MOTOR_APP_STATE_SEND_INTERSECTION:
        {
            dbgOutputLoc(DLOC_MINO_SEND_INTERSECTION);
            LineSituation intersection = motor_message.data;
            
            tx_message = buildMsg(MINOTAUR_MOTOR, MINOTAUR_CONTROL, sequence_number++, intersection);
            packMsg(tx_message, buffer);
            writeMessage(buffer);
            
            motor_appData.state = MOTOR_APP_STATE_RECEIVE;
            break;
        }
        
        case MOTOR_APP_STATE_RECEIVE:
            dbgOutputLoc(DLOC_MINO_RECEIVE);
            receiveFromMotorQueue(buffer);
            motor_message = unpackMsg(buffer);
            
            switch(motor_message.src){
            case THESEUS_CONTROL:
                //Write to motor controls
                dbgOutputLoc(motor_message.data);
                motor_appData.state = MOTOR_APP_STATE_WRITE_TO_MOTOR;
                break;
            case 0: // Internal PIC communication
                switch(motor_message.data & 0x00f0){
                case 0x00C0:    // LineSensor Message
                    if(motor_message.data < ADJUST_RIGHT){
                        motor_appData.state = MOTOR_APP_STATE_SEND_INTERSECTION;
                    }else{
                        // TODO some path correction stuff
                        motor_appData.state = MOTOR_APP_STATE_WRITE_TO_MOTOR;
                    }
                    break;
                case 0x0030:    // Encoder Message
                    // TODO some path correction stuff
                    motor_appData.state = MOTOR_APP_STATE_WRITE_TO_MOTOR;
                    break;
                }
                break;
            }
            break;
        

        /* The default state should never be executed. */
        default:
        {
            procFailure(DLOC_FAILURE_EXIT);
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
