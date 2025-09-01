#include "freertos/FreeRTOS.h"

#ifndef BUTTON_ISR_HANDLER_H
#define BUTTON_ISR_HANDLER_H

class button_isr_handler_t
{
private:
    static void IRAM_ATTR handler(void *arg);

public:
    void init();
};

extern button_isr_handler_t button_isr_handler;

#endif // BUTTON_ISR_HANDLER_H