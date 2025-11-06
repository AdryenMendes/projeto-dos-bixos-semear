#include "h_bridge.h"
#include "encoder.h"
#include "PID.h"
#include "uart_esp32.h"
#include "pid_ctrl.h"


QueueHandle_t target_rads_queue;
pcnt_unit_handle_t left_encoder;
pcnt_unit_handle_t right_encoder;
pid_ctrl_block_handle_t left_pid;
pid_ctrl_block_handle_t right_pid;

const char* TAG_MAIN = "Main";

// --- Funções de Inicialização ---

// Função para inicializar todos os periféricos de uma vez.
void init_all()
{
	// Inicializa os pinos de GPIO para ambos os motores
	init_gpio(LEFT_MOTOR);
	init_gpio(RIGHT_MOTOR);

	// Inicializa o PWM para ambos os motores
	init_pwm(LEFT_MOTOR);
	init_pwm(RIGHT_MOTOR);

	// Inicializa os encoders
	left_encoder = init_encoder(ENCODER_LEFT);
	right_encoder = init_encoder(ENCODER_RIGHT);

	// --- ALTERAÇÃO: Unificando a inicialização da UART ---
	// Inicializa a UART para leitura (receber comandos do Pi)
	init_uart_read();
	// Inicializa a UART para escrita (enviar telemetria para o Pi)
	init_uart_write(); 
}

// --- Tarefas do FreeRTOS ---

// Tarefa #1: Fica escutando a UART por novos comandos de velocidade do Raspberry Pi.
void uart_receive_task(void *pvParameters) {
	// Armazena o último comando válido para o caso de falha de comunicação
	target_rads_data_t last_target_rads = {0.0, 0.0};

	while (1) {
		// Tenta ler dados da UART com um pequeno timeout
		target_rads_data_t received_data = receive_data(&last_target_rads);
        
		// Envia os dados recebidos (ou o último valor válido) para a fila do motor.
		// xQueueOverwrite garante que a fila sempre tenha o valor mais recente,
		// evitando que o robô execute comandos antigos.
		xQueueOverwrite(target_rads_queue, &received_data);

		// Pequeno delay para não sobrecarregar a CPU com esta tarefa
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

// Tarefa #2: Executa o loop de controle PID para os motores.
void motor_control_task(void *pvParameters) {
	// Variáveis para acumular a saída do PID incremental
	float valpidL = 0, valpidR = 0;
	// Armazena a velocidade alvo atual
	target_rads_data_t current_target = {0.0, 0.0};

	while (1) {
 		// Verifica se há um novo alvo na fila, mas não bloqueia a tarefa se não houver.
		// Isso permite que o PID continue rodando com o alvo antigo, garantindo estabilidade.
		xQueueReceive(target_rads_queue, &current_target, (TickType_t)0);

 		// Executa o cálculo do PID para ambos os motores.
 		pid_calculate(left_pid, LEFT_MOTOR, current_target.target_left_rads, &valpidL, left_encoder);
		pid_calculate(right_pid, RIGHT_MOTOR, current_target.target_right_rads, &valpidR, right_encoder);

		// Delay que define a frequência do loop de controle (100 Hz).
		// Esta é a tarefa de maior prioridade e deve rodar rapidamente.
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

// --- NOVO: Tarefa #3: Envia dados de telemetria (velocidade medida) para o Raspberry Pi ---
void telemetry_send_task(void *pvParameters) {
    rads_data_t measured_rads;

    while(1) {
        // 1. Mede a velocidade real de cada roda usando a função corrigida do encoder.
        //    É CRUCIAL que a função 'encoder_get_speed_rads' esteja implementada corretamente.
        measured_rads.left_rads = encoder_get_speed_rads(left_encoder);
        measured_rads.right_rads = encoder_get_speed_rads(right_encoder);

        // 2. Envia os dados formatados (ex: "15.4;12.1\n") pela UART.
        send_data(measured_rads);

        // 3. Define a frequência de envio da telemetria.
        //    50ms resulta em 20 Hz, que é uma boa frequência para a odometria no ROS.
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


// --- Função Principal ---
void app_main(void)
{
	// Inicializa todo o hardware
	init_all();

	// Cria a fila que comunica a tarefa da UART com a tarefa de controle.
	// Tamanho 1 é suficiente, pois xQueueOverwrite substitui o valor.
	target_rads_queue = xQueueCreate(1, sizeof(target_rads_data_t));

	// Inicializa os controladores PID para cada motor
	left_pid = init_pid(LEFT_MOTOR);
	right_pid = init_pid(RIGHT_MOTOR);

	// Cria as tarefas que rodarão em paralelo nos diferentes núcleos do ESP32.
	// Tarefa de recepção de comandos:
	xTaskCreate(uart_receive_task, "UART Receive Task", 2048, NULL, 5, NULL);
	// Tarefa de controle dos motores (maior prioridade):
	xTaskCreate(motor_control_task, "Motor Control Task", 4096, NULL, 10, NULL);
    // --- NOVO: Cria a tarefa de envio de telemetria ---
	xTaskCreate(telemetry_send_task, "Telemetry Send Task", 2048, NULL, 5, NULL);
    
	ESP_LOGI(TAG_MAIN, "Inicialização completa. Tarefas criadas e rodando.");
}
