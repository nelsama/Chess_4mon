#include "chess.h"
#include "engine.h"

static const int16_t PV[7]={0,100,320,330,500,900,20000};
#define ENG_DEPTH 3
static chess_move_t g_abm[ENG_DEPTH][MAX_MOVES];
static int16_t     g_abs[ENG_DEPTH][MAX_MOVES];

static int16_t eval(const chess_state_t *s){
    int16_t sc=0; uint8_t i,p;
    for(i=0;i<64;i++){p=s->board[i];if(IS_EMPTY(p))continue;
        if(IS_WHITE(p))sc+=PV[p&7];else sc-=PV[p&7];
    }
    if(s->in_check){if(s->side==WHITE)sc-=50;else sc+=50;}
    return sc;
}

static int16_t msc(const chess_state_t *s,const chess_move_t *m){
    uint8_t vc,at; int16_t sc=0;
    if(m->flags==MOVE_FLAG_CAPTURE||m->flags==MOVE_FLAG_EN_PASSANT){
        vc=s->board[m->to];if(m->flags==MOVE_FLAG_EN_PASSANT)vc=(s->side==WHITE)?B_PAWN:W_PAWN;
        at=s->board[m->from];sc=(int16_t)PV[vc&7]*10-(int16_t)PV[at&7];
    }else if(m->flags>=MOVE_FLAG_PROMOTE_Q)sc=800;
    else if(m->flags==MOVE_FLAG_CASTLE_KING||m->flags==MOVE_FLAG_CASTLE_QUEEN)sc=200;
    return sc;
}

static void sort(chess_move_t *mv,uint8_t n,int16_t *ss){
    uint8_t i,j,b; chess_move_t t; int16_t ts;
    for(i=0;i<n-1;i++){b=i;for(j=i+1;j<n;j++)if(ss[j]>ss[b])b=j;
        if(b!=i){t=mv[i];mv[i]=mv[b];mv[b]=t;ts=ss[i];ss[i]=ss[b];ss[b]=ts;}
    }
}

static int16_t ab(chess_state_t *s,uint8_t dp,int16_t a,int16_t b){
    uint8_t i,c; int16_t v; chess_state_t cp;
    if(dp==0)return eval(s);
    c=chess_generate_moves(s,g_abm[dp]);if(c==0)return(s->in_check)?-30000+(int16_t)dp:0;
    for(i=0;i<c;i++)g_abs[dp][i]=msc(s,&g_abm[dp][i]);
    sort(g_abm[dp],c,g_abs[dp]);
    for(i=0;i<c;i++){cp=*s;chess_make_move(&cp,&g_abm[dp][i]);v=-ab(&cp,dp-1,-b,-a);if(v>=b)return b;if(v>a)a=v;}
    return a;
}

void engine_find_best_move(const chess_state_t *s,uint8_t diff,chess_move_t *best){
    uint8_t i,c,dp; int16_t v,bv; chess_state_t cp;
    dp=(diff==DIFF_EASY)?1:(diff==DIFF_MEDIUM)?2:3;
    c=chess_generate_moves((chess_state_t*)s,g_abm[0]);
    if(c==0){best->from=best->to=0;return;}
    if(c==1){*best=g_abm[0][0];return;}
    for(i=0;i<c;i++)g_abs[0][i]=msc(s,&g_abm[0][i]);
    sort(g_abm[0],c,g_abs[0]);
    bv=-32767;*best=g_abm[0][0];
    for(i=0;i<c;i++){cp=*s;chess_make_move(&cp,&g_abm[0][i]);v=-ab(&cp,dp-1,-32767,32767);if(v>bv){bv=v;*best=g_abm[0][i];}}
}

int16_t engine_evaluate(const chess_state_t *s){return eval(s);}
