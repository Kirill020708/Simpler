// history heruistics helper

#pragma once

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

#ifndef MOVE
#define MOVE

#include "move.h"

#endif /* MOVE */


#ifndef MOVEGEN
#define MOVEGEN

#include "moveGeneration.h"

#endif /* MOVEGEN */

struct HistoryHelper {
    int historyScore[2][64][64][2][2];
    int maxHistoryScore = 511;

    void clear() {
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 64; j++)
                for (int k = 0; k < 64; k++)
	                for (int k1 = 0; k1 < 2; k1++)
	                	for (int k2 = 0; k2 < 2; k2++)
                    		historyScore[i][j][k][k1][k2] = 0; 
    }

    inline void update(int color, Board& board, Move move, int score) {
        if (score < -maxHistoryScore)
            score = -maxHistoryScore;
        if (score > maxHistoryScore)
            score = maxHistoryScore; // clamp

        int oppositeColor = (color == WHITE) ? BLACK : WHITE;

        int st = move.getStartSquare();
        int tr = move.getTargetSquare();

        int stThreat = moveGenerator.isSquareAttacked(board, st, oppositeColor);
        int trThreat = moveGenerator.isSquareAttacked(board, tr, oppositeColor);

        historyScore[color][st][tr][stThreat][trThreat] +=
            score - historyScore[color][st][tr][stThreat][trThreat] * abs(score) / maxHistoryScore;
    }

    inline int getScore(int color, Board& board, Move move) {

        int oppositeColor = (color == WHITE) ? BLACK : WHITE;

        int st = move.getStartSquare();
        int tr = move.getTargetSquare();

        int stThreat = moveGenerator.isSquareAttacked(board, st, oppositeColor);
        int trThreat = moveGenerator.isSquareAttacked(board, tr, oppositeColor);

        return (historyScore[color][st][tr][stThreat][trThreat]) +
               maxHistoryScore; // to prevent negative values
    }

    inline int getScore(int color, Board& board, Move move, int stThreat) {

        int oppositeColor = (color == WHITE) ? BLACK : WHITE;

        int st = move.getStartSquare();
        int tr = move.getTargetSquare();

        int trThreat = moveGenerator.isSquareAttacked(board, tr, oppositeColor);

        return (historyScore[color][st][tr][stThreat][trThreat]) +
               maxHistoryScore; // to prevent negative values
    }
};

struct CorrHistoryHelper {
	const int sznd = (1 << 14) - 1;
	int corrHistTablePawn[2][1 << 14];
	int corrHistTableMinor[2][1 << 14];

	int corrHistTableWhite[2][1 << 14];
	int corrHistTableBlack[2][1 << 14];

	const int maxCorrHistValue = 300;

    void clear() {
        for (int i = 0; i < 2; i++)
            for (int j = 0; j <= sznd; j++){
            	corrHistTablePawn[i][j] = 0;
            	corrHistTableMinor[i][j] = 0;
            	corrHistTableWhite[i][j] = 0;
            	corrHistTableBlack[i][j] = 0;
            }
    }

	inline void update(int color, Board &board, int score) {
		score = clamp(score, -maxCorrHistValue, maxCorrHistValue);
		int index;
		
		index = board.zobristKeyPawn & sznd;
		corrHistTablePawn[color][index] +=
			score - corrHistTablePawn[color][index] * abs(score) / maxCorrHistValue;
		
		index = board.zobristKeyMinor & sznd;
		corrHistTableMinor[color][index] +=
			score - corrHistTableMinor[color][index] * abs(score) / maxCorrHistValue;
		
		index = board.zobristKeyWhite & sznd;
		corrHistTableWhite[color][index] +=
			score - corrHistTableWhite[color][index] * abs(score) / maxCorrHistValue;
		
		index = board.zobristKeyBlack & sznd;
		corrHistTableBlack[color][index] +=
			score - corrHistTableBlack[color][index] * abs(score) / maxCorrHistValue;
	}

    inline int getScore(int color, Board &board) {
    	int index, corrScore = 0;

		index = board.zobristKeyPawn & sznd;
		corrScore += (50 * corrHistTablePawn[color][index]) / 300;

		index = board.zobristKeyMinor & sznd;
		corrScore += (50 * corrHistTableMinor[color][index]) / 300;

		index = board.zobristKeyWhite & sznd;
		corrScore += (50 * corrHistTableWhite[color][index]) / 300;

		index = board.zobristKeyBlack & sznd;
		corrScore += (50 * corrHistTableBlack[color][index]) / 300;

		return corrScore;
    }
};