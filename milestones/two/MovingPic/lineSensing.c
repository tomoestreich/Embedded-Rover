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

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include "lineSensing.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

extern QueueHandle_t motorQueue;

bool writeToMotorQueue(QueueHandle_t motorQueue, enum LineSituation sit){
    return xQueueSendToBack(motorQueue, sit, portMAX_DELAY); 
}





/* *****************************************************************************
 End of File
 */
