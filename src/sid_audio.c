/* ==========================================================================
   SID_AUDIO.C - Efectos SID compactos
   ========================================================================== */
#include "sid_audio.h"
#include "romapi.h"

#define SID_BASE       0xD400
#define SID_FREQ_LO_1  (*(volatile uint8_t *)(SID_BASE + 0x00))
#define SID_FREQ_HI_1  (*(volatile uint8_t *)(SID_BASE + 0x01))
#define SID_PW_LO_1    (*(volatile uint8_t *)(SID_BASE + 0x02))
#define SID_PW_HI_1    (*(volatile uint8_t *)(SID_BASE + 0x03))
#define SID_CTRL_1     (*(volatile uint8_t *)(SID_BASE + 0x04))
#define SID_AD_1       (*(volatile uint8_t *)(SID_BASE + 0x05))
#define SID_SR_1       (*(volatile uint8_t *)(SID_BASE + 0x06))
#define SID_FREQ_LO_2  (*(volatile uint8_t *)(SID_BASE + 0x07))
#define SID_FREQ_HI_2  (*(volatile uint8_t *)(SID_BASE + 0x08))
#define SID_PW_LO_2    (*(volatile uint8_t *)(SID_BASE + 0x09))
#define SID_PW_HI_2    (*(volatile uint8_t *)(SID_BASE + 0x0A))
#define SID_CTRL_2     (*(volatile uint8_t *)(SID_BASE + 0x0B))
#define SID_FREQ_LO_3  (*(volatile uint8_t *)(SID_BASE + 0x0E))
#define SID_FREQ_HI_3  (*(volatile uint8_t *)(SID_BASE + 0x0F))
#define SID_CTRL_3     (*(volatile uint8_t *)(SID_BASE + 0x12))
#define SID_VOL        (*(volatile uint8_t *)(SID_BASE + 0x18))

#define N_C3  0x04F4
#define N_C4  0x09E8
#define N_E4  0x0C88
#define N_G4  0x0EE0
#define N_C5  0x13D0
#define N_D5  0x1650

void sid_init(void) {
    SID_VOL = 0x0F;
    SID_AD_1 = 0x0F; SID_SR_1 = 0x01;
    SID_PW_LO_1 = 0x00; SID_PW_HI_1 = 0x08;
    SID_CTRL_1 = 0x00; SID_CTRL_2 = 0x00; SID_CTRL_3 = 0x00;
}
void sid_off(void) {
    SID_CTRL_1 = 0x00; SID_CTRL_2 = 0x00; SID_CTRL_3 = 0x00;
    SID_VOL = 0x00;
}
void sid_play_tone(uint16_t f1, uint16_t f2, uint8_t ms) {
    if (f1) { SID_FREQ_LO_1 = (uint8_t)f1; SID_FREQ_HI_1 = (uint8_t)(f1 >> 8); SID_CTRL_1 = 0x41; }
    if (f2) { SID_FREQ_LO_2 = (uint8_t)f2; SID_FREQ_HI_2 = (uint8_t)(f2 >> 8); SID_CTRL_2 = 0x41; }
    rom_delay_ms(ms);
    SID_CTRL_1 = 0x40; SID_CTRL_2 = 0x40; rom_delay_ms(3);
}
void sid_sweep(uint16_t f_start, uint16_t f_end, uint8_t ms)
{ (void)f_start; (void)f_end; (void)ms; }
void sid_noise(uint8_t ms) { (void)ms; }

void sid_play_jingle(void) { }
void sid_sfx_move(void)    { sid_play_tone(0x0300, 0, 20); }
void sid_sfx_capture(void)  { }
void sid_sfx_check(void)    { }
void sid_sfx_checkmate(void){ sid_play_tone(N_C5, N_C4, 200); sid_play_tone(N_G4, N_C3, 300); }
void sid_sfx_castle(void)   { }
void sid_sfx_promote(void)  { sid_play_tone(N_G4, 0, 60); sid_play_tone(N_D5, 0, 100); }
void sid_sfx_illegal(void)  { }
void sid_sfx_select(void)   { }
void sid_cpu_thinking_start(void) { }
void sid_cpu_thinking_stop(void)  { }
