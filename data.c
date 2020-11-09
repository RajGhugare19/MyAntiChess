#include "defs.h"

int scan[8] = {REVERSE,FORWARD,FORWARD,FORWARD,FORWARD,REVERSE,REVERSE,REVERSE};

char PceChar[] = "PNBRQKpnbrqk.";
char SideChar[] = "bw-";
char RankChar[] = "12345678";
char FileChar[] = "abcdefgh";
int squareRank[64] = {0,0,0,0,0,0,0,0,
                    1,1,1,1,1,1,1,1,
                    2,2,2,2,2,2,2,2,
                    3,3,3,3,3,3,3,3,
                    4,4,4,4,4,4,4,4,
                    5,5,5,5,5,5,5,5,
                    6,6,6,6,6,6,6,6,
                    7,7,7,7,7,7,7,7,};
int squareFile[64] = {0,1,2,3,4,5,6,7,
                     0,1,2,3,4,5,6,7,
                     0,1,2,3,4,5,6,7,
                     0,1,2,3,4,5,6,7,
                     0,1,2,3,4,5,6,7,
                     0,1,2,3,4,5,6,7,
                     0,1,2,3,4,5,6,7,
                     0,1,2,3,4,5,6,7};

int PceColor[] = {WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BOTH};