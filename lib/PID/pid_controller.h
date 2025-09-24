#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include "h_bridge.h" // Necessário para o tipo motor_side_t

/**
 * @brief Reseta o estado acumulado (integral, erro anterior) dos controladores PID.
 *
 * Deve ser chamado uma vez na inicialização para garantir um começo limpo.
 */
void pid_reset_state(void);

/**
 * @brief Executa um ciclo completo do controle de velocidade para um motor.
 *
 * Esta função realiza a leitura do encoder, calcula a velocidade atual,
 * computa a saída do PID e envia o comando de PWM para a ponte H.
 *
 * @param side O motor a ser controlado (MOTOR_LEFT ou MOTOR_RIGHT).
 * @param target_rads A velocidade alvo desejada para o motor em radianos por segundo.
 */
void pid_update_motor(motor_side_t side, float target_rads);

#endif // PID_CONTROLLER_H

