/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _TASK_EYES_H    /* Guard against multiple inclusion */
#define _TASK_EYES_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "FreeRTOS.h"
#include "queue.h"

bool roverHasBeenSpotted(QueueHandle_t queue);

int numSensorHits(int data);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _TASK_EYES_H */

/* *****************************************************************************
 End of File
 */
