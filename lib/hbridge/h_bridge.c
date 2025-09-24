#include "h_bridge.h"
#include "esp_log.h"
#include <stdlib.h> 

static const char *TAG = "H_BRIDGE";

/**
 * @brief Configura a direção de um motor (para frente ou para trás).
 *
 * @param side O motor a ser configurado.
 * @param forward 'true' para mover para frente, 'false' para mover para trás.
 */
static void set_motor_direction(motor_side_t side, bool forward) {
    if (forward) {
        gpio_set_level(MOTOR_IN1_PIN(side), 1);
        gpio_set_level(MOTOR_IN2_PIN(side), 0);
    } else {
        gpio_set_level(MOTOR_IN1_PIN(side), 0);
        gpio_set_level(MOTOR_IN2_PIN(side), 1);
    }
}

void h_bridge_init(void) {
    // pinos de direção como saídas
    gpio_set_direction(H_BRIDGE_LEFT_IN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(H_BRIDGE_LEFT_IN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(H_BRIDGE_RIGHT_IN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(H_BRIDGE_RIGHT_IN2, GPIO_MODE_OUTPUT);

    // config do pino de Standby
    gpio_set_direction(H_BRIDGE_PIN_STBY, GPIO_MODE_OUTPUT);
    gpio_set_level(H_BRIDGE_PIN_STBY, 1); // Ativa o driver da ponte H

    // config timer do LEDC 
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_RESOLUTION,
        .freq_hz = LEDC_FREQUENCY_HZ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    // configuração canal PWM p motor esquerdo ---
    ledc_channel_config_t left_channel_config = {
        .gpio_num = H_BRIDGE_PWM_LEFT,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_LEFT,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&left_channel_config));

    // configuração canal PWM p motor direito ---
    ledc_channel_config_t right_channel_config = {
        .gpio_num = H_BRIDGE_PWM_RIGHT,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_RIGHT,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&right_channel_config));

    ESP_LOGI(TAG, "Ponte H inicializada com sucesso.");
}

void h_bridge_control_motor(motor_side_t side, int speed) {
    // direção com base no sinal da velocidade
    if (speed >= 0) {
        set_motor_direction(side, true); // Frente
    } else {
        set_motor_direction(side, false); // Ré
    }

    // calculo do duty cycle 
    uint32_t duty = abs(speed);

    // duty cycle não passe limite da resolução
    if (duty > 255) {
        duty = 255;
    }

    // define velocidade e atualiza PWM
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL(side), duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL(side));
}
