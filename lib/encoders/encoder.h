#ifndef H_BRIDGE_H
#define H_BRIDGE_H

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

typedef enum {
    MOTOR_LEFT,
    MOTOR_RIGHT
} motor_side_t;

#define MOTOR_LEFT_IN_1   GPIO_NUM_26
#define MOTOR_LEFT_IN_2   GPIO_NUM_25
#define MOTOR_LEFT_PWM    GPIO_NUM_33

#define MOTOR_RIGHT_IN_1  GPIO_NUM_19
#define MOTOR_RIGHT_IN_2  GPIO_NUM_18
#define MOTOR_RIGHT_PWM   GPIO_NUM_5

#define LEDC_TIMER          LEDC_TIMER_0
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_RESOLUTION     LEDC_TIMER_8_BIT // Resolução de 8 bits (0-255)
#define LEDC_FREQUENCY      (5000)

#define LEDC_CHANNEL_LEFT   LEDC_CHANNEL_0
#define LEDC_CHANNEL_RIGHT  LEDC_CHANNEL_1

#define MOTOR_IN1_PIN(side)     ((side) == MOTOR_LEFT ? MOTOR_LEFT_IN_1 : MOTOR_RIGHT_IN_1)
#define MOTOR_IN2_PIN(side)     ((side) == MOTOR_LEFT ? MOTOR_LEFT_IN_2 : MOTOR_RIGHT_IN_2)
#define MOTOR_PWM_PIN(side)     ((side) == MOTOR_LEFT ? MOTOR_LEFT_PWM : MOTOR_RIGHT_PWM)
#define LEDC_CHANNEL(side)      ((side) == MOTOR_LEFT ? LEDC_CHANNEL_LEFT : LEDC_CHANNEL_RIGHT)

/**
 * @brief Inicializa os pinos GPIO e os canais PWM para ambos os motores.
 */
void h_bridge_init(void);

/**
 * @brief Controla a velocidade e a direção de um motor específico.
 * @param side O motor a ser controlado (MOTOR_LEFT ou MOTOR_RIGHT).
 * @param speed A velocidade do motor, de -255 (marcha à ré máxima) a 255 (avanço máximo). 0 para o motor.
 */
void h_bridge_control_motor(motor_side_t side, int speed);

#endif // H_BRIDGE_H
