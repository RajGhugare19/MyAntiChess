#include <stdio.h>
#include "stdlib.h"

typedef unsigned long long BITBOARD;

#define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)) { \
printf("%s - Failed",#n); \
printf("On %s ",__DATE__); \
printf("At %s ",__TIME__); \
printf("In File %s ",__FILE__); \
printf("At Line %d\n",__LINE__); \
exit(1);}
#endif

//Sample FEN
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define FEN1 "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"

//Random bitboard generator
#define RAND_64 ((BITBOARD)rand() | \
				 (BITBOARD)rand() << 15 | \
				 (BITBOARD)rand() << 30 | \
				 (BITBOARD)rand() << 45 | \
				 ((BITBOARD)rand() & 0xf) << 60  )

enum { wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK, EMPTY };

enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };

enum {
  A1 = 0, B1, C1, D1, E1, F1, G1, H1,
  A2 = 8, B2, C2, D2, E2, F2, G2, H2,
  A3 = 16, B3, C3, D3, E3, F3, G3, H3,
  A4 = 24, B4, C4, D4, E4, F4, G4, H4,
  A5 = 32, B5, C5, D5, E5, F5, G5, H5,
  A6 = 40, B6, C6, D6, E6, F6, G6, H6,
  A7 = 48, B7, C7, D7, E7, F7, G7, H7,
  A8 = 56, B8, C8, D8, E8, F8, G8, H8, NO_SQ,
};

enum { BLACK, WHITE, BOTH};

enum { FALSE, TRUE };

enum { W, NW, N, NE, E, SE, S, SW };

enum {FORWARD, REVERSE};

int scan[8] = {REVERSE,FORWARD,FORWARD,FORWARD,FORWARD,REVERSE,REVERSE,REVERSE};

char PceChar[] = "PNBRQKpnbrqk.";
char SideChar[] = "wb-";
char RankChar[] = "12345678";
char FileChar[] = "abcdefgh";

BITBOARD clearRank[8];
BITBOARD maskRank[8];
BITBOARD clearFile[8];
BITBOARD maskFile[8];
BITBOARD maskPiece[64];
BITBOARD Rays[8][64];
BITBOARD moveKnight[64];
BITBOARD moveKing[64];

BITBOARD pieceKey[13][64];
BITBOARD sideKey;

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

typedef struct
{
	
	int pieces[64];
	int side;
	int pieceList[12][10];
	int pieceNum[12];
	
	BITBOARD pieceBits[12];

	/* Commonly derived positions */
	BITBOARD AllBlackPieces;
	BITBOARD AllWhitePieces;
	BITBOARD AllPieces;
	
	BITBOARD posKey;
	
	
} ChessBoard;

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
   ASSERT (bb != 0);
   bb |= bb >> 1; 
   bb |= bb >> 2;
   bb |= bb >> 4;
   bb |= bb >> 8;
   bb |= bb >> 16;
   bb |= bb >> 32;
   bb += 1;
   return (bb>>1);
}

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

void printBoard(const ChessBoard *pos)
{
	printf("\n GAME BOARD: \n\n");
	
	int sq,rank,file,piece;
	
	for(rank=RANK_8;rank>=RANK_1;rank--)
	{
		printf("%3c ",RankChar[rank]);
		for(file=FILE_A; file<=FILE_H;file++)
		{
			sq = rank*8+file;
			piece = pos->pieces[sq];
			printf("%3c",PceChar[piece]);
		}
		printf("\n");
	}
	printf("\n");
	printf("    ");
	for(file=FILE_A;file<=FILE_H;file++)
	{
		printf("%3c",'a'+file);
	}
	printf("\n");
	printf("side:%c\n",SideChar[pos->side]);
	printf("PosKey:  %llX\n",pos->posKey);	
}

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
		maskPiece[index] = (unit<<index);
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

BITBOARD generatePoskey(ChessBoard *pos)
{
	int sq = 0,piece=0;
	BITBOARD poskey = 0ULL;
	for (sq=0;sq<64;sq++)
	{
		if(piece!=EMPTY)
		{
			ASSERT(piece>=wP && piece<=bK);
			piece = pos->pieces[sq];
			poskey ^= pieceKey[piece][sq];
		}
	}
	if (pos->side == WHITE)
	{
		poskey ^= sideKey;
	}
	return poskey;	
}

void resetBoard(ChessBoard *pos)
{
	int i=0,j=0;;
	for(i=0;i<12;i++)
	{
		pos->pieceBits[i] = 0ULL;
		pos->pieceNum[i] = 0;
		for(j=0;j<10;j++)
		{
			pos->pieceList[i][j] = 0; 
		}
	}	
	
	for(i=0;i<64;i++)
	{
		pos->pieces[i] = EMPTY;
	}
	pos->posKey = 0ULL;
	
	pos->AllWhitePieces = 0ULL;
	pos->AllBlackPieces = 0ULL;
	pos->AllPieces = 0ULL;
	
	pos->side = BOTH;
}

int ParseFen(char *fen, ChessBoard *pos)
{
	ASSERT(fen!=NULL)
	ASSERT(pos!=NULL)
		
	int rank = RANK_8;
	int file = FILE_A;
	int piece=EMPTY;
	int count=0;
	int i=0;
	int sq=0;
	
	resetBoard(pos);
	while((rank>=RANK_1))
	{	
		count = 1;
		switch(*fen)
		{
			case 'p': piece = bP; break;
			case 'r': piece = bR; break;
			case 'n': piece = bN; break;
			case 'b': piece = bB; break;
			case 'k': piece = bK; break;
			case 'q': piece = bQ; break;
			case 'P': piece = wP; break;
			case 'R': piece = wR; break;
			case 'N': piece = wN; break;
			case 'B': piece = wB; break;
			case 'K': piece = wK; break;
			case 'Q': piece = wQ; break;
			
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				piece = EMPTY;
				count = *fen - '0';
				break;
			case ' ':
			case '/':
				rank--;
				file = FILE_A;
				fen++;
				continue;
			default:
				printf("FEN error \n");
				return -1;
		}
		for (i=0;i<count;i++)
		{
			sq = rank*8 + file;
			pos->pieces[sq] = piece;
			if(piece!=EMPTY)
			{
				pos->pieceBits[piece] |= maskPiece[sq];
				pos->AllPieces |= maskPiece[sq];
				if(piece>=wP && piece <=wK)
				{
					pos->AllWhitePieces |= maskPiece[sq];
				}
				else
				{
					pos->AllBlackPieces |= maskPiece[sq];
				}
			}
			file++;
		}	
		fen++;
	}
	ASSERT(*fen == 'w'|| *fen =='b');
	
	pos->side = (*fen == 'w') ? WHITE : BLACK;
	fen += 2;
	pos->posKey = generatePoskey(pos);
	return 0;
	
}

BITBOARD king(const ChessBoard *pos, const int side)
{
	//Returns a bitboard containing all possible moves for the white king
	BITBOARD top_left=0ULL,left=0ULL,bottom_left=0ULL,bottom=0ULL,bottom_right=0ULL,right=0ULL,top_right=0ULL,top=0ULL;
	BITBOARD king_file_A_cleared,king_file_H_cleared,Wking_valid;
	
	
	king_file_A_cleared = pos->pieceBits[wK] & clearFile[FILE_A];
	king_file_H_cleared = pos->pieceBits[wK] & clearFile[FILE_H];
	
	top_left = king_file_A_cleared << 7;
	left = king_file_A_cleared >> 1;
	bottom_left = king_file_A_cleared >> 9;
	
	top_right = king_file_H_cleared << 9;
	right = king_file_H_cleared << 1;
	bottom_right = king_file_H_cleared >> 7;
	
	top = pos->pieceBits[wK] << 8;
	bottom = pos->pieceBits[wK] >> 8;
	
	Wking_valid = (top_left | left | bottom_left | bottom | bottom_right | right | top_right | top)&(~(pos->AllWhitePieces));
	return Wking_valid;
}

BITBOARD knight(const ChessBoard *pos, const int side)
{
	//Returns a bitboard containing all possible moves for the white knight
	BITBOARD topmost_left=0ULL,top_left=0ULL,bottommost_left=0ULL,bottom_left=0ULL,bottommost_right=0ULL,bottom_right=0ULL,topmost_right=0ULL,top_right=0ULL;
	BITBOARD knight_file_A_cleared=0ULL,knight_file_AB_cleared=0ULL,knight_file_GH_cleared=0ULL,knight_file_H_cleared=0ULL,knight_valid=0ULL;
	if(side==WHITE)
	{
		knight_file_A_cleared = pos->pieceBits[wN] & clearFile[FILE_A];
		knight_file_AB_cleared = pos->pieceBits[wN] & clearFile[FILE_B] & clearFile[FILE_A];
		knight_file_GH_cleared = pos->pieceBits[wN] & clearFile[FILE_G] & clearFile[FILE_H];
		knight_file_H_cleared = pos->pieceBits[wN] & clearFile[FILE_H];
    }
    if(side==BLACK)
    {
        knight_file_A_cleared = pos->pieceBits[bN] & clearFile[FILE_A];
		knight_file_AB_cleared = pos->pieceBits[bN] & clearFile[FILE_B] & clearFile[FILE_A];
		knight_file_GH_cleared = pos->pieceBits[bN] & clearFile[FILE_G] & clearFile[FILE_H];
		knight_file_H_cleared = pos->pieceBits[bN] & clearFile[FILE_H];	
	}
	topmost_left = knight_file_A_cleared << 15;
	top_left = knight_file_AB_cleared << 6;
	bottom_left = knight_file_AB_cleared >> 10;
	bottommost_left = knight_file_A_cleared >> 17;
	
	topmost_right = knight_file_H_cleared << 17;
	top_right = knight_file_GH_cleared << 10;
	bottom_right = knight_file_GH_cleared >> 6;
	bottommost_right = knight_file_H_cleared >> 15;
	if (side==WHITE)
	{
		knight_valid = (topmost_left | top_left | bottommost_left | bottom_left | bottommost_right | bottom_right | topmost_right | top_right)&(~(pos->AllWhitePieces));
	}
	else
	{
		knight_valid = (topmost_left | top_left | bottommost_left | bottom_left | bottommost_right | bottom_right | topmost_right | top_right)&(~(pos->AllBlackPieces));
	}
	return knight_valid;
}

BITBOARD Wpawn(const ChessBoard *pos)
{
	BITBOARD TWO_SQUARE=0ULL,ONE_SQUARE=0ULL,CAPTURES_top_left=0ULL,CAPTURES_top_right=0ULL;
	BITBOARD pawn_rank_2_masked=0ULL,pawn_rank_2_capable=0ULL,pawn_file_A_cleared=0ULL,pawn_file_H_cleared=0ULL,Wpawn_valid;
	
	pawn_file_A_cleared = pos->pieceBits[wP] & clearFile[FILE_A];
	pawn_file_H_cleared = pos->pieceBits[wP] & clearFile[FILE_H];
	pawn_rank_2_masked = pos->pieceBits[wP] & maskRank[RANK_2];
	pawn_rank_2_capable = (~(pos->AllPieces)) & (pawn_rank_2_masked<<8);
	
	TWO_SQUARE = pawn_rank_2_capable << 8;
	ONE_SQUARE = pos->pieceBits[wP] << 8;
	CAPTURES_top_left = (pawn_file_A_cleared << 7) & pos->AllBlackPieces;
	CAPTURES_top_right = (pawn_file_H_cleared << 9) & pos->AllBlackPieces;
	
	Wpawn_valid = ((TWO_SQUARE | ONE_SQUARE ) & (~(pos->AllPieces))) | (CAPTURES_top_left | CAPTURES_top_right);
	return Wpawn_valid;
}


BITBOARD Bpawn(const ChessBoard *pos)
{
	BITBOARD TWO_SQUARE=0ULL,ONE_SQUARE=0ULL,CAPTURES_bottom_left=0ULL,CAPTURES_bottom_right=0ULL;
	BITBOARD pawn_rank_7_masked=0ULL,pawn_rank_7_capable=0ULL,pawn_file_A_cleared=0ULL,pawn_file_H_cleared=0ULL,Bpawn_valid;
	
	pawn_file_A_cleared = pos->pieceBits[bP] & clearFile[FILE_A];
	pawn_file_H_cleared = pos->pieceBits[bP] & clearFile[FILE_H];
	pawn_rank_7_masked = pos->pieceBits[bP] & maskRank[RANK_7];
	pawn_rank_7_capable = (~(pos->AllPieces)) & (pawn_rank_7_masked>>8);
	TWO_SQUARE = pawn_rank_7_capable >> 8;
	ONE_SQUARE = pos->pieceBits[bP] >> 8;
	CAPTURES_bottom_left = (pawn_file_A_cleared>>9) & pos->AllBlackPieces;
	CAPTURES_bottom_left = (pawn_file_H_cleared>>7) & pos->AllBlackPieces;
	Bpawn_valid = ((TWO_SQUARE | ONE_SQUARE ) & (~(pos->AllPieces))) | ( CAPTURES_bottom_left| CAPTURES_bottom_right);
	return Bpawn_valid;
}


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

void updateList(ChessBoard* pos)
{
	int i=0;
	int piece = 0;
	for (i=0;i<64;i++)
	{
		piece = pos->pieces[i];
		if(piece!=EMPTY)
		{
			pos->pieceList[piece][pos->pieceNum[piece]]=i;
			pos->pieceNum[piece]++;
		}
	}
}


int main()
{
	initClearMask();
	initmaskPiece();
	initRays();
	initPosKey();
	initKnightMoves();
	initKingMoves();
	
	ChessBoard sample[1];
 
    resetBoard(sample);
    
	ParseFen(FEN3,sample);
	printBoard(sample);
	int i=0;
	/*
	for(i=0;i<64;i++)
	{
		printf("%d\n",i);
		printBitboard(highbishopRays[i]);
	}*/
	
	isAttacked(sample,7,BLACK);

	return 0;
}
