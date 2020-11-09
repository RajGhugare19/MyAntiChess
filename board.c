#include "stdio.h"
#include "defs.h"
#include "stdlib.h"

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

void resetBoard(ChessBoard *pos)
{
	int i=0,j=0;
	pos->hisply = 0;
	for(i=0;i<12;i++)
	{
		pos->pieceBits[i] = 0ULL;
		pos->pieceNum[i] = 0;
		for(j=0;j<10;j++)
		{
			pos->pieceList[i][j] =-1; 
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

