#include "pid_controller.h"
#include "encoder_control.h"
#include "h_bridge.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "PID_CONTROLLER";

// Ganhos PID para o motor Esquerdo
#define PID_KP_LEFT 1.5f
#define PID_KI_LEFT 0.7f
#define PID_KD_LEFT 0.1f

// Ganhos PID para o motor Direito
#define PID_KP_RIGHT 1.5f
#define PID_KI_RIGHT 0.7f
#define PID_KD_RIGHT 0.1f

// Limites da saída PWM 
#define PWM_OUTPUT_MAX 255.0f
#define PWM_OUTPUT_MIN -255.0f

// parametros fisicos
#define PULSES_PER_REVOLUTION 300.0f // pulsos do encoder por volta do motor
#define PI 3.14159265f
#define CONTROL_LOOP_PERIOD_MS 20.0f // deve ser igual ao delay na tarefa principal

// fator de conversão
const float PULSE_TO_RADS_SEC_FACTOR = (2 * PI / PULSES_PER_REVOLUTION) * (1000.0f / CONTROL_LOOP_PERIOD_MS);

// estado interno
static float integral_left = 0.0f;
static float prev_error_left = 0.0f;

static float integral_right = 0.0f;
static float prev_error_right = 0.0f;


void pid_reset_state(void) {
    integral_left = 0.0f;
    prev_error_left = 0.0f;
    integral_right = 0.0f;
    prev_error_right = 0.0f;
}

static float get_current_speed_rads(encoder_side_t side) {
    int pulse_count = 0;
    encoder_get_count(side, &pulse_count);
    encoder_clear_count(side); // medir a variação
    return pulse_count * PULSE_TO_RADS_SEC_FACTOR;
}

void pid_update_motor(motor_side_t side, float target_rads) {
    // Obter a velocidade atual
    float current_rads = get_current_speed_rads((encoder_side_t)side);

    // calculo do erro
    float error = target_rads - current_rads;

    // seleção dos parametros
    float kp, ki, kd;
    float *integral, *prev_error;

    if (side == MOTOR_LEFT) {
        kp = PID_KP_LEFT;
        ki = PID_KI_LEFT;
        kd = PID_KD_LEFT;
        integral = &integral_left;
        prev_error = &prev_error_left;
    } else {
        kp = PID_KP_RIGHT;
        ki = PID_KI_RIGHT;
        kd = PID_KD_RIGHT;
        integral = &integral_right;
        prev_error = &prev_error_right;
    }

    // termo Proporcional
    float p_term = kp * error;

    // termo Integral (com anti-windup)
    *integral += error;
    // limita o acumulador p evitartermo integral crescendo demais
    if (*integral * ki > PWM_OUTPUT_MAX) *integral = PWM_OUTPUT_MAX / ki;
    if (*integral * ki < PWM_OUTPUT_MIN) *integral = PWM_OUTPUT_MIN / ki;
    float i_term = ki * (*integral);

    // termo derivativo
    float d_term = kd * (error - *prev_error);

    // Saida
    float output_pwm = p_term + i_term + d_term;
    *prev_error = error; // Atualiza o erro para o próximo ciclo

    // limita a saída final do PWM
    if (output_pwm > PWM_OUTPUT_MAX) output_pwm = PWM_OUTPUT_MAX;
    if (output_pwm < PWM_OUTPUT_MIN) output_pwm = PWM_OUTPUT_MIN;

    h_bridge_control_motor(side, (int)output_pwm);

    // ESP_LOGI(TAG, "Lado: %d | Alvo: %.2f | Atual: %.2f | PWM: %d", side, target_rads, current_rads, (int)output_pwm);
}
