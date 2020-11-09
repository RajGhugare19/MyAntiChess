#include "defs.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>

//Random bitboard generator
#define RAND_64 ((BITBOARD)rand() | \
				 (BITBOARD)rand() << 15 | \
				 (BITBOARD)rand() << 30 | \
				 (BITBOARD)rand() << 45 | \
				 ((BITBOARD)rand() & 0xf) << 60  )



BITBOARD clearRank[8];
BITBOARD maskRank[8];
BITBOARD clearFile[8];
BITBOARD maskFile[8];

BITBOARD clearPiece[64];
BITBOARD maskPiece[64];
BITBOARD Rays[8][64];
BITBOARD moveKnight[64];
BITBOARD moveKing[64];

BITBOARD pieceKey[13][64];
BITBOARD sideKey;

void initClearMask()
{
	BITBOARD unit = 1ULL;
	BITBOARD temp1,temp2;
	int index = 0;
	int i = 0;
	for(index=0;index<8;index++)
	{
		maskRank[index] = 0ULL;
		maskFile[index] = 0ULL;
		temp1 = unit << 8*index;
		temp2 = unit << index;
		for(i=0;i<8;i++)
		{
			maskRank[index] |= (temp1<<i);
			maskFile[index] |= (temp2<<(8*i));	
		}
		clearRank[index] = ~maskRank[index];
		clearFile[index] = ~maskFile[index];
	}
}

void initmaskPiece()
{
	int index = 0;
	BITBOARD unit = 1ULL;
	for (index=0;index<64;index++)
	{
		maskPiece[index] = 0ULL;
		clearPiece[index] = 0ULL;
		maskPiece[index] = (unit<<index);
		clearPiece[index] = ~maskPiece[index];
	}
}

void initRays()
{
	int index = 0 ,square = 0 ,temp = 0;
	for (index=0;index<8;index++)
	{
		switch(index)
		{
			case W:
				for(square=0;square<64;square++)
				{
					Rays[index][square] = 0ULL;
					temp = square-1;
					while(temp/8 == square/8 && temp>=0)
					{
						Rays[index][square] |= maskPiece[temp];
						temp--;
					}
					//printBitboard(Rays[index][square]);
				}
				break;
			case NW:
				for(square=0;square<64;square++)
				{
					Rays[index][square] = 0ULL;
					temp = square; 
					while(temp<=63)
					{
						if (square!=temp)
							Rays[index][square] |= maskPiece[temp];
						if(temp%8==0)
							break;
						temp += 7;
					}
					//printBitboard(Rays[index][square]);
				}
				break;
			case N:
				for(square=0;square<64;square++)
				{
					Rays[index][square] = 0ULL;
					temp = square; 
					while(temp<64)
					{
						if (square!=temp)
							Rays[index][square] |= maskPiece[temp];
						temp += 8;
					}
					//printBitboard(Rays[index][square]);
				}
				break;
			case NE:
				for(square=0;square<64;square++)
				{
					Rays[index][square] = 0ULL;
					temp = square; 
					while(temp<=63)
					{
						
						if(square!=temp)
							Rays[index][square] |= maskPiece[temp];
						if(temp%8 == 7)
							break;
						temp += 9;	
					}
					//printBitboard(Rays[index][square]);
				}
				break;
			case E:
				for(square=0;square<64;square++)
				{
					Rays[index][square] = 0ULL;
					temp = square;
					while(temp/8 == square/8)
					{
						if(square!=temp)
							Rays[index][square] |= maskPiece[temp];	
						temp ++;
					}
					//printBitboard(Rays[index][square]);
				}
				break;
			case SE:
				for(square=0;square<64;square++)
				{
					Rays[index][square] = 0ULL;
					temp=square;
					while(temp>0)
					{
						if(square!=temp)
							Rays[index][square] |= maskPiece[temp];
						if(temp%8 == 7)
							break;
						temp -= 7;						
					}
					//printBitboard(Rays[index][square]);
				}
				break;
			case S:
				for(square=0;square<64;square++)
				{
					Rays[index][square] = 0ULL;
					temp=square;
					while(temp>=0)
					{
						if(square!=temp)
							Rays[index][square] |= maskPiece[temp];
						temp -= 8;
					}
					//printBitboard(Rays[index][square]);;
				}
				break;
			case SW:
				for(square=0;square<64;square++)
				{
					Rays[index][square] = 0ULL;
					temp=square;
					while(temp>=0)
					{
						if(square!=temp)
							Rays[index][square] |= maskPiece[temp];
						if(temp%8==0)
						{
							break;	
						}
						temp -= 9;	
					}
					//printBitboard(Rays[index][square]);
				}
				break;
		}
			
	}
	
}

void initPosKey()
{
	int index = 0;
	int i = 0;
	for (index=wP;index<bK;index++)
	{
		for(i=0;i<64;i++)
		{
			pieceKey[index][i] = RAND_64;
		}
	}
	sideKey = RAND_64;
}

void initKnightMoves()
{
	int index = 0;
	BITBOARD topmost_left=0ULL,top_left=0ULL,bottommost_left=0ULL,bottom_left=0ULL,bottommost_right=0ULL,bottom_right=0ULL,topmost_right=0ULL,top_right=0ULL;
	BITBOARD knight_file_A_cleared=0ULL,knight_file_AB_cleared=0ULL,knight_file_GH_cleared=0ULL,knight_file_H_cleared=0ULL;
	for (index=0;index<64;index++)
	{
		moveKnight[index] = 0ULL;
		knight_file_A_cleared = maskPiece[index] & clearFile[FILE_A];
		knight_file_AB_cleared = maskPiece[index] & clearFile[FILE_B] & clearFile[FILE_A];
		knight_file_GH_cleared = maskPiece[index] & clearFile[FILE_G] & clearFile[FILE_H];
		knight_file_H_cleared = maskPiece[index] & clearFile[FILE_H];
		topmost_left = knight_file_A_cleared << 15;
		top_left = knight_file_AB_cleared << 6;
		bottom_left = knight_file_AB_cleared >> 10;
		bottommost_left = knight_file_A_cleared >> 17;
		
		topmost_right = knight_file_H_cleared << 17;
		top_right = knight_file_GH_cleared << 10;
		bottom_right = knight_file_GH_cleared >> 6;
		bottommost_right = knight_file_H_cleared >> 15;
			
		moveKnight[index] = (topmost_left | top_left | bottom_left | bottommost_left | bottom_right | bottommost_right | top_right | topmost_right);
	}
}

void initKingMoves()
{
	int index = 0;
	BITBOARD top_left=0ULL,left=0ULL,bottom_left=0ULL,bottom=0ULL,bottom_right=0ULL,right=0ULL,top_right=0ULL,top=0ULL;
	BITBOARD king_file_A_cleared,king_file_H_cleared;
	for (index=0;index<64;index++)
	{
		moveKing[index] = 0ULL;
		king_file_A_cleared = maskPiece[index] & clearFile[FILE_A];
		king_file_H_cleared = maskPiece[index] & clearFile[FILE_H];
		
		top_left = king_file_A_cleared << 7;
		left = king_file_A_cleared >> 1;
		bottom_left = king_file_A_cleared >> 9;
		
		top_right = king_file_H_cleared << 9;
		right = king_file_H_cleared << 1;
		bottom_right = king_file_H_cleared >> 7;
		
		top = maskPiece[index] << 8;
		bottom = maskPiece[index] >> 8;
		
		moveKing[index] = (top_left | left | bottom_left | bottom | bottom_right | right | top_right | top);
	}
}

void allInit()
{
    initClearMask();
    initmaskPiece();
    initPosKey();
    initRays();
    initKnightMoves();
    initKingMoves();
}