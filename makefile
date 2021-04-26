CC=gcc
ASMBIN=nasm

all: encrypt draw cc link
encrypt:
	$(ASMBIN) -o encrypt.o -f elf64 encrypt.asm
draw:
	$(ASMBIN) -o draw.o -f elf64 draw.asm
cc:
	$(CC) -c -g -O0 main.c
link:
	$(CC) -o result main.o encrypt.o draw.o
debug:
	gdb result
clean:
	rm *.o result
