; ============================================
; sid_audio.s - SID 6581 en assembler 6502
; ============================================

.export _sid_init, _sid_off, _sid_play_tone
.export _sid_sweep, _sid_noise, _sid_play_jingle
.export _sid_sfx_move, _sid_sfx_capture, _sid_sfx_check
.export _sid_sfx_checkmate, _sid_sfx_castle, _sid_sfx_promote
.export _sid_sfx_illegal, _sid_sfx_select
.export _sid_cpu_thinking_start, _sid_cpu_thinking_stop

.importzp sp

.segment "CODE"

; Macro para llamar rom_delay_ms con valor inmediato 16-bit
.macro delay val
    lda #<(val)
    ldx #>(val)
    jsr $BF33
.endmacro

; --------------------------------------------------
; sid_init(void)
; --------------------------------------------------
_sid_init:
    lda #$0F
    sta $D418
    lda #$0F
    sta $D405
    lda #$01
    sta $D406
    lda #$0F
    sta $D40C
    lda #$02
    sta $D40D
    lda #$00
    sta $D402
    lda #$08
    sta $D403
    sta $D40A
    lda #$00
    sta $D409
    lda #$00
    sta $D404
    sta $D40B
    sta $D412
    rts

; --------------------------------------------------
; sid_off(void)
; --------------------------------------------------
_sid_off:
    lda #$00
    sta $D404
    sta $D40B
    sta $D412
    sta $D418
    rts

; --------------------------------------------------
; sid_play_tone(uint16_t f1, uint16_t f2, uint8_t ms)
; Parametros en pila CC65:
;   sp+1: ms(lo)  -- pero es uint8_t, el compilador pone solo 1 byte?
;   sp+2: f2_lo   -- en realidad CC65 promueve todo a 16-bit?
; Vamos a asumir que CC65 pasa uint8_t como 1 byte y uint16_t como 2.
; Stack CC65 crece hacia arriba, sp apunta al primer byte libre.
; Parametros se pushean de derecha a izquierda:
;   1. ms     (uint8_t) -> sp+1
;   2. f2     (uint16_t) -> sp+2(lo), sp+3(hi)
;   3. f1     (uint16_t) -> sp+4(lo), sp+5(hi)
; --------------------------------------------------
_sid_play_tone:
    ldy #5
    lda (sp),y
    beq @nof1
    ldy #4
    lda (sp),y
    sta $D400
    ldy #5
    lda (sp),y
    sta $D401
    lda #$41
    sta $D404
@nof1:
    ldy #3
    lda (sp),y
    beq @nof2
    ldy #2
    lda (sp),y
    sta $D407
    ldy #3
    lda (sp),y
    sta $D408
    lda #$41
    sta $D40B
@nof2:
    ldy #1
    lda (sp),y
    ldx #0
    jsr $BF33       ; rom_delay_ms(ms)

    lda #$40
    sta $D404
    sta $D40B
    delay 3
    rts

; --------------------------------------------------
; Stubs
; --------------------------------------------------
_sid_sweep:
_sid_noise:
_sid_play_jingle:
_sid_cpu_thinking_start:
_sid_cpu_thinking_stop:
_sid_sfx_select:
_sid_sfx_illegal:
_sid_sfx_castle:
_sid_sfx_capture:
_sid_sfx_check:
    rts

; --------------------------------------------------
; sid_sfx_move(void)
; --------------------------------------------------
_sid_sfx_move:
    lda #$00
    sta $D400
    lda #$03
    sta $D401
    lda #$41
    sta $D404
    delay 20
    lda #$40
    sta $D404
    rts

; --------------------------------------------------
; sid_sfx_checkmate(void)
; --------------------------------------------------
_sid_sfx_checkmate:
    lda #$D0
    sta $D400
    lda #$13
    sta $D401
    lda #$41
    sta $D404
    lda #$E8
    sta $D407
    lda #$09
    sta $D408
    lda #$41
    sta $D40B
    delay 200

    lda #$E0
    sta $D400
    lda #$0E
    sta $D401
    lda #$F4
    sta $D407
    lda #$04
    sta $D408
    delay 300

    lda #$40
    sta $D404
    sta $D40B
    rts

; --------------------------------------------------
; sid_sfx_promote(void)
; --------------------------------------------------
_sid_sfx_promote:
    lda #$E0
    sta $D400
    lda #$0E
    sta $D401
    lda #$41
    sta $D404
    delay 60

    lda #$50
    sta $D400
    lda #$16
    sta $D401
    delay 100

    lda #$40
    sta $D404
    rts
