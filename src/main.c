#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "h_bridge.h"
#include "encoder_control.h"
#include "pid_controller.h" 

static const char *TAG = "ROBOT_MAIN";

// período do loop de controle
#define CONTROL_LOOP_PERIOD_MS 20

/**
 * @brief Tarefa principal que executa o loop de controle do robô.
 */
void robot_control_task(void *pvParameters) {
    // Velocidade alvo
    const float target_speed = 3.14159f; // Meia volta por segundo

    // garantir ciclo de execução preciso
    TickType_t last_wake_time = xTaskGetTickCount();

    while (1) {
        // controle PID p ambos os motores
        pid_update_motor(MOTOR_LEFT, target_speed);
        pid_update_motor(MOTOR_RIGHT, target_speed);

        // aguarda próximo ciclo
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(CONTROL_LOOP_PERIOD_MS));
    }
}

void app_main(void) {
    // inicializa os módulos hardware
    h_bridge_init();
    encoders_init();

    // reseta o estado do controlador PID
    pid_reset_state();
    ESP_LOGI(TAG, "Hardware e controlador PID inicializados.");

    // cria a tarefa de controle
    xTaskCreate(robot_control_task, "RobotControlTask", 4096, NULL, 5, NULL);
}
