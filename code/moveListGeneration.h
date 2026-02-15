// generates list of moves for current position

#pragma once

#ifndef MOVE
#define MOVE

#include "move.h"

#endif /* MOVE */

#ifndef MAGIC
#define MAGIC

#include "magicNumbers.h"

#endif /* MAGIC */

#ifndef MOVEGEN
#define MOVEGEN

#include "moveGeneration.h"

#endif /* MOVEGEN */

#ifndef BOARD
#define BOARD

#include "board.h"

#endif /* BOARD */

#ifndef PIECESQTABLE
#define PIECESQTABLE

#include "pieceSquareTable.h"

#endif /* PIECESQTABLE */

#ifndef HISTORY
#define HISTORY

#include "historyHelper.h"

#endif /* HISTORY */

const int maxListSize = 256;

// score: 10 bits for history (or see) < 2 bit for killer < 16 bits for mvv-lva < 1 bit for TT move

struct MoveListGenerator {
    const int killerMoveShift = 10, captureShift = 12, hashMoveShift = 28;

    Move moveList[maxDepth][maxListSize];
    int moveListSize[maxDepth];

    Move hashMove;

    Move killerMove, killerBackup;

    int material[6] = {0, 100, 300, 300, 500, 900};

    int seeTable[64][64];

    inline void generateMoves(Board &board, HistoryHelper &historyHelper, int color, int depth, bool doSort,
                              bool onlyCaptures) {
        Board boardCopy = board;
        moveListSize[depth] = 0;

        Bitboard friendPieces, opponentPieces;

        if (color == WHITE) {
            friendPieces = board.whitePieces;
            opponentPieces = board.blackPieces;
        } else {
            friendPieces = board.blackPieces;
            opponentPieces = board.whitePieces;
        }

        Bitboard pieces = friendPieces;
        int currentEvaluation = board.evaluation;

        int prevMoveTarget = board.ply1Sq;

        while (pieces > 0) {
            int startSquare = pieces.getFirstBitNumberAndExclude();
            Bitboard moves = moveGenerator.moves(board, startSquare);
            if (onlyCaptures) {
                moves &= opponentPieces;
            }
            moves &= (~friendPieces);

            while (moves > 0) {
                int targetSquare = moves.getFirstBitNumberAndExclude();

                int captureCoeff = 0, isCapture = 0, sseEval;
                if (opponentPieces.getBit(targetSquare)) {
                    isCapture = 1;
                    // cout<<Move(startSquare,targetSquare,0).convertToUCI()<<'
                    // '<<moveGenerator.sseEval(targetSquare,color,startSquare)<<'\n';
                    int attackingPiece = board.occupancyPiece(startSquare);
                    int capturedPiece = board.occupancyPiece(targetSquare);

                    int captureEval;
                    captureEval = sseEval = moveGenerator.sseEval(board, targetSquare, color, startSquare);

                    seeTable[startSquare][targetSquare] = sseEval;

                    int historyScore = historyHelper.getScore(board, color, Move(startSquare, targetSquare, NOPIECE));
                    int normHistoryScore = historyScore - historyHelper.maxHistoryScore;
                    float historyScoreF = float(normHistoryScore) / historyHelper.maxHistoryScore;

                    if (captureEval <= -100 && onlyCaptures)
                    	continue;

                    if (captureEval >= -220 - historyScoreF * 100)
                        captureCoeff += (1 << 15);

                    if (targetSquare == prevMoveTarget)
                        captureCoeff += 300;

                    captureCoeff += (material[capturedPiece] + historyScore * 20) + 10;

                    // cout<<Move(startSquare,targetSquare,0).convertToUCI()<<' '<<captureEval<<'\n';
                } else
                    captureCoeff =
                        (1 << 15); // if move isn't capture, make it's value below winning SSE but before loosing

                if (board.occupancyPiece(startSquare) == PAWN &&
                    ((color == WHITE && targetSquare < 8) || (color == BLACK && targetSquare >= 56))) { // promotion
                    Move promotionMoves[4];
                    promotionMoves[0] =
                        Move(startSquare, targetSquare, KNIGHT, (material[KNIGHT] + captureCoeff) << captureShift);
                    promotionMoves[1] =
                        Move(startSquare, targetSquare, BISHOP, (material[BISHOP] + captureCoeff) << captureShift);
                    promotionMoves[2] = Move(startSquare, targetSquare, ROOK, (material[ROOK] + captureCoeff) << captureShift);
                    promotionMoves[3] = Move(startSquare, targetSquare, QUEEN, (material[QUEEN] + captureCoeff) << captureShift);
                    
                    for (int i = 0; i < 4; i++) {
                        board.makeMove(promotionMoves[i]);
                        if (moveGenerator.isInCheck(board, color)) {
                            board = boardCopy;
                            continue;
                        }
                        board = boardCopy;
                        if (promotionMoves[i] == hashMove)
                            promotionMoves[i].score += (1 << hashMoveShift);
                        if (onlyCaptures)
                            promotionMoves[i].score += (sseEval + 15);
                        moveList[depth][moveListSize[depth]++] = promotionMoves[i];
                    }
                } else {
                    board.makeMove(Move(startSquare, targetSquare, NOPIECE));
                    if (moveGenerator.isInCheck(board, color)) {
                        board = boardCopy;
                        continue;
                    }
                    board = boardCopy;
                    Move move = Move(startSquare, targetSquare, NOPIECE);
                    move.score += (captureCoeff << captureShift);

                    if (!isCapture || !onlyCaptures)
                        move.score += historyHelper.getScore(board, color, move);
                    else
                        move.score += (sseEval + 15);
                    
                    if (move == hashMove)
                        move.score += (1 << hashMoveShift);
                    if (move == killerMove)
                        move.score += (1 << (killerMoveShift + 1));
                    else if (move == killerBackup)
                        move.score += (1 << killerMoveShift);
                    moveList[depth][moveListSize[depth]++] = move;
                }
            }
        }
        if (doSort)
            stable_sort(moveList[depth], moveList[depth] + moveListSize[depth]);
    }

    inline void generateMovesForPerft(Board &board, int color, int depth) { // optimized gen without scores
        Board boardCopy = board;
        moveListSize[depth] = 0;

        Bitboard friendPieces, opponentPieces;

        if (color == WHITE) {
            friendPieces = board.whitePieces;
            opponentPieces = board.blackPieces;
        } else {
            friendPieces = board.blackPieces;
            opponentPieces = board.whitePieces;
        }

        Bitboard pieces = friendPieces;

        while (pieces > 0) {
            int startSquare = pieces.getFirstBitNumberAndExclude();
            Bitboard moves = moveGenerator.moves(board, startSquare);
            moves &= (~friendPieces);

            while (moves > 0) {
                int targetSquare = moves.getFirstBitNumberAndExclude();

                if (board.occupancyPiece(startSquare) == PAWN &&
                    ((color == WHITE && targetSquare < 8) || (color == BLACK && targetSquare >= 56))) { // promotion
                    Move promotionMoves[4];
                    promotionMoves[0] = Move(startSquare, targetSquare, KNIGHT);
                    promotionMoves[1] = Move(startSquare, targetSquare, BISHOP);
                    promotionMoves[2] = Move(startSquare, targetSquare, ROOK);
                    promotionMoves[3] = Move(startSquare, targetSquare, QUEEN);
                    for (int i = 0; i < 4; i++) {
                        board.makeMove(promotionMoves[i]);
                        if (moveGenerator.isInCheck(board, color)) {
                            board = boardCopy;
                            continue;
                        }
                        moveList[depth][moveListSize[depth]++] = promotionMoves[i];
                    }
                } else {
                    board.makeMove(Move(startSquare, targetSquare, NOPIECE));
                    if (moveGenerator.isInCheck(board, color)) {
                        board = boardCopy;
                        continue;
                    }
                    moveList[depth][moveListSize[depth]++] = Move(startSquare, targetSquare, NOPIECE);
                }
                board = boardCopy;
            }
        }
    }

    bool isStalled(Board &board, int color) {
        Board boardCopy = board;
        Bitboard friendPieces;

        if (color == WHITE)
            friendPieces = board.whitePieces;
        else
            friendPieces = board.blackPieces;

        Bitboard pieces = friendPieces;

        while (pieces > 0) {
            int startSquare = pieces.getFirstBitNumberAndExclude();
            Bitboard moves = moveGenerator.moves(board, startSquare);
            moves &= (~friendPieces);

            while (moves > 0) {
                int targetSquare = moves.getFirstBitNumberAndExclude();

                if (board.occupancyPiece(startSquare) == PAWN &&
                    ((color == WHITE && targetSquare < 8) || (color == BLACK && targetSquare >= 56))) { // promotion
                    Move promotionMoves[4];
                    promotionMoves[0] =
                        Move(startSquare, targetSquare, KNIGHT, pieceSquareTable.materialEval[KNIGHT] << captureShift);
                    promotionMoves[1] =
                        Move(startSquare, targetSquare, BISHOP, pieceSquareTable.materialEval[BISHOP] << captureShift);
                    promotionMoves[2] =
                        Move(startSquare, targetSquare, ROOK, pieceSquareTable.materialEval[ROOK] << captureShift);
                    promotionMoves[3] =
                        Move(startSquare, targetSquare, QUEEN, pieceSquareTable.materialEval[QUEEN] << captureShift);
                    for (int i = 0; i < 4; i++) {
                        board.makeMove(promotionMoves[i]);
                        if (moveGenerator.isInCheck(board, color)) {
                            board = boardCopy;
                            continue;
                        }
                        board = boardCopy;
                        return false;
                    }
                } else {
                    board.makeMove(Move(startSquare, targetSquare, NOPIECE));
                    if (moveGenerator.isInCheck(board, color)) {
                        board = boardCopy;
                        continue;
                    }
                    board = boardCopy;
                    return false;
                }
            }
        }
        return true;
    }
};