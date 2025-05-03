// Programação de Detecção de Bordas

#define F_CPU   16000000
#include <util/delay.h>
#include <avr/io.h>

char dig7seg[16] = {  0x3F, 0x06, 0x5B, 0x4F, 
                      0x66, 0x6D, 0x7D, 0x07, 
                      0x7F, 0x6F, 0x77, 0x7C, 
                      0x39, 0x5E, 0x79, 0x71  };

void disp7seg_int(void) {
    DDRD  = 0xFF;  // Pinos 0 a 7 como saída (segmentos)
    PORTD = 0x00;

    DDRB |= (1<<0); // Pino 8 como saída (display unidades)
    DDRB |= (1<<1); // Pino 9 como saída (display dezenas)

    // Configura pinos 12 e 13 como entrada (PB4 e PB5) com pull-up
    DDRB &= ~((1<<4) | (1<<5)); // Entradas
    PORTB |= (1<<4) | (1<<5);   // Pull-up ativado
}

unsigned char disp7seg_dig = 0;

void disp7seg(unsigned char d) {
    if (disp7seg_dig % 2) {
        PORTB &= ~(1 << 1); // Desliga display dezenas
        PORTD = dig7seg[d % 10]; // Unidades
        PORTB |= (1 << 0);  // Liga display unidades
    } else {
        PORTB &= ~(1 << 0); // Desliga display unidades
        PORTD = dig7seg[(d / 10) % 10]; // Dezenas
        PORTB |= (1 << 1);  // Liga display dezenas
    }
    disp7seg_dig++;
}

int main(void) {
    unsigned char valor = 0;
    unsigned char b0_ultimo = 1; // Estado anterior do botão B0 (não pressionado)
    unsigned char b1_ultimo = 1; // Estado anterior do botão B1

    disp7seg_int();

    while (1) {
        disp7seg(valor);

        // Leitura dos botões
        unsigned char b0_estado = PINB & (1 << 4); // Pino 12
        unsigned char b1_estado = PINB & (1 << 5); // Pino 13

        // Botão B0 pressionado (detecta borda de descida)
        if (b0_ultimo && !b0_estado) {
            valor = (valor + 1) % 100; // Após 99 volta para 0
        }

        // Botão B1 pressionado
        if (b1_ultimo && !b1_estado) {
            if (valor > 0) valor--;
        }

        // Atualiza os estados anteriores
        b0_ultimo = b0_estado;
        b1_ultimo = b1_estado;

        _delay_ms(10); // Delay para multiplexação e debounce
    }
}
