#include "stdio.h"
#include "defs.h"
#include "stdlib.h"

int isAttacked(const ChessBoard *pos, const int sq, const int side)
{
	ASSERT(sq<64 && sq>=0);
	ASSERT(side==WHITE || side==BLACK);
	int dir =0;
	BITBOARD blockers = 0ULL;
	BITBOARD blocker = 0ULL;
	if (side==WHITE)
	{
		if((((maskPiece[sq]&clearFile[FILE_H])>>7) | ((maskPiece[sq]&clearFile[FILE_A])>>9)) & pos->pieceBits[wP]){
			printf("ATTACKED BY WHITE PAWN");
			return TRUE;
		}
		if(moveKnight[sq]&pos->pieceBits[wN]){
			printf("ATTACKED BY A WHITE KNIGHT");
			return TRUE;
		}
		if(moveKing[sq]&pos->pieceBits[wK]){
			printf("ATTACKED BY A WHITE KING");
			return TRUE;
		}
		for(dir=0;dir<8;dir++)
		{
			blocker = 0ULL;
			blockers = Rays[dir][sq] & pos->AllPieces;
			if(!blockers)
			{
				continue;
			}
			if (scan[dir]==FORWARD)
			{
				blocker = blockers & (-blockers);
				if(dir%2 == 0)
				{
					if(blocker & (pos->pieceBits[wR]|pos->pieceBits[wQ]))
					{
						printf("ATTACKED BY WHITE ROOK OR WHITE QUEEN");
						return TRUE;
					}
				}
				else
				{
					if(blocker & (pos->pieceBits[wB]|pos->pieceBits[wQ]))
					{
						printf("ATTACKED BY WHITE BISHOP OR WHITE QUEEN");
						return TRUE;
					}	
					
				}
				
			}
			else
			{
				blocker = highestSetBit(blockers);
				if(dir%2 == 0)
				{
					if(blocker & (pos->pieceBits[wR]|pos->pieceBits[wQ]))
					{
						printf("ATTACKED BY WHITE ROOK OR WHITE QUEEN");
						return TRUE;
					}
				}
				else
				{
					if(blocker & (pos->pieceBits[wB]|pos->pieceBits[wQ]))
					{
						printf("ATTACKED BY WHITE BISHOP OR WHITE QUEEN");
						return TRUE;
					}	
					
				}
			}
			
		}
	}
	if (side==BLACK)
	{
		if((((maskPiece[sq]&clearFile[FILE_H])<<9) | ((maskPiece[sq]&clearFile[FILE_A])<<7)) & pos->pieceBits[bP]){
			printf("ATTACKED BY BLACK PAWN");
			return TRUE;
		}
		if(moveKnight[sq]&pos->pieceBits[bN]){
			printf("ATTACKED BY A BLACK KNIGHT");
			return TRUE;
		}
		if(moveKing[sq]&pos->pieceBits[bK]){
			printf("ATTACKED BY A WHITE KING");
			return TRUE;
		}
		for(dir=0;dir<8;dir++)
		{
			blocker = 0ULL;
			blockers = Rays[dir][sq] & pos->AllPieces;
			if(!blockers)
			{
				continue;
			}
			if (scan[dir]==FORWARD)
			{
				blocker = blockers & (-blockers);
				if(dir%2 == 0)
				{
					if(blocker & (pos->pieceBits[bR]|pos->pieceBits[bQ]))
					{
						printf("ATTACKED BY BLACK ROOK OR BLACK QUEEN");
						return TRUE;
					}
				}
				else
				{
					if(blocker & (pos->pieceBits[bB]|pos->pieceBits[bQ]))
					{
						printf("ATTACKED BY BLACK BISHOP OR BLACK QUEEN");
						return TRUE;
					}	
					
				}
				
			}
			else
			{
				blocker = highestSetBit(blockers);
				if(dir%2 == 0)
				{
					if(blocker & (pos->pieceBits[bR]|pos->pieceBits[bQ]))
					{
						printf("ATTACKED BY BLACK ROOK OR BLACK QUEEN");
						return TRUE;
					}
				}
				else
				{
					if(blocker & (pos->pieceBits[bB]|pos->pieceBits[bQ]))
					{
						printf("ATTACKED BY BLACK BISHOP OR BLACK QUEEN");
						return TRUE;
					}	
					
				}
			}
			
		}
	}
	printf("not attacked");
	return FALSE;
}
