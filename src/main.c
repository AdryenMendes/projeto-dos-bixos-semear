#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "h_bridge.h"         // Inclui nosso módulo de motor
#include "encoder_control.h"  // Inclui nosso módulo de encoder

static const char *TAG = "APP_MAIN";

// Tarefa de teste que move os motores e le encoders
void robot_control_task(void *pvParameters) {
    int pulse_count_L = 0;
    int pulse_count_R = 0;
    
    // Velocidade de teste constante para os motores 
    int test_speed = 150; 

    ESP_LOGI(TAG, "Iniciando tarefa de controle...");

    while (1) {
        // Mover ambos os motores para a frente com a velocidade de teste
        h_bridge_control_motor(MOTOR_LEFT, test_speed);
        h_bridge_control_motor(MOTOR_RIGHT, test_speed);
        
        // Lê a contagem de pulsos dos encoders
        encoder_get_count(ENCODER_LEFT, &pulse_count_L);
        encoder_get_count(ENCODER_RIGHT, &pulse_count_R);

        // Mostra os valores lidos no log
        ESP_LOGI(TAG, "Contagem Esquerda: %d, Contagem Direita: %d", pulse_count_L, pulse_count_R);

        // Aguarda um pouco antes da próxima leitura
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void) {
    // inicializa o hardware dos motores
    h_bridge_init();
    ESP_LOGI(TAG, "Módulo da Ponte H inicializado.");

    // inicializa o hardware dos encoders
    encoders_init();
    ESP_LOGI(TAG, "Módulo dos Encoders inicializado.");

    // cria e inicia a tarefa de controle principal
    xTaskCreate(
        robot_control_task,         // Função da tarefa
        "Robot Control Task",       // Nome da tarefa
        4096,                       // Tamanho da pilha
        NULL,                       // Parâmetro da tarefa
        5,                          // Prioridade
        NULL                        // Handle da tarefa (opcional)
    );
}
