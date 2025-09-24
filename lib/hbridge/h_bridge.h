#ifndef H_BRIDGE_H
#define H_BRIDGE_H

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

typedef enum {
    MOTOR_LEFT,
    MOTOR_RIGHT
} motor_side_t;


// pino de standby do driver 
#define H_BRIDGE_PIN_STBY       GPIO_NUM_32

// pinos de direção
#define H_BRIDGE_LEFT_IN1       GPIO_NUM_25
#define H_BRIDGE_LEFT_IN2       GPIO_NUM_33
#define H_BRIDGE_RIGHT_IN1      GPIO_NUM_10
#define H_BRIDGE_RIGHT_IN2      GPIO_NUM_15

// pinos de velocidade
#define H_BRIDGE_PWM_LEFT       GPIO_NUM_26
#define H_BRIDGE_PWM_RIGHT      GPIO_NUM_20


// configurações PWM
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE
#define LEDC_RESOLUTION         LEDC_TIMER_8_BIT // Resolução de 8-bit (0-255)
#define LEDC_FREQUENCY_HZ       5000 // Frequência de 5kHz é comum para motores

#define LEDC_CHANNEL_LEFT       LEDC_CHANNEL_0
#define LEDC_CHANNEL_RIGHT      LEDC_CHANNEL_1


// macros auxiliares
#define MOTOR_IN1_PIN(side) ((side) == MOTOR_LEFT ? H_BRIDGE_LEFT_IN1 : H_BRIDGE_RIGHT_IN1)
#define MOTOR_IN2_PIN(side) ((side) == MOTOR_LEFT ? H_BRIDGE_LEFT_IN2 : H_BRIDGE_RIGHT_IN2)

#define LEDC_CHANNEL(side)  ((side) == MOTOR_LEFT ? LEDC_CHANNEL_LEFT : LEDC_CHANNEL_RIGHT)


/**
 * @brief Inicializa todos os GPIOs e canais PWM necessários para a ponte H.
 *
 * Esta função deve ser chamada uma única vez durante a inicialização do sistema.
 * Ela configura ambos os motores e o timer PWM compartilhado.
 */
void h_bridge_init(void);

/**
 * @brief Controla a velocidade e a direção de um motor específico.
 *
 * @param side O motor a ser controlado (MOTOR_LEFT ou MOTOR_RIGHT).
 * @param speed A velocidade desejada, de -255 (ré máxima) a 255 (frente máxima).
 *              Um valor de 0 para o motor.
 */
void h_bridge_control_motor(motor_side_t side, int speed);

#endif 
