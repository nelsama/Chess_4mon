/* ==========================================================================
   MAIN.C - Ajedrez 6502 + SID 6581
   ========================================================================== */
#include "chess.h"
#include "engine.h"
#include "render.h"
#include "input.h"
#include "sid_audio.h"
#include "romapi.h"

static chess_state_t g_state;
static chess_move_t  g_move;
static chess_move_t  g_legal_moves[MAX_MOVES];
static uint8_t       g_legal_count;
static uint8_t       g_cursor_sq, g_selected_sq, g_flipped;
static uint8_t       g_human_turn, g_move_idx;
static uint8_t       g_difficulty, g_player_color;

/* ==========================================================================
   MENUS
   ========================================================================== */
static uint8_t menu_difficulty(void) {
    uint8_t k;
    rom_uart_puts("\033[2J\033[H\033[36;1m");
    rom_uart_puts("=== MicroChess v1.0.0 ===\r\n\r\n");
    rom_uart_puts("Dificultad:\r\n [1] Facil\r\n [2] Medio\r\n [3] Dificil\r\n\033[0m\r\n");
    rom_uart_puts("\033[2mWASD:mov F:vol N:nue Q:sal\033[0m");
    while (1) { k = input_wait_key();
        if (k == KEY_1) return DIFF_EASY;
        if (k == KEY_2) return DIFF_MEDIUM;
        if (k == KEY_3) return DIFF_HARD;
    }
}

static uint8_t menu_player_color(void) {
    uint8_t k;
    rom_uart_puts("\033[2J\033[H\033[36;1m");
    rom_uart_puts("=== AJEDREZ 6502 + SID ===\r\n\r\n");
    rom_uart_puts("Juegas con:\r\n [1] Blancas\r\n [2] Negras\r\n\033[0m\r\n");
    rom_uart_puts("\033[2mWASD:mov F:vol N:nue Q:sal\033[0m");
    while (1) { k = input_wait_key();
        if (k == KEY_1) return WHITE;
        if (k == KEY_2) return BLACK;
    }
}

/* ==========================================================================
   LOGICA DEL CURSOR
   ========================================================================== */
static uint8_t move_cursor(uint8_t sq, uint8_t key) {
    uint8_t r = sq >> 3, c = sq & 7;
    if (g_flipped) {
        if (key == KEY_UP && r < 7) r++;
        if (key == KEY_DOWN && r > 0) r--;
        if (key == KEY_LEFT && c < 7) c++;
        if (key == KEY_RIGHT && c > 0) c--;
    } else {
        if (key == KEY_UP && r > 0) r--;
        if (key == KEY_DOWN && r < 7) r++;
        if (key == KEY_LEFT && c > 0) c--;
        if (key == KEY_RIGHT && c < 7) c++;
    }
    return (r << 3) | c;
}

static uint8_t find_move_to(uint8_t to_sq) {
    uint8_t i;
    for (i = 0; i < g_legal_count; i++)
        if (g_legal_moves[i].to == to_sq && g_legal_moves[i].from == g_selected_sq) return i;
    return 0xFF;
}

/* ==========================================================================
   BUCLE PRINCIPAL
   ========================================================================== */
static void game_loop(void) {
    uint8_t k, dir;

    chess_init(&g_state);
    g_human_turn = (g_player_color == WHITE);
    g_flipped = (g_player_color == BLACK) ? 1 : 0;
    g_cursor_sq = 52;
    g_selected_sq = 0xFF;
    g_legal_count = 0;

    render_init();
    render_highlight_cursor(g_cursor_sq, 0, g_flipped);

    if (g_human_turn)
        g_legal_count = chess_generate_moves(&g_state, g_legal_moves);

    if (!g_human_turn) {
        render_draw_board(&g_state, g_flipped);
        render_status(" CPU PENSANDO...");
        engine_find_best_move(&g_state, g_difficulty, &g_move);
        chess_make_move(&g_state, &g_move);
        g_cursor_sq = g_move.to;
        g_human_turn = 1;
        g_legal_count = chess_generate_moves(&g_state, g_legal_moves);
    }

    render_highlight_cursor(g_cursor_sq, 0, g_flipped);
    render_draw_board(&g_state, g_flipped);
    sid_sfx_move();
    render_status(" TU TURNO");

    while (!g_state.game_over) {
        k = input_wait_key();

        dir = KEY_NONE;
        if (k == KEY_UP) dir = KEY_UP;
        if (k == KEY_DOWN) dir = KEY_DOWN;
        if (k == KEY_RIGHT) dir = KEY_RIGHT;
        if (k == KEY_LEFT) dir = KEY_LEFT;

        if (dir != KEY_NONE) {
            g_cursor_sq = move_cursor(g_cursor_sq, dir);
            render_highlight_cursor(g_cursor_sq, (g_selected_sq != 0xFF), g_flipped);
            render_draw_board(&g_state, g_flipped);
            continue;
        }

        if (k == KEY_ENTER || k == KEY_SPACE) {
            if (!g_human_turn) continue;

            if (g_selected_sq == 0xFF) {
                if (!IS_EMPTY(g_state.board[g_cursor_sq]) &&
                    COLOR_OF(g_state.board[g_cursor_sq]) == g_state.side) {
                    g_selected_sq = g_cursor_sq;
                    render_highlight_cursor(g_cursor_sq, 1, g_flipped);
                    render_draw_board(&g_state, g_flipped);
                }
                continue;
            }

            if (g_cursor_sq == g_selected_sq) {
                g_selected_sq = 0xFF;
                render_highlight_cursor(g_cursor_sq, 0, g_flipped);
                render_draw_board(&g_state, g_flipped);
                continue;
            }

            g_move_idx = find_move_to(g_cursor_sq); if(g_move_idx == 0xFF) { g_selected_sq = 0xFF; render_highlight_cursor(g_cursor_sq, 0, g_flipped); render_draw_board(&g_state, g_flipped); continue; }
            if (g_move_idx != 0xFF) {
                g_move = g_legal_moves[g_move_idx];
                g_selected_sq = 0xFF;
                g_legal_count = 0;

                sid_sfx_move();
                chess_make_move(&g_state, &g_move);
                g_human_turn = 0;
                render_highlight_cursor(g_cursor_sq, 0, g_flipped);
                render_draw_board(&g_state, g_flipped);

                if (chess_is_checkmate(&g_state)) {
                    sid_sfx_checkmate();
                    g_state.game_over = 1;
                    g_state.result = (g_state.side == WHITE) ? 2 : 1;
                    break;
                }
                if (chess_is_stalemate(&g_state)) {
                    g_state.game_over = 1; g_state.result = 0; break;
                }

                render_status(" CPU PENSANDO...");
                engine_find_best_move(&g_state, g_difficulty, &g_move);
                chess_make_move(&g_state, &g_move);
                g_cursor_sq = g_move.to;
                g_human_turn = 1;

                if (chess_is_checkmate(&g_state)) {
                    sid_sfx_checkmate();
                    g_state.game_over = 1;
                    g_state.result = (g_state.side == WHITE) ? 2 : 1;
                    break;
                }
                if (chess_is_stalemate(&g_state)) {
                    g_state.game_over = 1; g_state.result = 0; break;
                }

                g_legal_count = chess_generate_moves(&g_state, g_legal_moves);
                render_highlight_cursor(g_cursor_sq, 0, g_flipped);
                render_draw_board(&g_state, g_flipped);
                sid_sfx_move();
                render_status(" TU TURNO");
            }
            continue;
        }

        if (k == KEY_ESC) {
            g_selected_sq = 0xFF;
            render_highlight_cursor(g_cursor_sq, 0, g_flipped);
            render_draw_board(&g_state, g_flipped);
            continue;
        }

        if (k == KEY_F) {
            g_flipped = !g_flipped;
            g_selected_sq = 0xFF;
            render_highlight_cursor(g_cursor_sq, 0, g_flipped);
            render_draw_board(&g_state, g_flipped);
            continue;
        }

        if (k == KEY_N) return;
        if (k == KEY_Q) { g_state.game_over = 1; g_state.result = 0xFF; break; }
    }

    if (g_state.result == 1) render_status(" JAQUE MATE! Ganan BLANCAS. [N]uevo [Q]uit");
    else if (g_state.result == 2) render_status(" JAQUE MATE! Ganan NEGRAS. [N]uevo [Q]uit");
    else if (g_state.result == 0) render_status(" EMPATE! [N]uevo [Q]uit");
    else if (g_state.result == 0xFF) render_status(" Cancelado. [N]uevo [Q]uit");
}

/* ==========================================================================
   MAIN
   ========================================================================== */
int main(void) {
    uint8_t k;

    asm("cli");
    sid_init();
    input_init();

    g_difficulty = menu_difficulty();
    g_player_color = menu_player_color();

    while (1) {
        game_loop();
        while (1) {
            k = input_wait_key();
            if (k == KEY_N) {
                g_difficulty = menu_difficulty();
                g_player_color = menu_player_color();
                break;
            }
            if (k == KEY_Q || k == KEY_ESC) {
                sid_off();
                render_cleanup();
                return 0;
            }
        }
    }
}
