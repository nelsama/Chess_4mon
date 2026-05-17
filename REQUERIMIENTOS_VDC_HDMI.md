# Requerimientos del Sistema: Terminal HDMI + Modo Gráfico para Tang Nano 9K con Core 6502

---

## 1. Plataforma

| Especificación | Valor |
|:---------------|:------|
| **FPGA** | GW1NR-9 (Tang Nano 9K) |
| **BRAM total** | 468 Kbits |
| **Core CPU** | 6502 personalizado |
| **ROM sistema** | 16 KB (128 Kbits en BRAM) |
| **RAM sistema** | 16 KB (128 Kbits en BRAM) |
| **Salida de video** | HDMI (TMDS) |
| **Resolución base** | **1280×720 píxeles @ 60Hz (720p)** — relación 16:9, estándar universal HDMI |
| **Pixel clock** | 74.25 MHz |
| **Serial (UART)** | Funcionando en paralelo para comunicación con terminal externo |

---

## 2. Distribución de BRAM

| Componente | Tamaño | BRAM usada |
|:-----------|:------:|:----------:|
| ROM 6502 (16 KB) | 128 Kbits | 27% |
| RAM 6502 (16 KB) | 128 Kbits | 27% |
| **VRAM modo gráfico** (320×180, 2 bpp) | **115.2 Kbits** | **25%** |
| **Font ROM** (256 chars × 8×12) | **24.6 Kbits** | **5%** |
| **Tileset** (128 tiles × 8×8, 2 bpp) | **16 Kbits** | **3%** |
| **Tilemap** (40×22 celdas, 1 byte/celda) | **7 Kbits** | **2%** |
| **Sprites** (16 sprites × 16×16, 2 bpp) | **8.5 Kbits** | **2%** |
| Atributos de sprites (x, y, tile, flags) | 0.5 Kbits | <1% |
| **Margen libre** | **40.2 Kbits (~5 KB)** | **9%** |
| **TOTAL** | **468 Kbits** | **100%** |

---

## 3. Resolución y Escalado

```
┌──────────────┐     ┌──────────────┐
│  VRAM        │     │  Font ROM    │
│  320×180     │     │  256×8×12    │
│  2 bpp       │     │  1 bpp       │
└──────┬───────┘     └──────┬───────┘
       │                     │
       ▼                     ▼
┌──────────────────────────────────┐
│          VDC                      │
│                                   │
│  Modo texto:                      │
│  - Font 8×12 → escalado 2×H → 16 │
│                         2×V → 24 │
│  - 80 cols × 16 px = 1280        │
│  - 30 filas × 24 px = 720        │
│                                   │
│  Modo gráfico:                    │
│  - 320×180 → escalado 4×H → 1280 │
│                         4×V → 720 │
│  (escalado exacto, sin distorsión)│
└──────────────┬───────────────────┘
               │
               ▼
        ┌──────────┐
        │  TMDS    │
        │  720p    │
        │  60Hz    │
        └──────────┘
```

### 3.1 Escalado en modo texto

| Parámetro | Valor |
|:----------|:------|
| Font nativo | 8 × 12 píxeles |
| Escalado horizontal | 2× → 16 píxeles por carácter |
| Escalado vertical | 2× → 24 píxeles por carácter |
| Columnas | 80 × 16 px = 1280 px |
| Filas | 30 × 24 px = 720 px |
| **Resultado** | **80 × 30, mapeo perfecto 1:1 a 720p** |

### 3.2 Escalado en modo gráfico

| Parámetro | Valor |
|:----------|:------|
| Resolución nativa | 320 × 180 píxeles |
| Escalado | 4× horizontal, 4× vertical |
| Resolución final | 1280 × 720 píxeles |
| **Resultado** | **Escalado exacto 4×, sin deformación ni bordes negros** |

---

## 4. Modos de Video

### 4.1 Modo Texto (Terminal)

| Parámetro | Valor |
|:----------|:------|
| Columnas × filas | **80 × 30** caracteres |
| Tamaño de carácter en pantalla | 16 × 24 píxeles (font 8×12 escalado 2×) |
| Resolución efectiva | 1280 × 720 píxeles (1:1, sin bordes) |
| Colores de frente | 16 colores seleccionables por carácter |
| Colores de fondo | 16 colores seleccionables por carácter |
| **VRAM modo texto** | 80 × 30 × 2 bytes = 4,800 bytes (código + color) = **38.4 Kbits** |

> **Nota:** modo texto y modo gráfico **NO** coexisten. La VRAM de 115 Kbits se reutiliza entre ambos modos.

### 4.2 Modo Gráfico (HiRes)

| Parámetro | Valor |
|:----------|:------|
| Resolución nativa | **320 × 180** píxeles |
| Bits por píxel | **2 bpp** (4 colores simultáneos) |
| Relación de aspecto | **16:9** nativa |
| Escalado a HDMI | **4×** → 1280 × 720, **sin bordes ni letterbox** |
| **VRAM modo gráfico** | 320 × 180 × 2 = **115,200 bits = 115.2 Kbits** |

### 4.3 Capa de Tiles

| Parámetro | Valor |
|:----------|:------|
| Cantidad de tiles en tileset | **128 tiles** |
| Tamaño de tile | 8 × 8 píxeles |
| Colores por tile | 4 colores (2 bpp) |
| Grid de tilemap | **40 × 22** celdas |
| **Tileset** | 128 × 8 × 8 × 2 = **16,384 bits = 16 Kbits** |
| **Tilemap** | 40 × 22 × 1 byte = 880 bytes = **7,040 bits = 7 Kbits** |
| Comportamiento | Capa superpuesta sobre el fondo. Donde el tile tiene color 00 (transparente) se ve el fondo. |

### 4.4 Capa de Sprites

| Parámetro | Valor |
|:----------|:------|
| Cantidad de sprites simultáneos | **16 sprites** |
| Tamaño de sprite | 16 × 16 píxeles |
| Colores por sprite | 4 colores (2 bpp) |
| **Sprite patterns** | 16 × 16 × 16 × 2 = **8,192 bits = 8 Kbits** |
| **Atributos por sprite** | Posición X (8 bits), Posición Y (8 bits), Tile index (6 bits), Flags (2 bits: flip X, flip Y) = 24 bits × 16 = 384 bits |
| Prioridad | Se renderizan **encima** de tiles y fondo. Color 00 = transparente. |

---

## 5. Orden de Renderizado (de abajo hacia arriba)

1. **Fondo** (VRAM bitmap 320×180, 4 colores)
2. **Tiles** (40×22 grid, superpuestos sobre el fondo)
3. **Sprites** (16 sprites, superpuestos sobre tiles y fondo)
4. Salida → **HDMI 720p**

---

## 6. Mapa de memoria del VDC

Base del VDC: `$A000`

### 6.1 Registros de Control

| Offset | Nombre | R/W | Descripción |
|:------:|:-------|:---:|:------------|
| `$00` | `VDC_CTRL` | W | Bit 0: modo (0=texto, 1=gráfico). Bit 1: mostrar tiles. Bit 2: mostrar sprites. Bit 3: cursor parpadeante. Bits 4-7: reservado. |
| `$01` | `VDC_DATA` | W | **Modo texto**: escribir carácter aquí → el VDC lo coloca en la posición del cursor y avanza automáticamente. Interpreta `$0A` (LF = newline), `$0D` (CR = carriage return), `$08` (BS = backspace), `$1B` (ESC = comando). |
| `$02` | `VDC_COLOR` | W | Color del último carácter escrito (nibble alto = fondo, nibble bajo = frente) |
| `$03` | `VDC_CURSOR_X` | R/W | Columna actual del cursor (0-79) |
| `$04` | `VDC_CURSOR_Y` | R/W | Fila actual del cursor (0-29) |
| `$05` | `VDC_BGCOLOR` | W | Color de fondo general por defecto |
| `$06` | `VDC_FGCOLOR` | W | Color de frente por defecto |
| `$07` | `VDC_PALETTE` | W | Selección de paleta activa (0 o 1) para modo gráfico/tiles/sprites |
| `$08` | `VDC_GPIXEL` | W | **Modo gráfico**: escribir color de píxel aquí → el VDC lo dibuja en (GX, GY) y auto-incrementa GX |
| `$09` | `VDC_GX` | R/W | Posición X del píxel en modo gráfico (0-319) |
| `$0A` | `VDC_GY` | R/W | Posición Y del píxel en modo gráfico (0-179) |
| `$0B` | `VDC_TILE_IDX` | W | Índice de tile (0-127) a colocar en (TILE_X, TILE_Y) |
| `$0C` | `VDC_TILE_X` | W | Columna del tilemap (0-39) |
| `$0D` | `VDC_TILE_Y` | W | Fila del tilemap (0-21) |
| `$0E` | `VDC_SPR_ATTR` | W | Selecciona sprite (0-15) para configurar |
| `$0F` | `VDC_SPR_DATA` | W | Escribe secuencialmente los bytes de atributo del sprite seleccionado |
| `$10` | `VDC_STATUS` | R | Bit 0: VSync flag (se limpia al leerlo). Bits 1-7: reservado. |

### 6.2 Mapa de memoria (modo texto)

| Dirección (base + offset) | Tamaño | Contenido |
|:-------------------------:|:------:|:----------|
| `$A010` - `$A9FF` | 2,400 bytes | **VRAM texto**: código de carácter (1 byte por posición) |
| `$AA00` - `$B3EF` | 2,400 bytes | **Atributos de color**: nibble alto = fondo, nibble bajo = frente |

### 6.3 Mapa de memoria (modo gráfico)

| Dirección (base + offset) | Tamaño | Contenido |
|:-------------------------:|:------:|:----------|
| `$A010` - `$DC09` | 14,400 bytes | **Bitmap**: 320×180 × 2 bits por píxel (4 píxeles por byte) |
| `$DC0A` - `$DC8D` | 880 bytes | **Tilemap**: 40×22 celdas, 1 byte por celda (índice de tile 0-127) |
| `$DC8E` - `$DD8D` | 256 bytes | **Atributos de sprites**: 16 sprites × 16 bytes cada uno |

---

## 7. Paletas de Color

2 paletas de 4 colores cada una. Cada color es RGB de 12 bits (4 bits por canal):

| Índice | Paleta 0 (por defecto) | Paleta 1 |
|:------:|:-----------------------|:---------|
| 00 | Negro (#000) | Negro (#000) |
| 01 | Blanco (#FFF) | Rojo (#F00) |
| 10 | Cyan (#0FF) | Verde (#0F0) |
| 11 | Magenta (#F0F) | Azul (#00F) |

> **Opcional futuro:** agregar registros `$11-$18` para redefinir los 8 colores en caliente.

---

## 8. Temporización HDMI (720p @ 60Hz)

Estándar **CEA-861 / VESA** para 1280×720 @ 60Hz:

| Parámetro | Valor |
|:----------|:------|
| Pixel clock | **74.25 MHz** |
| Horizontal activo | 1280 píxeles |
| H. Front Porch | 110 píxeles |
| H. Sync | 40 píxeles |
| H. Back Porch | 220 píxeles |
| Total horizontal | 1650 píxeles |
| Vertical activo | 720 líneas |
| V. Front Porch | 5 líneas |
| V. Sync | 5 líneas |
| V. Back Porch | 20 líneas |
| Total vertical | 750 líneas |
| TMDS serial clock | 742.5 MHz (10:1 serialización) |

---

## 9. Font ROM

| Parámetro | Valor |
|:----------|:------|
| Caracteres | 256 (códigos 0x00 - 0xFF) |
| Tamaño de carácter | **8 × 12 píxeles** (formato VGA estándar) |
| Formato | 1 bit por píxel (monocromo, el color se aplica desde VRAM) |
| **BRAM** | 256 × 8 × 12 = **24,576 bits = 24.6 Kbits** |
| Contenido | ASCII estándar (0x20-0x7E) + caracteres semigráficos (0x80-0xFF, estilo C64) |

---

## 10. Interfaz con el 6502

El 6502 se comunica con el VDC mediante **lecturas y escrituras a memoria mapeada**. No hay puertos de E/S.

### 10.1 Ejemplo modo texto (assembler)

```asm
; Terminal loop: leer UART y escribir en pantalla
terminal_loop:
  JSR uart_read      ; leer carácter por UART
  STA VDC_DATA       ; escribirlo en pantalla (VDC maneja cursor, scroll, etc.)
  JMP terminal_loop

; Escribir cadena
print_string:
  LDA $0000, Y
  BEQ print_end
  STA VDC_DATA
  INY
  JMP print_string
print_end:
  RTS

; Posicionar cursor
LDA #10
STA VDC_CURSOR_X    ; columna 10
LDA #5
STA VDC_CURSOR_Y    ; fila 5

; Color rojo sobre azul
LDA #$01            ; fondo=negro(0), frente=rojo(1)
STA VDC_COLOR
```

### 10.2 Ejemplo modo gráfico (assembler)

```asm
; Cambiar a modo gráfico
LDA #$01
STA VDC_CTRL

; Dibujar píxel blanco en (100, 50)
LDA #100
STA VDC_GX
LDA #50
STA VDC_GY
LDA #$01            ; color blanco
STA VDC_GPIXEL      ; dibuja y auto-incrementa X

; Dibujar línea horizontal (10 píxeles)
LDX #10
hline_loop:
  STA VDC_GPIXEL    ; dibuja y avanza X solo
  DEX
  BNE hline_loop
```

### 10.3 Ejemplo modo gráfico (C)

```c
// Terminal loop
void putchar(char c) {
    VDC_DATA = c;
}

// Dibujar píxel
void pset(int x, int y, int color) {
    VDC_GX = x;
    VDC_GY = y;
    VDC_GPIXEL = color;
}

// Poner tile
void tile(int col, int fila, int idx) {
    VDC_TILE_X = col;
    VDC_TILE_Y = fila;
    VDC_TILE_IDX = idx;
}

// Configurar sprite
void sprite(int n, int x, int y, int t, int fx, int fy) {
    VDC_SPR_ATTR = n;
    VDC_SPR_DATA = x;
    VDC_SPR_DATA = y;
    VDC_SPR_DATA = t | (fx << 6) | (fy << 7);
}

// Esperar VSync
void wait_vsync() {
    while(!(VDC_STATUS & 1));
}
```

---

## 11. Recursos FPGA estimados

| Componente | LUTs (de 8,640) | BRAM (Kbits) |
|:-----------|:---------------:|:------------:|
| Core 6502 | ~2,000 (23%) | 256 (55%) |
| VDC (control + generación de video) | ~1,500 (17%) | — |
| TMDS encoder (HDMI) | ~400 (5%) | — |
| PLL | — | 1 bloque |
| VRAM (320×180) + Font + Tiles + Sprites | — | 171.6 (37%) |
| **Total estimado** | **~3,900 (45%)** | **427.6 (91%)** |
| **Disponible** | **~4,740 (55%)** | **40.4 (9%)** |

---

## 12. Tips y notas para la implementación

Estos son detalles que **no están escritos en la especificación principal** pero que la IA debería tener en cuenta al implementar.

### Tip 1 — Estado después de reset

- Al iniciar la FPGA, el VDC debe quedar en **modo texto** (bit 0 de `VDC_CTRL` = 0).
- Color por defecto: blanco sobre negro (`VDC_COLOR` = $10).
- Cursor en (0,0).
- La VRAM debe inicializarse con espacios (código $20) y color por defecto para que la pantalla arranque limpia.
- Tiles y sprites desactivados (bits 1 y 2 de `VDC_CTRL` = 0).

### Tip 2 — Auto-incremento en modo gráfico

- Al escribir en `VDC_GPIXEL`, el VDC dibuja el píxel en la posición actual (`VDC_GX`, `VDC_GY`) y luego **incrementa `VDC_GX` en 1**.
- Si `VDC_GX` llega a 320, se resetea a 0 y **`VDC_GY` se incrementa en 1**.
- Si `VDC_GY` llega a 180, se resetea a 0.
- Esto permite dibujar líneas horizontales sin tocar registros, solo escribiendo píxeles consecutivos.

### Tip 3 — Interpretación de caracteres especiales en modo texto

- `$0A` (LF): baja el cursor a la siguiente fila, X=0. Si Y=29, hace scroll.
- `$0D` (CR): cursor X=0 (no cambia Y).
- `$08` (BS): cursor X-1 (si X>0), no borra el carácter.
- `$1B` (ESC): marca el inicio de una secuencia de escape. El siguiente byte se interpreta como comando:
  - `ESC [ A` → cursor arriba (si Y>0)
  - `ESC [ B` → cursor abajo (si Y<29)
  - `ESC [ C` → cursor derecha (si X<79)
  - `ESC [ D` → cursor izquierda (si X>0)
  - `ESC [ J` → limpiar pantalla
  - `ESC [ H` → cursor a (0,0)
  - Otros códigos: ignorados.

### Tip 4 — Scroll automático en hardware

- Cuando se recibe un LF (`$0A`) y el cursor está en la última fila (Y=29), el VDC debe:
  1. Copiar la fila 1 a fila 0, fila 2 a fila 1, ..., fila 29 a fila 28.
  2. Limpiar la última fila (fila 29) con espacios y color por defecto.
  3. Dejar el cursor en (0, 29).
- Esta operación debe completarse en **un solo ciclo de barrido vertical** para evitar artifacts.

### Tip 5 — VSync flag

- `VDC_STATUS` bit 0 se pone en 1 al inicio del VSync.
- Se limpia automáticamente cuando el 6502 **lee** el registro.
- Sirve para que el 6502 sincronice animaciones. Ejemplo de uso:

```asm
wait_vsync:
  LDA VDC_STATUS
  AND #$01
  BEQ wait_vsync
  ; acá va la lógica de animación
```

### Tip 6 — Parpadeo del cursor

- Activado por el bit 3 de `VDC_CTRL`.
- El cursor parpadea alternando entre visible e invisible cada **16 frames** (~267 ms a 60 fps).
- En modo texto, el cursor se dibuja como un rectángulo blanco invertido en la posición (`VDC_CURSOR_X`, `VDC_CURSOR_Y`).

### Tip 7 — Sprites sin detección de colisión

- Esta versión **no incluye** detección de colisión entre sprites en hardware.
- Si se necesita en el futuro, se puede agregar como un módulo adicional sin modificar la interfaz del 6502.

### Tip 8 — Paletas fijas por ahora

- Las paletas definidas en la sección 7 son fijas en esta versión.
- Opcional futuro: agregar registros `$11-$18` para redefinir los 8 colores (2 paletas × 4 colores) como RGB444 en caliente.

### Tip 9 — UART y terminal externo

- El UART funciona en paralelo al VDC.
- El firmware del 6502 debe tener un loop principal que lea de UART y escriba en `VDC_DATA`.
- Opcional: agregar un buffer FIFO en hardware para no perder caracteres si el 6502 está ocupado.

---

## 13. Entregables

Para implementar este sistema se requiere:

### 13.1 Módulo VDC (Verilog/SystemVerilog)

- Máquina de estados que lee la VRAM según el modo activo
- Generador de temporización HDMI (720p @ 60Hz)
- Escalador 2× (texto) y 4× (gráfico)
- Blender de capas (fondo + tiles + sprites)
- TMDS encoder para salida HDMI
- Auto-incremento de cursor/píxel
- Scroll automático en hardware
- Interpretación de caracteres especiales (LF, CR, BS, ESC)
- Parpadeo de cursor

### 13.2 Integración con core 6502 existente

- Adaptar el decodificador de direcciones para mapear el VDC en el rango `$A000-$DD8D`
- Conectar buses de datos y control entre 6502 y VDC
- Conectar señal IRQ (opcional, para VSync)

### 13.3 Firmware 6502 (rutinas básicas)

- `putchar(c)` — escribir carácter en pantalla (modo texto)
- `print(str)` — imprimir cadena
- `pset(x, y, color)` — dibujar píxel (modo gráfico)
- `tile(col, fila, idx)` — colocar tile
- `sprite(n, x, y, tile, flags)` — posicionar sprite
- `terminal_loop()` — bucle principal que lee UART y escribe en pantalla

### 13.4 Font ROM (binario)

- Archivo `.mem` o `.hex` con 256 caracteres de 8×12 píxeles
- Incluir ASCII imprimible (0x20-0x7E) + caracteres semigráficos (0x80-0xFF)
- Formato: 12 bytes por carácter (1 byte por fila)

---

## 14. Posibles expansiones futuras

- Redefinición de paletas desde el 6502 (registros `$11-$18`)
- Caracteres redefinibles (cargar font desde el 6502 a una BRAM R/W)
- Buffer UART FIFO en hardware con interrupción
- Detección de colisión de sprites
- Sonido básico (PWM o generador de tonos)
- Más sprites o sprites de mayor tamaño
- Modo de doble buffer para evitar tearing
