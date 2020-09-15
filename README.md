# MAC - My Anti Chess

A fast [anti-chess](https://lichess.org/team/antichess) simulator for Reinforcement learning in C.

## Table of Contents
  - [Table of Contents](#table-of-contents)
  - [How to use](#how-to-use)
  - [About the Env](#about-the-env)
    - [Board Representation](#board-representation)
    - [Move Representation](#move-representation)
  - [Functionalities](#functionalities)
    - [Initialization](#initialization)
    - [Move Generation](#move-generation)
    - [Game termination](#game-termination)
    - [1v1 Game](#1v1-game)
  - [To Do](#to-do)

## How to use
Download or clone the repo and enter the folder and use the following command
  ```
    make  
  ```
Now you can run the mac executable which will display a the starting position on the command line, and available moves for black. There are many pre-defined useful function which are written in failry readable code, after reading the description you can use them to
1) Run tree search
2) Find terminal states
3) Find 3 fold repetitions
4) Even play a nice ol' game of anti-chess :)

## About the Env

### Board Representation:

The board is primariliy a C structure. It uses [bitboard representation](https://www.chessprogramming.org/Bitboards) to take advantage of the 64 bit processor and generate moves much faster as compared to normal loops. Each piece type has its own bitboard. [De Bruijn bitscan](https://www.chessprogramming.org/BitScan) is used to find the lowest and the higest bit position speedily. Each board position is encrypted using a 64 bit hash-key which is found using XOR,randomly generated piece and position keys. So given a position, a unique hashkey is generated which helps to find repetitions.

### Move Representation:
Each move is represented by a 20 bit number. Where:
* The least significant six bits represent the From square of the move.
* The next six bits represent the To square of the move.
* The next 4 bits represent which piece has been captured, if any.
* The next 4 bit represent which peice has the pawn been promoted to, if any.

## Functionalities

### Initialization
The *allInit* function is the initialisation function which should be called before any experiments. This function initializes all data buffers like hashkeys, masking bitboards and default rook, bishop and knight moves.

You can start the board from any [fen position](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation) by defining fen in defs.h and calling the function *ParseFen*. I have pre-defined some FEN in defs.h

```
//Sample FEN
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN1 "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define TEST "rnbqkbQr/ppRp1pPp/3B4/2p1pP2/1P1P1K2/1N4nP/P1P1P3/5RBN w KQkq e6 0 1"
#define TEST2 "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 0 1"
```


### Move Generation
Moves for every piece are generated using the [classical approach](https://www.chessprogramming.org/Classical_Approach) of bitboard representation.                                                      *generateAllMoves* is a function which generates all moves given the current position.

### Game Termination
A game ends when
* Either of the player loose all of their pieces.
* Three-fold repetition occurs on the board.
* The number of moves exceed 1024.                                             

*done* function in done.c tracks all three returns different values accordingly

### 1v1 Game
Uncomment the play function call in mac.c to play a game of anti-chess. Moves should be entered in string format in the terminal. For example:                                                      
```
b3b4
a7a8q
```

## To do
* Training an agent to solve anti-chess in C using [darknet](https://pjreddie.com/darknet/)
* Looking for someone who has expertise in python c extensions or ctypes or cffi or cython extensions to extend the environment and import it in python.
* Adding automatic monte-carlo tree search functionalities

