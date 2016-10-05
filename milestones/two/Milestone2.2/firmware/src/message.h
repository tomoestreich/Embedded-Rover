/* ************************************************************************** */
/** Messaging routine file header

  @Company
    Team 13

  @File Name
    debug.h

  @Summary
    Messaging library header for ECE 4534 Team 13 rover

  @Description
    Function declarations for the messaging library as well as message structs
 */
/* ************************************************************************** */

#ifndef _MESSAGE_H    /* Guard against multiple inclusion */
#define _MESSAGE_H

//****** Include Files ******//
#include "FreeRTOS.h"
#include "queue.h"

// C Libraries

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

//****** Constant Declarations ******//
// #define EXAMPLE_CONSTANT 0; 
#define THESEUS_EYES 1
#define THESEUS_CONTROL 2
#define THESEUS_MOTOR 3
#define MINOTAUR_EYES 4
#define MINOTAUR_CONTROL 5
#define MINOTAUR_MOTOR 6
#define RASPBERRY_PI 7
#define DEFAULT 15
    
//******* Data Types *******//
// Message type struct declarations
// Debugging message struct
typedef struct{
    unsigned char src;
    unsigned char dst;
    unsigned char seq;
    unsigned char data[10];
} dbg_msg;

// Normal operational message struct
typedef struct{
    unsigned int src;
    unsigned int dst;
    unsigned int seq;
    unsigned int data;
} data_msg;
 
// Packed message buffer declarations
typedef char dbg_msg_buffer[13];
typedef char msg_buffer[4];

//****** Interface Functions ******//
// Message struct to buffer packing functions
void packMsg(data_msg msg, unsigned char *buffer);

// Debug struct to buffer packing function
void packDbgMsg(dbg_msg msg, unsigned char *buffer);

// Buffer to message struct unpacking function
data_msg unpackMsg(unsigned char buffer[4]);

// NO UNPACK NEEDED FOR DEBUG -- WILL ONLY TRAVEL TO PI

// Create msg struct with all desired data -- set to 0 if not desired
data_msg buildMsg(int src, int dst, int seq, int data);

// Create debug msg struct with desired data -- set to 0 when not desired
dbg_msg buildDbgMsg(int src, int dst, int seq, unsigned char dbg[10]);

// Function declaration for writing message to the USART 
// NOTE USART MUST BE SET UP TO USE THIS FUNCTION
int writeMessage(unsigned char msg[]);

// Function for reading WiFly buffer into message and performing checksum
int readMessage(QueueHandle_t queue, unsigned char *buffer);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _MESSAGE_H */
/* *****************************************************************************
 End of File
 */