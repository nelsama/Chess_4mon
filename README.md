# ♚ Ajedrez 6502 + SID 6581

Juego de ajedrez contra la CPU para el procesador **6502** corriendo sobre el monitor para FPGA **Tang Nano 9K**, con display ANSI por UART y efectos de sonido generados por el chip **SID 6581**.

![Ajedrez 6502](images/screen.png)

## ✨ Características

- 🧠 **Motor de ajedrez con Minimax + Alpha-Beta**: 3 niveles de dificultad (2, 3 y 4 ply)
- 🎵 **Audio SID 6581**: Efectos para mover, capturar, jaque, jaque mate, enroque y promocion
- ♔ **Modo Unicode**: Piezas con glifos reales (♔♕♖♗♘♙♚♛♜♝♞♟) en terminales UTF-8
- 🔤 **Modo ASCII**: Compatible con cualquier terminal (KQRBNP + colores ANSI)
- 🎮 **Control con cursor**: Movimiento con WASD/Flechas + Enter para seleccionar
- 🔄 **Tablero volteable**: Tecla F para cambiar perspectiva
- ⚡ **Optimizado para 6502**: Codigo eficiente en C89 estricto compatible con CC65

## 🛠️ Especificaciones Técnicas

### Hardware Objetivo
- **CPU**: MOS 6502 @ 3.375MHz
- **Audio**: SID 6581 ($D400)
- **Plataforma**: Tang Nano 9K FPGA
- **Display**: Terminal ANSI compatible via UART

### Software
- **Compilador**: CC65 (`cl65`)
- **Lenguaje**: C89 estricto + ensamblador inline
- **Dependencias**: `romapi.h` (proporcionada por el monitor ROM)

### Motor de Ajedrez
- Algoritmo: Minimax con poda Alpha-Beta
- Evaluacion: Material (P=100, N=320, B=330, R=500, Q=900, K=20000) + Piece-Square Tables
- Ordenacion de movimientos: MVV-LVA (capturas primero)
- Profundidad: 2 ply (Facil), 3 ply (Medio), 4 ply (Dificil)
- Reglas completas: enroque, en passant, promocion, triple repeticion (via historial), 50 movimientos

## 🎮 Controles

| Tecla | Accion |
|---|---|
| `WASD` / `↑←↓→` | Mover cursor por el tablero |
| `Enter` / `Space` | Seleccionar pieza / Confirmar destino |
| `Esc` | Cancelar seleccion |
| `F` | Voltear tablero (vista negras/blancas) |
| `M` | Cambiar Unicode / ASCII |
| `N` | Nuevo juego (reiniciar menus) |
| `Q` | Salir al monitor |

## 🎵 Efectos de Sonido SID

| Evento | Sonido |
|---|---|
| Mover pieza | Click de madera |
| Captura | Barrido + impacto |
| Jaque | Pitidos alternados |
| Jaque mate | Fanfarria descendente |
| Enroque | Doble click |
| Promocion | Arpegio ascendente |
| Movimiento ilegal | Zumbido |
| CPU pensando | Tono ambiente suave |

## 📋 Menus del Juego

Al iniciar se muestran tres menus consecutivos:

1. **Modo de display**: [1] Unicode / [2] ASCII
2. **Dificultad**: [1] Facil / [2] Medio / [3] Dificil
3. **Color del jugador**: [1] Blanco / [2] Negro

## 🚀 Instalacion y Compilacion

### Requisitos
- [CC65](https://cc65.github.io/) compilador cruzado para 6502
- Make
- Conexion serial al dispositivo Tang Nano 9K

### Compilar
```bash
make
```

### Ejecutar en la Tang Nano 9K
```
LOAD chess.bin 0800
R 0800
```

## 📁 Estructura del Proyecto

```
ajedres/
├── src/
│   ├── main.c          # Punto de entrada + menus + loop principal
│   ├── startup.s       # Codigo de inicio (stack, BSS)
│   ├── chess.c         # Tablero, reglas, generacion de movimientos
│   ├── engine.c        # Motor CPU: minimax + alpha-beta + PST
│   ├── render.c        # Renderizado ANSI (Unicode/ASCII)
│   ├── input.c         # Captura de teclas (UART, secuencias escape)
│   └── sid_audio.c     # Efectos de sonido SID 6581
├── include/
│   ├── romapi.h        # API de la ROM del monitor
│   ├── chess.h         # Tipos y constantes del juego
│   ├── engine.h        # API del motor de ajedrez
│   ├── render.h        # API de renderizado
│   ├── input.h         # API de entrada
│   └── sid_audio.h     # API de audio
├── config/
│   └── programa.cfg    # Configuracion del linker
├── makefile
└── README.md
```

## ⚙️ Makefile

```bash
make          # Compilar chess.bin
make clean    # Limpiar archivos generados
make info     # Ver tamano del binario
make map      # Ver mapa de memoria
```
