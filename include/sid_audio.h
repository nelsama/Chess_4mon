/* ==========================================================================
   SID_AUDIO.H - Efectos de sonido SID 6581 para ajedrez
   ========================================================================== */
#ifndef SID_AUDIO_H
#define SID_AUDIO_H

#include <stdint.h>

/* Inicializar chip SID */
void sid_init(void);

/* Silenciar todas las voces */
void sid_off(void);

/* Tono simple: frecuencia 1, frecuencia 2 (0 = no usar), duración ms */
void sid_play_tone(uint16_t f1, uint16_t f2, uint8_t ms);

/* Barrido de frecuencia: de f_start a f_end en ms */
void sid_sweep(uint16_t f_start, uint16_t f_end, uint8_t ms);

/* Ruido corto (para movimientos ilegales) */
void sid_noise(uint8_t ms);

/* Jingle de inicio (nuevo juego) */
void sid_play_jingle(void);

/* Efectos del juego */
void sid_sfx_move(void);        /* Click de madera al mover */
void sid_sfx_capture(void);     /* Captura: barrido + impacto */
void sid_sfx_check(void);       /* Jaque: pitidos alternados */
void sid_sfx_checkmate(void);   /* Jaque mate: fanfarria */
void sid_sfx_castle(void);      /* Enroque: doble click */
void sid_sfx_promote(void);     /* Promoción: arpegio */
void sid_sfx_illegal(void);     /* Movimiento ilegal: zumbido */
void sid_sfx_select(void);      /* Seleccionar pieza: click agudo */

/* CPU pensando: tono ambiente suave */
void sid_cpu_thinking_start(void);
void sid_cpu_thinking_stop(void);

#endif /* SID_AUDIO_H */
