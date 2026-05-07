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

struct HistoryHelper {
    int16_t historyScore[2][64][64][2][2];
    int16_t pieceSquareHistory[2][8][64][2][2];
    int16_t counterHistory[2][8][64][8][64];
    int16_t contPly2History[2][8][64][8][64];
    int16_t captHistoryScore[2][8][64][8];
    int16_t maxHistoryScore = 511;

    Bitboard whiteAttacks, blackAttacks;

    void clear() {
    	memset(historyScore, 0, sizeof(historyScore));
    }

    inline void update(Board &board, int color, Move move, int score) {
        if (score < -maxHistoryScore)
            score = -maxHistoryScore;
        if (score > maxHistoryScore)
            score = maxHistoryScore; // clamp

	    int tr = move.getTargetSquare();

        if (board.occupancy(tr) == EMPTY) {
	        int st = move.getStartSquare();

	        int stTh, trTh;
	        if (color == WHITE) {
	        	stTh = (blackAttacks & (1ull << st)) > 0;
	        	trTh = (blackAttacks & (1ull << tr)) > 0;
	        } else {
	        	stTh = (whiteAttacks & (1ull << st)) > 0;
	        	trTh = (whiteAttacks & (1ull << tr)) > 0;
	        }

	        historyScore[color][st][tr][stTh][trTh] +=
	            score - historyScore[color][st][tr][stTh][trTh] * abs(score) / maxHistoryScore;

	        pieceSquareHistory[color][board.occupancyPiece(st)][tr][stTh][trTh] +=
	            score - pieceSquareHistory[color][board.occupancyPiece(st)][tr][stTh][trTh] * abs(score) / maxHistoryScore;

	        counterHistory[color][board.ply1Ps][board.ply1Sq][board.occupancyPiece(st)][tr] +=
	            score - counterHistory[color][board.ply1Ps][board.ply1Sq][board.occupancyPiece(st)][tr] * abs(score) / maxHistoryScore;

	        contPly2History[color][board.ply2Ps][board.ply2Sq][board.occupancyPiece(st)][tr] +=
	            score - contPly2History[color][board.ply2Ps][board.ply2Sq][board.occupancyPiece(st)][tr] * abs(score) / maxHistoryScore;

	        contPly2History[color][board.ply4Ps][board.ply4Sq][board.occupancyPiece(st)][tr] +=
	            score - contPly2History[color][board.ply4Ps][board.ply4Sq][board.occupancyPiece(st)][tr] * abs(score) / maxHistoryScore;

	    } else {
	    	int movedPiece = board.occupancyPiece(move.getStartSquare());
	    	int capturedPiece = board.occupancyPiece(tr);

	        captHistoryScore[color][movedPiece][tr][capturedPiece] +=
	            score - captHistoryScore[color][movedPiece][tr][capturedPiece] * abs(score) / maxHistoryScore;
	    }
    }

    inline int getScore(Board &board, int color, Move move) {

        int tr = move.getTargetSquare();

        if (board.occupancy(tr) == EMPTY) {
        	int st = move.getStartSquare();

	        int stTh, trTh;
	        if (color == WHITE) {
	        	stTh = (blackAttacks & (1ull << st)) > 0;
	        	trTh = (blackAttacks & (1ull << tr)) > 0;
	        } else {
	        	stTh = (whiteAttacks & (1ull << st)) > 0;
	        	trTh = (whiteAttacks & (1ull << tr)) > 0;
	        }

	        int history = 0;
	        
	        history += (historyScore[color][st][tr][stTh][trTh]) * historyScoreFromTo;
	        history += (pieceSquareHistory[color][board.occupancyPiece(st)][tr][stTh][trTh]) * historyScorePieceTo;

	        history += (counterHistory[color][board.ply1Ps][board.ply1Sq][board.occupancyPiece(st)][tr]) * historyScorePly1;

	        history += (contPly2History[color][board.ply2Ps][board.ply2Sq][board.occupancyPiece(st)][tr]) * historyScorePly2;

	        history += (contPly2History[color][board.ply4Ps][board.ply4Sq][board.occupancyPiece(st)][tr]) * historyScorePly4;

	        history /= 2048;
	        return history + maxHistoryScore; // to prevent negative values

	    } else {
	    	int movedPiece = board.occupancyPiece(move.getStartSquare());
	    	int capturedPiece = board.occupancyPiece(tr);

	        return (captHistoryScore[color][movedPiece][tr][capturedPiece]) +
	               maxHistoryScore;
	    }
    }
};

struct CorrHistoryHelper {
	const static int size = 16;

	const int sznd = (1 << size) - 1;
	int16_t corrHistTablePawn[2][1 << size];
	int16_t corrHistTableMinor[2][1 << size];

	int16_t corrHistTableWhite[2][1 << size];
	int16_t corrHistTableBlack[2][1 << size];

	int16_t corrHistLastmove[2][8][64];
	int16_t corrHist2ply[2][8][64][8][64];

	const int maxCorrHistValue = 300;

    void clear() {
        for (int i = 0; i < 2; i++)
            for (int j = 0; j <= sznd; j++){
            	corrHistTablePawn[i][j] = 0;
            	corrHistTableMinor[i][j] = 0;
            	corrHistTableWhite[i][j] = 0;
            	corrHistTableBlack[i][j] = 0;
            }
        memset(corrHistLastmove, 0, sizeof(corrHistLastmove));
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
		
		corrHistLastmove[color][board.ply1Ps][board.ply1Sq] +=
			score - corrHistLastmove[color][board.ply1Ps][board.ply1Sq] * abs(score) / maxCorrHistValue;
		
		corrHist2ply[color][board.ply2Ps][board.ply2Sq][board.ply1Ps][board.ply1Sq] +=
			score - corrHist2ply[color][board.ply2Ps][board.ply2Sq][board.ply1Ps][board.ply1Sq] * abs(score) / maxCorrHistValue;
	}

    inline int getScore(int color, Board &board) {
    	int index, corrScore = 0;

		index = board.zobristKeyPawn & sznd;
		corrScore += (corrhistPawn * corrHistTablePawn[color][index]);

		index = board.zobristKeyMinor & sznd;
		corrScore += (corrhistMinor * corrHistTableMinor[color][index]);

		index = board.zobristKeyWhite & sznd;
		corrScore += (corrhistColor * corrHistTableWhite[color][index]);

		index = board.zobristKeyBlack & sznd;
		corrScore += (corrhistColor * corrHistTableBlack[color][index]);

		corrScore += (corrhistFromTo * corrHistLastmove[color][board.ply1Ps][board.ply1Sq]);

		corrScore += (corrhistPly1 * corrHist2ply[color][board.ply2Ps][board.ply2Sq][board.ply1Ps][board.ply1Sq]);

		corrScore /= 1024;

		return corrScore;
    }
};

CorrHistoryHelper corrhistHelper;