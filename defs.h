#ifndef DEFS_H
#define DEFS_H


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

typedef unsigned long long BITBOARD;

#define MAXPOSSIBLEMOVES 1024
#define MAXPOSSIBLEMOVE 256
#define MAXMOVES 2048

//Sample FEN
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN1 "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define TEST "rnbqkbQr/ppRp1pPp/3B4/2p1pP2/1P1P1K2/1N4nP/P1P1P3/5RBN w KQkq e6 0 1"
#define TEST2 "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 0 1"

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

typedef struct
{
	int move;
	BITBOARD poskey;
} History;


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

	int hisply;
	History hist[MAXPOSSIBLEMOVES];
	
} ChessBoard;


typedef struct{
	int move[MAXPOSSIBLEMOVE];
	int count;
} MoveList;

/*GLOBALS*/

extern int scan[8];

extern char PceChar[13];
extern char SideChar[3];
extern char RankChar[8];
extern char FileChar[8];

int squareRank[64];
int squareFile[64];

extern int PceColor[13];

extern BITBOARD clearRank[8];
extern BITBOARD maskRank[8];
extern BITBOARD clearFile[8];
extern BITBOARD maskFile[8];
extern BITBOARD maskPiece[64];
extern BITBOARD clearPiece[64];
extern BITBOARD Rays[8][64];
extern BITBOARD moveKnight[64];
extern BITBOARD moveKing[64];

extern BITBOARD pieceKey[13][64];
extern BITBOARD sideKey;

extern const unsigned int Findex64[64];
extern const unsigned int Rindex64[64];


/*MACROS*/

#define FROMSQ(m) ((m) & 0x3F)
#define TOSQ(m) ((m>>6) & 0x3F)
#define CAPTURED(m) ((m>>12) & 0xF)
#define PROMOTED(m) ((m>>16) & 0xF)


/*functions*/

//init.c
extern void allInit();

// bitboard.c
extern void printBitboard();
extern BITBOARD highestSetBit();
extern int bitscanForward();
extern int bitscanReverse();

//hashkey.c
extern BITBOARD generatePoskey();

//board.c
extern void resetBoard();
extern int ParseFen();
extern void printBoard();
extern void updateList();

//io.c
extern char *printSquare();
extern char *printMove();
extern void printMoveList();

//movegen.c
extern void addPawnCaptures();
extern void pawnCaptures();
extern void pawnSilentMoves();
extern void kingCaptures();
extern void kingSilentMoves();
extern void knightCaptures();
extern void knightSilentMoves();
extern void bishopCaptures();
extern void bishopSilentMoves();
extern void rookCaptures();
extern void rookSilentMoves();
extern void queenCaptures();
extern void queenSilentMoves();
extern void generateAllMoves();
extern void addCaptureMoves();
extern void addSilentMoves();

//attack.c
extern int isAttacked();

//makemove.c
extern void removePiece();
extern void addPiece();
extern void makeMove();
extern void takeMove();
#endif