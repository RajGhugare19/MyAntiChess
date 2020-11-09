#include "stdio.h"
#include "defs.h"

char *printSquare(const int sq)
{
    static char squarestring[3];

    int file,rank;
 
    file = squareFile[sq];
    rank = squareRank[sq];

    
    sprintf(squarestring, "%c%c", ('a' + file), ('1'+rank));

    return squarestring;
}

char *printMove(const int move)
{
    static char movestring[6];
    int filef,rankf,filet,rankt;
    
    filef = squareFile[FROMSQ(move)];
    rankf = squareRank[FROMSQ(move)];

    filet = squareFile[TOSQ(move)];
    rankt = squareRank[TOSQ(move)];


    int promoted = PROMOTED(move);

    if(promoted)
    {
        char piecePromoted = 'q';
        if(promoted==wN || promoted==bK)
        {
            piecePromoted = 'n';
        }
        if(promoted==wR || promoted==bR)
        {
            piecePromoted = 'r';
        }
        if(promoted==wB||promoted==bB)
        {
            piecePromoted = 'b';
        }
        sprintf(movestring, "%c%c%c%c%c", ('a'+filef),('1'+rankf),('a'+filet),('1'+rankt),(piecePromoted));
    }
    else
    {
        sprintf(movestring, "%c%c%c%c", ('a'+filef),('1'+rankf),('a'+filet),('1'+rankt));
    }
    return movestring;
}

void printMoveList(MoveList *list)
{
    int index;
    int move;
    for (index=0;index<list->count;index++)
    {
        printf("%s", printMove(list->move[index]));
        printf("\n");
    }
    printf("Total number of moves available are %d ", list->count);
}
