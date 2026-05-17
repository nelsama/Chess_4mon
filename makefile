CC65_HOME ?= D:/cc65
CC = $(CC65_HOME)/bin/cl65
CA65 = $(CC65_HOME)/bin/ca65
LD65 = $(CC65_HOME)/bin/ld65

CFLAGS = -t none -O --cpu 6502 -I src -I include
ASFLAGS = -t none --cpu 6502

all: output/chess.bin

build:
	-mkdir build
output:
	-mkdir output

output/chess.bin: build/startup.o build/main.o build/chess.o build/engine.o build/render.o build/input.o build/sid_audio.o build/chess_asm.o | build output
	$(LD65) -C config/programa.cfg -m output/chess.map -o $@ $^ $(CC65_HOME)/lib/none.lib

build/main.o: src/main.c | build
	$(CC) -c $(CFLAGS) -o $@ $<

build/chess.o: src/chess.c | build
	$(CC) -c $(CFLAGS) -o $@ $<

build/engine.o: src/engine.c | build
	$(CC) -c $(CFLAGS) -o $@ $<

build/render.o: src/render.c | build
	$(CC) -c $(CFLAGS) -o $@ $<

build/input.o: src/input.c | build
	$(CC) -c $(CFLAGS) -o $@ $<

build/startup.o: src/startup.s | build
	$(CA65) $(ASFLAGS) -o $@ $<

build/sid_audio.o: src/sid_audio.s | build
	$(CA65) $(ASFLAGS) -o $@ $<

build/chess_asm.o: src/chess_asm.s | build
	$(CA65) $(ASFLAGS) -o $@ $<

clean:
	-rmdir /s /q build
	-rmdir /s /q output
	@echo Limpio.

.PHONY: all build output clean

pad:
	py -c "b=open('output/chess.bin','rb').read();open('output/chess.bin','wb').write(b+b'\x00'*(14208-len(b)))"
