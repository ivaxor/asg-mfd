#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "include/button_event_queue_handler_t.h"
#include "include/button_raw_event_queue_handler_t.h"

static const int64_t DEBOUNCE_TIMEOUT_US = 25000;

const char *button_raw_event_queue_handler_t::TAG = "button_raw_event_queue_handler_t";
int64_t button_raw_event_queue_handler_t::last_valid_event_timestamps[GPIO_NUM_MAX] = {0};
int64_t button_raw_event_queue_handler_t::last_press_timestamps[GPIO_NUM_MAX] = {0};
QueueHandle_t button_raw_event_queue_handler_t::queue;

void button_raw_event_queue_handler_t::init()
{
    queue = xQueueCreate(10, sizeof(button_raw_event_t));
    if (queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create button raw event queue");
        return;
    }
}

void button_raw_event_queue_handler_t::add_to_queue_isr(button_raw_event_t *button_raw_event)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(queue, button_raw_event, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE)
    {
        portYIELD_FROM_ISR();
    }
}

void button_raw_event_queue_handler_t::task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");

    while (1)
    {
        button_raw_event_t button_raw_event;
        xQueueReceive(queue, &button_raw_event, portMAX_DELAY);

        int64_t time_since_last_event = button_raw_event.timestamp - last_valid_event_timestamps[button_raw_event.gpio_num];
        if (time_since_last_event < DEBOUNCE_TIMEOUT_US)
        {
            ESP_LOGD(TAG, "Ignoring noisy event for GPIO %u. Time since last: %lld us.", button_raw_event.gpio_num, time_since_last_event);
            continue;
        }
        last_valid_event_timestamps[button_raw_event.gpio_num] = button_raw_event.timestamp;

        ESP_LOGI(TAG, "Button raw event received. GPIO: %u. Pressed: %u. Timestamp: %lli", button_raw_event.gpio_num, button_raw_event.pressed, button_raw_event.timestamp);

        button_event_t button_event;
        button_event.gpio_num = button_raw_event.gpio_num;
        button_event.timestamp = button_raw_event.timestamp;

        switch (button_raw_event.pressed)
        {
        case true:
            button_event.type = PRESS;
            button_event.duration = 0;
            last_press_timestamps[button_raw_event.gpio_num] = button_raw_event.timestamp;
            break;
        case false:
            button_event.type = RELEASE;
            if (last_press_timestamps[button_raw_event.gpio_num] > 0)
                button_event.duration = button_raw_event.timestamp - last_press_timestamps[button_raw_event.gpio_num];
            else
                button_event.duration = 0;
            break;
        }

        button_event_queue_handler_t::add_to_queue(button_event);
    }
}
