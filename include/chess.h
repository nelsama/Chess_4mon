/* ==========================================================================
   CHESS.H - Definiciones y API del motor de ajedrez
   ========================================================================== */
#ifndef CHESS_H
#define CHESS_H

#include <stdint.h>

#define WHITE       0
#define BLACK       1

#define EMPTY       0
#define W_PAWN      1
#define W_KNIGHT    2
#define W_BISHOP    3
#define W_ROOK      4
#define W_QUEEN     5
#define W_KING      6
#define B_PAWN      7
#define B_KNIGHT    8
#define B_BISHOP    9
#define B_ROOK      10
#define B_QUEEN     11
#define B_KING      12

#define IS_WHITE(p) ((p) >= W_PAWN && (p) <= W_KING)
#define IS_BLACK(p) ((p) >= B_PAWN && (p) <= B_KING)
#define COLOR_OF(p) (IS_WHITE(p) ? WHITE : BLACK)
#define IS_EMPTY(p) ((p) == EMPTY)

#define MAX_MOVES   28

#define MOVE_FLAG_QUIET         0
#define MOVE_FLAG_CAPTURE       1
#define MOVE_FLAG_CASTLE_KING   2
#define MOVE_FLAG_CASTLE_QUEEN  3
#define MOVE_FLAG_EN_PASSANT    4
#define MOVE_FLAG_PROMOTE_Q     5
#define MOVE_FLAG_PROMOTE_N     6

#define CASTLE_WK  0x01
#define CASTLE_WQ  0x02
#define CASTLE_BK  0x04
#define CASTLE_BQ  0x08

#define DIFF_EASY    1
#define DIFF_MEDIUM  2
#define DIFF_HARD    3

typedef struct {
    uint8_t from, to, flags, promote;
} chess_move_t;

typedef struct {
    uint8_t  board[64];
    uint8_t  side;
    uint8_t  castling;
    uint8_t  en_passant;
    uint8_t  halfmove_clock;
    uint8_t  fullmove_number;
    uint8_t  in_check;
    uint8_t  game_over;
    uint8_t  result;
} chess_state_t;

void chess_init(chess_state_t *s);
uint8_t chess_generate_moves(chess_state_t *s, chess_move_t *moves);
void chess_make_move(chess_state_t *s, const chess_move_t *m);
uint8_t chess_is_checkmate(chess_state_t *s);
uint8_t chess_is_stalemate(chess_state_t *s);
char chess_piece_char(uint8_t piece);
void chess_square_str(uint8_t sq, char *out);
uint8_t chess_parse_square(const char *in);

#define SQUARE(row, col) ((row) * 8 + (col))
#define ROW_OF(sq)       ((sq) / 8)
#define COL_OF(sq)       ((sq) & 7)

#endif
