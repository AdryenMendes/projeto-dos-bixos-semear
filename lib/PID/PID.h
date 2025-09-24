#ifndef PID_H
#define PID_H

// Incluímos a ponte H aqui para poder usar o tipo 'motor_side_t'
#include "h_bridge.h"

/**
 * @brief Zera as variáveis do PID. Chame isso uma vez no começo.
 */
void pid_zera_tudo();

/**
 * @brief Roda um ciclo do PID para um motor.
 *
 * Esta função já lê o encoder, calcula o PID e manda o comando
 * para o motor de uma vez só.
 *
 * @param lado Qual motor usar (MOTOR_LEFT ou MOTOR_RIGHT).
 * @param velocidade_alvo A velocidade que você quer que o motor atinja (em rad/s).
 */
void pid_atualiza_motor(motor_side_t lado, float velocidade_alvo);

#endif 
