#ifndef ENCODER_CONTROL_H
#define ENCODER_CONTROL_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

// Motor Esquerdo
#define MOTOR_L_IN1_PIN         26  // pino de direcao 1
#define MOTOR_L_IN2_PIN         25  // pino de direcao2
#define MOTOR_L_PWM_PIN         33  // pino de velocidade
#define ENCODER_L_A_PIN         35  // pino A do encode
#define ENCODER_L_B_PIN         32  // pino B do encoder

// Motor Direito
#define MOTOR_R_IN1_PIN         19  // pino de direcao 1
#define MOTOR_R_IN2_PIN         18  // pino de direcao 2
#define MOTOR_R_PWM_PIN         5   // pino de velocidade (PWM)
#define ENCODER_R_A_PIN         34  // pino A do encoder
#define ENCODER_R_B_PIN         39  // Ppno B do encoder

// PWM 
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL_L          LEDC_CHANNEL_0
#define LEDC_CHANNEL_R          LEDC_CHANNEL_1
#define LEDC_DUTY_RESOLUTION    LEDC_TIMER_8_BIT
#define LEDC_FREQUENCY          5000 // frequencia 

// PCNT
#define PCNT_HIGH_LIMIT         10000
#define PCNT_LOW_LIMIT          -10000


void motors_init(void);

void encoders_init(void);

void motor_and_encoder_test_task(void *pvParameters);

#endif // ENCODER_CONTROL_H
