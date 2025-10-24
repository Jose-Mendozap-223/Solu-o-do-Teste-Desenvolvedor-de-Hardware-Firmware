#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

// Pines
#define PIN_SIGNAL    PD2    // sinal externo (Ativo em nível baixo)
#define PIN_MOTOR     PD3    // Gate do MOSFET
#define UART_BAUD     9600

// funções
void init_pins(void);
void init_uart(uint32_t baud);
void uart_send(const char *message);
bool read_signal(void);
void motor_on(void);
void motor_off(void);

int main(void) {
    init_pins();
    init_uart(UART_BAUD);

    while(1) {
        if(!read_signal()) {  // nível baixo detecção
            motor_on();
            uart_send("Motor ON\n");

            delay_ms(10000);

            motor_off();
            uart_send("Motor OFF\n");
        }
    }

    return 0;
}

// inicialização pinos
void init_pins(void) {
    // PIN_SIGNAL pull-up
    DDRD &= ~(1 << PIN_SIGNAL);
    PORTD |= (1 << PIN_SIGNAL);

    // PIN_MOTOR Output
    DDRD |= (1 << PIN_MOTOR);
    PORTD &= ~(1 << PIN_MOTOR);  // começa desligado
}

// Leitura de sinal (Ativo em nível baixo)
bool read_signal(void) {
    return PIND & (1 << PIN_SIGNAL);  // 1 = alto, 0 = baixo
}

// ligar motor
void motor_on(void) {
    PORTD |= (1 << PIN_MOTOR);
}

// desligar motor
void motor_off(void) {
    PORTD &= ~(1 << PIN_MOTOR);
}

// inicialização UART
void init_uart(uint32_t baud) {
    uint16_t ubrr = F_CPU/16/baud - 1;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    UCSR0B = (1 << TXEN0);                        // habilitar comunicação
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);      // 8 bits, 1 stop
}

// enviar mensagem UART
void uart_send(const char *message) {
    while(*message) {
        while(!(UCSR0A & (1 << UDRE0)));  // aguardando buffer vazio
        UDR0 = *message++;
    }
}
