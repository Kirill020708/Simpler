// searches the best move

#pragma once

#ifndef MOVEGENLIST
#define MOVEGENLIST

#include "moveListGeneration.h"

#endif /* MOVEGENLIST */

#ifndef BOARD
#define BOARD

#include "board.h"

#endif /* BOARD */

#ifndef EVALUATOR
#define EVALUATOR

#include "evaluation.h"

#endif /* EVALUATOR */

#ifndef TRANSPOSTABLE
#define TRANSPOSTABLE

#include "transpositionTable.h"

#endif /* TRANSPOSTABLE */

#ifndef HISTORY
#define HISTORY

#include "historyHelper.h"

#endif /* HISTORY */

struct StackState {
    bool excludeTTmove = false;
    Move excludeMove;
    Move bestMove;
    vector<Move> pvLine;
};

enum NodeType {
    PV,
    NonPV
};

struct Worker {
    bool stopSearch;
    bool doneSearch;
    int nodesLim = 1e9;

    int hardTimeBound;

    int boardCurrentAge;

    int rootScore;
    Move bestMove;

    int seldepth;

    ll nodes = 0, singularExtended = 0;

    MoveListGenerator moveListGenerator;
    HistoryHelper historyHelper;

    CorrHistoryHelper corrhistHelper;

    NNUEevaluator nnueEvaluator;

    StackState searchStack[maxDepth];

    OccuredPositionsHelper occuredPositionsHelper;

    std::chrono::steady_clock::time_point searchStartTime;

    int rootNodes[1<<16];

    float texelSearch(Board &board, int color, float alpha, float beta, int ply) {
        float staticEval;
        if (moveListGenerator.isStalled(board, color) || evaluator.insufficientMaterialDraw(board))
            staticEval = evaluator.evaluateStalledPosition(board, color, ply);
        else {
            staticEval = evaluator.evaluatePositionDeterministic(board);
            if (color == BLACK)
                staticEval = -staticEval;
        }

        float bestScore = staticEval;

        alpha = max(alpha, staticEval);
        if (alpha >= beta)
            return bestScore;

        Board boardCopy = board;
        moveListGenerator.generateMoves(board, historyHelper, color, ply, DO_SORT, ONLY_CAPTURES);

        for (int currentMove = 0; currentMove < moveListGenerator.moveListSize[ply]; currentMove++) {
            Move move = moveListGenerator.moveList[ply][currentMove];
            board.makeMove(move);

            float score = -texelSearch(board, (color == WHITE) ? BLACK : WHITE, -beta, -alpha, ply + 1);

            board = boardCopy;
            if (bestScore < score) {
                bestScore = score;
                if (alpha < score)
                    alpha = score;
                if (alpha >= beta)
                    return bestScore;
            }
        }
        return bestScore;
    }

    ull zobristAfterMove(Board &board, Move move) {

    	char color = board.boardColor;

	    bool castlingWhiteQueensideBroke = board.castlingWhiteQueensideBroke;
	    bool castlingWhiteKingsideBroke = board.castlingWhiteKingsideBroke;
	    bool castlingBlackQueensideBroke = board.castlingBlackQueensideBroke;
	    bool castlingBlackKingsideBroke = board.castlingBlackKingsideBroke;

	    char enPassantColumn = board.enPassantColumn;

	    ull zobristKey = board.zobristKey;

        board.calculateZobristAfterMove(move);

        ull newKey = board.getZobristKey();

        board.boardColor = color;

	    board.castlingWhiteQueensideBroke = castlingWhiteQueensideBroke;
	    board.castlingWhiteKingsideBroke = castlingWhiteKingsideBroke;
	    board.castlingBlackQueensideBroke = castlingBlackQueensideBroke;
	    board.castlingBlackKingsideBroke = castlingBlackKingsideBroke;

	    board.enPassantColumn = enPassantColumn;

	    board.zobristKey = zobristKey;

	    return newKey;
    }

    void correctTTscore(TableEntry &ttEntry, int alpha, int beta) {

        if (ttEntry.type == LOWER_BOUND && // when calculated TT node we got alpha>=beta
            ttEntry.score < beta)
            ttEntry.score = NO_EVAL;

        if (ttEntry.type == UPPER_BOUND &&
        	ttEntry.score > alpha)
            ttEntry.score = NO_EVAL;
    }

    template<NodeType nodePvType>
    int quiescentSearch(Board &board, int color, int alpha, int beta, int ply) {

        constexpr bool isPvNode = nodePvType != NonPV;

    	if (isPvNode)
    		seldepth = max(seldepth, ply + 1);

        if (stopSearch || nodes >= nodesLim) {
            stopSearch = true;
            return 0;
        }

    	if ((nodes & 1023) == 0) {
        	std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
            ll timeThinked = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - searchStartTime).count();
            if (timeThinked >= hardTimeBound) {
	            stopSearch = true;
	            return 0;
	        }
	    }

        nodes++;

        ull currentZobristKey = board.getZobristKey();
        auto ttEntry = transpositionTable.get(board, currentZobristKey, ply);

        auto prEntry = ttEntry;
        correctTTscore(ttEntry, alpha, beta);

        int nodeType = ttEntry.type;

        if (ttEntry.score != NO_EVAL)
            return ttEntry.score;

        Move ttMove = ttEntry.move;

        if (!moveGenerator.isMoveLegal(board, ttMove) || board.isQuietMove(ttMove))
        	ttMove = Move();

        moveListGenerator.hashMove = ttMove;

        int rawStaticEval, staticEval;
        if (moveListGenerator.isStalled(board, color) || evaluator.insufficientMaterialDraw(board))
            return evaluator.evaluateStalledPosition(board, color, ply);
        else {
        	if (ttEntry.eval != NO_EVAL)
        		rawStaticEval = ttEntry.eval;
        	else {
            	rawStaticEval = evaluator.evaluatePosition(board, color, nnueEvaluator);
            	transpositionTable.writeStaticEval(currentZobristKey, rawStaticEval);
        	}
        	staticEval = rawStaticEval + corrhistHelper.getScore(color, board);
        }

        ttEntry = prEntry;
        correctTTscore(ttEntry, staticEval, staticEval);
        if (ttEntry.score != NO_EVAL)
            staticEval = ttEntry.score;

        int bestScore = staticEval;

        bool isMovingSideInCheck = moveGenerator.isInCheck(board, color);

        alpha = max(alpha, staticEval);
        if (alpha >= beta) {
            transpositionTable.write(board, currentZobristKey, bestScore, rawStaticEval, 0, LOWER_BOUND, boardCurrentAge,
                                              ttEntry.move, ply);
            return bestScore;
        }

        // moveListGenerator.killerMove=moveListGenerator.hashMove;
        Board boardCopy = board;
        if (ttMove == Move()) {
        	moveListGenerator.generateMoves(board, historyHelper, color, ply, DO_SORT, ONLY_CAPTURES);
        }

        #if !defined DO_HCE
        __int16_t accumW[hiddenLayerSize], accumB[hiddenLayerSize];
        for (int i = 0; i < hiddenLayerSize; i += 16) {

            _mm256_storeu_si256((__m256i *)&accumW[i], _mm256_loadu_si256((__m256i *)&nnueEvaluator.hlSumW[i]));

            _mm256_storeu_si256((__m256i *)&accumB[i], _mm256_loadu_si256((__m256i *)&nnueEvaluator.hlSumB[i]));

            // accumW[i]=nnueEvaluator.hlSumW[i];
            // accumB[i]=nnueEvaluator.hlSumB[i];
        }
        #endif

        int oppositeColor = (color == WHITE) ? BLACK : WHITE;

        bool isFirstMove = 1;
        char type = UPPER_BOUND;
        Move newTTmove = Move();

        if (ttMove != Move())
        	moveListGenerator.moveListSize[ply] = 1;

        bool searchedTTmove = false;

        for (int currentMove = 0; currentMove < moveListGenerator.moveListSize[ply]; currentMove++) {
            Move move = moveListGenerator.moveList[ply][currentMove];

            if(ttMove != Move() && !searchedTTmove) {
            	move = ttMove;
            	searchedTTmove = true;
            }


            int seeEval = moveListGenerator.seeTable[move.getStartSquare()][move.getTargetSquare()];
        	if(move == ttMove)
        		seeEval = moveGenerator.sseEval(board, move.getTargetSquare(), color, move.getStartSquare());

        	if (staticEval + 100 < alpha && seeEval <= 0)
        		continue;

            ull newKey = zobristAfterMove(board, move);
            transpositionTable.prefetch(newKey);

            board.makeMove(move, nnueEvaluator);


            // transpositionTable.prefetch(board.getZobristKey());

            int score = -quiescentSearch<nodePvType>(board, (color == WHITE) ? BLACK : WHITE, -beta, -alpha, ply + 1);

            board = boardCopy;

            #if !defined DO_HCE
            for (int i = 0; i < hiddenLayerSize; i += 16) {

                _mm256_storeu_si256((__m256i *)&nnueEvaluator.hlSumW[i], _mm256_loadu_si256((__m256i *)&accumW[i]));

                _mm256_storeu_si256((__m256i *)&nnueEvaluator.hlSumB[i], _mm256_loadu_si256((__m256i *)&accumB[i]));

                // nnueEvaluator.hlSumW[i]=accumW[i];
                // nnueEvaluator.hlSumB[i]=accumB[i];
            }
            #endif

            isFirstMove = 0;
            if (stopSearch)
                return 0;
            if (bestScore < score) {
                if (score > alpha)
                    type = EXACT;
                bestScore = score;
                newTTmove = move;
                if (alpha < score)
                    alpha = score;
                if (alpha >= beta) {
                    transpositionTable.write(board, currentZobristKey, score, rawStaticEval, 0, LOWER_BOUND,
                                                      boardCurrentAge, move, ply);
                    return bestScore;
                }
            }

            if (move == ttMove) {
		        moveListGenerator.hashMove = ttMove;

            	moveListGenerator.generateMoves(board, historyHelper, color, ply, DO_SORT, ONLY_CAPTURES);
            }
        }
        transpositionTable.write(board, currentZobristKey, bestScore, rawStaticEval, 0, type, boardCurrentAge,
                                          newTTmove, ply);
        return bestScore;
    }

#define killMovesNumber 2
    Move killers[maxDepth][killMovesNumber];
    int killersAge[maxDepth][killMovesNumber];

    int staticEvaluationHistory[maxDepth];

	template<NodeType nodePvType>
    int search(Board &board, int color, int depth, int isRoot, int alpha, int beta, int ply, int extended) {
    	
        constexpr bool isPvNode = nodePvType != NonPV;

    	if (isPvNode)
    		seldepth = max(seldepth, ply + 1);

        if (stopSearch || nodes >= nodesLim) {
            stopSearch = true;
            return 0;
        }

    	if ((nodes & 1023) == 0) {
        	std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
            ll timeThinked = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - searchStartTime).count();
            if (timeThinked >= hardTimeBound) {
	            stopSearch = true;
	            return 0;
	        }
	    }

	    if (!isRoot) {
	    	alpha = max(alpha, -MATE_SCORE + ply);
	    	beta = min(beta, MATE_SCORE - ply);
	    	if (alpha >= beta)
	    		return alpha;
	    }

        nodes++;

        if (!isRoot && board.age - board.lastIrreversibleMoveAge > 100)
        	return DRAW_SCORE;

        ull currentZobristKey = board.getZobristKey();

        if (!isRoot) {
            for (int repAge = board.age - 4; repAge > max(board.lastIrreversibleMoveAge, -1); repAge -= 1)
                if (currentZobristKey == occuredPositionsHelper.occuredPositions[repAge])
                    return DRAW_SCORE;
        }

        if (!isRoot && evaluator.insufficientMaterialDraw(board))
            return evaluator.evaluateStalledPosition(board, color, ply);

        int rawStaticEval, staticEval;

        auto ttEntry = transpositionTable.get(board, currentZobristKey, ply);
        auto corrEntry = ttEntry;
        correctTTscore(corrEntry, alpha, beta);

        int nodeType = ttEntry.type;

        if (corrEntry.score != NO_EVAL &&
        	ttEntry.depth >= depth &&
        	!isRoot &&
        	!isPvNode &&
        	!searchStack[ply].excludeTTmove)

            return corrEntry.score;

    	if (ttEntry.eval != NO_EVAL)
    		rawStaticEval = ttEntry.eval;
    	else {
        	rawStaticEval = evaluator.evaluatePosition(board, color, nnueEvaluator);
            transpositionTable.writeStaticEval(currentZobristKey, rawStaticEval);
    	}
    	staticEval = rawStaticEval + corrhistHelper.getScore(color, board);

        auto scorrEntry = ttEntry;
        correctTTscore(scorrEntry, staticEval, staticEval);

        if (scorrEntry.score != NO_EVAL)
            staticEval = scorrEntry.score;

        // cout<<board.generateFEN()<<' '<<staticEval<<'\n';
        bool improving = false;
        bool isMovingSideInCheck = moveGenerator.isInCheck(board, color);

        if (isMovingSideInCheck)
            staticEvaluationHistory[ply] = NONE_SCORE;
        else {
            staticEvaluationHistory[ply] = staticEval;
            if (ply >= 2) {
                int previousEval = staticEvaluationHistory[ply - 2];
                if (previousEval == NONE_SCORE || previousEval < staticEval)
                    improving = true;
            }
        }

        bool isMateScores = (abs(alpha) >= MATE_SCORE_MAX_PLY ||
					    	 abs(beta) >= MATE_SCORE_MAX_PLY ||
					    	 abs(staticEval) >= MATE_SCORE_MAX_PLY);


        // Reverse futility pruning
        if (!isRoot &&
        	!isMovingSideInCheck &&
        	ttEntry.type == NONE &&
        	!isPvNode &&
            !searchStack[ply].excludeTTmove &&
            !isMateScores) {

            int margin = (30 - improving * 15) * max(depth, 1) * max(depth, 1);

            if (staticEval >= beta + margin)
                return (staticEval + beta) / 2;
        }

        if (depth <= 0){
        	nodes--;
            return quiescentSearch<nodePvType>(board, color, alpha, beta, ply);
        }

        int oppositeColor = (color == WHITE) ? BLACK : WHITE;

        // Null move pruning
        if (!isRoot &&
        	!isMovingSideInCheck &&
            ((board.whitePieces | board.blackPieces) ^ (board.pawns | board.kings)) >
                0 &&              // pieces except kings and pawns exist (to prevent zugzwang)
            staticEval >= beta &&
            !isPvNode &&
            !searchStack[ply].excludeTTmove &&
            !isMateScores) {

            int R = floor(4 +
            	depth / 5.0 +
            	min((staticEval - beta) / 200.0, 5.0));

            int prevEnPassColumn = board.makeNullMove();
            int score = -search<NonPV>(board, oppositeColor, depth - 1 - R, 0, -beta, -beta + 1, ply + 1, extended);
            board.makeNullMove();
            board.enPassantColumn = prevEnPassColumn;
            if (score >= beta)
                return score;
        }

        if (!isRoot &&
        	!isPvNode &&
        	!isMovingSideInCheck &&
        	!searchStack[ply].excludeTTmove &&
        	!isMateScores) { // Razoring

            int margin = 150 * depth * depth + 200;

            if (staticEval + margin < alpha) {
                int qEval = quiescentSearch<NonPV>(board, color, alpha - 1, alpha, ply + 1);

                if (depth == 1 ||
                	(depth <= 2 && ttEntry.type != NONE && ttEntry.score < alpha - margin - 50))
                	return qEval;

                if (qEval < alpha)
                    return qEval;

                if (depth > 1 && depth <= 3 && qEval > beta + 200)
                	depth--;
            }
        }

        Board boardCopy = board;

        Move ttMove = ttEntry.move;

        if (isRoot && depth > 1)
            ttMove = bestMove;

        moveListGenerator.hashMove = ttMove;

        #if !defined DO_HCE
        __int16_t accumW[hiddenLayerSize], accumB[hiddenLayerSize];
        for (int i = 0; i < hiddenLayerSize; i += 16) {

            _mm256_storeu_si256((__m256i *)&accumW[i], _mm256_loadu_si256((__m256i *)&nnueEvaluator.hlSumW[i]));

            _mm256_storeu_si256((__m256i *)&accumB[i], _mm256_loadu_si256((__m256i *)&nnueEvaluator.hlSumB[i]));

            // accumW[i]=nnueEvaluator.hlSumW[i];
            // accumB[i]=nnueEvaluator.hlSumB[i];
        }
        #endif

        int probcutDepthR = 4;
        //ProbCut
        if (!isRoot &&
        	depth >= probcutDepthR &&
        	!isPvNode && 
        	!isMovingSideInCheck &&
        	!searchStack[ply].excludeTTmove &&
        	!isMateScores) {

        	int probcutBeta = beta + 200;

        	if (ttEntry.type == NONE || ttEntry.score >= probcutBeta || ttEntry.depth < depth - probcutDepthR) {

	        	moveListGenerator.generateMoves(board, historyHelper, color, ply, DO_SORT, ONLY_CAPTURES);

		        for (int currentMove = 0; currentMove < moveListGenerator.moveListSize[ply]; currentMove++) {
		            Move move = moveListGenerator.moveList[ply][currentMove];

		            board.makeMove(move, nnueEvaluator);

		            int score = -quiescentSearch<NonPV>(board, oppositeColor, -probcutBeta, -probcutBeta + 1, ply + 1);

		            if (score >= probcutBeta)
		            	score = -search<NonPV>(board, oppositeColor, depth - probcutDepthR, 0, -probcutBeta, -probcutBeta + 1,
	                                    ply + 1, extended);

		            board = boardCopy;

		            #if !defined DO_HCE
		            for (int i = 0; i < hiddenLayerSize; i += 16) {

		                _mm256_storeu_si256((__m256i *)&nnueEvaluator.hlSumW[i], _mm256_loadu_si256((__m256i *)&accumW[i]));

		                _mm256_storeu_si256((__m256i *)&nnueEvaluator.hlSumB[i], _mm256_loadu_si256((__m256i *)&accumB[i]));

		                // nnueEvaluator.hlSumW[i]=accumW[i];
		                // nnueEvaluator.hlSumB[i]=accumB[i];
		            }
		            #endif

		            if (score >= probcutBeta) {
	                    transpositionTable.write(board, currentZobristKey, score, rawStaticEval, depth - probcutDepthR, LOWER_BOUND,
	                                             boardCurrentAge, move, ply);
	                    return score;
		            }
		        }
		    }
        }


        int killerMove = 0, killerBackup = 1;

        moveListGenerator.killerMove = killers[ply][0];
        moveListGenerator.killerBackup = killers[ply][1];

        if (killersAge[ply][0] < killersAge[ply][1]) {
        	swap(killerMove, killerBackup);
        	swap(moveListGenerator.killerMove, moveListGenerator.killerBackup);
        }


        // Internal Iterative Reductions (IIR)
        if (depth >= 6 &&
        	isPvNode &&
        	nodeType == NONE) {

            depth--;
        }

        Bitboard whiteAttacks = moveGenerator.computeAttackBitboardsW(board);
        Bitboard blackAttacks = moveGenerator.computeAttackBitboardsB(board);

        bool doTTmoveBeforeMovegen = true;

        if(ttMove == Move() || searchStack[ply].excludeTTmove){
        	doTTmoveBeforeMovegen = false;
        	historyHelper.whiteAttacks = whiteAttacks;
        	historyHelper.blackAttacks = blackAttacks;
        	moveListGenerator.generateMoves(board, historyHelper, color, ply, DO_SORT, ALL_MOVES);
        	if (moveListGenerator.moveListSize[ply] == 0)
        		return evaluator.evaluateStalledPosition(board, color, ply);
        } else {
        	moveListGenerator.moveListSize[ply] = 1;
        }

        // Singular extensions
        int extendTTmove = 0;
        if (
        	extended <= 30 &&
        	ply < maxDepth - 10 &&
        	ttMove != Move() &&
        	depth >= 7 &&
        	ttEntry.depth >= depth - 3 &&
        	!searchStack[ply].excludeTTmove &&
        	nodeType != UPPER_BOUND &&
        	abs(MATE_SCORE) - abs(ttEntry.score) > maxDepth
        	){

        	searchStack[ply + 1].excludeTTmove = true;
        	searchStack[ply + 1].excludeMove = ttMove;
        	int singularBeta = ttEntry.score - depth;
        	int singularScore = search<nodePvType>(board, color, depth / 2, 0, singularBeta - 1, singularBeta, ply + 1, extended);

        	searchStack[ply + 1].excludeTTmove = false;

        	if (singularScore < singularBeta){

        		singularExtended++;

        		extendTTmove = 1;

        		// Double extentions
        		if (!isPvNode && singularScore < singularBeta - 30)
        			extendTTmove++;

        	} else if (singularScore >= beta && MATE_SCORE - abs(singularScore) > maxDepth)
        		return beta; // Multicut
        }

        int bestScore = -inf;
        char type = UPPER_BOUND;
        int movesSearched = 0;
        int quietMovesSearched = 0;
        Move newTTmove = Move();
        int numberOfMoves = moveListGenerator.moveListSize[ply];

        bool isTTCapture = (ttMove != Move() && !board.isQuietMove(ttMove));

        bool searchedTTmove = false;

        if (ttMove != Move() && doTTmoveBeforeMovegen) {
        	moveListGenerator.moveListSize[ply] = 1;
        }

        if (isPvNode)
        	searchStack[ply].pvLine = vector<Move>();

        for (int currentMove = 0; currentMove < moveListGenerator.moveListSize[ply]; currentMove++) {
            Move move = moveListGenerator.moveList[ply][currentMove];

            if (move == searchStack[ply].excludeMove) {
                if (searchStack[ply].excludeTTmove)
                    continue;
            }

            if (doTTmoveBeforeMovegen && !searchedTTmove) {
            	move = ttMove;
            	searchedTTmove = true;
            }


        	historyHelper.whiteAttacks = whiteAttacks;
        	historyHelper.blackAttacks = blackAttacks;

            int historyValue = historyHelper.getScore(board, color, move) - historyHelper.maxHistoryScore;
            float historyValueF = historyValue / float(historyHelper.maxHistoryScore);

            bool isKiller = (
        		move == killers[ply][0] ||
        		move == killers[ply][1]);

            int extendDepth = 0;

            if (move == ttMove)
                extendDepth += extendTTmove;

            bool isMoveInteresting = (moveGenerator.isInCheck(board, oppositeColor) || // checking move
                                      !board.isQuietMove(move));                       // non-quiet move

            bool isCapture = (board.whitePieces | board.blackPieces).getBit(move.getTargetSquare());
            bool inCheck = moveGenerator.isInCheck(board, oppositeColor);
            bool isPromotion = (move.getPromotionFlag() > 0);
            int sseEval = 0;
            if (isCapture){
                sseEval = moveListGenerator.seeTable[move.getStartSquare()][move.getTargetSquare()];
            	if(doTTmoveBeforeMovegen && currentMove == 0)
            		sseEval = moveGenerator.sseEval(board, move.getTargetSquare(), color, move.getStartSquare());
            }

            bool beingMated = (alpha <= -MATE_SCORE_MAX_PLY ||
            				   bestScore <= -MATE_SCORE_MAX_PLY ||
            				   isMateScores);

            // Conditions for moveloop pruning
            if (!beingMated &&
            	!isRoot &&
            	currentMove > 0 &&
            	!isMovingSideInCheck) {

            	// Late move pruning (LMP)
	            if (!isPvNode &&
	            	movesSearched > 3 + depth * depth * (1 - isTTCapture * 0.5) &&
	            	historyValue < 0) {

	            	break;
	            }
	            
	            // History pruning
	            if (!isPvNode &&
	            	movesSearched > 0 &&
	            	!isMoveInteresting &&
	            	historyValue < -100 * depth * depth) {

	            	continue;
	            }

	            // Futility pruning (FP)
	            int fpMargin = max((150 + historyValueF * 75 - isTTCapture * 100), float(0)) * depth * depth;

	            if (movesSearched > 0 &&
	            	staticEval < alpha - fpMargin &&
	                !isMoveInteresting &&
	                !searchStack[ply].excludeTTmove
	            ) {

	                continue;
	            }

	            // Captures SEE pruning
	            int seeMargin[4] = {0, 200, 400, 900};

	            if (movesSearched > 0 &&
	            	!isPvNode &&
	            	!inCheck &&
	            	depth <= 3 &&
	                sseEval <= -seeMargin[depth] && !searchStack[ply].excludeTTmove) {

	                continue;
	            }
	        }

            ull newKey = zobristAfterMove(board, move);
            transpositionTable.prefetch(newKey);


            occuredPositionsHelper.occuredPositions[board.age + 1] = newKey;

            board.makeMove(move, nnueEvaluator);

            // transpositionTable.prefetch(board.getZobristKey());

            // cout<<move.convertToUCI()<<' '<<newStaticEval<<'\n';


            int prevNodes = nodes;

            int score;
            if (movesSearched) { // Principal variation search

                // Late move reduction
                const int LMR_FULL_MOVES = 2; // number of moves to search with full depth
                const int LMR_MIN_DEPTH = 3;  // don't reduct depth if it's more or equal to this value

                int lmrReduction =
                    floor(lmrLogTable[depth][movesSearched] + 0.5 
                    	- 1 * (isPvNode)
                    	- 1.5 * historyValueF
                    	+ 0.5 * (!improving)
                    	+ 1 * (isTTCapture)
                    	- 1 * (isCapture)
                    	- 0.002 * sseEval
                        - 1 * (isKiller)); // reduction of depth

                if (lmrReduction < 0)
                    lmrReduction = 0;

                lmrReduction = min(lmrReduction, depth - 1);


                bool doLMRcapture = true;
                if (inCheck)
                	doLMRcapture = false;

                if (movesSearched >= LMR_FULL_MOVES && !isMovingSideInCheck && depth >= LMR_MIN_DEPTH &&
                    doLMRcapture 
                ) {
                    score = -search<NonPV>(board, oppositeColor, depth - 1 - lmrReduction, 0, -(alpha + 1), -alpha,
                                    ply + 1, extended);
                } else
                    score = alpha + 1; // if LMR is restricted, do this to do PVS

                if (score > alpha) {
                    score = -search<NonPV>(board, oppositeColor, depth - 1 + extendDepth, 0, -(alpha + 1), -alpha,
                                    ply + 1, extended + extendDepth);
                    if (isPvNode && score > alpha && score < beta)
                        score =
                            -search<nodePvType>(board, oppositeColor, depth - 1 + extendDepth, 0, -beta, -alpha, ply + 1, extended + extendDepth);
                }
            } else
                score = -search<nodePvType>(board, oppositeColor, depth - 1 + extendDepth, 0, -beta, -alpha, ply + 1, extended + extendDepth);

            board = boardCopy;

            #if !defined DO_HCE
            for (int i = 0; i < hiddenLayerSize; i += 16) {

                _mm256_storeu_si256((__m256i *)&nnueEvaluator.hlSumW[i], _mm256_loadu_si256((__m256i *)&accumW[i]));

                _mm256_storeu_si256((__m256i *)&nnueEvaluator.hlSumB[i], _mm256_loadu_si256((__m256i *)&accumB[i]));

                // nnueEvaluator.hlSumW[i]=accumW[i];
                // nnueEvaluator.hlSumB[i]=accumB[i];
            }
            #endif

            if (isRoot)
            	rootNodes[move.move] += (nodes - prevNodes);

            movesSearched++;
            if (!isMoveInteresting)
                quietMovesSearched++;

            if (stopSearch) {
                return 0;
            }

            if (bestScore < score) {
                if (score > alpha)
                    type = EXACT;
                bestScore = score;
                newTTmove = move;
                if (isPvNode) {
                	searchStack[ply].pvLine = searchStack[ply + 1].pvLine;
                	searchStack[ply].pvLine.push_back(move);
                }
                searchStack[ply].bestMove = move;
                if (isRoot) {
                    bestMove = move;
                    rootScore = score;
                }
                if (alpha < score)
                    alpha = score;
                if (alpha >= beta) {
                    if (board.isQuietMove(move)) {
                        // update killer move

                    	if (killers[ply][0] == move)
                    		killersAge[ply][0] = nodes;
                    	else if (killers[ply][1] == move)
                    		killersAge[ply][1] = nodes;
                    	else if (killers[ply][0] == Move()) {
                    		killers[ply][0] = move;
                    		killersAge[ply][0] = nodes;
                    	}
                    	else if (killers[ply][1] == Move()) {
                    		killers[ply][1] = move;
                    		killersAge[ply][1] = nodes;
                    	}
                    	else if (killersAge[ply][0] < killersAge[ply][1]) {
                    		killers[ply][0] = move;
                    		killersAge[ply][0] = nodes;
                    	} else {
                    		killers[ply][1] = move;
                    		killersAge[ply][1] = nodes;
                    	}

                    }

                    if (!isMovingSideInCheck && (newTTmove == Move() || board.isQuietMove(newTTmove))) {
                    	staticEval = rawStaticEval + corrhistHelper.getScore(color, board);
                    	if (score > staticEval)
                    		corrhistHelper.update(color, board, (score - staticEval) * depth / 8);
                    }

		        	historyHelper.whiteAttacks = whiteAttacks;
		        	historyHelper.blackAttacks = blackAttacks;

		        	int historyBonus = 10 * depth + 0;
		        	int maluseBonus = 10 * depth + 0;

                    historyHelper.update(board, color, move, historyBonus);

                    for (int previousMoves = 0; previousMoves < currentMove;
                         previousMoves++) { // negate all searched non-capture moves
                        Move prevMove = moveListGenerator.moveList[ply][previousMoves];
                        historyHelper.update(board, color, prevMove, -maluseBonus);
                    }

                    transpositionTable.write(board, currentZobristKey, score, rawStaticEval, depth, LOWER_BOUND,
                                             boardCurrentAge, newTTmove, ply);
                    return bestScore;
                }
            }

            if (doTTmoveBeforeMovegen && currentMove == 0) {
	        	historyHelper.whiteAttacks = whiteAttacks;
	        	historyHelper.blackAttacks = blackAttacks;

		        moveListGenerator.hashMove = ttMove;
		        moveListGenerator.killerMove = killers[ply][killerMove];
		        moveListGenerator.killerBackup = killers[ply][killerBackup];

            	moveListGenerator.generateMoves(board, historyHelper, color, ply, DO_SORT, ALL_MOVES);
            }
        }

        if (type == UPPER_BOUND)
        	newTTmove = Move();

        if (!isMovingSideInCheck && (newTTmove == Move() || board.isQuietMove(newTTmove))) {
            staticEval = rawStaticEval + corrhistHelper.getScore(color, board);
        	if (type == EXACT || bestScore < staticEval)
        		corrhistHelper.update(color, board, (bestScore - staticEval) * depth / 8);
        }

        if (bestScore == -inf)
        	bestScore = alpha;

        transpositionTable.write(board, currentZobristKey, bestScore, rawStaticEval, depth, type, boardCurrentAge, newTTmove, ply);
        return bestScore;
    }

    int startSearch(Board &board, int depth, int alpha, int beta) {
        return search<PV>(board, board.boardColor, depth, true, alpha, beta, 0, 0);
        doneSearch = true;
    }

    Worker() {
        historyHelper.clear();
    }

    void init() {
        historyHelper.clear();
    }

    int aspirationSearch(Board &board, int depth, int expectedScore) {
        const int aspirationWindow = 25, aspirationWindowMult = 2;
        int alphaWindow = aspirationWindow, betaWindow = aspirationWindow;
        while (true) {
            int alpha = max(-MATE_SCORE, expectedScore - alphaWindow);
            int beta = min(MATE_SCORE, expectedScore + betaWindow);
            int score = startSearch(board, depth, alpha, beta);
            if (score <= alpha)
                alphaWindow *= aspirationWindowMult;
            else if (score >= beta)
                betaWindow *= aspirationWindowMult;
            else
                return score;
        }
    }

    void IDsearchDatagen(Board &board, int maxDepth, int nodesLimit, int nodesH) {
        nodesLim = nodesH;
        nodes = 0;

        int color = board.boardColor;

        stopSearch = false;

        int score = 0;

        for (int depth = 1; depth <= maxDepth; depth++) {
            // workers[0].nnueEvaluator.printAccum();
            // cout<<'\n';
            int alpha = -inf * 2, beta = inf * 2;

            if (depth == 1)
            	search<PV>(board, board.boardColor, depth, 1, alpha, beta, 0, 0);
            else
            	aspirationSearch(board, depth, score);

            score = rootScore;

            // cout<<"depth "<<depth<<" score "<<rootScore<<' '<<bestMove.convertToUCI()<<'\n';

            if (nodes >= min(nodesLimit, nodesH))
                break;
        }
        stopSearch = true;
    }

    bool minimal = false;
    bool doNormalization = true;

    void IDsearch(Board &board, int maxDepth, int softBound, int hardBound, int nodesLimit, int nodesH, bool isMainThread, bool printUCI, vector<Worker> &workers) {
        
        nodesLim = nodesH;
        nodes = 0;
        hardTimeBound = hardBound;
        seldepth = 0;

        int color = board.boardColor;

        stopSearch = false;

        Move bestMoves[257];
        int scores[257];
        int times[257];
        int dnodes[257];
        int prevTimeThinked = 0, prevNodes = 0;
        float branchFactor = 2;

        int score = 0;
        searchStartTime = std::chrono::steady_clock::now();

        for (int i = 0; i < (1 << 16); i++)
        	rootNodes[i] = 0;

        for (int depth = 1; depth <= maxDepth; depth++) {
            // workers[0].nnueEvaluator.printAccum();
            // cout<<'\n';
            int alpha = -MATE_SCORE, beta = MATE_SCORE;

            if (depth == 1)
            	search<PV>(board, board.boardColor, depth, 1, alpha, beta, 0, 0);
            else
            	aspirationSearch(board, depth, score);

            score = rootScore;

	        if (bestMove == Move()) {
	        	auto ttEntry = transpositionTable.get(board, board.getZobristKey(), 0);
	        	if (ttEntry.move != Move() && moveGenerator.isMoveLegal(board, ttEntry.move))
	        		bestMove = ttEntry.move;
	        	else {
	        		moveListGenerator.generateMoves(board, historyHelper, color, 0, DO_SORT, ALL_MOVES);
	        		bestMove = moveListGenerator.moveList[0][0];
	        	}
	        }

            if (isMainThread) {
            	std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
	            ll timeThinked = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - searchStartTime).count();

	            scores[depth] = score;
	            bestMoves[depth] = bestMove;
	            dnodes[depth] = nodes - prevNodes;
	            prevNodes = nodes;
	            times[depth] = max(1ll, timeThinked - prevTimeThinked);
	            prevTimeThinked = timeThinked;

	            // for(int i=1;i<workers[0].pvLineSize;i++)
	            // 	cout<<workers[0].pvLine[i].convertToUCI()<<' ';

	            bool stopIDsearch = false;

	            if (nodes >= min(nodesLimit, nodesH))
	                stopIDsearch = true;

	            int timeUntilHardBound = hardBound - timeThinked;
	            if (timeUntilHardBound <= 0 || stopSearch)
	            	stopIDsearch = true;

	            if (depth > 1)
	                branchFactor =
	                    (branchFactor + clamp(float(dnodes[depth]) / dnodes[depth - 1], float(1.3), float(6))) / 2;

	            int estimatedTimeForNextDepth = times[depth] * branchFactor;

	            int bestMoveStreak = 1;
	            for (int i = depth - 1; i >= 1; i--) {
	            	if (bestMoves[depth] != bestMoves[i])
	            		break;
	            	bestMoveStreak++;
	            }

	            float bestmoveStabilityMult[5] = {2.50, 1.20, 0.90, 0.80, 0.75};

	            bestMoveStreak = min(bestMoveStreak, 5);

	            if (nodes == 0)
	            	nodes = 1;

	            float bestmoveNodePart = float(rootNodes[bestMove.move]) / nodes;

	            int targetTime = softBound 
	            * bestmoveStabilityMult[bestMoveStreak - 1]
	            * (1.7 - bestmoveNodePart);

	            if (timeThinked >= targetTime) {
	            	stopIDsearch = true;
	            }

	            // cout<<estimatedTimeForNextDepth<<'\n';
	            // if (timeThinked >= softBound) {
	            //     if (depth >= 3) {
	            //         if (bestMoves[depth] == bestMoves[depth - 1] &&
	            //             bestMoves[depth] == bestMoves[depth - 2]) { // if best move is stable, abort the search
	            //             stopIDsearch = true;
	            //         }
	            //         if (timeUntilHardBound < estimatedTimeForNextDepth) {
	            //             stopIDsearch = true;
	            //         }
	            //     }
	            // }

            	int totalNodes = 0;
            	for (int i = 0; i < workers.size(); i++)
            		totalNodes += workers[i].nodes;

            	if (printUCI && (!minimal || stopIDsearch || depth == maxDepth)) {
	                cout << "info depth " << depth;
	                cout << " seldepth ";
	                cout << seldepth;
	                cout << " score ";
	                if (MATE_SCORE - abs(score) > maxDepth){
	                    cout << "cp ";
	                	if (doNormalization)
	                		cout << normalizeNNUEscore(score, board.getNormalizeMaterial());
	                	else
	                		cout << score;
	                }
	                else {
	                    cout << "mate ";
	                    if (score > 0)
	                        cout << (MATE_SCORE - score + 1) / 2;
	                    else
	                        cout << (-MATE_SCORE - score - 1) / 2;
	                }
	                cout << " nodes " << totalNodes;
	                cout << " nps " << (totalNodes * (long long)(1000)) / (timeThinked + 1);
	                cout << " hashfull " << transpositionTable.getHashfull();
	                cout << " time " << timeThinked;
	                cout << " pv ";
	                reverse(searchStack[0].pvLine.begin(), searchStack[0].pvLine.end());
	                for (auto move:searchStack[0].pvLine)
	                	cout << move.convertToUCI() << ' ';
	                cout << endl;
	            }

                if (stopIDsearch)
                	break;
            }

            if (stopSearch)
            	break;
        }
        
        if (printUCI)
        	cout << "bestmove " << bestMove.convertToUCI() << endl;
    }
};

struct Searcher {
    bool doInfoOutput = true;
    int threadNumber = 1;
    vector<Worker> workers;
    bool stopIDsearch;
    bool minimal = false;

    Searcher() {
        workers.resize(threadNumber);
    }

    void iterativeDeepeningSearch(int maxDepth, int softBound, int hardBound, int nodesLimit, int nodesH) {
        workers[0].nodesLim = nodesH;
        stopIDsearch = false;
        int color = mainBoard.boardColor;
        vector<thread> threadPool(threadNumber);
        vector<Board> boards(threadNumber, mainBoard);
        for (int i = 0; i < threadNumber; i++) {
            workers[i].nodes = 0;
            workers[i].bestMove = Move();
            workers[i].minimal = minimal;
            workers[i].stopSearch = false;
            workers[i].nnueEvaluator = mainNnueEvaluator;
            workers[i].occuredPositionsHelper = mainOccuredPositionsHelper;
            mainBoard.initNNUE(workers[i].nnueEvaluator);
            for (ll j = 0; j < 256; j++) {
                for (ll j1 = 0; j1 < 2; j1++) {
                    workers[i].killers[j][j1] = Move();
                    workers[i].killersAge[j][j1] = 0;
                }
            }
        }

        for (int i = 1; i < threadNumber; i++)
            threadPool[i] = thread(&Worker::IDsearch, &workers[i], ref(boards[i]), maxDepth, softBound, hardBound, nodesLimit, nodesH, false, 0, ref(workers));

        workers[0].IDsearch(ref(boards[0]), maxDepth, softBound, hardBound, nodesLimit, nodesH, true, doInfoOutput, ref(workers));

        for (int i = 1; i < threadNumber; i++) {
        	workers[i].stopSearch = true;
        	threadPool[i].join();
        }
    }
};

Searcher searcher;