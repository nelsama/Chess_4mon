/* ==========================================================================
   INPUT.C - Entrada UART con virtual keys
   ========================================================================== */
#include "input.h"
#include "romapi.h"

static uint8_t esc_state;
static uint8_t after_cr;

void input_init(void) { esc_state = 0; after_cr = 0; }

uint8_t input_get_key(void) {
    char c;
    if (!rom_uart_rx_ready()) return KEY_NONE;
    c = rom_uart_getc();

    if (esc_state == 0) {
        if (c == 0x1B) { esc_state = 1; return KEY_NONE; }
        if (c == '\r') { after_cr = 1; return KEY_ENTER; }
        if (c == '\n') {
            if (after_cr) { after_cr = 0; return KEY_NONE; }
            after_cr = 0;
            return KEY_ENTER;
        }
        after_cr = 0;
        if (c == ' ')  return KEY_SPACE;
        if (c == 'w' || c == 'W') return KEY_UP;
        if (c == 's' || c == 'S') return KEY_DOWN;
        if (c == 'd' || c == 'D') return KEY_RIGHT;
        if (c == 'a' || c == 'A') return KEY_LEFT;
        if (c == 'n' || c == 'N') return KEY_N;
        if (c == 'f' || c == 'F') return KEY_F;
        if (c == 'm' || c == 'M') return KEY_M;
        if (c == 'q' || c == 'Q') return KEY_Q;
        if (c == '1') return KEY_1;
        if (c == '2') return KEY_2;
        if (c == '3') return KEY_3;
        return KEY_NONE;
    }

    if (esc_state == 1) {
        if (c == 0x1B) { return KEY_NONE; }
        if (c == '[') { esc_state = 2; return KEY_NONE; }
        esc_state = 0;
        return KEY_ESC;
    }

    esc_state = 0;
    if (c == 'A') return KEY_UP;
    if (c == 'B') return KEY_DOWN;
    if (c == 'C') return KEY_RIGHT;
    if (c == 'D') return KEY_LEFT;
    return KEY_NONE;
}

uint8_t input_wait_key(void) {
    uint8_t k;
    while (1) {
        k = input_get_key();
        if (k != KEY_NONE) return k;
    }
}

void input_wait_any_key(void) { input_wait_key(); }
