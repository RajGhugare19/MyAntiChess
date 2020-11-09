all:
	gcc mac.c init.c bitboards.c hashkeys.c board.c data.c attack.c io.c movegen.c makemove.c -o mac