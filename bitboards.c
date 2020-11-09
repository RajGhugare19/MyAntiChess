#include "stdio.h"
#include "stdlib.h"
#include "defs.h"

const unsigned int Findex64[64] = {
    0,  1, 48,  2, 57, 49, 28,  3,
   61, 58, 50, 42, 38, 29, 17,  4,
   62, 55, 59, 36, 53, 51, 43, 22,
   45, 39, 33, 30, 24, 18, 12,  5,
   63, 47, 56, 27, 60, 41, 37, 16,
   54, 35, 52, 21, 44, 32, 23, 11,
   46, 26, 40, 15, 34, 20, 31, 10,
   25, 14, 19,  9, 13,  8,  7,  6
};

const unsigned int Rindex64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

void printBitboard(BITBOARD bb)
{
	BITBOARD unit = 1ULL;
	int shift = 0;
	int rank = RANK_8;
	int file = FILE_A;
	for(rank=RANK_8;rank>=RANK_1;rank--)
	{
			for(file=FILE_A;file<=FILE_H;file++)
			{
				shift = rank*8 + file;
				if(bb & unit<<shift)
					printf("x ");
				else
					printf("- ");
			}
			printf("\n");
	}	
	printf("\n\n");
}

int bitscanReverse(BITBOARD bb) 
{

/**
 * bitScanReverse
 * @authors Kim Walisch, Mark Dickinson
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of most significant one bit
 */
   const BITBOARD debruijn64 = 0x03f79d71b4cb0a89;
   ASSERT (bb != 0);
   bb |= bb >> 1; 
   bb |= bb >> 2;
   bb |= bb >> 4;
   bb |= bb >> 8;
   bb |= bb >> 16;
   bb |= bb >> 32;
   return Rindex64[(bb * debruijn64) >> 58];
}

int bitscanForward(BITBOARD bb)
{
	const BITBOARD debruijn64 = 0x03f79d71b4cb0a89;
	ASSERT (bb != 0);
	return Findex64[((bb & -bb) * debruijn64) >> 58 ] ;
}

BITBOARD highestSetBit(BITBOARD bb)
{
   bb |= bb >> 1; 
   bb |= bb >> 2;
   bb |= bb >> 4;
   bb |= bb >> 8;
   bb |= bb >> 16;
   bb |= bb >> 32;
   bb += 1;
   return (bb>>1);
}