/* ==========================================================================
   RENDER.H - Renderizado ANSI del tablero de ajedrez
   ========================================================================== */
#ifndef RENDER_H
#define RENDER_H

#include "chess.h"

/* Inicializar pantalla, ocultar cursor */
void render_init(void);

/* Dibujar el tablero completo */
void render_draw_board(const chess_state_t *s, uint8_t flipped);

/* Actualizar una casilla del tablero */
void render_update_square(uint8_t sq, const chess_state_t *s, uint8_t mode, uint8_t flipped);

/* Resaltar una casilla con cursor */
void render_highlight_cursor(uint8_t sq, uint8_t selected, uint8_t flipped);

/* Quitar resaltado de una casilla */
void render_clear_highlight(uint8_t sq, uint8_t flipped);

/* Mostrar mensaje en barra de estado */
void render_status(const char *msg);

/* Mostrar lista de movimientos legales resaltados */
void render_legal_moves(const chess_move_t *moves, uint8_t count,
                        uint8_t flipped, uint8_t show);

/* Dibujar panel lateral con info */
void render_sidebar(const chess_state_t *s, uint8_t difficulty,
                    uint8_t player_color, uint8_t cpu_thinking);

/* Limpiar pantalla y restaurar cursor */
void render_cleanup(void);

/* Limpiar solo la zona del tablero */
void render_clear_board_area(void);

#endif /* RENDER_H */
