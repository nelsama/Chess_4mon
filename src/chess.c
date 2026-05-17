/* ==========================================================================
   CHESS.C - Logica de ajedrez compacta para 6502
   ========================================================================== */
#include "chess.h"

/* Direcciones */
static const int8_t KDX[8]={-2,-1,1,2,2,1,-1,-2};
static const int8_t KDY[8]={-1,-2,-2,-1,1,2,2,1};
static const int8_t GDX[8]={-1,0,1,1,1,0,-1,-1};
static const int8_t GDY[8]={-1,-1,-1,0,1,1,1,0};
static const int8_t SX[4]={-1,0,1,0};
static const int8_t SY[4]={0,-1,0,1};
static const int8_t DX[4]={-1,1,1,-1};
static const int8_t DY[4]={-1,-1,1,1};

static const uint8_t INIT_BOARD[64] = {
    B_ROOK,B_KNIGHT,B_BISHOP,B_QUEEN,B_KING,B_BISHOP,B_KNIGHT,B_ROOK,
    B_PAWN,B_PAWN,B_PAWN,B_PAWN,B_PAWN,B_PAWN,B_PAWN,B_PAWN,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    W_PAWN,W_PAWN,W_PAWN,W_PAWN,W_PAWN,W_PAWN,W_PAWN,W_PAWN,
    W_ROOK,W_KNIGHT,W_BISHOP,W_QUEEN,W_KING,W_BISHOP,W_KNIGHT,W_ROOK
};

static chess_move_t g_pseudo[MAX_MOVES];
static chess_state_t g_copy;
static chess_move_t g_check_moves[MAX_MOVES];

void chess_init(chess_state_t *s) {
    uint8_t i;
    for(i=0;i<64;i++) s->board[i]=INIT_BOARD[i];
    s->side=WHITE; s->castling=0x0F; s->en_passant=0xFF;
    s->halfmove_clock=0; s->fullmove_number=1;
    s->in_check=0; s->game_over=0; s->result=0;
}

static uint8_t ok(uint8_t r,uint8_t c){return r<8&&c<8;}
static uint8_t sr(uint8_t s){return s>>3;}
static uint8_t sc(uint8_t s){return s&7;}
static uint8_t ms(uint8_t r,uint8_t c){return(r<<3)|c;}
static uint8_t find_king(const uint8_t *b,uint8_t sd){
    uint8_t kp=(sd==WHITE)?W_KING:B_KING,i;
    for(i=0;i<64;i++) if(b[i]==kp) return i;
    return 0xFF;
}

static uint8_t patt(const uint8_t *b,uint8_t sq,uint8_t tg,uint8_t sd){
    uint8_t r=sr(sq),c=sc(sq),tr=sr(tg),tc=sc(tg),i,nr,nc,p=b[sq];
    if(IS_EMPTY(p)||COLOR_OF(p)!=sd) return 0;
    if(p==W_PAWN) return(tr==r-1&&(tc==c-1||tc==c+1));
    if(p==B_PAWN) return(tr==r+1&&(tc==c-1||tc==c+1));
    if(p==W_KNIGHT||p==B_KNIGHT){
        for(i=0;i<8;i++){nr=r+KDY[i];nc=c+KDX[i];if(ok(nr,nc)&&nr==tr&&nc==tc)return 1;}
        return 0;
    }
    if(p==W_KING||p==B_KING){
        for(i=0;i<8;i++){nr=r+GDY[i];nc=c+GDX[i];if(ok(nr,nc)&&nr==tr&&nc==tc)return 1;}
        return 0;
    }
    if(p==W_BISHOP||p==B_BISHOP){
        for(i=0;i<4;i++){nr=r;nc=c;while(1){nr+=DY[i];nc+=DX[i];if(!ok(nr,nc))break;if(nr==tr&&nc==tc)return 1;if(!IS_EMPTY(b[ms(nr,nc)]))break;}}
        return 0;
    }
    if(p==W_ROOK||p==B_ROOK){
        for(i=0;i<4;i++){nr=r;nc=c;while(1){nr+=SY[i];nc+=SX[i];if(!ok(nr,nc))break;if(nr==tr&&nc==tc)return 1;if(!IS_EMPTY(b[ms(nr,nc)]))break;}}
        return 0;
    }
    if(p==W_QUEEN||p==B_QUEEN){
        for(i=0;i<4;i++){nr=r;nc=c;while(1){nr+=SY[i];nc+=SX[i];if(!ok(nr,nc))break;if(nr==tr&&nc==tc)return 1;if(!IS_EMPTY(b[ms(nr,nc)]))break;}}
        for(i=0;i<4;i++){nr=r;nc=c;while(1){nr+=DY[i];nc+=DX[i];if(!ok(nr,nc))break;if(nr==tr&&nc==tc)return 1;if(!IS_EMPTY(b[ms(nr,nc)]))break;}}
        return 0;
    }
    return 0;
}

static uint8_t attacked(const uint8_t *b,uint8_t sq,uint8_t by){
    uint8_t i;
    for(i=0;i<64;i++) if(!IS_EMPTY(b[i])&&COLOR_OF(b[i])==by&&patt(b,i,sq,by)) return 1;
    return 0;
}
static uint8_t in_check(const uint8_t *b,uint8_t sd){
    uint8_t ks=find_king(b,sd);
    return(ks!=0xFF&&attacked(b,ks,sd^1));
}

static uint8_t add(chess_move_t *mv,uint8_t n,uint8_t f,uint8_t t,uint8_t fl,uint8_t pr){
    mv[n].from=f;mv[n].to=t;mv[n].flags=fl;mv[n].promote=pr;return n+1;
}

static uint8_t slide(const uint8_t *b,uint8_t sq,uint8_t sd,uint8_t *dx,uint8_t *dy,uint8_t nd,chess_move_t *mv,uint8_t n){
    uint8_t d,r=sr(sq),c=sc(sq),nr,nc,nsq,tg,op=sd^1;
    for(d=0;d<nd;d++){nr=r;nc=c;while(1){nr+=dy[d];nc+=dx[d];if(!ok(nr,nc))break;nsq=ms(nr,nc);tg=b[nsq];if(IS_EMPTY(tg)){n=add(mv,n,sq,nsq,MOVE_FLAG_QUIET,0);if(n>=MAX_MOVES)return n;}else{if(COLOR_OF(tg)==op)n=add(mv,n,sq,nsq,MOVE_FLAG_CAPTURE,0);break;}}}
    return n;
}

static uint8_t gen_pseudo(const uint8_t *b,uint8_t sd,uint8_t cl,uint8_t ep,chess_move_t *mv){
    uint8_t sq,p,r,c,nr,nc,nsq,tg,n=0,op=sd^1,i;
    for(sq=0;sq<64&&n<MAX_MOVES;sq++){
        p=b[sq];if(IS_EMPTY(p)||COLOR_OF(p)!=sd)continue;
        r=sr(sq);c=sc(sq);
        if(p==W_PAWN||p==B_PAWN){
            int8_t dir=(p==W_PAWN)?-1:1;
            uint8_t sr2=(p==W_PAWN)?6:1;
            nr=r+dir;nc=c;
            if(ok(nr,nc)&&IS_EMPTY(b[ms(nr,nc)])){
                        if((p==W_PAWN&&nr==0)||(p==B_PAWN&&nr==7)){
                            n=add(mv,n,sq,ms(nr,nc),MOVE_FLAG_PROMOTE_Q,(p==W_PAWN)?W_QUEEN:B_QUEEN);
                            n=add(mv,n,sq,ms(nr,nc),MOVE_FLAG_PROMOTE_N,(p==W_PAWN)?W_KNIGHT:B_KNIGHT);
                        }else{n=add(mv,n,sq,ms(nr,nc),MOVE_FLAG_QUIET,0);}
                if(r==sr2&&IS_EMPTY(b[ms(r+2*dir,c)]))n=add(mv,n,sq,ms(r+2*dir,c),MOVE_FLAG_QUIET,0);
            }
            if(ok(nr,c-1)){nsq=ms(nr,c-1);tg=b[nsq];if(!IS_EMPTY(tg)&&COLOR_OF(tg)==op){if((p==W_PAWN&&nr==0)||(p==B_PAWN&&nr==7)){n=add(mv,n,sq,nsq,MOVE_FLAG_PROMOTE_Q,(p==W_PAWN)?W_QUEEN:B_QUEEN);n=add(mv,n,sq,nsq,MOVE_FLAG_PROMOTE_N,(p==W_PAWN)?W_KNIGHT:B_KNIGHT);}else n=add(mv,n,sq,nsq,MOVE_FLAG_CAPTURE,0);}}
            if(ok(nr,c+1)){nsq=ms(nr,c+1);tg=b[nsq];if(!IS_EMPTY(tg)&&COLOR_OF(tg)==op){if((p==W_PAWN&&nr==0)||(p==B_PAWN&&nr==7)){n=add(mv,n,sq,nsq,MOVE_FLAG_PROMOTE_Q,(p==W_PAWN)?W_QUEEN:B_QUEEN);n=add(mv,n,sq,nsq,MOVE_FLAG_PROMOTE_N,(p==W_PAWN)?W_KNIGHT:B_KNIGHT);}else n=add(mv,n,sq,nsq,MOVE_FLAG_CAPTURE,0);}}
        }
        else if(p==W_KNIGHT||p==B_KNIGHT){
            for(i=0;i<8;i++){nr=r+KDY[i];nc=c+KDX[i];if(!ok(nr,nc))continue;nsq=ms(nr,nc);tg=b[nsq];if(IS_EMPTY(tg))n=add(mv,n,sq,nsq,MOVE_FLAG_QUIET,0);else if(COLOR_OF(tg)==op)n=add(mv,n,sq,nsq,MOVE_FLAG_CAPTURE,0);}
        }
        else if(p==W_BISHOP||p==B_BISHOP)n=slide(b,sq,sd,(uint8_t*)DX,(uint8_t*)DY,4,mv,n);
        else if(p==W_ROOK||p==B_ROOK)n=slide(b,sq,sd,(uint8_t*)SX,(uint8_t*)SY,4,mv,n);
        else if(p==W_QUEEN||p==B_QUEEN){n=slide(b,sq,sd,(uint8_t*)SX,(uint8_t*)SY,4,mv,n);n=slide(b,sq,sd,(uint8_t*)DX,(uint8_t*)DY,4,mv,n);}
        else if(p==W_KING||p==B_KING){
            for(i=0;i<8;i++){nr=r+GDY[i];nc=c+GDX[i];if(!ok(nr,nc))continue;nsq=ms(nr,nc);tg=b[nsq];if(IS_EMPTY(tg))n=add(mv,n,sq,nsq,MOVE_FLAG_QUIET,0);else if(COLOR_OF(tg)==op)n=add(mv,n,sq,nsq,MOVE_FLAG_CAPTURE,0);}
            {
                uint8_t kr=sd?0:7,atk=sd^1,ck=sd?CASTLE_BK:CASTLE_WK,cq=sd?CASTLE_BQ:CASTLE_WQ;
                if(!attacked(b,ms(kr,4),atk)){
                    if((cl&ck)&&IS_EMPTY(b[ms(kr,5)])&&IS_EMPTY(b[ms(kr,6)])&&!attacked(b,ms(kr,5),atk)&&!attacked(b,ms(kr,6),atk))n=add(mv,n,sq,ms(kr,6),MOVE_FLAG_CASTLE_KING,0);
                    if((cl&cq)&&IS_EMPTY(b[ms(kr,3)])&&IS_EMPTY(b[ms(kr,2)])&&IS_EMPTY(b[ms(kr,1)])&&!attacked(b,ms(kr,3),atk)&&!attacked(b,ms(kr,2),atk))n=add(mv,n,sq,ms(kr,2),MOVE_FLAG_CASTLE_QUEEN,0);
                }
            }
        }
    }
    return n;
}

void chess_make_move(chess_state_t *s,const chess_move_t *m){
    uint8_t p=s->board[m->from],cp=s->board[m->to],r;
    s->board[m->to]=p;s->board[m->from]=0;
    if(m->flags==MOVE_FLAG_EN_PASSANT){
        if(s->side==0)s->board[ms(sr(m->to)+1,sc(m->to))]=0;
        else s->board[ms(sr(m->to)-1,sc(m->to))]=0;
    }
    if(m->flags==MOVE_FLAG_PROMOTE_Q||m->flags==MOVE_FLAG_PROMOTE_N)s->board[m->to]=m->promote;
    if(m->flags==MOVE_FLAG_CASTLE_KING){r=sr(m->from);s->board[ms(r,5)]=s->board[ms(r,7)];s->board[ms(r,7)]=0;}
    if(m->flags==MOVE_FLAG_CASTLE_QUEEN){r=sr(m->from);s->board[ms(r,3)]=s->board[ms(r,0)];s->board[ms(r,0)]=0;}
    if(p==6)s->castling&=~3;
    if(p==12)s->castling&=~12;
    if(p==4&&m->from==ms(7,7))s->castling&=~1;
    if(p==4&&m->from==ms(7,0))s->castling&=~2;
    if(p==10&&m->from==ms(0,7))s->castling&=~4;
    if(p==10&&m->from==ms(0,0))s->castling&=~8;
    if(cp==4&&m->to==ms(7,7))s->castling&=~1;
    if(cp==4&&m->to==ms(7,0))s->castling&=~2;
    if(cp==10&&m->to==ms(0,7))s->castling&=~4;
    if(cp==10&&m->to==ms(0,0))s->castling&=~8;
    s->en_passant=0xFF;
    if(p==1&&sr(m->from)==6&&sr(m->to)==4)s->en_passant=sc(m->to);
    if(p==7&&sr(m->from)==1&&sr(m->to)==3)s->en_passant=sc(m->to);
    if(p==1||p==7||!IS_EMPTY(cp))s->halfmove_clock=0;else s->halfmove_clock++;
    if(s->side==1)s->fullmove_number++;
    s->side ^= 1;
    s->in_check=in_check(s->board,s->side);
}


uint8_t chess_generate_moves(chess_state_t *s,chess_move_t *moves){
    uint8_t i,c,lc=0,ks;
    c=gen_pseudo(s->board,s->side,s->castling,s->en_passant,g_pseudo);
    for(i=0;i<c;i++){
        g_copy=*s;chess_make_move(&g_copy,&g_pseudo[i]);
        ks=find_king(g_copy.board,s->side);
        if(ks!=0xFF&&!attacked(g_copy.board,ks,s->side^1)){moves[lc++]=g_pseudo[i];if(lc>=MAX_MOVES)break;}
    }
    return lc;
}

uint8_t chess_is_checkmate(chess_state_t *s){
    if(!s->in_check)return 0;
    return!chess_generate_moves(s,g_check_moves);
}
uint8_t chess_is_stalemate(chess_state_t *s){
    if(s->in_check)return 0;
    if(s->halfmove_clock>=100)return 1;
    return!chess_generate_moves(s,g_check_moves);
}

char chess_piece_char(uint8_t p){
    static const char t[13]={' ','P','N','B','R','Q','K','p','n','b','r','q','k'};
    return(p<13)?t[p]:' ';
}
void chess_square_str(uint8_t sq,char *o){o[0]='a'+sc(sq);o[1]='8'-sr(sq);o[2]=0;}
uint8_t chess_parse_square(const char *in){
    uint8_t c,r;if(in[0]<'a'||in[0]>'h'||in[1]<'1'||in[1]>'8')return 0xFF;
    c=in[0]-'a';r='8'-in[1];return ms(r,c);
}
