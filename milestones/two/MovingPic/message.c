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

//****** Include Files ******//
// Local
#include "message.h"
#include "debug.h"

// C Libraries


//****** Global Variable Declarations ******//
//int global_data;


//****** Local Functions ******//
// Local function for generating checksum values -- one byte output
unsigned char getChecksum(unsigned char buffer[]){
    unsigned char chksum;
    int i, byte;
    int bytes = sizeof(buffer) - 1;
    
    // Loop through bytes in msg calculating number of ones in each byte
    while (byte < bytes){
        for (i=0; i<8; i++){
            chksum = chksum + (buffer[byte] >> i) % 2;
        }
        byte++;
    }
    return chksum; 
}

// Messaging library functions
// Main message packing function--MSG struct as input and msg_buf pointer inputs
void packMsg(data_msg msg, unsigned char *buffer){
    // source and destination in first location
    buffer[0] = (msg.src << 4) & 0xf0;
    buffer[0] = buffer[0] | (msg.dst & 0xf);
    
    // sequence number in second
    buffer[1] = msg.seq & 0xff;
    
    // data in the third
    buffer[2] = msg.data & 0xff;
    
    // calculate the current message checksum
    buffer[3] = getChecksum(buffer) & 0xff;
}

// Debug message packing function--msg struct and msg_buf pointer as inputs
void packDbgMsg(dbg_msg msg, unsigned char *buffer){
    // source and destination in first location
    buffer[0] = (msg.src << 4) & 0xf0;
    buffer[0] = buffer[0] | (msg.dst & 0xf);
    
    // sequence number in second
    buffer[1] = msg.seq & 0xff;
    
    // loop through debug message data storing in buffer
    int i;
    for (i=0; i<10; i++){
        buffer[i+2] = msg.data[i];
    }
    
    // calculate the current message checksum
    buffer[12] = getChecksum(buffer) & 0xff;
}

// Function definition for unpacking message into struct
void unpackMsg(unsigned char buffer[4], data_msg * msg){
    // take buffer bytes and place in desired fields -- return result
//    data_msg msg;
    msg->src = (buffer[0] & 0xf0) >> 4;
    msg->dst = buffer[0] & 0xf;
    msg->seq = buffer[1];
    msg->data = buffer[2];
//    if (buffer[3] != getChecksum(buffer)){
//        msg.data = 0xff;
//    }
//    return msg;
}

// Function declaration for creating a new message struct
data_msg buildMsg(int src, int dst, int seq, int data){
    // set desired values and return it
    data_msg msg;
    msg.src = src;
    msg.dst = dst;
    msg.seq = seq;
    msg.data = data;
    return msg;
}

// Function declaration for creating a new debug message struct
dbg_msg buildDbgMsg(int src, int dst, int seq, unsigned char dbg[10]){
    // set desired values and return it
    dbg_msg msg;
    msg.src = src;
    msg.dst = dst;
    msg.seq = seq;
    
    // loop through the message adding the struct data and return struct
    int i;
    for (i=0; i<10; i++){
        msg.data[i] = dbg[i];
    }
    return msg;
}

// Function for writingString to USART0
int writeMessage(unsigned char msg[]){
    int i = 0;

    // Write message byte by byte if transmitter is ready
    while (1)
    {
        /* Send character */
        if(!DRV_USART0_TransmitBufferIsFull()){
            // Write byte to uart and increment to next location
            DRV_USART0_WriteByte(msg[i]);
            i++;
            if (i == sizeof(msg)){
                return 1;
            }
        }
    }
    return 0;
}
/* *****************************************************************************
 End of File
 */
