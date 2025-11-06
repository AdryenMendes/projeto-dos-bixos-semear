#include "encoder.h"

// valor pode mudar
#define RADS_PER_TICK    0.00475f
#define SAMPLE_TIME_MS   50.0f

const char *TAG_ENCODER = "Encoder";



/**
 * @brief configu e inicializa unidade de PCNT p um encoder em quadratura
 * funcao configura hardware p contar pulsos e detectar direção
 * 
 * @param encoder Lado do motor a ser inicializado
 * @return O handle p unidade PCNT configurada
 */
pcnt_unit_handle_t init_encoder(encoder_side_t encoder)
{
	pcnt_unit_handle_t selected_encoder = NULL;

	ESP_LOGI(TAG_ENCODER, "Instalando a unidade PCNT para o encoder %s...", (encoder == ENCODER_LEFT) ? "ESQUERDO" : "DIREITO");
    
	// unidade de contagem
	pcnt_unit_config_t unit_config = {
		.high_limit = PCNT_HIGH_LIMIT,
 		.low_limit = PCNT_LOW_LIMIT,
	};
	ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &selected_encoder));

	// filtro de glitch p ignorar ruídos
	pcnt_glitch_filter_config_t filter_config = {
		.max_glitch_ns = 1000,
	};
	ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(selected_encoder, &filter_config));

	// canais A e B p decodific em quadratura
	ESP_LOGI(TAG_ENCODER, "Instalando os canais A e B");
	pcnt_chan_config_t channel_a_config = {
		.edge_gpio_num = ENCODER_INPUT_A(encoder),
 		.level_gpio_num = ENCODER_INPUT_B(encoder),
	};
	pcnt_channel_handle_t pcnt_channel_a = NULL;
	ESP_ERROR_CHECK(pcnt_new_channel(selected_encoder, &channel_a_config, &pcnt_channel_a));

	pcnt_chan_config_t channel_b_config = {
		.edge_gpio_num = ENCODER_INPUT_B(encoder),
		.level_gpio_num = ENCODER_INPUT_A(encoder),
	};
	pcnt_channel_handle_t pcnt_channel_b = NULL;
	ESP_ERROR_CHECK(pcnt_new_channel(selected_encoder, &channel_b_config, &pcnt_channel_b));

	// ações de incremento/decremento c base na fase entre canais
	ESP_LOGI(TAG_ENCODER, "Definindo as ações de borda e nível");
	ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_channel_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
	ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_channel_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
	ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_channel_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
	ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_channel_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

	// ativa limpa e inicia unidade de contagem
	ESP_LOGI(TAG_ENCODER, "Habilitando e iniciando a unidade PCNT");
	ESP_ERROR_CHECK(pcnt_unit_enable(selected_encoder));
	ESP_ERROR_CHECK(pcnt_unit_clear_count(selected_encoder));
	ESP_ERROR_CHECK(pcnt_unit_start(selected_encoder));

	return selected_encoder;
	}


/**
 * @brief mede veloc angular atual do motor
 * 
 * função zera contador do encoder, espera por um período (SAMPLE_TIME_MS),
 * lê o número de pulsos acumulados e calcula veloc em radianos por segundos
 * 
 * @param encoder handle da unidade PCNT do encoder a ser lido.
 * @return velocidade calculada em rad/s (float).
 */

float encoder_get_speed_rads(pcnt_unit_handle_t encoder)
{
	int pulse_count = 0;
	// converte tempo de amostragem p segundos p cálculo
	const float sample_time_s = SAMPLE_TIME_MS / 1000.0f;

	// zera contagem p iniciar nova janela de medição
	pcnt_unit_clear_count(encoder);
    
	// espera tempo de amostragem
	vTaskDelay(pdMS_TO_TICKS(SAMPLE_TIME_MS));

	// pega o número de pulsos contados no intervalo de tempo
	ESP_ERROR_CHECK(pcnt_unit_get_count(encoder, &pulse_count));

	// calcula a velocidade em radianos / segundo
	// veloc = (num de Pulsos * rad por pulso) / tempo em degundos
	float speed_rads = (pulse_count * RADS_PER_TICK) / sample_time_s;
    
	// log p depuração
	// ESP_LOGI(TAG_ENCODER, "Pulsos em %.0fms: %d -> Vel: %.2f rad/s", SAMPLE_TIME_MS, pulse_count, speed_rads);

	return speed_rads;
}