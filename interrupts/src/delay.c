#include "utilities.h"

void delay_init(delay_t *delay, uint32_t duration)
{
    if (delay == 0 || duration > portMAX_DELAY)
        while (1)
            ;
    delay->duration = duration;
    delay->running = 0;
}

bool delay_read(delay_t *delay)
{
    bool timeArrived = false;
    if (delay == 0)
        while (1)
            ;

    if (!delay->running)
    {
        delay->startTime = esp_timer_get_time();
        delay->running = 1;
    }
    else if ((uint32_t)(esp_timer_get_time() - delay->startTime) >= delay->duration)
    {
        timeArrived = true;
        delay->running = 0;
    }

    return timeArrived;
}

void delay_write(delay_t *delay, uint32_t duration)
{
    delay->duration = duration;
    delay->running = 0;
}