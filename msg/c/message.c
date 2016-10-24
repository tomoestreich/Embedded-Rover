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

// C Libraries


//****** Global Variable Declarations ******//
QueueHandle_t wiflyQueue;
unsigned char rxBuffer[4];
BaseType_t pxHigherPriorityTaskWoken;
short msg_itr;
unsigned char seq_num;

//****** Local Functions ******//
// USART TX byte function wrapper
int usartTransmitByte(unsigned char byte){
    // Attempt to send the byte -- Return 1 for success, else 0
    if(!DRV_USART0_TransmitBufferIsFull()){
        DRV_USART0_WriteByte(byte);
        return 1;
    }
    return 0;
}

// Send to Queue Wrapper for wiflyQueue
void sendToWiflyQueue(void){
    xQueueSendToBackFromISR(wiflyQueue, &rxBuffer, &pxHigherPriorityTaskWoken);
}

// Receive from queue wrapper for wiflyQueue
void receiveFromWiflyQueue(unsigned char *buf){
    xQueueReceive(wiflyQueue, buf, portMAX_DELAY);
}

// Local function for generating checksum values -- one byte output
unsigned char getChecksum(unsigned char buffer[4]){
    unsigned char chksum = 0;
    int i, j;
    
    // Loop through bytes in msg calculating number of ones in each byte
    for(i=0; i<3; i++){
        for(j=0; j<8; j++){
            chksum = chksum + ((buffer[i] >> j) & 0x1);
        }
    }
    return chksum; 
}

// Receive interrupt callback function definition
void usartReceiveEventHandler(const SYS_MODULE_INDEX index){
    // Initialize variables
    pxHigherPriorityTaskWoken = pdFALSE;
    
    // If data is present, receive until not present
    while(!DRV_USART0_ReceiverBufferIsEmpty()){
        rxBuffer[msg_itr++] = PLIB_USART_ReceiverByteReceive(USART_ID_1);
        
        // Once full message is received send it along and reinitialize variables
        if(msg_itr == 4){
            // Send the received value to the queue when ready
            sendToWiflyQueue();
            msg_itr = 0;
        }
        // if it is a ping from the pi send it back and clear variables
        else if(rxBuffer[0] == 0x00){
            usartTransmitByte(0x0);
            msg_itr = 0;
        }
    }
    portEND_SWITCHING_ISR(pxHigherPriorityTaskWoken);
}

// Main message packing function--MSG struct as input and msg_buf pointer inputs
void packMsg(data_msg msg, unsigned char *buffer){
    // source and destination in first location
    buffer[0] = (msg.src << 4) & 0xf0;
    buffer[0] = buffer[0] | (msg.dst & 0xf);
    
    // sequence number in second
    buffer[1] = seq_num & 0xff;
    
    // data in the third
    buffer[2] = msg.data & 0xff;
    
    // calculate the current message checksum
    buffer[3] = getChecksum(buffer) & 0xff;
}

// Function definition for unpacking message into struct
data_msg unpackMsg(unsigned char buffer[4]){
    // take buffer bytes and place in desired fields -- return result
    data_msg msg;
    
    // perform checksum -- unpack if valid -- else give error msg struct 
    if (getChecksum(buffer) != buffer[3]){
        msg = buildMsg(0, 0, 0xff);
    }
    else{
        msg.src = (buffer[0] & 0xf0) >> 4;
        msg.dst = buffer[0] & 0xf;
        msg.seq = buffer[1];
        msg.data = buffer[2];
    }
    return msg;
}

//****** library functions ******//
// Function declaration for creating a new message struct
data_msg buildMsg(int src, int dst, int data){
    // set desired values and return it
    data_msg msg;
    msg.src = src;
    msg.dst = dst;
    msg.seq = 0x00;         // Will assign when message is sent
    msg.data = data;
    return msg;
}

// Function for writingString to USART0
int sendMessage(data_msg msg){
    int i = 0;
    msg_buffer buf;
    
    // Check the size of the message after unpacking
    packMsg(msg, buf);
    if(sizeof(buf) != 4){
        return 0;
    }
    
    // Write message byte by byte
    for(i=0; i<4; i++){
        usartTransmitByte(buf[i]);
    }
    seq_num = (seq_num + 1) % 256;
    return 1;
}

// Function for taking a 4byte char array message and unpacking it/checking for 
// common errors
int receiveMessage(data_msg *msg){
    // check if message is available for reading
    msg_buffer buf;
    if(xQueueReceive(wiflyQueue, &buf, portMAX_DELAY) == pdTRUE){
        *msg = unpackMsg(buf);
        return 1;
    }
    else{
        return 0;
    }
}

// Initialize USART function definition
int initUSART(void){
    // Initialize library data members
    msg_itr = 0;
    seq_num = 0x00;
    int j;
    for(j=0; j<4; j++){
        rxBuffer[j] = 0x00;
    }
    
    // Initialize the messsage queue for receiving messages
    wiflyQueue = xQueueCreate(40, 4);
    if(!wiflyQueue){
        return 0;
    }
    // Set the callback function the library callback function
    DRV_USART_ByteReceiveCallbackSet(DRV_USART_INDEX_0, usartReceiveEventHandler);
    
    // Initialize the USART0 interrupt
    SYS_INT_SourceEnable(INT_SOURCE_USART_1_RECEIVE);
    return 1;
}
/* *****************************************************************************
 End of File
 */
