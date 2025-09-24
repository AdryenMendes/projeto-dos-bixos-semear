// encoder_control.c

#include "encoder_control.h"

static const char *TAG = "ENCODER_TEST";

// Handles unidades PCNT
static pcnt_unit_handle_t pcnt_unit_L = NULL;
static pcnt_unit_handle_t pcnt_unit_R = NULL;

// Funçao auxiliar p configurar um unuco encoder
static void init_single_encoder(pcnt_unit_handle_t *unit, int pin_a, int pin_b) {
    pcnt_unit_config_t unit_config = {
        .high_limit = PCNT_HIGH_LIMIT,
        .low_limit = PCNT_LOW_LIMIT,
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, unit));

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(*unit, &filter_config));

    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = pin_a,
        .level_gpio_num = pin_b,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(*unit, &chan_a_config, &pcnt_chan_a));
    
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = pin_b,
        .level_gpio_num = pin_a,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(*unit, &chan_b_config, &pcnt_chan_b));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_ERROR_CHECK(pcnt_unit_enable(*unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(*unit));
    ESP_ERROR_CHECK(pcnt_unit_start(*unit));
}

void encoders_init(void) {
    ESP_LOGI(TAG, "iniciando encoders...");
    init_single_encoder(&pcnt_unit_L, ENCODER_L_A_PIN, ENCODER_L_B_PIN);
    init_single_encoder(&pcnt_unit_R, ENCODER_R_A_PIN, ENCODER_R_B_PIN);
}

// funcao auxiliar p defini velocidade e direcao dum motor
static void set_motor_speed(char motor, int speed) {
    // limitar velocidade de -255 a 255
    if (speed > 255) speed = 255;
    if (speed < -255) speed = -255;

    if (motor == 'L') {
        if (speed >= 0) {
            gpio_set_level(MOTOR_L_IN1_PIN, 1);
            gpio_set_level(MOTOR_L_IN2_PIN, 0);
        } else {
            gpio_set_level(MOTOR_L_IN1_PIN, 0);
            gpio_set_level(MOTOR_L_IN2_PIN, 1);
        }
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_L, abs(speed));
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_L);
    } else if (motor == 'R') {
 
        // Direcao
        if (speed >= 0) {
            gpio_set_level(MOTOR_R_IN1_PIN, 0);
            gpio_set_level(MOTOR_R_IN2_PIN, 1); 
        } else {
            gpio_set_level(MOTOR_R_IN1_PIN, 1); 
            gpio_set_level(MOTOR_R_IN2_PIN, 0);
        }
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_R, abs(speed));
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_R);
    }
}

void motors_init(void) {
    ESP_LOGI(TAG, "Inicializando motores...");
    // pinos de direcao como saida
    gpio_set_direction(MOTOR_L_IN1_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_L_IN2_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_R_IN1_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_R_IN2_PIN, GPIO_MODE_OUTPUT);

    // timer do LEDC
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RESOLUTION,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // canal LEDC p motor esquerdo
    ledc_channel_config_t ledc_channel_L = {
        .gpio_num = MOTOR_L_PWM_PIN,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_L,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_L));

    // canal LEDC p motor direito
    ledc_channel_config_t ledc_channel_R = {
        .gpio_num = MOTOR_R_PWM_PIN,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_R,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_R));
}

void motor_and_encoder_test_task(void *pvParameters) {
    int pulse_count_L = 0;
    int pulse_count_R = 0;

    // velocidade constante p os motores 
    int test_speed = 150; // Velocidade de 0 a 255

    while (1) {
        // move motores
        set_motor_speed('L', test_speed);
        set_motor_speed('R', test_speed);
        
        pcnt_unit_clear_count(pcnt_unit_L);
        pcnt_unit_clear_count(pcnt_unit_R);

        // le contagem de pulsos dos encoders
        ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit_L, &pulse_count_L));
        ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit_R, &pulse_count_R));

        // mostra valores lidos
        ESP_LOGI(TAG, "Contagem Encoder Esquerdo: %d, Contagem Encoder Direito: %d", pulse_count_L, pulse_count_R);

        // espera p ler de novo
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
