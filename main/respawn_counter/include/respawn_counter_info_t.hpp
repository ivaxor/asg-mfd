#include "freertos/FreeRTOS.h"

#ifndef RESPAWN_COUNTER_INFO_H
#define RESPAWN_COUNTER_INFO_H

typedef enum {
    REVERT,
    TOKENS_INCREASE,
    TOKENS_INCREASE_SELECTED,
    TOKENS_DECREASE,
    TOKENS_DECREASE_SELECTED,
    SAVE,    
} RESPAWN_COUNTER_SETUP_MENU;

typedef struct
{
    uint8_t priority;
    bool disabled;
    uint16_t min;
    uint16_t max;
    uint16_t batch_size;
} respawn_counter_policy_t;

typedef struct
{
    uint16_t respawn_tokens;
    uint16_t current_respawn_tokens;
    respawn_counter_policy_t *policies;
    uint8_t policies_length;
} respawn_counter_info_t;

#endif // RESPAWN_COUNTER_INFO_H