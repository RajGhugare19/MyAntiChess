#include "stdio.h"
#include "defs.h"
#include "stdlib.h"

void removePiece(ChessBoard *pos,const int square)
{
    ASSERT(pos->pieces[square] != EMPTY);
    int piece,color,i,pindex=-1;
    piece = pos->pieces[square];
    pos->pieces[square] = EMPTY;
    color = PceColor[piece];
    pos->pieceBits[piece] &= clearPiece[square];
    if(color==BLACK)
    {
        pos->AllBlackPieces &= clearPiece[square];
    }
    else
    {
        pos->AllWhitePieces &= clearPiece[square];
    }
    pos->AllPieces &= clearPiece[square];
    pos->posKey ^= pieceKey[piece][square];
    for(i=0;i<pos->pieceNum[piece];i++)
    {
        //printf("   %d   \n",piece);
        if(pos->pieceList[piece][i]==square)
        {
            pindex = i;
            break;
        }
    }
    ASSERT(pindex != -1);
    pos->pieceNum[piece]--;
    pos->pieceList[piece][pindex] = pos->pieceList[piece][pos->pieceNum[piece]];
}

void addPiece(ChessBoard *pos, const int square, const int piece)
{
    ASSERT(pos->pieces[square]==EMPTY)
    int color;
    pos->pieces[square] = piece;
    color = PceColor[piece];
    pos->posKey ^= pieceKey[piece][square];
    pos->pieceBits[piece] |= maskPiece[square];
    if(color==BLACK)
    {
        pos->AllBlackPieces |= maskPiece[square];
    }
    else
    {
        pos->AllWhitePieces |= maskPiece[square];
    }
    pos->AllPieces |= maskPiece[square];
    pos->posKey ^= pieceKey[piece][square];
    pos->pieceList[piece][pos->pieceNum[piece]] = square;
    pos->pieceNum[piece]++;
}

void movePiece(ChessBoard *pos, const int from, const int to)
{
    ASSERT(pos->pieces[from] != EMPTY);
    ASSERT(pos->pieces[to]==EMPTY);
    int color,piece,i;
    piece = pos->pieces[from];
    color = PceColor[piece];

    pos->pieces[from] = EMPTY;
    pos->pieces[to] = piece;

    pos->posKey ^= pieceKey[piece][from];
    pos->posKey ^= pieceKey[piece][to];

    pos->pieceBits[piece] &= clearPiece[from];
    pos->pieceBits[piece] |= maskPiece[to];
    if(color==BLACK)
    {
        pos->AllBlackPieces &= clearPiece[from];
        pos->AllBlackPieces |= maskPiece[to];
    }
    else
    {
        pos->AllWhitePieces &= clearPiece[from];
        pos->AllWhitePieces |= maskPiece[to];
    }
    pos->AllPieces &= clearPiece[from];
    pos->AllPieces |= maskPiece[to];

    for(i=0;i<pos->pieceNum[piece];i++)
    {
        if(pos->pieceList[piece][i] == from)
        {
            pos->pieceList[piece][i] = to;
            break;
        }
    }
    ASSERT(pos->pieceList[piece][i]==to);
}

void makeMove(ChessBoard *pos, int move)
{
    int from,to,captured,promoted;
    from = FROMSQ(move);
    to = TOSQ(move);
    captured = CAPTURED(move);
    promoted = PROMOTED(move);
    pos->hist[pos->hisply].poskey = pos->posKey;
    pos->hist[pos->hisply].move = move;
    if(captured != EMPTY)
    {
        removePiece(pos,to);
        
    }
    pos->hisply++;
    movePiece(pos,from,to);
    if(promoted)
    {
        removePiece(pos,to);
        addPiece(pos,to,promoted);
    }
    pos->side ^= 1;
}

void takeMove(ChessBoard *pos, int move)
{
    int from,to,captured,promoted;
    pos->hisply--;
    move = pos->hist[pos->hisply].move;
    from = FROMSQ(move);
    to = TOSQ(move);
    captured = CAPTURED(move);
    promoted = PROMOTED(move);
    if(promoted)
    {
        removePiece(pos,to);
        addPiece(pos,to,bP - 6*(pos->side));
    }
    movePiece(pos,to,from);
    if(captured != EMPTY)
    {
        addPiece(pos,to,captured);
    }
    pos->posKey = pos->hist[pos->hisply].poskey;
    pos->side ^= 1;
}