// perfomance test

#pragma once

#ifndef MOVEGENLIST
#define MOVEGENLIST

#include "moveListGeneration.h"

#endif /* MOVEGENLIST */

#ifndef BOARD
#define BOARD

#include "board.h"

#endif /* BOARD */

MoveListGenerator moveListGeneratorForPerft;

struct Perftester {
    bool stopPertft;

    int perft(Board &board, int color, int depth) {
        if (depth == 0)
            return 1;
        Board boardCopy = board;
        moveListGeneratorForPerft.generateMovesForPerft(board, color, depth);
        int sum = 0;
        for (int currentMove = 0; currentMove < moveListGeneratorForPerft.moveListSize[depth]; currentMove++) {
            Move move = moveListGeneratorForPerft.moveList[depth][currentMove];
            board.makeMoveLight(move);
            sum += perft(board, color == WHITE ? BLACK : WHITE, depth - 1);
            board = boardCopy;
        }
        return sum;
    }

    void perfTest(int maxDepth) {
        Board boardCopy = mainBoard;
        for (int depth = 1; depth <= maxDepth; depth++) {
            std::chrono::steady_clock::time_point timeBegin = std::chrono::steady_clock::now();
            int color = mainBoard.boardColor;
            moveListGeneratorForPerft.generateMovesForPerft(mainBoard, color, depth);
            ll nodes = 0;
            for (int currentMove = 0; currentMove < moveListGeneratorForPerft.moveListSize[depth]; currentMove++) {
                Move move = moveListGeneratorForPerft.moveList[depth][currentMove];
                mainBoard.makeMoveLight(move);
                int nmb = perft(mainBoard, color == WHITE ? BLACK : WHITE, depth - 1);
                cout << move.convertToUCI() << ' ' << nmb << '\n';
                mainBoard = boardCopy;
                nodes += nmb;
            }
            std::chrono::steady_clock::time_point timeEnd = std::chrono::steady_clock::now();
            ll time = std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeBegin).count();
            cout << "depth " << depth << " nodes " << nodes << " nodes/sec " << nodes * (1000000) / time << " time "
                 << ld(time) / 1e6 << "s\n\n";
        }
    }
};

Perftester perftester;