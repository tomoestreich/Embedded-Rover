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
    PLIB_PORTS_DirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_G, 0x0080);

    // Set desired pins to low
    PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_A, 0x0400);
    PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_B, 0x3800);
    PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_D, 0x0420);
    PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_E, 0x00FF);
    PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_G, 0x0080);

}

/* *****************************************************************************
 End of File
 */
