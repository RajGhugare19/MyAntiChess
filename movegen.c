#include "stdio.h"
#include "defs.h"
#include "stdlib.h"

void addCaptureMove(const ChessBoard *pos,  MoveList *list,int square, int to)
{
	int move = 0;
    int captured = pos->pieces[to];
	ASSERT(captured != EMPTY);
    move = square | (to<<6) | (captured<<12);
	list->move[list->count] = move;	
    list->count++;
	return;
}

void addSilentMove(const ChessBoard *pos, MoveList *list, int square, int to)
{
	int promotion;
	ASSERT(pos->pieces[to] == EMPTY);
	int move = square | (to<<6);
	if(pos->pieces[square]==wP && squareRank[to]==RANK_8)
	{
		for(promotion=wN;promotion<=wQ;promotion++)
		{
			list->move[list->count] = move | (promotion<<16);
			list->count++;
		}
	}
	if(pos->pieces[square]==bP && squareRank[to]==RANK_1)
	{
		for(promotion=bN;promotion<=bQ;promotion++)
		{
			list->move[list->count] = move | (promotion<<16);
			list->count++;
		}
	}
	move |= (EMPTY<<12);
	list->move[list->count] = move;
	list->count ++;
}

void addPawnCaptures(const ChessBoard *pos,  MoveList *list,int square, int to)
{
    //Pawn captures need different function because they can be promoted to different pieces as well
	int promotion;
    int move = 0;
    int captured = pos->pieces[to];
	ASSERT(captured!=EMPTY);
    move = square | (to<<6) | (captured<<12);
	if(pos->side==WHITE)
	{
		if(squareRank[to]==RANK_8)
		{
			for(promotion=wN;promotion<=wQ;promotion++)
			{
				list->move[list->count] = move | (promotion<<16);
				list->count++;
			}
		}
		else
		{
			list->move[list->count] = move;	
			list->count++;
		}
	}
	if(pos->side==BLACK)
	{
		if(squareRank[to]==RANK_1)
		{
			for(promotion=bN;promotion<=bQ;promotion++)
			{
				list->move[list->count] = move | (promotion<<16);
				list->count++;
			}
		}
		else
		{
			list->move[list->count] = move;	
			list->count++;
		}
	}
    return;
}

void pawnCaptures(const ChessBoard *pos, MoveList *list)
{
    int i=0;
    int square=-1,to;
    BITBOARD pawn_mask = 0ULL;
	if(pos->side==WHITE)
	{
		for(i=0;i<pos->pieceNum[wP];i++)
		{
			square = pos->pieceList[wP][i];
			pawn_mask = maskPiece[square];
			if(((pawn_mask & clearFile[FILE_A]) << 7) & pos->AllBlackPieces)
			{
				to = square + 7;
				addPawnCaptures(pos,list,square,to);
			}
			if(((pawn_mask & clearFile[FILE_H]) << 9) & pos->AllBlackPieces)
			{
				to = square + 9;
				addPawnCaptures(pos,list,square,to);
			}
		}
	}
	else
	{
		for(i=0;i<pos->pieceNum[bP];i++)
    {
        square = pos->pieceList[bP][i];
        pawn_mask = maskPiece[square];
        if(((pawn_mask & clearFile[FILE_A]) >> 9) & pos->AllWhitePieces)
        {
            to = square - 9;
            addPawnCaptures(pos,list,square,to);
        }
        if(((pawn_mask & clearFile[FILE_H]) >> 7) & pos->AllWhitePieces)
        {
            to = square - 7;
            addPawnCaptures(pos,list,square,to);
        }
    }
	}
}

void knightCaptures(const ChessBoard *pos, MoveList *list)
{
	int i = 0;
	int square = 0,to;
	BITBOARD knight_captures = 0ULL;
	if(pos->side == WHITE)
	{
		for(i=0;i<pos->pieceNum[wN];i++)
		{
			square = pos->pieceList[wN][i];
			knight_captures = moveKnight[square] & pos->AllBlackPieces;
			while(knight_captures)
			{
				to = bitscanForward(knight_captures);
				addCaptureMove(pos,list,square,to);
				knight_captures &= clearPiece[to];
			}
		}
	}
	else
	{
		for(i=0;i<pos->pieceNum[bN];i++)
		{
			square = pos->pieceList[bN][i];
			knight_captures = moveKnight[square] & pos->AllWhitePieces;
			while(knight_captures)
			{
				to = bitscanForward(knight_captures);
				addCaptureMove(pos,list,square,to);
				knight_captures &= clearPiece[to];
			}
		}	
	}
}

void kingCaptures(const ChessBoard *pos, MoveList *list)
{
	int i = 0;
	int square = 0,to;
	BITBOARD king_captures = 0ULL;
	if(pos->side==WHITE)
	{
		if(pos->pieceNum[wK]==0)
		{
			return;
		}
		square = pos->pieceList[wK][0];
		king_captures = moveKing[square] & pos->AllBlackPieces;
		while(king_captures)
		{
			to = bitscanForward(king_captures);
			addCaptureMove(pos,list,square,to);
			king_captures &= clearPiece[to];
		}
	}	
	if(pos->side==BLACK)
	{
		if(pos->pieceNum[bK]==0)
		{
			return;
		}
		square = pos->pieceList[bK][0];
		king_captures = moveKing[square] & pos->AllWhitePieces;
		while(king_captures)
		{
			to = bitscanForward(king_captures);
			addCaptureMove(pos,list,square,to);
			king_captures &= clearPiece[to];
		}
	}	
}

void bishopCaptures(const ChessBoard *pos, MoveList *list)
{
	int i,dir;
	int square = 0,to;
	BITBOARD blocker=0ULL;
	BITBOARD blockers=0ULL;
	if(pos->side == WHITE)
	{
		for(i=0;i<pos->pieceNum[wB];i++)
		{
			square =  pos->pieceList[wB][i];
			for(dir=NW;dir<=SW;dir=dir+2)
			{
				blockers = Rays[dir][square] & pos->AllPieces;
				if(scan[dir]==FORWARD)
				{
					blocker = blockers & (-blockers);
				}
				else
				{
					blocker = highestSetBit(blockers);
				}
				
				blocker = blocker & pos->AllBlackPieces;
				if(blocker)
				{
					to = bitscanForward(blocker);
					addCaptureMove(pos,list,square,to);
				}
			}
		}
	}
	else
	{
		for(i=0;i<pos->pieceNum[bB];i++)
		{
			square =  pos->pieceList[bB][i];
			for(dir=NW;dir<=SW;dir=dir+2)
			{
				blockers = Rays[dir][square] & pos->AllPieces;
				if(scan[dir]==FORWARD)
				{
					blocker = blockers & (-blockers);
				}
				else
				{
					blocker = highestSetBit(blockers);
				}
				
				blocker = blocker & pos->AllWhitePieces;
				if(blocker)
				{
					to = bitscanForward(blocker);
					addCaptureMove(pos,list,square,to);
				}
			}
		}
	}
}

void rookCaptures(const ChessBoard *pos, MoveList *list)
{
	int i,dir;
	int square = 0,to;
	BITBOARD blocker=0ULL;
	BITBOARD blockers=0ULL;
	if(pos->side == WHITE)
	{
		for(i=0;i<pos->pieceNum[wR];i++)
		{
			square =  pos->pieceList[wR][i];
			for(dir=W;dir<=S;dir=dir+2)
			{
				blockers = Rays[dir][square] & pos->AllPieces;
				if(scan[dir]==FORWARD)
				{
					blocker = blockers & (-blockers);
				}
				else
				{
					blocker = highestSetBit(blockers);
				}
				
				blocker = blocker & pos->AllBlackPieces;
				if(blocker)
				{
					to = bitscanForward(blocker);
					addCaptureMove(pos,list,square,to);
				}
			}
		}
	}
	else	
	{
		for(i=0;i<pos->pieceNum[bR];i++)
		{
			square =  pos->pieceList[bR][i];
			for(dir=W;dir<=S;dir=dir+2)
			{
				blockers = Rays[dir][square] & pos->AllPieces;
				if(scan[dir]==FORWARD)
				{
					blocker = blockers & (-blockers);
				}
				else
				{
					blocker = highestSetBit(blockers);
				}
				
				blocker = blocker & pos->AllWhitePieces;
				if(blocker)
				{
					to = bitscanForward(blocker);
					addCaptureMove(pos,list,square,to);
				}
			}
		}
	}
}

void queenCaptures(const ChessBoard *pos, MoveList *list)
{
	int i,dir;
	int square = 0,to;
	BITBOARD blocker=0ULL;
	BITBOARD blockers=0ULL;
	if(pos->side == WHITE)
	{
		for(i=0;i<pos->pieceNum[wQ];i++)
		{
			square =  pos->pieceList[wQ][i];
			for(dir=0;dir<=7;dir++)
			{
				blockers = Rays[dir][square] & pos->AllPieces;
				if(scan[dir]==FORWARD)
				{
					blocker = blockers & (-blockers);
				}
				else
				{
					blocker = highestSetBit(blockers);
				}
				
				blocker = blocker & pos->AllBlackPieces;
				if(blocker)
				{
					to = bitscanForward(blocker);
					addCaptureMove(pos,list,square,to);
				}
			}
		}
	}
	else
	{
		for(i=0;i<pos->pieceNum[bQ];i++)
		{
			square =  pos->pieceList[bQ][i];
			for(dir=0;dir<=7;dir++)
			{
				blockers = Rays[dir][square] & pos->AllPieces;
				if(scan[dir]==FORWARD)
				{
					blocker = blockers & (-blockers);
				}
				else
				{
					blocker = highestSetBit(blockers);
				}
				
				blocker = blocker & pos->AllWhitePieces;
				if(blocker)
				{
					to = bitscanForward(blocker);
					addCaptureMove(pos,list,square,to);
				}
			}
		}
	}
}

void pawnSilentMoves(const ChessBoard *pos, MoveList *list)
{
	int i=0;
    int square=-1;
    BITBOARD pawn_mask = 0ULL;
	if(pos->side==WHITE)
	{
		for(i=0;i<pos->pieceNum[wP];i++)
		{
			square = pos->pieceList[wP][i];
			pawn_mask = maskPiece[square];
			if(squareRank[square]==1 && (~(pos->AllPieces) & pawn_mask<<8))
			{
				addSilentMove(pos,list,square,square+8);
				if(~(pos->AllPieces) & (pawn_mask<<16))
				{
					addSilentMove(pos,list,square,square+16);
				}
			}
			else
			{
				if((pawn_mask<<8) & ~(pos->AllPieces))
				{
					addSilentMove(pos,list,square,square+8);
				}
			}
		}
	}
	else
	{
		for(i=0;i<pos->pieceNum[bP];i++)
		{
			square = pos->pieceList[bP][i];
			pawn_mask = maskPiece[square];
			if(squareRank[square]==6 &&  (~(pos->AllPieces) & pawn_mask>>8))
			{
				addSilentMove(pos,list,square,square-8);
				if(~(pos->AllPieces) & (pawn_mask>>16))
				{
					addSilentMove(pos,list,square,square-16);
				}
			}
			else
			{
				if((pawn_mask>>8) & ~(pos->AllPieces))
				{
					addSilentMove(pos,list,square,square-8);
				}
			}
		}
	}
}

void kingSilentMoves(const ChessBoard *pos, MoveList *list)
{
	int i = 0;
	int square = 0,to;
	BITBOARD king = 0ULL;
	if(pos->side==WHITE)
	{
		if(pos->pieceNum[wK]==0)
		{
			return;
		}
		square = pos->pieceList[wK][0];
		king = moveKing[square] & ~pos->AllPieces;
		while(king)
		{
			to = bitscanForward(king);
			addSilentMove(pos,list,square,to);
			king &= clearPiece[to];
		}
	}	
	if(pos->side==BLACK)
	{
		if(pos->pieceNum[bK]==0)
		{
			return;
		}
		square = pos->pieceList[bK][0];
		king = moveKing[square] & ~pos->AllPieces;
		while(king)
		{
			to = bitscanForward(king);
			addSilentMove(pos,list,square,to);
			king &= clearPiece[to];
		}
	}	
}

void knightSilentMoves(const ChessBoard *pos, MoveList *list)
{ 
	int i = 0;
	int square = 0,to;
	BITBOARD knight = 0ULL;
	if(pos->side == WHITE)
	{
		for(i=0;i<pos->pieceNum[wN];i++)
		{
			square = pos->pieceList[wN][i];
			knight = moveKnight[square] & ~pos->AllPieces;
			while(knight)
			{
				to = bitscanForward(knight);
				addSilentMove(pos,list,square,to);
				knight &= clearPiece[to];
			}
		}
	}
	else
	{
		for(i=0;i<pos->pieceNum[bN];i++)
		{
			square = pos->pieceList[bN][i];
			knight = moveKnight[square] & ~pos->AllPieces;
			while(knight)
			{
				to = bitscanForward(knight);
				addSilentMove(pos,list,square,to);
				knight &= clearPiece[to];
			}
		}	
	}
}

void bishopSilentMoves(const ChessBoard *pos, MoveList *list)
{
	int i,dir,side;
	side = pos->side;
	int square = 0,to,blk_square;
	BITBOARD blocker=0ULL;
	BITBOARD blockers=0ULL;
	BITBOARD moves=0ULL;
	for(i=0;i<pos->pieceNum[bB - 6*side];i++)
	{
		square =  pos->pieceList[bB - 6*side][i];
		for(dir=NW;dir<=SW;dir=dir+2)
		{
			blockers = Rays[dir][square] & pos->AllPieces;
			if(scan[dir]==FORWARD)
			{
				blocker = blockers & (-blockers);
			}
			else
			{
				blocker = highestSetBit(blockers);
			}
			if(blocker)
			{
				blk_square = bitscanForward(blocker);
				moves = (Rays[dir][square] ^ Rays[dir][blk_square]) & ~pos->AllPieces;
			}
			else
			{
				moves = Rays[dir][square];
			}
			while(moves)
			{
				to = bitscanForward(moves);
				addSilentMove(pos,list,square,to);
				moves &= clearPiece[to];
			}	
		}
	}
}

void rookSilentMoves(const ChessBoard *pos, MoveList *list)
{
	int i,dir,side;
	side = pos->side;
	int square = 0,to,blk_square;
	BITBOARD blocker=0ULL;
	BITBOARD blockers=0ULL;
	BITBOARD moves=0ULL;
	for(i=0;i<pos->pieceNum[bR - 6*side];i++)
	{
		square =  pos->pieceList[bR - 6*side][i];
		for(dir=W;dir<=S;dir=dir+2)
		{
			blockers = Rays[dir][square] & pos->AllPieces;
			if(scan[dir]==FORWARD)
			{
				blocker = blockers & (-blockers);
			}
			else
			{
				blocker = highestSetBit(blockers);
			}
			if(blocker)
			{
				blk_square = bitscanForward(blocker);
				moves = (Rays[dir][square] ^ Rays[dir][blk_square]) & ~pos->AllPieces;
			}
			else
			{
				moves = Rays[dir][square];
			}
			
			while(moves)
			{
				to = bitscanForward(moves);
				addSilentMove(pos,list,square,to);
				moves &= clearPiece[to];
			}	
		}
	}
}

void queenSilentMoves(const ChessBoard *pos, MoveList *list)
{
	int i,dir,side;
	side = pos->side;
	int square = 0,to,blk_square;
	BITBOARD blocker=0ULL;
	BITBOARD blockers=0ULL;
	BITBOARD moves=0ULL;
	for(i=0;i<pos->pieceNum[bQ - 6*side];i++)
	{
		square =  pos->pieceList[bQ - 6*side][i];
		for(dir=W;dir<=SW;dir++)
		{
			blockers = Rays[dir][square] & pos->AllPieces;
			if(scan[dir]==FORWARD)
			{
				blocker = blockers & (-blockers);
			}
			else
			{
				blocker = highestSetBit(blockers);
			}
			if(blocker)
			{
				blk_square = bitscanForward(blocker);
				moves = (Rays[dir][square] ^ Rays[dir][blk_square]) & ~pos->AllPieces;
			}
			else
			{
				moves = Rays[dir][square];
			}
			
			while(moves)
			{
				to = bitscanForward(moves);
				addSilentMove(pos,list,square,to);
				moves &= clearPiece[to];
			}	
		}
	}
}

void generateAllMoves(const ChessBoard *pos, MoveList *list)
{
    list->count = 0;
	pawnCaptures(pos, list);
	knightCaptures(pos, list);
	kingCaptures(pos, list);
	bishopCaptures(pos, list);
	rookCaptures(pos, list);
	queenCaptures(pos, list);
	if(list->count == 0)
	{
		pawnSilentMoves(pos, list);
		kingSilentMoves(pos, list);
		knightSilentMoves(pos, list);
		bishopSilentMoves(pos, list);
		rookSilentMoves(pos, list);
		queenSilentMoves(pos, list);
	}
}
