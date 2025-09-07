#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "include/button_event_queue_handler_t.h"
#include "../respawn_counter/include/respawn_counter_service_t.h"
#include "../game_mode/include/game_mode_info_t.h"
#include "../game_mode/include/game_mode_service_t.h"

const char *button_event_queue_handler_t::TAG = "button_event_queue_handler_t";
QueueHandle_t button_event_queue_handler_t::queue;

void button_event_queue_handler_t::init()
{
    queue = xQueueCreate(10, sizeof(button_event_t));
    if (queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create button event queue");
        return;
    }
}

void button_event_queue_handler_t::add_to_queue(button_event_t event)
{
    if (xQueueSend(queue, &event, 0) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to send button event to queue");
        return;
    }
}

void button_event_queue_handler_t::task(void *pvParameter)
{
    while (1)
    {
        button_event_t button_event;
        xQueueReceive(queue, &button_event, portMAX_DELAY);
        ESP_LOGI(TAG, "Button event received. GPIO %u. Type: %u. Duration: %lli ms", button_event.gpio_num, button_event.type, button_event.duration / 1000);

        game_mode_info_t *game_mode_info = game_mode_service_t::get();
        switch (game_mode_info->mode)
        {
        case RESPAWN_COUNTER:
            respawn_counter_service_t::handle_button_event(button_event);
            break;
        }
    }
}