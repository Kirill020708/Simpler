// evaluation of the current position on board

#pragma once

#ifndef BOARD
#define BOARD

#include "board.h"

#endif /* BOARD */

#ifndef BOARDHELP
#define BOARDHELP

#include "boardHelper.h"

#endif /* BOARDHELP */

#ifndef PIECESQTABLE
#define PIECESQTABLE

#include "pieceSquareTable.h"

#endif /* PIECESQTABLE */

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

#ifndef TRANSPOSTABLE
#define TRANSPOSTABLE

#include "transpositionTable.h"

#endif /* TRANSPOSTABLE */

#ifndef NNUE
#define NNUE

#include "nnue.h"

#endif /* NNUE */


#ifndef HISTORY
#define HISTORY

#include "historyHelper.h"

#endif /* HISTORY */


struct Evaluator {

    bool showInfo = false;
    bool uciOutput = false;

    float mobilityScoreMg[8] = {0, 0, 1, 2, 1, 0, -1, 0};
    float mobilityScoreEg[8] = {0, 0, 2, 3, 2, 1, 1, 0}; // v2

    float kingAttackersWeightMg[8] = {0, 8, 20, 20, 20, 20, 20, 0};
    float kingAttackersWeightEg[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    float doubledPawnPenaltyMg = 12, doubledPawnPenaltyEg = 7;

    float isolatedPawnPenaltyMg[8] = {10, 15, 20, 25, 25, 20, 15, 10};
    float isolatedPawnPenaltyEg[8] = {0, 5, 10, 10, 10, 10, 5, 0};

    float passedPawnScoreMg[8] = {0, 3.5, 7, 17.5, 31.5, 56, 98, 0};
    float passedPawnScoreEg[8] = {0, 6.5, 13, 32.5, 58.5, 104, 182, 0};

    float pawnIslandPenaltyMg = 15, pawnIslandPenaltyEg = 15;

    float pawnDistancePenalty[8] = {0, 0, 0, 5, 10, 15, 20, 25};

    float tempoScore = 20;

    float bishopPairMg = 30, bishopPairEg = 60;

    float uncatchablePassedPawnScore[5] = {0, 100, 200, 300};

    float rookOnOpenFileScore = 15;

    float knightOutpostScore = 20, bishopOutpostScore = 14;

    void initFromVector(vector<float> weights) {
        int iter = 0;
        for (int i = 0; i < 8; i++)
            mobilityScoreMg[i] = weights[iter++];
        for (int i = 0; i < 8; i++)
            mobilityScoreEg[i] = weights[iter++];
        for (int i = 0; i < 8; i++)
            kingAttackersWeightMg[i] = weights[iter++];
        for (int i = 0; i < 8; i++)
            kingAttackersWeightEg[i] = weights[iter++];

        // doubledPawnPenaltyMg=weights[iter++];
        // doubledPawnPenaltyEg=weights[iter++];

        for (int i = 0; i < 8; i++)
            isolatedPawnPenaltyMg[i] = weights[iter++];
        for (int i = 0; i < 8; i++)
            isolatedPawnPenaltyEg[i] = weights[iter++];
        for (int i = 0; i < 8; i++)
            passedPawnScoreMg[i] = weights[iter++];
        for (int i = 0; i < 8; i++)
            passedPawnScoreEg[i] = weights[iter++];

        pawnIslandPenaltyMg = weights[iter++];
        pawnIslandPenaltyEg = weights[iter++];

        for (int i = 0; i < 8; i++)
            pawnDistancePenalty[i] = weights[iter++];

        tempoScore = weights[iter++];

        bishopPairMg = weights[iter++];
        bishopPairEg = weights[iter++];
    }

    vector<float> writeToVector() {
        vector<float> weights;

        for (int i = 0; i < 8; i++)
            weights.push_back(mobilityScoreMg[i]);
        for (int i = 0; i < 8; i++)
            weights.push_back(mobilityScoreEg[i]);
        for (int i = 0; i < 8; i++)
            weights.push_back(kingAttackersWeightMg[i]);
        for (int i = 0; i < 8; i++)
            weights.push_back(kingAttackersWeightEg[i]);

        // weights.push_back(doubledPawnPenaltyMg);
        // weights.push_back(doubledPawnPenaltyEg);

        for (int i = 0; i < 8; i++)
            weights.push_back(isolatedPawnPenaltyMg[i]);
        for (int i = 0; i < 8; i++)
            weights.push_back(isolatedPawnPenaltyEg[i]);
        for (int i = 0; i < 8; i++)
            weights.push_back(passedPawnScoreMg[i]);
        for (int i = 0; i < 8; i++)
            weights.push_back(passedPawnScoreEg[i]);

        weights.push_back(pawnIslandPenaltyMg);
        weights.push_back(pawnIslandPenaltyEg);

        for (int i = 0; i < 8; i++)
            weights.push_back(pawnDistancePenalty[i]);

        weights.push_back(tempoScore);

        weights.push_back(bishopPairMg);
        weights.push_back(bishopPairEg);

        return weights;
    }

    void writeToFile(string path) {
        ofstream out(path);
        out << "mobility mg: ";
        for (int i = 0; i < 8; i++)
            out << mobilityScoreMg[i] << ' ';
        out << endl;

        out << "mobility eg: ";
        for (int i = 0; i < 8; i++)
            out << mobilityScoreEg[i] << ' ';
        out << endl;

        out << "king attack mg: ";
        for (int i = 0; i < 8; i++)
            out << kingAttackersWeightMg[i] << ' ';
        out << endl;

        out << "king attack eg: ";
        for (int i = 0; i < 8; i++)
            out << kingAttackersWeightEg[i] << ' ';
        out << endl;

        out << "doubled mg: " << doubledPawnPenaltyMg << endl;
        out << "doubled eg: " << doubledPawnPenaltyEg << endl;

        out << "isolated mg: ";
        for (int i = 0; i < 8; i++)
            out << isolatedPawnPenaltyMg[i] << ' ';
        out << endl;

        out << "isolated eg: ";
        for (int i = 0; i < 8; i++)
            out << isolatedPawnPenaltyEg[i] << ' ';
        out << endl;

        out << "passed mg: ";
        for (int i = 0; i < 8; i++)
            out << passedPawnScoreMg[i] << ' ';
        out << endl;

        out << "passed eg: ";
        for (int i = 0; i < 8; i++)
            out << passedPawnScoreEg[i] << ' ';
        out << endl;

        out << "islands mg: " << pawnIslandPenaltyMg << endl;
        out << "islands eg: " << pawnIslandPenaltyEg << endl;

        out << "shield dist: ";
        for (int i = 0; i < 8; i++)
            out << pawnDistancePenalty[i] << ' ';
        out << endl;

        out << "tempo: " << tempoScore << endl;

        out << "bishop pair mg: " << bishopPairMg << endl;
        out << "bishop pair eg: " << bishopPairEg << endl;
    }

    void init(vector<string> strs) {
        for (auto inp : strs) {
            string type;
            vector<string> strEvals;
            for (int i = 0; i < inp.length(); i++)
                if (inp[i] == ':') {
                    type = inp.substr(0, i);
                    strEvals = splitStr(inp.substr(i + 1, inp.length() - i - 1), " ");
                    break;
                }
            vector<float> evals;
            for (auto ev : strEvals)
                if (ev != "")
                    evals.push_back(stof(ev));

            if (type == "mobility mg")
                for (int i = 0; i < 8; i++)
                    mobilityScoreMg[i] = evals[i];

            if (type == "mobility eg")
                for (int i = 0; i < 8; i++)
                    mobilityScoreEg[i] = evals[i];

            if (type == "king attack mg")
                for (int i = 0; i < 8; i++)
                    kingAttackersWeightMg[i] = evals[i];

            if (type == "king attack eg")
                for (int i = 0; i < 8; i++)
                    kingAttackersWeightEg[i] = evals[i];

            if (type == "doubled mg")
                doubledPawnPenaltyMg = evals[0];

            if (type == "doubled eg")
                doubledPawnPenaltyEg = evals[0];

            if (type == "isolated mg")
                for (int i = 0; i < 8; i++)
                    isolatedPawnPenaltyMg[i] = evals[i];

            if (type == "isolated eg")
                for (int i = 0; i < 8; i++)
                    isolatedPawnPenaltyEg[i] = evals[i];

            if (type == "passed mg")
                for (int i = 0; i < 8; i++)
                    passedPawnScoreMg[i] = evals[i];

            if (type == "passed eg")
                for (int i = 0; i < 8; i++)
                    passedPawnScoreEg[i] = evals[i];

            if (type == "islands mg")
                pawnIslandPenaltyMg = evals[0];

            if (type == "islands eg")
                pawnIslandPenaltyEg = evals[0];

            if (type == "shield dist")
                for (int i = 0; i < 8; i++)
                    pawnDistancePenalty[i] = evals[i];

            if (type == "tempo")
                tempoScore = evals[0];

            if (type == "bishop pair mg")
                bishopPairMg = evals[0];
            if (type == "bishop pair eg")
                bishopPairEg = evals[0];

            // if(type=="open king main file")
            // 	openKingMainFilePenalty=evals[0];
            // if(type=="open king near file")
            // 	openKingNearFilePenalty=evals[0];
        }
    }

    void initFromFile(string path) {
        ifstream in(path);
        string inp;
        vector<string> strs;
        while (getline(in, inp))
            strs.push_back(inp);
        init(strs);
    }

    inline float evaluateKingShield(Board &board) {
        float mainColumnEvaluationW = 0, nearColumnEvaluationW = 0;
        float mainColumnEvaluationB = 0, nearColumnEvaluationB = 0;

        float endgameWeight = board.endgameWeight();

        int whiteKingPos = (board.kings & board.whitePieces).getFirstBitNumber();
        int col = boardHelper.getColumnNumber(whiteKingPos);
        Bitboard mainColumn = boardHelper.columnUp[whiteKingPos];

        int dist = boardHelper.distanceColumn(mainColumn & board.whitePieces & board.pawns, WHITE);
        if (dist <= 0) {
            cout << dist << '\n';
            exit(0);
        }
        assert(dist > 0);
        mainColumnEvaluationW -= pawnDistancePenalty[dist];

        if (col > 0) {
            dist = boardHelper.distanceColumn(boardHelper.columnUp[whiteKingPos - 1] & board.whitePieces & board.pawns,
                                              WHITE);
            if (dist <= 0) {
                cout << dist << '\n';
                exit(0);
            }
            assert(dist > 0);
            nearColumnEvaluationW -= pawnDistancePenalty[dist];
        }
        if (col < 7) {
            dist = boardHelper.distanceColumn(boardHelper.columnUp[whiteKingPos + 1] & board.whitePieces & board.pawns,
                                              WHITE);
            if (dist <= 0) {
                cout << dist << '\n';
                exit(0);
            }
            assert(dist > 0);
            nearColumnEvaluationW -= pawnDistancePenalty[dist];
        }
        if (whiteKingPos == 58 || whiteKingPos == 50) { // c1, c2
            dist = boardHelper.distanceColumn(boardHelper.getColumn(col - 2) & board.whitePieces & board.pawns, WHITE);
            if (dist <= 0) {
                cout << dist << '\n';
                exit(0);
            }
            assert(dist > 0);
            nearColumnEvaluationW -= pawnDistancePenalty[dist];
        }

        // if(col==3||col==4){ // d or e column
        // 	// mainColumnEvaluationW*=0.4;
        // 	// nearColumnEvaluationW*=0.4;
        // }

        int blackKingPos = (board.kings & board.blackPieces).getFirstBitNumber();
        col = boardHelper.getColumnNumber(blackKingPos);
        mainColumn = boardHelper.getColumn(col);

        dist = boardHelper.distanceColumn(mainColumn & board.blackPieces & board.pawns, BLACK);
        if (dist <= 0) {
            cout << dist << '\n';
            exit(0);
        }
        mainColumnEvaluationB += pawnDistancePenalty[dist];
        assert(dist > 0);

        if (col > 0) {
            dist = boardHelper.distanceColumn(boardHelper.getColumn(col - 1) & board.blackPieces & board.pawns, BLACK);
            if (dist <= 0) {
                cout << dist << '\n';
                exit(0);
            }
            assert(dist > 0);
            nearColumnEvaluationB += pawnDistancePenalty[dist];
        }
        if (col < 7) {
            dist = boardHelper.distanceColumn(boardHelper.getColumn(col + 1) & board.blackPieces & board.pawns, BLACK);
            if (dist <= 0) {
                cout << dist << '\n';
                exit(0);
            }
            assert(dist > 0);
            nearColumnEvaluationB += pawnDistancePenalty[dist];
        }
        if (blackKingPos == 2 || blackKingPos == 10) { // c8, c7
            dist = boardHelper.distanceColumn(boardHelper.getColumn(col - 2) & board.blackPieces & board.pawns, BLACK);
            if (dist <= 0) {
                cout << dist << '\n';
                exit(0);
            }
            assert(dist > 0);
            nearColumnEvaluationB += pawnDistancePenalty[dist];
        }

        // if(col==3||col==4){ // d or e column
        // 	// mainColumnEvaluationB*=0.4;
        // 	// nearColumnEvaluationB*=0.4;
        // }

        float evaluation = mainColumnEvaluationW + mainColumnEvaluationB;

        evaluation += (nearColumnEvaluationW + nearColumnEvaluationB) * 0.6;

        evaluation *= (1 - endgameWeight);

        return evaluation;
    }

    float evaluateBishopPair(Board &board) {
        float endgameWeight = board.endgameWeight();
        float evaluation = 0;
        if ((board.whitePieces & board.bishops).popcnt() == 2)
            evaluation += (bishopPairMg * (1 - endgameWeight) + bishopPairEg * endgameWeight);
        if ((board.blackPieces & board.bishops).popcnt() == 2)
            evaluation -= (bishopPairMg * (1 - endgameWeight) + bishopPairEg * endgameWeight);
        return evaluation;
    }

    inline bool insufficientMaterialDraw(Board &board) {
        if ((board.pawns | board.queens | board.rooks) != 0)
            return false;
        if (((board.knights | board.bishops) & board.whitePieces).popcnt() >= 2)
            return false;
        if (((board.knights | board.bishops) & board.blackPieces).popcnt() >= 2)
            return false;
        return true;
    }

    inline float evaluatePositionDeterministic(Board &board) { // board evaluation with white's perspective
        float endgameWeight = board.endgameWeight();

        float evaluation = 0;

        float PSTevaluation = 0;

        float mobilityEvaluation = 0;
        float kingAttackersEvaluation = 0;

        float rookOnOpenFileEvaluation = 0;

        float outpostEvaluation = 0;

        int opponentKing = (board.kings & board.blackPieces).getFirstBitNumber();
        Bitboard kingArea = (boardHelper.kingMoves[opponentKing] | (1ull << opponentKing));
        Bitboard pieces = board.whitePieces & (~board.pawns);

        Bitboard friendPawns = board.pawns & board.whitePieces;
        Bitboard opponentPawns = board.pawns & board.blackPieces;

        while (pieces > 0) {
            int square = pieces.getFirstBitNumberAndExclude();
            int pieceType = board.occupancyPiece(square);

            PSTevaluation += pieceSquareTable.getPieceEval(pieceType, square, WHITE, endgameWeight);
            // cout<<square<<' '<<pieceSquareTable.getPieceEval(pieceType,square,WHITE,endgameWeight)<<'\n';

            Bitboard moves = moveGenerator.moves(board, square);
            int mobility = moves.popcnt();
            mobilityEvaluation += mobility * (mobilityScoreMg[pieceType] * (1 - endgameWeight) +
                                              mobilityScoreEg[pieceType] * endgameWeight);

            int numberOfAttacks = (moves & kingArea).popcnt();
            kingAttackersEvaluation += numberOfAttacks * (kingAttackersWeightMg[pieceType] * (1 - endgameWeight) +
                                                          kingAttackersWeightEg[pieceType] * endgameWeight);

            if (pieceType == ROOK) {
                Bitboard column = boardHelper.columns[square];
                if ((column & friendPawns) == 0)
                    rookOnOpenFileEvaluation += rookOnOpenFileScore;
            }

            if (pieceType == KNIGHT) {
                if (boardHelper.getRowNumber(square) <= 3 &&
                    (opponentPawns & boardHelper.possibleOutpostDefendersWhite[square]) == 0) {
                    if (friendPawns & boardHelper.pawnCaptures[BLACK][square])
                        outpostEvaluation += knightOutpostScore;
                    else
                        outpostEvaluation += knightOutpostScore / 2;
                }
            }

            // if(pieceType==BISHOP){
            // 	if(boardHelper.getRowNumber(square)<=3 &&
            // (opponentPawns&boardHelper.possibleOutpostDefendersWhite[square])==0){
            // 		if(friendPawns&boardHelper.pawnCaptures[BLACK][square])
            // 			outpostEvaluation+=bishopOutpostScore;
            // 		else
            // 			outpostEvaluation+=bishopOutpostScore/2;
            // 	}
            // }
        }

        opponentKing = (board.kings & board.whitePieces).getFirstBitNumber();
        kingArea = (boardHelper.kingMoves[opponentKing] | (1ull << opponentKing));
        pieces = board.blackPieces & (~board.pawns);

        friendPawns = board.pawns & board.blackPieces;
        opponentPawns = board.pawns & board.whitePieces;

        while (pieces > 0) {
            int square = pieces.getFirstBitNumberAndExclude();
            int pieceType = board.occupancyPiece(square);

            PSTevaluation += pieceSquareTable.getPieceEval(pieceType, square, BLACK, endgameWeight);
            // cout<<square<<' '<<pieceSquareTable.getPieceEval(pieceType,square,BLACK,endgameWeight)<<'\n';

            Bitboard moves = moveGenerator.moves(board, square);
            int mobility = moves.popcnt();
            mobilityEvaluation -= mobility * (mobilityScoreMg[pieceType] * (1 - endgameWeight) +
                                              mobilityScoreEg[pieceType] * endgameWeight);

            int numberOfAttacks = (moves & kingArea).popcnt();
            kingAttackersEvaluation -= numberOfAttacks * (kingAttackersWeightMg[pieceType] * (1 - endgameWeight) +
                                                          kingAttackersWeightEg[pieceType] * endgameWeight);

            if (pieceType == ROOK) {
                Bitboard column = boardHelper.columns[square];
                if ((column & friendPawns) == 0)
                    rookOnOpenFileEvaluation -= rookOnOpenFileScore;
            }

            if (pieceType == KNIGHT) {
                if (boardHelper.getRowNumber(square) >= 4 &&
                    (opponentPawns & boardHelper.possibleOutpostDefendersBlack[square]) == 0) {
                    if (friendPawns & boardHelper.pawnCaptures[WHITE][square])
                        outpostEvaluation -= knightOutpostScore;
                    else
                        outpostEvaluation -= knightOutpostScore / 2;
                }
            }

            // if(pieceType==BISHOP){
            // 	if(boardHelper.getRowNumber(square)>=4 &&
            // (opponentPawns&boardHelper.possibleOutpostDefendersBlack[square])==0){
            // 		if(friendPawns&boardHelper.pawnCaptures[WHITE][square])
            // 			outpostEvaluation-=bishopOutpostScore;
            // 		else
            // 			outpostEvaluation-=bishopOutpostScore/2;
            // 	}
            // }
        }

        rookOnOpenFileEvaluation *= (1 - endgameWeight);

        outpostEvaluation *= (1 - endgameWeight);

        float isolatedPawnEvaluation = 0;
        float passedPawnsEvaluation = 0;
        int whiteUncatchablePawn = 10, whitePosPas = 10;

        Bitboard pawns = board.pawns & board.whitePieces;
        friendPawns = board.pawns & board.whitePieces;
        opponentPawns = board.pawns & board.blackPieces;

        int friendKing = (board.kings & board.whitePieces).getFirstBitNumber();
        opponentKing = (board.kings & board.blackPieces).getFirstBitNumber();
        kingArea = (boardHelper.kingMoves[opponentKing] | (1ull << opponentKing));

        while (pawns) {
            int square = pawns.getFirstBitNumberAndExclude();

            PSTevaluation += pieceSquareTable.getPieceEval(PAWN, square, WHITE, endgameWeight);
            // cout<<square<<' '<<pieceSquareTable.getPieceEval(PAWN,square,WHITE,endgameWeight)<<'\n';

            int col = boardHelper.getColumnNumber(square), row = boardHelper.getRowNumber(square);
            int dst = 7 - boardHelper.getRowNumber(square), dstToPass = 7 - dst;
            if (dstToPass == 6) // 2nd rank
                dstToPass = 5;
            int cnt = (boardHelper.neighborColumns[square] & friendPawns).popcnt();

            if (cnt == 0)
                isolatedPawnEvaluation -=
                    (isolatedPawnPenaltyMg[col] * (1 - endgameWeight) + isolatedPawnPenaltyEg[col] * endgameWeight);

            Bitboard column = boardHelper.columns[square], nbColumns = boardHelper.neighborColumns[square];

            if ((boardHelper.possiblePawnDefendersWhite[square] & opponentPawns) == 0) {
                passedPawnsEvaluation +=
                    (passedPawnScoreMg[dst] * (1 - endgameWeight) + passedPawnScoreEg[dst] * endgameWeight);

                int nmbOfMovesToCatch =
                    max(boardHelper.getRowNumber(opponentKing), abs(col - boardHelper.getColumnNumber(opponentKing)));
                if (boardHelper.columnUp[square] & (1ull << friendKing))
                    dstToPass++;
                if (board.boardColor == BLACK)
                    nmbOfMovesToCatch--;
                if (nmbOfMovesToCatch > dstToPass)
                    whiteUncatchablePawn = min(whiteUncatchablePawn, dstToPass);
            }

            if ((boardHelper.columnUp[square] & opponentPawns) == 0) {
                int nmbOfMovesToCatch =
                    max(boardHelper.getRowNumber(opponentKing), abs(col - boardHelper.getColumnNumber(opponentKing)));
                if (boardHelper.columnUp[square] & (1ull << friendKing))
                    dstToPass++;
                if (board.boardColor == BLACK)
                    nmbOfMovesToCatch--;
                if (nmbOfMovesToCatch > dstToPass)
                    whitePosPas = min(whitePosPas, dstToPass);
            }

            Bitboard moves = moveGenerator.moves(board, square);
            int numberOfAttacks = (moves & kingArea).popcnt();
            // kingAttackersEvaluation+=numberOfAttacks*(kingAttackersWeightMg[PAWN]*(1-endgameWeight)+kingAttackersWeightEg[PAWN]*endgameWeight);
        }

        int blackUncatchablePawn = 10, blackPosPas = 10;

        pawns = board.pawns & board.blackPieces;
        friendPawns = board.pawns & board.blackPieces;
        opponentPawns = board.pawns & board.whitePieces;

        friendKing = (board.kings & board.blackPieces).getFirstBitNumber();
        opponentKing = (board.kings & board.whitePieces).getFirstBitNumber();
        kingArea = (boardHelper.kingMoves[opponentKing] | (1ull << opponentKing));

        while (pawns) {
            int square = pawns.getFirstBitNumberAndExclude();

            PSTevaluation += pieceSquareTable.getPieceEval(PAWN, square, BLACK, endgameWeight);
            // cout<<square<<' '<<pieceSquareTable.getPieceEval(PAWN,square,BLACK,endgameWeight)<<'\n';

            int col = boardHelper.getColumnNumber(square), row = boardHelper.getRowNumber(square);
            int dst = boardHelper.getRowNumber(square), dstToPass = 7 - dst;
            if (dstToPass == 6) // 2nd rank
                dstToPass = 5;
            int cnt = (boardHelper.neighborColumns[square] & friendPawns).popcnt();

            if (cnt == 0)
                isolatedPawnEvaluation +=
                    (isolatedPawnPenaltyMg[col] * (1 - endgameWeight) + isolatedPawnPenaltyEg[col] * endgameWeight);

            Bitboard column = boardHelper.columns[square], nbColumns = boardHelper.neighborColumns[square];

            if ((boardHelper.possiblePawnDefendersBlack[square] & opponentPawns) == 0) {
                // cout<<square<<'\n';
                passedPawnsEvaluation -=
                    (passedPawnScoreMg[dst] * (1 - endgameWeight) + passedPawnScoreEg[dst] * endgameWeight);

                int nmbOfMovesToCatch = max(7 - boardHelper.getRowNumber(opponentKing),
                                            abs(col - boardHelper.getColumnNumber(opponentKing)));
                if (boardHelper.columnUp[square] & (1ull << friendKing))
                    dstToPass--;
                if (board.boardColor == WHITE)
                    nmbOfMovesToCatch--;
                // cout<<dstToPass<<' '<<boardHelper.getRowNumber(opponentKing)<<'\n';
                if (nmbOfMovesToCatch > dstToPass)
                    blackUncatchablePawn = min(blackUncatchablePawn, dstToPass);
            }

            if ((boardHelper.columnDown[square] & opponentPawns) == 0) {
                int nmbOfMovesToCatch = max(7 - boardHelper.getRowNumber(opponentKing),
                                            abs(col - boardHelper.getColumnNumber(opponentKing)));
                if (boardHelper.columnUp[square] & (1ull << friendKing))
                    dstToPass--;
                if (board.boardColor == WHITE)
                    nmbOfMovesToCatch--;
                if (nmbOfMovesToCatch > dstToPass)
                    blackPosPas = min(blackPosPas, dstToPass);
            }

            Bitboard moves = moveGenerator.moves(board, square);
            int numberOfAttacks = (moves & kingArea).popcnt();
            // kingAttackersEvaluation-=numberOfAttacks*(kingAttackersWeightMg[PAWN]*(1-endgameWeight)+kingAttackersWeightEg[PAWN]*endgameWeight);
        }

        if (board.boardColor == BLACK) {
            if (blackUncatchablePawn < 10)
                blackUncatchablePawn--;
            if (blackPosPas < 10)
                blackPosPas--;
        }

        bool onlyPawnsEndgame = false;
        if ((board.whitePieces | board.blackPieces) == (board.pawns | board.kings))
            onlyPawnsEndgame = true;

        if (0) {
            // cout<<whiteUncatchablePawn<<' '<<whitePosPas<<' '<<blackUncatchablePawn<<' '<<blackPosPas<<'\n';
            // if(whiteUncatchablePawn<blackPosPas)
            // 	passedPawnsEvaluation+=uncatchablePassedPawnScore[min(blackPosPas-whiteUncatchablePawn,3)];

            // if(blackUncatchablePawn<whitePosPas)
            // 	passedPawnsEvaluation-=uncatchablePassedPawnScore[min(whitePosPas-blackUncatchablePawn,3)];

            int diff = abs(whiteUncatchablePawn - blackUncatchablePawn);
            if (diff > 3)
                diff = 3;
            if (whiteUncatchablePawn < blackUncatchablePawn)
                passedPawnsEvaluation += uncatchablePassedPawnScore[diff];
            else
                passedPawnsEvaluation -= uncatchablePassedPawnScore[diff];
        }

        // passedPawnsEvaluation*=(0.7*(1-endgameWeight)+1.3*endgameWeight);

        int numberOfDoubled = 0;

        float pawnIslandsEvaluation = 0;
        float pawnIslandPenalty = (pawnIslandPenaltyMg * (1 - endgameWeight) + pawnIslandPenaltyEg * endgameWeight);
        int prevW = 0, prevB = 0;
        for (int col = 0; col < 8; col++) {
            Bitboard column = boardHelper.getColumn(col);
            int colW = (column & board.pawns & board.whitePieces) > 0;
            int colB = (column & board.pawns & board.blackPieces) > 0;
            if (!colW && prevW)
                pawnIslandsEvaluation -= pawnIslandPenalty;
            prevW = colW;

            if (!colB && prevB)
                pawnIslandsEvaluation += pawnIslandPenalty;
            prevB = colB;

            colW = (column & board.pawns & board.whitePieces).popcnt();
            colB = (column & board.pawns & board.blackPieces).popcnt();
            if (colW > 1)
                numberOfDoubled += colW - 1;
            if (colB > 1)
                numberOfDoubled -= (colB - 1);
        }

        float doubledPawnEvaluation =
            -(numberOfDoubled * (doubledPawnPenaltyMg * (1 - endgameWeight) + doubledPawnPenaltyEg * endgameWeight));

        if (prevW)
            pawnIslandsEvaluation -= pawnIslandPenalty;

        if (prevB)
            pawnIslandsEvaluation += pawnIslandPenalty;

        float tempoEval = 0;
        if (board.boardColor == WHITE)
            tempoEval += tempoScore;
        else
            tempoEval -= tempoScore; // tempo

        // float kingShieldEvaluation=evaluateKingShield(board);

        evaluation += PSTevaluation;
        evaluation += mobilityEvaluation;
        evaluation += kingAttackersEvaluation;
        evaluation += isolatedPawnEvaluation;
        // evaluation+=doubledPawnEvaluation;
        evaluation += passedPawnsEvaluation;
        evaluation += pawnIslandsEvaluation;
        evaluation += tempoEval;
        // evaluation+=kingShieldEvaluation;
        evaluation += evaluateBishopPair(board);
        evaluation += rookOnOpenFileEvaluation;
        evaluation += outpostEvaluation;

        if (showInfo == true) {
            cout << "Material and piece-square tables: " << PSTevaluation << " cp" << endl;
            cout << "Mobility: " << mobilityEvaluation << " cp" << endl;
            cout << "King attackers: " << kingAttackersEvaluation << " cp" << endl;
            // cout<<"King shield: "<<kingShieldEvaluation<<" cp"<<endl;
            cout << "Isolated pawns: " << isolatedPawnEvaluation << " cp" << endl;
            cout << "Doubled pawns: " << doubledPawnEvaluation << " cp" << endl;
            cout << "Passed pawns: " << passedPawnsEvaluation << " cp" << endl;
            cout << "Pawn islands: " << pawnIslandsEvaluation << " cp" << endl;
            cout << "Tempo: " << tempoEval << " cp" << endl;
        }

        // evaluation+=evaluateMobility();

        // evaluation+=evaluatePassedPawns();
        // evaluation+=evaluateIsolatedPawns();

        // evaluation+=evaluateKingAttackers();

        // const int attackSquareScore=1;
        // evaluation+=(moveGenerator.numOfSquaresAttackedByWhite()-moveGenerator.numOfSquaresAttackedByWhite())*attackSquareScore;
        return evaluation;
    }

    int evaluatePosition(Board &board) {
        if (insufficientMaterialDraw(board))
            return DRAW_SCORE;

        int evaluation = int(evaluatePositionDeterministic(board));
        return evaluation;
    }

    int evaluatePosition(Board &board, int color, NNUEevaluator &nnueEvaluator) { // board evaluation with NNUE

        if (insufficientMaterialDraw(board))
            return DRAW_SCORE;

        int evaluation;

        #if defined DO_HCE
            evaluation = evaluatePosition(board) * ((color == WHITE) ? 1 : -1);
        #else
            evaluation = nnueEvaluator.evaluate(color, board.getOutputBucket());
        #endif

        return evaluation;
    }

    int evaluatePosition(Board &board, int color, NNUEevaluator &nnueEvaluator, CorrHistoryHelper &corrhistHelper) { // board evaluation with corrhist
    	return evaluatePosition(board, color, nnueEvaluator) + corrhistHelper.getScore(color, board);
    }

    int evaluatePosition1(Board &board, int color) { // board evaluation with (color)'s perspective
        return evaluatePosition(board) * ((color == WHITE) ? 1 : -1);
    }

    int evaluateStalledPosition(Board &board, int color, int depthFromRoot) {
        if (insufficientMaterialDraw(board))
            return DRAW_SCORE;

        if (moveGenerator.isInCheck(board, color))
            return -(MATE_SCORE - depthFromRoot);
        return DRAW_SCORE;
    }
};

Evaluator evaluator;
