#include "render.h"
#include "romapi.h"

static uint8_t g_cur = 52;
static uint8_t g_sel = 0xFF;

static const char *UP[13] = {
    "   ","\xe2\x99\x99  ","\xe2\x99\x98  ","\xe2\x99\x97  ",
    "\xe2\x99\x96  ","\xe2\x99\x95  ","\xe2\x99\x94  ",
    "\xe2\x99\x9f  ","\xe2\x99\x9e  ","\xe2\x99\x9d  ",
    "\xe2\x99\x9c  ","\xe2\x99\x9b  ","\xe2\x99\x9a  "
};

static void go(uint8_t r, uint8_t c) {
    uint8_t t;
    rom_uart_putc(0x1B); rom_uart_putc('[');
    if(r>=10){t=r/10;rom_uart_putc('0'+t);r%=10;}
    rom_uart_putc('0'+r); rom_uart_putc(';');
    if(c>=10){t=c/10;rom_uart_putc('0'+t);c%=10;}
    rom_uart_putc('0'+c); rom_uart_putc('H');
}

void render_init(void){ rom_uart_puts("\033[?25l\033[2J"); go(1,1); }
void render_cleanup(void){ rom_uart_puts("\033[0m\033[?25h\033[2J"); }

#define BR(r) (3+(r))
#define BC(c) (3+(c)*3)

void render_draw_board(const chess_state_t *s, uint8_t flipped) {
    uint8_t r,c,dr,dc,sq,p; char bf[2]; const char *bg; (void)flipped;

    go(1,4);
    for(c=0;c<8;c++){dc=flipped?7-c:c;bf[0]='a'+dc;rom_uart_putc(bf[0]);rom_uart_puts("  ");}

    for(r=0;r<8;r++){
        dr=flipped?7-r:r;
        go(BR(r),1); bf[0]='8'-dr;
        rom_uart_puts("\033[0m"); rom_uart_putc(bf[0]);

        for(c=0;c<8;c++){
            dc=flipped?7-c:c; sq=(dr<<3)|dc; p=s->board[sq];
            if(sq==g_sel) bg="\033[44m";
            else if(sq==g_cur) bg="\033[46m";
            else if((r+c)&1) bg="\033[48;5;244m";
            else bg="\033[47m";

            go(BR(r),BC(c)); rom_uart_puts(bg);
            if(IS_WHITE(p)) rom_uart_puts("\033[37;1m");
            else if(IS_BLACK(p)) rom_uart_puts("\033[30m");
            rom_uart_puts(UP[p]);
        }
        go(BR(r),27); rom_uart_puts("\033[0m"); rom_uart_putc(bf[0]);
    }

    go(BR(8),4);
    for(c=0;c<8;c++){dc=flipped?7-c:c;bf[0]='a'+dc;rom_uart_putc(bf[0]);rom_uart_puts("  ");}
    go(BR(8)+2,1); rom_uart_puts("\033[0m");
}

void render_update_square(uint8_t sq, const chess_state_t *s, uint8_t mode, uint8_t flipped)
{ (void)sq; (void)s; (void)mode; (void)flipped; }

void render_highlight_cursor(uint8_t sq, uint8_t sel, uint8_t flipped) {
    (void)flipped;
    g_cur = sq;
    if(sel) g_sel = sq; else g_sel = 0xFF;
}

void render_clear_highlight(uint8_t sq, uint8_t flipped)
{ (void)sq; (void)flipped; }

void render_legal_moves(const chess_move_t *m, uint8_t c, uint8_t f, uint8_t s)
{ (void)m; (void)c; (void)f; (void)s; }

void render_status(const char *msg) {
    go(14,1);
    rom_uart_puts("\033[0m\033[40m\033[37m\033[K");
    rom_uart_puts(msg);
    rom_uart_puts("\033[0m");
}

void render_sidebar(const chess_state_t *st, uint8_t d, uint8_t pc, uint8_t ct)
{ (void)st; (void)d; (void)pc; (void)ct; }
