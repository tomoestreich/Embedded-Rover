/*******************************************************************************
 System Interrupts File

  File Name:
    system_interrupt.c

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the
    interrupt sub-system.

  Summary:
    This file contains source code for the interrupt vector functions in the
    system.

  Description:
    This file contains source code for the interrupt vector functions in the
    system.  It implements the system and part specific vector "stub" functions
    from which the individual "Tasks" functions are called for any modules
    executing interrupt-driven in the MPLAB Harmony system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    interrupt-driven in the system.  These handles are passed into the individual
    module "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <xc.h>
#include <sys/attribs.h>
#include "line_sensing_app.h"
#include "motor_app.h"
#include "system_definitions.h"
#include "debug.h"

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************

extern QueueHandle_t lineQueue;
char lineSensor[4] = {0x0, 0xff, 0x0f, 0xf0};
uint64_t line_sensor_i = 0;

extern QueueHandle_t motorQueue;
char encoder[5] = {0x0, 0x3, 0x9, 0xc, 0xf};
uint64_t encoder_i = 0;

char usart_buffer[4];
int usart_i = 0;

// Line Sensor Interrupt
void IntHandlerDrvTmrInstance0(void)
{
    dbgOutputVal(DLOC_MINO_LINE_INT);
    BaseType_t pxHigherPriorityTaskWoken=pdFALSE;

    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_2);
    xQueueSendToBackFromISR(lineQueue, &lineSensor[line_sensor_i%4], &pxHigherPriorityTaskWoken);
    line_sensor_i++;
    
    dbgOutputVal(0);
    portEND_SWITCHING_ISR(pxHigherPriorityTaskWoken);
}

// Motor/Encoder Interrupt
void IntHandlerDrvTmrInstance1(void)
{
    dbgOutputLoc(DLOC_MINO_ENCODER_INT);
    BaseType_t pxHigherPriorityTaskWoken=pdFALSE;
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_4);
    
    xQueueSendToBackFromISR(motorQueue, &encoder[encoder_i%4], &pxHigherPriorityTaskWoken);
    encoder_i++;
    
    //TODO write to motor
    
    portEND_SWITCHING_ISR(pxHigherPriorityTaskWoken);
}

 void IntHandlerDrvUsartInstance0(void)
{
    dbgOutputLoc(DLOC_MINO_USART);
    BaseType_t pxHigherPriorityTaskWoken=pdFALSE;
    if(!DRV_USART0_ReceiverBufferIsEmpty()){
        usart_buffer[usart_i++] = DRV_USART0_ReadByte();
        if(usart_i == 4){
            xQueueSendToBackFromISR(motorQueue, &usart_buffer, &pxHigherPriorityTaskWoken);
            usart_i = 0;
        }
    }
    dbgOutputLoc(DLOC_MINO_USART + 1);
     
    DRV_USART_TasksTransmit(sysObj.drvUsart0);
    DRV_USART_TasksReceive(sysObj.drvUsart0);
    DRV_USART_TasksError(sysObj.drvUsart0);
    
    portEND_SWITCHING_ISR(pxHigherPriorityTaskWoken);
}
 
 
 

 

 

 

 

 
  
/*******************************************************************************
 End of File
*/

