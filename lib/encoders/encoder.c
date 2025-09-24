#include "h_bridge.h"
#include <math.h>

void h_bridge_init(void) {
    gpio_set_direction(MOTOR_LEFT_IN_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_LEFT_IN_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_RIGHT_IN_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_RIGHT_IN_2, GPIO_MODE_OUTPUT);

    //  Timer do LEDC (PWM) 
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_RESOLUTION,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Canal LEDC para o Motor Esquerdo ---
    ledc_channel_config_t ledc_channel_L = {
        .gpio_num = MOTOR_PWM_PIN(MOTOR_LEFT),
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL(MOTOR_LEFT),
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_L));

    // Canal LEDC para o Motor Direito ---
    ledc_channel_config_t ledc_channel_R = {
        .gpio_num = MOTOR_PWM_PIN(MOTOR_RIGHT),
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL(MOTOR_RIGHT),
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_R));
}

void h_bridge_control_motor(motor_side_t side, int speed) {
    // Limita a velocidade ao intervalo da resolução do PWM
    if (speed > 255) speed = 255;
    if (speed < -255) speed = -255;

    // speed > 0 para a frente, speed < 0 para trás
    if (speed >= 0) {
        gpio_set_level(MOTOR_IN1_PIN(side), 1);
        gpio_set_level(MOTOR_IN2_PIN(side), 0);
    } else {
        gpio_set_level(MOTOR_IN1_PIN(side), 0);
        gpio_set_level(MOTOR_IN2_PIN(side), 1);
    }

    // Define o duty cycle do PWM
    uint32_t duty = abs(speed);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL(side), duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL(side));
}```

