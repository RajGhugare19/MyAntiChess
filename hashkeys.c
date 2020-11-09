#include "stdio.h"
#include "defs.h"
#include "stdlib.h"

BITBOARD generatePoskey(ChessBoard *pos)
{
	int sq = 0,piece=0;
	BITBOARD poskey = 0ULL;
	for (sq=0;sq<64;sq++)
	{
		piece = pos->pieces[sq];
		if(piece!=EMPTY)
		{
			ASSERT(piece>=wP && piece<=bK);
			poskey ^= pieceKey[piece][sq];
		}
	}
	if (pos->side == WHITE)
	{
		poskey ^= sideKey;
	}
	return poskey;	
}