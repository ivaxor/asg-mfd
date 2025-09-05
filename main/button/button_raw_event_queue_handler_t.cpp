#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "include/button_event_queue_handler_t.h"
#include "include/button_raw_event_queue_handler_t.h"

const char *button_raw_event_queue_handler_t::TAG = "button_raw_event_queue_handler_t";
int64_t button_raw_event_queue_handler_t::press_timestamps[GPIO_NUM_MAX];
int64_t button_raw_event_queue_handler_t::click_timestamps[GPIO_NUM_MAX];
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
    xQueueSendFromISR(queue, button_raw_event, NULL);
}

void button_raw_event_queue_handler_t::task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting task");
    
    while (1)
    {
        button_raw_event_t button_raw_event;
        xQueueReceive(queue, &button_raw_event, portMAX_DELAY);
        ESP_LOGI(TAG, "Button raw event received. GPIO: %u. Pressed: %u. Timestamp: %lli", button_raw_event.gpio_num, button_raw_event.pressed, button_raw_event.timestamp);

        button_event_t button_event;
        int64_t lastPressTimestamp = press_timestamps[button_raw_event.gpio_num];

        // 25 ms
        if (lastPressTimestamp != 0 && (button_raw_event.timestamp - lastPressTimestamp) < 25000)
        {
            ESP_LOGW(TAG, "Ignoring button event to debounce");
            continue;
        }

        switch (button_raw_event.pressed)
        {
        case 0:
            if (lastPressTimestamp == 0)
            {
                ESP_LOGW(TAG, "Double depress detected");
                continue;
            }

            button_event.type = CLICK;
            button_event.duration = button_raw_event.timestamp - lastPressTimestamp;
            button_event.gpio_num = button_raw_event.gpio_num;

            press_timestamps[button_raw_event.gpio_num] = 0;
            break;

        case 1:
            if (lastPressTimestamp != 0)
            {
                ESP_LOGW(TAG, "Double press detected");
                continue;
            }

            button_event.type = PRESS;
            button_event.duration = 0;
            button_event.gpio_num = button_raw_event.gpio_num;

            press_timestamps[button_raw_event.gpio_num] = button_raw_event.timestamp;
            break;
        }

        button_event_queue_handler_t::add_to_queue(button_event);
    }
}