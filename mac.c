#include "stdio.h"
#include "defs.h"
#include "stdlib.h"

int main() {	

	allInit();	

	ChessBoard sample[1];
	MoveList movelist[1];

	ParseFen(TEST,sample);
	updateList(sample);
	printBoard(sample);
	generateAllMoves(sample,movelist);
}
