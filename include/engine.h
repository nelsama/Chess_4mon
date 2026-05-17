/* ==========================================================================
   ENGINE.H - Motor de ajedrez para CPU
   ========================================================================== */
#ifndef ENGINE_H
#define ENGINE_H

#include "chess.h"

/* Encontrar el mejor movimiento para el bando actual */
void engine_find_best_move(const chess_state_t *s, uint8_t difficulty, chess_move_t *best);

/* Evaluar posición (valor positivo = bueno para blancas) */
int16_t engine_evaluate(const chess_state_t *s);

#endif /* ENGINE_H */
