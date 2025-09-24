#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "h_bridge.h"  
#include "encoder_control.h"
#include "pid_controller.h"

static const char *TAG = "ROBOT_MAIN";

#define CONTROL_LOOP_PERIOD_MS 20

void robot_control_task(void *pvParameters) {
    const float target_speed = 3.14159f;
    TickType_t last_wake_time = xTaskGetTickCount();

    while (1) {
        pid_update_motor(MOTOR_LEFT, target_speed);
        pid_update_motor(MOTOR_RIGHT, target_speed);
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(CONTROL_LOOP_PERIOD_MS));
    }
}

void app_main(void) {
    // inicia módulos de hardware
    h_bridge_init(); 
    encoders_init();

    // reseta o estado do controlador PID
    pid_reset_state();
    ESP_LOGI(TAG, "Hardware e controlador PID inicializados.");

    // cria a tarefa de controle do robô
    xTaskCreate(robot_control_task, "RobotControlTask", 4096, NULL, 5, NULL);
}

