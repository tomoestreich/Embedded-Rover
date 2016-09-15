/* ************************************************************************** */
/** Debug Routine Source File

  @Company
    Team 13

  @File Name
    debug.c

  @Summary
    Debugging library source code for ECE 4534 Team 13 rover

  @Description
    Function definitions for the debugging library for general rover processes
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "debug.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */
void dbgOutInit(void)
{
    // Set pins as outputs 
    PLIB_PORTS_DirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_A, 0x0400);
    PLIB_PORTS_DirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_B, 0x3800);
    PLIB_PORTS_DirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_D, 0x0420);
    PLIB_PORTS_DirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_E, 0x00FF);
    //PLIB_PORTS_DirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_F, 0x0001);
    PLIB_PORTS_DirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_G, 0x0180);

    // Set desired pins to low
    //PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_A, 0x0400);
    //PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_B, 0x3800);
    //PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_D, 0x0420);
    //PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_E, 0x00FF);
    //PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_F, 0x0001);
    //PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_G, 0x0180);
    //PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_8, 0);

}

// Function for placing a value on pins 29-43 odds only (29LSb, 43 MSb)
void dbgOutputVal(unsigned char outVal)
{
    // Set GPIO pins based on bits of outLoc
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_7, (outVal & 0x01));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_6, (outVal & 0x02));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_4, (outVal & 0x04));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_2, (outVal & 0x08));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_0, (outVal & 0x10));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_5, (outVal & 0x20));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_13, (outVal & 0x40));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_8, (outVal & 0x80));
}

// Function for placing a location value on pins 30-44 even only (30LSb, 44 MSb)
void dbgOutputLoc(unsigned char outLoc)
{
    // Set GPIO pins based on bits of outLoc
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_7, (outLoc & 0x01));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_5, (outLoc & 0x02));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_3, (outLoc & 0x04));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_E, PORTS_BIT_POS_1, (outLoc & 0x08));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_10, (outLoc & 0x10));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_11, (outLoc & 0x20));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_12, (outLoc & 0x40));
    PLIB_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_10, (outLoc & 0x80));
}

void procFailure(unsigned char outLoc)
{
    // output the location, stop the timer, light LED5, and enter infinite loop
    dbgOutputLoc(outLoc);
    DRV_TMR0_Stop();
    LATA = PORTA | 0x0008;
    DRV_TMR0_Stop();
    while(1){}
}

/* *****************************************************************************
 End of File
 */
