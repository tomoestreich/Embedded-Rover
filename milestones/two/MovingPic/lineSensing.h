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

#ifndef _LINE_SENSING    /* Guard against multiple inclusion */
#define _LINE_SENSING

#include "system_definitions.h"


enum LineSituation{
    four_way,
    three_way_right,
    three_way_left,
    three_way_back,
    two_way_right,
    two_way_left,
    adjust_right,
    adjust_left,
    deadend
};

    
    bool writeToMotorQueue(QueueHandle_t motorQueue, enum LineSituation sit);

#endif /* _LINE_SENSING */

/* *****************************************************************************
 End of File
 */
