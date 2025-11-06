#include "uart_esp32.h"
#include "h_bridge.h"
#include "PID.h"
#include "uart_esp32.h"
#include <stdio.h> 

/**
 * @brief config e inicializa a porta UART p receber dados
 * 
 * a função e a init_uart_write() foram corrigidas p usar a mesma
 * porta UART (UART_PORT_NUM), evitar conflitos hardware
 */

void init_uart_read()
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // instala o driver da UART
    uart_driver_install(UART_PORT_NUM, BUFFER_LEN, 0, 0, NULL, 0);
    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_set_pin(UART_PORT_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}


target_rads_data_t receive_data(target_rads_data_t *last_target_rads)
{
    target_rads_data_t target_rads = {0, 0};
    uint8_t buffer[BUFFER_LEN];

    // tenta ler bytes UART c timeout de 50ms
    int len = uart_read_bytes(UART_PORT_NUM, buffer, BUFFER_LEN - 1, pdMS_TO_TICKS(50));

    if (len > 0)
    {
        buffer[len] = '\0'; // add terminador nulo p tratar como string

        // usa strtok p separar string por ;
        char *token = strtok((char *)buffer, ";");
        if (token != NULL)
        {
            target_rads.target_left_rads = atof(token);
        }
        
        token = strtok(NULL, ";");
        if (token != NULL)
        {
            target_rads.target_right_rads = atof(token);
        }

        // atualiza e retorna novo valor lido
        *last_target_rads = target_rads;
        return target_rads;
    }

    // se leitura falhar retorna o ultimo valor valido
    return *last_target_rads;
}




void send_data(rads_data_t rads)
{
    // usa buffer de char, é melhor p manipulação de strings
    char buffer[128];

    // formata a string de forma segura c snprintf
    // adiciona '\n' no final
    int len = snprintf(buffer, sizeof(buffer), "%.2f;%.2f\n", rads.left_rads, rads.right_rads);

    // envia número de bytes que mensagem real contém
    if (len > 0) {
        uart_write_bytes(UART_PORT_NUM, (const char *)buffer, len);
    }
}
