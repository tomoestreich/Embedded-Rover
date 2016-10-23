/* ************************************************************************** */
/** Messaging routine file header

  @Company
    Team 13

  @File Name
    message.h

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
#include "system_config.h"
#include "system_definitions.h"

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
// Normal operational message struct
typedef struct{
    unsigned int src;
    unsigned int dst;
    unsigned int seq;
    unsigned int data;
} data_msg;
 
// Packed message buffer declarations
typedef char msg_buffer[4];

//****** Interface Functions ******//
// Message struct to buffer packing functions
void packMsg(data_msg msg, unsigned char *buffer);

// Buffer to message struct unpacking function
data_msg unpackMsg(unsigned char buffer[4]);

// Create msg struct with all desired data -- set to 0 if not desired
data_msg buildMsg(int src, int dst, int data);

// Function declaration for writing message to the USART 
// NOTE USART MUST BE SET UP TO USE THIS FUNCTION
int sendMessage(data_msg msg);

// Function for reading WiFly buffer into message and performing checksum
int receiveMessage(data_msg *msg);

// USART Initialize function call 
int initUSART(void);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _MESSAGE_H */
/* *****************************************************************************
 End of File
 */