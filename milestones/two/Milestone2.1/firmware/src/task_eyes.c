/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#include "task_eyes.h"

#define BITMASK_0 0x0001
#define BITMASK_1 0x0002
#define BITMASK_2 0x0004
#define BITMASK_3 0x0008

#define SENSOR_BL 0
#define SENSOR_TL 1
#define SENSOR_TR 2
#define SENSOR_BR 3

int probabilities[4] = {0, 0, 0, 0};

bool roverHasBeenSpotted(QueueHandle_t queue){
    int data = 0;
        
    xQueueReceive(queue, &data, 0);
    
    if(data != 0){
        
        switch(numSensorHits(data)){
        case 1:
            return true;
            break;
        case 2:
            return true;
            break;
        case 3:
            return false;
            break;
        case 4:
            return false;
            break;
        default:
            return false;
            break;
        }
        
    }else{
        return false;
    }
}

void whichSensors(int data, int * sensors){
    int i = 0;
    
    if(data & BITMASK_0 != 0){
        sensors[i] = SENSOR_BR;
        i++;
    }
    
    if(data & BITMASK_1 != 0){
        sensors[i] = SENSOR_TR;
        i++;
    }
    
    if(data & BITMASK_2 != 0){
        sensors[i] = SENSOR_TL;
        i++;
    }
    
    if(data & BITMASK_3 != 0){
        sensors[i] = SENSOR_BL;
        i++;
    }
}

int numSensorHits(int data){
    int numHits = 0;
    int i;
    for(i=0; i < 4; i++){
        if(data % 2 == 1){
            data = data >> 1;
            numHits++;
        }
    }
    return numHits;
}

/* *****************************************************************************
 End of File
 */
