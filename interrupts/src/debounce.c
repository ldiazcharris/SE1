#include "utilities.h"

static debounce_t debounceState;
static delay_t delay_button;
static delay_t delay_default;

#define DELAY_TIME 40
#define DELAY_DEFAULT 40
#define LED3 GPIO_PIN_4

bool debounce_init()
{
    debounceState = BUTTON_UP;
    delay_init(&delay_button, DELAY_TIME);
    delay_init(&delay_default, DELAY_DEFAULT);
    return true;
}

debounce_t debounce_up(int pinState)
{
    switch(debounceState)
    {
    case BUTTON_UP:

        if (pinState == 1)
        {
            delay_read(&delay_default);
            debounceState = BUTTON_FALLING;
        }

        break;

    case BUTTON_FALLING:
        if (delay_read(&delay_default))
        {
            if (pinState == 1)
            {
                debounceState = BUTTON_DOWN;
            }
            else
            {
                debounceState = BUTTON_UP;
            }
        }
        break;
    case BUTTON_DOWN:
        if (pinState == 0)
        {
            delay_read(&delay_default);
            debounceState = BUTTON_RISING;
        }
        break;
    case BUTTON_RISING:
        if (delay_read(&delay_default))
        {
            if (pinState == 0)
            {
                debounceState = BUTTON_UP;
            }
            else
            {
                debounceState = BUTTON_DOWN;
            }
        }
        break;
    default:
        break;

    } // Fin Switch
    return debounceState;
}
