/* ==========================================================================
   INPUT.H - Manejo de entrada por UART (cursor + WASD/Arrows)
   ========================================================================== */
#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

/* Teclas virtuales (post-procesado de secuencias escape) */
#define KEY_NONE        0
#define KEY_UP          1
#define KEY_DOWN        2
#define KEY_RIGHT       3
#define KEY_LEFT        4
#define KEY_ENTER       5
#define KEY_ESC         6
#define KEY_N           7
#define KEY_F           8
#define KEY_M           9
#define KEY_Q           10
#define KEY_U           11
#define KEY_SPACE       12
#define KEY_R           13
#define KEY_1           14
#define KEY_2           15
#define KEY_3           16

/* Inicializar estado del input */
void input_init(void);

/* Leer una tecla (no bloqueante). Retorna KEY_NONE si no hay tecla.
   Procesa secuencias escape ANSI y WASD. */
uint8_t input_get_key(void);

/* Esperar hasta que haya una tecla disponible (bloqueante) */
uint8_t input_wait_key(void);

/* Esperar a que el usuario presione una tecla (bloqueante, ignora la tecla) */
void input_wait_any_key(void);

#endif /* INPUT_H */
