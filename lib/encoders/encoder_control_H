
#ifndef ENCODER_CONTROL_H
#define ENCODER_CONTROL_H

#include "freertos/FreeRTOS.h"
#include "driver/pulse_cnt.h"
#include "esp_log.h"

typedef enum {
    ENCODER_LEFT,
    ENCODER_RIGHT
} encoder_side_t;

#define ENCODER_L_A_PIN     35
#define ENCODER_L_B_PIN     32

#define ENCODER_R_A_PIN     34
#define ENCODER_R_B_PIN     39 // Pino 39 é ADC1_CH3, ok para entrada digital

#define ENCODER_A_PIN(side) ((side) == ENCODER_LEFT ? ENCODER_L_A_PIN : ENCODER_R_A_PIN)
#define ENCODER_B_PIN(side) ((side) == ENCODER_LEFT ? ENCODER_L_B_PIN : ENCODER_R_B_PIN)

#define PCNT_HIGH_LIMIT     10000
#define PCNT_LOW_LIMIT      -10000


void encoders_init(void);
esp_err_t encoder_get_count(encoder_side_t side, int *count);
esp_err_t encoder_clear_count(encoder_side_t side);

#endif
