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


void init_all()
{
	init_gpio(LEFT_MOTOR);
	init_gpio(RIGHT_MOTOR);

	init_pwm(LEFT_MOTOR);
	init_pwm(RIGHT_MOTOR);

	left_encoder = init_encoder(ENCODER_LEFT);
	right_encoder = init_encoder(ENCODER_RIGHT);

	// inicializa a UART p leitur
	init_uart_read();
	// inicializa a UART p escrita
	init_uart_write(); 
}

// fica escutando UART por novos comandos de veloc do Raspberry Pi
void uart_receive_task(void *pvParameters) {
	// armazena ultimo comando p caso de falha de comunicação
	target_rads_data_t last_target_rads = {0.0, 0.0};

	while (1) {
		// tenta ler dados da UART com um timeout
		target_rads_data_t received_data = receive_data(&last_target_rads);
        
		// envia dados recebidos ou ultimo valor p a fila do motor
		// xQueueOverwrite garante q fila sempre tenha valor mais recente
		xQueueOverwrite(target_rads_queue, &received_data);

		// pequeno delay p n sobrecarregar a CPU com esta tarefa
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

// executa o loop de controle PID p motores
void motor_control_task(void *pvParameters) {
	// variaveis p acumular saída do PID incremental
	float valpidL = 0, valpidR = 0;
	// armazena veloc alvo atual
	target_rads_data_t current_target = {0.0, 0.0};

	while (1) {
 		// verifica se tem novo alvo na fila, mas n bloqueia tarefa se n tiver
		xQueueReceive(target_rads_queue, &current_target, (TickType_t)0);

 		pid_calculate(left_pid, LEFT_MOTOR, current_target.target_left_rads, &valpidL, left_encoder);
		pid_calculate(right_pid, RIGHT_MOTOR, current_target.target_right_rads, &valpidR, right_encoder);

		// Delay 
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

// envia dados de telemetria (velocidade medida) p Raspberry Pi
void telemetry_send_task(void *pvParameters) {
    rads_data_t measured_rads;

    while(1) {
        // mede velo real das rodas usando função corrigida do encoder
        measured_rads.left_rads = encoder_get_speed_rads(left_encoder);
        measured_rads.right_rads = encoder_get_speed_rads(right_encoder);

        // envia dados formatados  pela UART
        send_data(measured_rads);

        // frequência de envio da telemetria
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}



void app_main(void)
{
	init_all();

	// fila q comunica tarefa da UART com tarefa de controle
	target_rads_queue = xQueueCreate(1, sizeof(target_rads_data_t));

	left_pid = init_pid(LEFT_MOTOR);
	right_pid = init_pid(RIGHT_MOTOR);

	// recepção de comandos
	xTaskCreate(uart_receive_task, "UART Receive Task", 2048, NULL, 5, NULL);
	// controle dos motores
	xTaskCreate(motor_control_task, "Motor Control Task", 4096, NULL, 10, NULL);
    // envio de telemetria
	xTaskCreate(telemetry_send_task, "Telemetry Send Task", 2048, NULL, 5, NULL);
    
	ESP_LOGI(TAG_MAIN, "Inicialização completa. Tarefas criadas e rodando.");
}
