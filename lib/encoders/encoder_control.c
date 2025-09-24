#include "encoder_control.h"

static const char *TAG = "ENCODER_CONTROL";

// Handles para as unidades PCNT
static pcnt_unit_handle_t pcnt_unit_L = NULL;
static pcnt_unit_handle_t pcnt_unit_R = NULL;

// Função para configurar um unico encoder
static void init_single_encoder(encoder_side_t side) {
    pcnt_unit_handle_t* p_pcnt_unit = (side == ENCODER_LEFT) ? &pcnt_unit_L : &pcnt_unit_R;
    int pin_a = ENCODER_A_PIN(side);
    int pin_b = ENCODER_B_PIN(side);

    pcnt_unit_config_t unit_config = {
        .high_limit = PCNT_HIGH_LIMIT,
        .low_limit = PCNT_LOW_LIMIT,
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, p_pcnt_unit));

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(*p_pcnt_unit, &filter_config));

    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = pin_a,
        .level_gpio_num = pin_b,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(*p_pcnt_unit, &chan_a_config, &pcnt_chan_a));
    
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = pin_b,
        .level_gpio_num = pin_a,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(*p_pcnt_unit, &chan_b_config, &pcnt_chan_b));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_ERROR_CHECK(pcnt_unit_enable(*p_pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(*p_pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(*p_pcnt_unit));
}

void encoders_init(void) {
    ESP_LOGI(TAG, "Inicializando encoders...");
    init_single_encoder(ENCODER_LEFT);
    init_single_encoder(ENCODER_RIGHT);
    ESP_LOGI(TAG, "Encoders inicializados.");
}

esp_err_t encoder_get_count(encoder_side_t side, int *count) {
    pcnt_unit_handle_t unit = (side == ENCODER_LEFT) ? pcnt_unit_L : pcnt_unit_R;
    return pcnt_unit_get_count(unit, count);
}

esp_err_t encoder_clear_count(encoder_side_t side) {
    pcnt_unit_handle_t unit = (side == ENCODER_LEFT) ? pcnt_unit_L : pcnt_unit_R;
    return pcnt_unit_clear_count(unit);
}
