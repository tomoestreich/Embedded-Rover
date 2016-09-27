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

#include "system_config.h"
#include "system_definitions.h"
#include "queue.h"
#include "app_public.h"

char recieveChar(QueueHandle_t queue){
    char character;
    
    xQueueReceive(queue, &character, portMAX_DELAY);
    
    return character;
}

void sendCharFromISR(QueueHandle_t queue, char character){
    xQueueSendToBackFromISR(queue, &character, NULL);
}