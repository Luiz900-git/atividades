#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define LED_RED 11
#define LED_YELLOW 12
#define LED_GREEN 13
#define BUTTON 5

volatile bool processo_ativo = false;
volatile int estado = 0;

int64_t turn_off_callback(alarm_id_t id, void *user_data);

void iniciar_sequencia() {
    if (processo_ativo) return;
    processo_ativo = true;
    estado = 0;
    gpio_put(LED_RED, 1);
    gpio_put(LED_YELLOW, 1);
    gpio_put(LED_GREEN, 1);
    add_alarm_in_ms(3000, turn_off_callback, NULL, false);
}

int64_t turn_off_callback(alarm_id_t id, void *user_data) {
    if (estado == 0) {
        gpio_put(LED_GREEN, 0);
        estado++;
        add_alarm_in_ms(3000, turn_off_callback, NULL, false);
    } else if (estado == 1) {
        gpio_put(LED_YELLOW, 0);
        estado++;
        add_alarm_in_ms(3000, turn_off_callback, NULL, false);
    } else if (estado == 2) {
        gpio_put(LED_RED, 0);
        processo_ativo = false;
    }
    return 0;
}

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON && (events & GPIO_IRQ_EDGE_FALL)) {
        iniciar_sequencia();
    }
}

int main() {
    stdio_init_all();
    
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_init(LED_YELLOW);
    gpio_set_dir(LED_YELLOW, GPIO_OUT);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    
    gpio_init(BUTTON);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_up(BUTTON);
    gpio_set_irq_enabled_with_callback(BUTTON, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    
    while (true) {
        tight_loop_contents();
    }
}
