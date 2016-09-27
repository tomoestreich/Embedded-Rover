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

char recieveChar(QueueHandle_t queue);

void sendCharFromISR(QueueHandle_t queue, char character);