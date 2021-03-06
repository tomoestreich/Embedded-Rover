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

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "task_eyes.h"

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


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


bool writeString(char * str){
    //int size = strlen(str);
    //DRV_USART0_Write(str, size);
    
    if(*str == '\0')
    {
        return true;
    }

    /* Write a character at a time, only if transmitter is empty */
    while (1)
    {
        /* Send character */
        if(!DRV_USART0_TransmitBufferIsFull()){
            DRV_USART0_WriteByte(*str);

            /* Increment to address of next character */
            str++;
        }

        if(*str == '\0')
        {
            return true;
        }
    }
    return false;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_TIMER_2);
    
    //PLIB_USART_Enable(USART_ID_1);
    //DRV_USART0_Initialize();
    
    eyesQueue = xQueueCreate(10, sizeof(int));
    rxQueue = xQueueCreate(20, sizeof(char));

    sensorDataI = 0;
    
    TRISA = 0;
    LATA = PORTA & 0x0;
    
    DRV_TMR0_Start();
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    char readbuf[255];
    int i = 0;
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
            if(uxQueueMessagesWaiting(eyesQueue)){
                if(roverHasBeenSpotted(eyesQueue)){
                    LATA = PORTA | 0x0008;
                    
                    writeString("SEEN");
                    
                    appData.state = WAIT_FOR_CONFIRMATION;
                }
            }
            break;
        }

        case WAIT_FOR_CONFIRMATION:
        {
                //int readBytes = DRV_USART0_Read(readbuf, 254);
            if(xQueueReceive(rxQueue, &readbuf, portMAX_DELAY) == pdTRUE){
                if(readbuf[0] == 'o'){
                    appData.state = APP_STATE_SERVICE_TASKS;
                    LATA = PORTA & 0x0;
                    readbuf[0] = 0;
                }
            }
            break;
        }
        

        /* The default state should never be executed. */
        default:
        {
            LATA = PORTA & 0x0;
            break;
        }
    }
} 

/*******************************************************************************
 End of File
 */
