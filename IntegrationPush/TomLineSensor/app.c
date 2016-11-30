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

uint8_t txBuffer[1] = {0x11};
uint8_t rxBuffer[1] = {0};
uint16_t deviceaddr = 0x7C;


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


/* TODO:  Add any necessary local functions.
 */


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

void APP_Initialize(void) {
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    //DRV_TMR0_Start();
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_TIMER_4);
    DRV_TMR0_Start();
    dbgOutInit();
    init_LineFollower();
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks(void) {

    /* Check the application's current state. */
    switch (appData.state) {
            /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
            if (appInitialized) {

                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            break;
        }


        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


void task_LineFollower(void) {
    DRV_I2C_BUFFER_HANDLE drvI2CRDBUFHandle;
    drvI2CRDBUFHandle = DRV_I2C_TransmitThenReceive(
            appData.i2c_handle,
            deviceaddr,
            txBuffer,
            (sizeof (txBuffer)),
            rxBuffer,
            (sizeof (rxBuffer)),
            NULL);


    findOptions(rxBuffer[0]);
    
    appData.count++;
    //DRV_I2C_Close(appData.i2c_handle);
}

void init_LineFollower(void) {

    // NEVER CLOSE, MIGHT NEED TO DO THAT
    // IF SO ALWAYS OPEN AND CLOSE FOR EVERY READING
    appData.i2c_handle = DRV_I2C_Open(DRV_I2C_INDEX_0, DRV_IO_INTENT_READWRITE);
    if (DRV_HANDLE_INVALID == appData.i2c_handle) {
        dbgOutputVal(0xAA);
        return;
    }
    return;
}

void findOptions(uint8_t sensData) {
    
    uint8_t tempOptions;
    
    
    bool right, left, straight = false;
    
    // always going backwards
    tempOptions = tempOptions | 0x04;
    // Determines if left is an option
    if ((sensData & 0x60) == 0x60) {
        tempOptions = tempOptions | 0x02;
        left = true;
    } else{
        tempOptions = tempOptions & 0xFD;
        left = false;
    }
    // Determines if right is an option
    if ((sensData & 0x06) == 0x06) {
        tempOptions = tempOptions | 0x01;
        right = true;
    } else{
        tempOptions = tempOptions & 0xFE;
        right = false; 
    }
    
    // Determine if straight is an option
    if ((sensData & 0x18) == 0x18) {
        tempOptions = tempOptions | 0x08;
        straight = true;
    } else{
        tempOptions = tempOptions & 0xF7;
        straight = false;
    }
    // Delay .4 seconds before sending options
    // checks to see if straight is an option as well as a turn
    if(appData.count < 400){
        if(sensData == 0x00)
            appData.options = (appData.options | tempOptions) & 0xF7;
        else{
            appData.options = (appData.options | tempOptions);
        }
        
    }
    else{
        dbgOutputLoc(appData.options);
        appData.count = 0;
        appData.options = appData.options & 0x00;
    }
    
    // DETERMINE IF ROVER IS ON TRACK
    checkOnLine(sensData, right, left);    
}


void checkOnLine( uint8_t sensData, bool right, bool left){
    
    uint8_t line2Motor;
    
    // if B2 is lit and not turning right
    if(!right && ((sensData & 0x04) == 0x04)){
        // say move right
        line2Motor = 0x01;
    }
    // if B5 is lit and not turning left
    else if(!left && ((sensData & 0x20) == 0x20)){
        // say move right
        line2Motor = 0x08;
    }
    // if nothing is lit and you cant turn, turn around
    else if(sensData == 0x00 && (!left || !right)) {
        // turn around
        line2Motor = 0x06;
    }
    else{
        line2Motor = 0x00;
    }
    
    dbgOutputVal(line2Motor);
}

/*******************************************************************************
 End of File
 */
