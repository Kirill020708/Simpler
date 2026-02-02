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

    float texelSearch(Board &board, int color, float alpha, float beta, int depthFromRoot) {
        float staticEval;
        if (moveListGenerator.isStalled(board, color) || evaluator.insufficientMaterialDraw(board))
            staticEval = evaluator.evaluateStalledPosition(board, color, depthFromRoot);
        else {
            staticEval = evaluator.evaluatePositionDeterministic(board);
            if (color == BLACK)
                staticEval = -staticEval;
        }

        float maxEvaluation = staticEval;

        alpha = max(alpha, staticEval);
        if (alpha >= beta)
            return maxEvaluation;

        Board boardCopy = board;
        moveListGenerator.generateMoves(board, historyHelper, color, depthFromRoot, DO_SORT, ONLY_CAPTURES);

        for (int currentMove = 0; currentMove < moveListGenerator.moveListSize[depthFromRoot]; currentMove++) {
            Move move = moveListGenerator.moveList[depthFromRoot][currentMove];
            board.makeMove(move);

            float score = -texelSearch(board, (color == WHITE) ? BLACK : WHITE, -beta, -alpha, depthFromRoot + 1);

            board = boardCopy;
            if (maxEvaluation < score) {
                maxEvaluation = score;
                if (alpha < score)
                    alpha = score;
                if (alpha >= beta)
                    return maxEvaluation;
            }
        }
        return maxEvaluation;
    }

    template<NodeType nodePvType>
    int quiescentSearch(Board &board, int color, int alpha, int beta, int depthFromRoot) {

        constexpr bool isPvNode = nodePvType != NonPV;

    	if (isPvNode)
    		seldepth = max(seldepth, depthFromRoot + 1);

    	if ((nodes & 1023) == 0) {
        	std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
            ll timeThinked = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - searchStartTime).count();
            if (timeThinked >= hardTimeBound)
            	stopSearch = true;
	        if (stopSearch || nodes >= nodesLim) {
	            stopSearch = true;
	            return 0;
	        }
	    }

        nodes++;

        ull currentZobristKey = board.getZobristKey();
        auto [hashTableEvaluation, bestHashMove] =
            transpositionTableQuiescent.get(board, currentZobristKey, 0, alpha, beta, depthFromRoot);
        int nodeType = transpositionTableQuiescent.getNodeType(currentZobristKey);
        if (hashTableEvaluation != NO_EVAL) {
            return hashTableEvaluation;

            // alpha=max(alpha,hashTableEvaluation);
            // if(alpha>=beta)
            // 	return alpha;
        }
        if (!moveGenerator.isMoveLegal(board, bestHashMove))
        	bestHashMove = Move();
        moveListGenerator.hashMove = bestHashMove;
        Move ttMove = bestHashMove;

        int staticEval;
        if (moveListGenerator.isStalled(board, color) || evaluator.insufficientMaterialDraw(board))
            staticEval = evaluator.evaluateStalledPosition(board, color, depthFromRoot);
        else
            staticEval = evaluator.evaluatePosition(board, color, nnueEvaluator, corrhistHelper);

        auto ttEntry = transpositionTableQuiescent.getEntry(board, currentZobristKey, depthFromRoot);
        if (ttEntry.evaluation != NO_EVAL)
            staticEval = ttEntry.evaluation;

        int maxEvaluation = staticEval;

        bool isMovingSideInCheck = moveGenerator.isInCheck(board, color);

        int numOfPiecesOnBoard = board.numberOfPieces();

        alpha = max(alpha, staticEval);
        if (alpha >= beta) {
            transpositionTableQuiescent.write(board, currentZobristKey, maxEvaluation, 0, LOWER_BOUND, boardCurrentAge,
                                              bestHashMove, depthFromRoot);
            return maxEvaluation;
        }

        // moveListGenerator.killerMove=moveListGenerator.hashMove;
        Board boardCopy = board;
        if (ttMove == Move()) {
        	moveListGenerator.generateMoves(board, historyHelper, color, depthFromRoot, DO_SORT, ONLY_CAPTURES);
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
        bestHashMove = Move();

        if (ttMove != Move())
        	moveListGenerator.moveListSize[depthFromRoot] = 1;

        bool searchedTTmove = false;

        for (int currentMove = 0; currentMove < moveListGenerator.moveListSize[depthFromRoot]; currentMove++) {
            Move move = moveListGenerator.moveList[depthFromRoot][currentMove];

            if(ttMove != Move() && !searchedTTmove) {
            	move = ttMove;
            	searchedTTmove = true;
            }


            int seeEval = moveListGenerator.seeTable[move.getStartSquare()][move.getTargetSquare()];
        	if(move == ttMove)
        		seeEval = moveGenerator.sseEval(board, move.getTargetSquare(), color, move.getStartSquare());

        	if (staticEval + 100 < alpha && seeEval <= 0)
        		continue;

            bool castlingWhiteQueensideBroke = board.castlingWhiteQueensideBroke;
		    bool castlingWhiteKingsideBroke = board.castlingWhiteKingsideBroke;
		    bool castlingBlackQueensideBroke = board.castlingBlackQueensideBroke;
		    bool castlingBlackKingsideBroke = board.castlingBlackKingsideBroke;

		    char enPassantColumn = board.enPassantColumn;

		    ull zobristKey = board.zobristKey;

            board.calculateZobristAfterMove(move);

            ull newKey = board.getZobristKey();
            transpositionTable.prefetch(newKey);
            evaluationTranspositionTable.prefetch(newKey);

            board.boardColor = color;

		    board.castlingWhiteQueensideBroke = castlingWhiteQueensideBroke;
		    board.castlingWhiteKingsideBroke = castlingWhiteKingsideBroke;
		    board.castlingBlackQueensideBroke = castlingBlackQueensideBroke;
		    board.castlingBlackKingsideBroke = castlingBlackKingsideBroke;

		    board.enPassantColumn = enPassantColumn;

		    board.zobristKey = zobristKey;


            board.makeMove(move, nnueEvaluator);


            // transpositionTableQuiescent.prefetch(board.getZobristKey());

            int score = -quiescentSearch<nodePvType>(board, (color == WHITE) ? BLACK : WHITE, -beta, -alpha, depthFromRoot + 1);

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
            if (maxEvaluation < score) {
                if (score > alpha)
                    type = EXACT;
                maxEvaluation = score;
                bestHashMove = move;
                if (alpha < score)
                    alpha = score;
                if (alpha >= beta) {
                    transpositionTableQuiescent.write(board, currentZobristKey, maxEvaluation, 0, LOWER_BOUND,
                                                      boardCurrentAge, bestHashMove, depthFromRoot);
                    return maxEvaluation;
                }
            }

            if (move == ttMove) {
		        moveListGenerator.hashMove = ttMove;

            	moveListGenerator.generateMoves(board, historyHelper, color, depthFromRoot, DO_SORT, ONLY_CAPTURES);
            }
        }
        transpositionTableQuiescent.write(board, currentZobristKey, maxEvaluation, 0, type, boardCurrentAge,
                                          bestHashMove, depthFromRoot);
        return maxEvaluation;
    }

#define killMovesNumber 2
    Move killerMovesTable[maxDepth][killMovesNumber];
    int killerMovesCount[maxDepth][killMovesNumber];
    int killerMovesAge[maxDepth][killMovesNumber];

    int staticEvaluationHistory[maxDepth];

	template<NodeType nodePvType>
    int search(Board &board, int color, int depth, int isRoot, int alpha, int beta, int depthFromRoot, int extended) {
    	
        constexpr bool isPvNode = nodePvType != NonPV;

    	if (isPvNode)
    		seldepth = max(seldepth, depthFromRoot + 1);

    	if ((nodes & 1023) == 0) {
        	std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
            ll timeThinked = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - searchStartTime).count();
            if (timeThinked >= hardTimeBound)
            	stopSearch = true;
	        if (stopSearch || nodes >= nodesLim) {
	            stopSearch = true;
	            return 0;
	        }
	    }

	    if (!isRoot) {
	    	alpha = max(alpha, -MATE_SCORE + depthFromRoot);
	    	beta = min(beta, MATE_SCORE - depthFromRoot);
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
            return evaluator.evaluateStalledPosition(board, color, depthFromRoot);

        int staticEval = evaluator.evaluatePosition(board, color, nnueEvaluator, corrhistHelper);
        // cout<<board.generateFEN()<<' '<<staticEval<<'\n';
        bool improving = false;
        bool isMovingSideInCheck = moveGenerator.isInCheck(board, color);

        if (isMovingSideInCheck)
            staticEvaluationHistory[depthFromRoot] = NONE_SCORE;
        else {
            staticEvaluationHistory[depthFromRoot] = staticEval;
            if (depthFromRoot >= 2) {
                int previousEval = staticEvaluationHistory[depthFromRoot - 2];
                if (previousEval == NONE_SCORE || previousEval < staticEval)
                    improving = true;
            }
        }

        auto [hashTableEvaluation, bestHashMove] = transpositionTable.get(board, currentZobristKey, depth, alpha, beta, depthFromRoot);
        if (!moveGenerator.isMoveLegal(board, bestHashMove))
        	bestHashMove = Move();

        if (hashTableEvaluation != NO_EVAL) {
            if (!isRoot && !isPvNode && !searchStack[depthFromRoot].excludeTTmove)
                return hashTableEvaluation;

            staticEval = hashTableEvaluation;
        }
        auto ttEntry = transpositionTable.getEntry(board, currentZobristKey, depthFromRoot);
        if (ttEntry.evaluation != NO_EVAL)
            staticEval = ttEntry.evaluation;

        bool isMateScores = (abs(alpha) >= MATE_SCORE_MAX_PLY ||
					    	 abs(beta) >= MATE_SCORE_MAX_PLY ||
					    	 abs(staticEval) >= MATE_SCORE_MAX_PLY);

        // occuredPositions[board.age]=currentZobristKey;

        int nodeType = transpositionTable.getNodeType(currentZobristKey);

        // Reverse futility pruning
        if (!isRoot &&
        	!isMovingSideInCheck &&
        	ttEntry.evaluation == NO_EVAL &&
        	!isPvNode &&
            !searchStack[depthFromRoot].excludeTTmove &&
            !isMateScores) {

            int margin = (50 - improving * 30) * max(depth, 1) * max(depth, 1);

            if (staticEval >= beta + margin)
                return (staticEval + beta) / 2;
        }

        if (depth <= 0){
        	nodes--;
            return quiescentSearch<nodePvType>(board, color, alpha, beta, depthFromRoot);
        }

        int oppositeColor = (color == WHITE) ? BLACK : WHITE;

        // Null move pruning
        if (!isRoot &&
        	!isMovingSideInCheck && // position not in check
            ((board.whitePieces | board.blackPieces) ^ (board.pawns | board.kings)) >
                0 &&              // pieces except kings and pawns exist (to prevent zugzwang)
            staticEval >= beta && // static evaluation >= beta
            !isPvNode &&
            !searchStack[depthFromRoot].excludeTTmove &&
            !isMateScores) {

            int R = floor(4 +
            	depth / 5.0 +
            	min((staticEval - beta) / 200.0, 5.0));

            int prevEnPassColumn = board.makeNullMove();
            int score = -search<NonPV>(board, oppositeColor, depth - 1 - R, 0, -beta, -beta + 1, depthFromRoot + 1, extended);
            board.makeNullMove();
            board.enPassantColumn = prevEnPassColumn;
            if (score >= beta)
                return score;
        }

        if (!isRoot && !isPvNode && !isMovingSideInCheck &&
        	!searchStack[depthFromRoot].excludeTTmove && !isMateScores) { // Razoring
            int margin = 150 * depth * depth + 200;

            if (staticEval + margin < alpha) {
                int qEval = quiescentSearch<NonPV>(board, color, alpha - 1, alpha, depthFromRoot + 1);

                if (depth == 1 ||
                	(depth <= 2 && ttEntry.evaluation != NO_EVAL && ttEntry.evaluation < alpha - margin - 50))
                	return qEval;

                if (qEval < alpha)
                    return qEval;

                if (depth > 1 && depth <= 3 && qEval > beta + 200)
                	depth--;
            }
        }

        Board boardCopy = board;

        moveListGenerator.hashMove = bestHashMove;
        if (isRoot && depth > 1)
            moveListGenerator.hashMove = bestMove;
        Move ttMove = moveListGenerator.hashMove;


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
        	!searchStack[depthFromRoot].excludeTTmove &&
        	!isMateScores) {

        	int probcutBeta = beta + 200;

        	if (ttEntry.evaluation == NO_EVAL || ttEntry.evaluation >= probcutBeta || ttEntry.depth < depth - probcutDepthR) {

	        	moveListGenerator.generateMoves(board, historyHelper, color, depthFromRoot, DO_SORT, ONLY_CAPTURES);

		        for (int currentMove = 0; currentMove < moveListGenerator.moveListSize[depthFromRoot]; currentMove++) {
		            Move move = moveListGenerator.moveList[depthFromRoot][currentMove];

		            board.makeMove(move, nnueEvaluator);

		            int score = -quiescentSearch<NonPV>(board, oppositeColor, -probcutBeta, -probcutBeta + 1, depthFromRoot + 1);

		            if (score >= probcutBeta)
		            	score = -search<NonPV>(board, oppositeColor, depth - probcutDepthR, 0, -probcutBeta, -probcutBeta + 1,
	                                    depthFromRoot + 1, extended);

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
	                    transpositionTable.write(board, currentZobristKey, score, depth - probcutDepthR, LOWER_BOUND,
	                                             boardCurrentAge, move, depthFromRoot);
	                    return score;
		            }
		        }
		    }
        }

        // int bestKillerMove=0;
        // for(int i=0;i<killMovesNumber;i++)
        // 	if(killerMovesAge[depthFromRoot][i]>killerMovesAge[depthFromRoot][bestKillerMove]&&killerMovesCount[depthFromRoot][i]>1)
        // 		bestKillerMove=i;

        int killerMove = -1, killerBackup = -1;

        for (int i = 0; i < killMovesNumber; i++) {
            if (killerMove == -1 || killerMovesAge[depthFromRoot][i] > killerMovesAge[depthFromRoot][killerMove]) {
                killerBackup = killerMove;
                killerMove = i;
            } else if (killerBackup == -1 ||
                       killerMovesAge[depthFromRoot][i] > killerMovesAge[depthFromRoot][killerBackup])
                killerBackup = i;
        }
        moveListGenerator.killerMove = killerMovesTable[depthFromRoot][killerMove];
        moveListGenerator.killerBackup = killerMovesTable[depthFromRoot][killerBackup];
        // cout<<board.generateFEN()<<' '<<moveListGenerator.killerMove.convertToUCI()<<'
        // '<<moveListGenerator.killerBackup.convertToUCI()<<endl;

        // if(
        // 	depth>=6 &&
        // 	isPvNode &&
        // 	ttMove==Move()){

        // 	search(board,color,depth/2,0,alpha,beta,depthFromRoot+1);
        // 	moveListGenerator.hashMove=searchStack[depthFromRoot+1].bestMove;
        // }

        if (depth >= 6 && isPvNode && nodeType == NONE) {

            depth--;
        }

        Bitboard whiteAttacks = moveGenerator.computeAttackBitboardsW(board);
        Bitboard blackAttacks = moveGenerator.computeAttackBitboardsB(board);

        bool doTTmoveBeforeMovegen = true;

        if(ttMove == Move() || searchStack[depthFromRoot].excludeTTmove){
        	doTTmoveBeforeMovegen = false;
        	historyHelper.whiteAttacks = whiteAttacks;
        	historyHelper.blackAttacks = blackAttacks;
        	moveListGenerator.generateMoves(board, historyHelper, color, depthFromRoot, DO_SORT, ALL_MOVES);
        	if (moveListGenerator.moveListSize[depthFromRoot] == 0)
        		return evaluator.evaluateStalledPosition(board, color, depthFromRoot);
        } else {
        	moveListGenerator.moveListSize[depthFromRoot] = 1;
        }

        int extendTTmove = 0;
        if (
        	extended <= 30 &&
        	depthFromRoot < maxDepth - 10 &&
        	ttMove != Move() &&
        	depth >= 7 &&
        	ttEntry.depth >= depth - 3 &&
        	!searchStack[depthFromRoot].excludeTTmove &&
        	nodeType != UPPER_BOUND &&
        	abs(MATE_SCORE) - abs(ttEntry.evaluation) > maxDepth
        	){

        	searchStack[depthFromRoot + 1].excludeTTmove = true;
        	searchStack[depthFromRoot + 1].excludeMove = ttMove;
        	int singularBeta = ttEntry.evaluation - depth;
        	int singularScore = search<nodePvType>(board, color, depth / 2, 0, singularBeta - 1, singularBeta, depthFromRoot + 1, extended);

        	searchStack[depthFromRoot + 1].excludeTTmove = false;

        	if (singularScore < singularBeta){

        		singularExtended++;

        		extendTTmove = 1;
        		if (!isPvNode && singularScore < singularBeta - 30)
        			extendTTmove++;

        		// cout<<board.generateFEN()<<' '<<ttMove.convertToUCI()<<' '<<ttEntry.evaluation<<' '<<singularScore<<' '<<int(ttEntry.depth)<<'\n';
        	} else if (singularScore >= beta && MATE_SCORE - abs(singularScore) > maxDepth)
        		return beta;
        }

        int maxEvaluation = -inf;
        char type = UPPER_BOUND;
        int movesSearched = 0;
        int quietMovesSearched = 0;
        bestHashMove = Move();
        int numberOfMoves = moveListGenerator.moveListSize[depthFromRoot];

        bool isTTCapture = (ttMove != Move() && !board.isQuietMove(ttMove));

        bool searchedTTmove = false;

        if (ttMove != Move() && doTTmoveBeforeMovegen) {
        	moveListGenerator.moveListSize[depthFromRoot] = 1;
        }

        for (int currentMove = 0; currentMove < moveListGenerator.moveListSize[depthFromRoot]; currentMove++) {
            Move move = moveListGenerator.moveList[depthFromRoot][currentMove];

            if (move == searchStack[depthFromRoot].excludeMove) {
                if (searchStack[depthFromRoot].excludeTTmove)
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
        		move == killerMovesTable[depthFromRoot][killerMove] ||
        		move == killerMovesTable[depthFromRoot][killerBackup]);

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
            				   maxEvaluation <= -MATE_SCORE_MAX_PLY ||
            				   isMateScores);

            if (!beingMated && !isRoot && !isPvNode && movesSearched > 3 + depth * depth * 3 && !isMovingSideInCheck && !isMoveInteresting && historyValue < 0) {
            	continue;
            }
            
            if (!beingMated && !isRoot && !isPvNode && movesSearched > 0 && !isMovingSideInCheck && !isMoveInteresting && historyValue < -100 * depth * depth) {
            	continue;
            }

            int premovefutilityMargin = max((150 + historyValueF * 75 - isTTCapture * 100), float(0)) * depth * depth;
            if (!isRoot && movesSearched > 0 && !isMovingSideInCheck && staticEval < alpha - premovefutilityMargin &&
                !isMoveInteresting && !beingMated &&
                !searchStack[depthFromRoot].excludeTTmove
            ) {

                continue;
            }

            int seeMargin[4] = {0, 200, 400, 900};

            if (!beingMated && !isRoot && movesSearched > 0 && !isPvNode && !isMovingSideInCheck && !inCheck && depth <= 3 &&
                sseEval <= -seeMargin[depth] && !searchStack[depthFromRoot].excludeTTmove) {

                continue;
            }

		    bool castlingWhiteQueensideBroke = board.castlingWhiteQueensideBroke;
		    bool castlingWhiteKingsideBroke = board.castlingWhiteKingsideBroke;
		    bool castlingBlackQueensideBroke = board.castlingBlackQueensideBroke;
		    bool castlingBlackKingsideBroke = board.castlingBlackKingsideBroke;

		    char enPassantColumn = board.enPassantColumn;

		    ull zobristKey = board.zobristKey;

            board.calculateZobristAfterMove(move);

            ull newKey = board.getZobristKey();
            transpositionTable.prefetch(newKey);
            evaluationTranspositionTable.prefetch(newKey);

            board.boardColor = color;

		    board.castlingWhiteQueensideBroke = castlingWhiteQueensideBroke;
		    board.castlingWhiteKingsideBroke = castlingWhiteKingsideBroke;
		    board.castlingBlackQueensideBroke = castlingBlackQueensideBroke;
		    board.castlingBlackKingsideBroke = castlingBlackKingsideBroke;

		    board.enPassantColumn = enPassantColumn;

		    board.zobristKey = zobristKey;


            occuredPositionsHelper.occuredPositions[board.age + 1] = newKey;

            board.makeMove(move, nnueEvaluator);

            // transpositionTable.prefetch(board.getZobristKey());

            int newStaticEval = -evaluator.evaluatePosition(board, oppositeColor, nnueEvaluator, corrhistHelper);

            // cout<<move.convertToUCI()<<' '<<newStaticEval<<'\n';


            int prevNodes = nodes;

            int score;
            if (movesSearched) { // Principal variation search

                // Late move reduction
                const int LMR_FULL_MOVES = 2; // number of moves to search with full depth
                const int LMR_MIN_DEPTH = 3;  // don't reduct depth if it's more or equal to this value
                int LMR_DEPTH_REDUCTION =
                    floor(lmrLogTable[depth][movesSearched] + 0.5 -
                          1 * (isPvNode)-1.5 * float(historyValue) / historyHelper.maxHistoryScore +
                          0.5 * (!improving) + (isTTCapture) * 1 - (isCapture) * 1 - sseEval * (0.002) -
                          1 * (isKiller)); // reduction of depth

                if (LMR_DEPTH_REDUCTION < 0)
                    LMR_DEPTH_REDUCTION = 0;

                LMR_DEPTH_REDUCTION = min(LMR_DEPTH_REDUCTION, depth - 1);


                bool doLMRcapture = true;
                if (inCheck)
                	doLMRcapture = false;

                if (movesSearched >= LMR_FULL_MOVES && !isMovingSideInCheck && depth >= LMR_MIN_DEPTH &&
                    doLMRcapture // don't do LMR with interesting moves
                    // historyHelper.getScore(color,move)<historyHelper.maxHistoryScore // history score is negative
                ) {
                    score = -search<NonPV>(board, oppositeColor, depth - 1 - LMR_DEPTH_REDUCTION, 0, -(alpha + 1), -alpha,
                                    depthFromRoot + 1, extended);
                } else
                    score = alpha + 1; // if LMR is restricted, do this to do PVS

                if (score > alpha) {
                    score = -search<NonPV>(board, oppositeColor, depth - 1 + extendDepth, 0, -(alpha + 1), -alpha,
                                    depthFromRoot + 1, extended + extendDepth);
                    if (isPvNode && score > alpha && score < beta)
                        score =
                            -search<nodePvType>(board, oppositeColor, depth - 1 + extendDepth, 0, -beta, -alpha, depthFromRoot + 1, extended + extendDepth);
                }
            } else
                score = -search<nodePvType>(board, oppositeColor, depth - 1 + extendDepth, 0, -beta, -alpha, depthFromRoot + 1, extended + extendDepth);

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

            if (maxEvaluation < score) {
                if (score > alpha)
                    type = EXACT;
                maxEvaluation = score;
                bestHashMove = move;
                searchStack[depthFromRoot].bestMove = move;
                if (isRoot) {
                    bestMove = move;
                    rootScore = score;
                }
                if (alpha < score)
                    alpha = score;
                if (alpha >= beta) {
                    if (board.isQuietMove(move)) {
                        // update killer move
                        bool killerStored = false; // flag "is killer move stored"
                        int badKiller = -1;        // if some killer move has only one storing, it can be replaced
                        for (int i = 0; i < killMovesNumber; i++) {
                            if (killerMovesTable[depthFromRoot][i] == move) {
                                killerMovesCount[depthFromRoot][i]++;
                                killerMovesAge[depthFromRoot][i] = nodes;
                                killerStored = true;
                                break;
                            }
                            // if(killerMovesCount[depthFromRoot][i]==1)
                            // 	badKiller=i;
                        }
                        if (!killerStored) {
                            for (int i = 0; i < killMovesNumber; i++) {
                                if (killerMovesTable[depthFromRoot][i] == Move()) {
                                    killerMovesTable[depthFromRoot][i] = move;
                                    killerMovesCount[depthFromRoot][i] = 1;
                                    killerMovesAge[depthFromRoot][i] = nodes;
                                    killerStored = true;
                                    break;
                                }
                            }
                        }
                        if (!killerStored) {
                            if (badKiller == -1) {
                                // get oldest killer
                                for (int i = 0; i < killMovesNumber; i++)
                                    if (badKiller == -1 ||
                                        killerMovesAge[depthFromRoot][i] < killerMovesAge[depthFromRoot][badKiller])
                                        badKiller = i;
                            }
                            killerMovesTable[depthFromRoot][badKiller] = move;
                            killerMovesCount[depthFromRoot][badKiller] = 1;
                            killerMovesAge[depthFromRoot][badKiller] = nodes;
                        }
                    }

                    if (!isMovingSideInCheck && (bestHashMove == Move() || board.isQuietMove(bestHashMove))) {
                    	staticEval = evaluator.evaluatePosition(board, color, nnueEvaluator, corrhistHelper);
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
                        Move prevMove = moveListGenerator.moveList[depthFromRoot][previousMoves];
                        historyHelper.update(board, color, prevMove, -maluseBonus);
                    }

                    transpositionTable.write(board, currentZobristKey, maxEvaluation, depth, LOWER_BOUND,
                                             boardCurrentAge, bestHashMove, depthFromRoot);
                    return maxEvaluation;
                }
            }

            if (doTTmoveBeforeMovegen && currentMove == 0) {
	        	historyHelper.whiteAttacks = whiteAttacks;
	        	historyHelper.blackAttacks = blackAttacks;

		        moveListGenerator.hashMove = ttMove;
		        moveListGenerator.killerMove = killerMovesTable[depthFromRoot][killerMove];
		        moveListGenerator.killerBackup = killerMovesTable[depthFromRoot][killerBackup];

            	moveListGenerator.generateMoves(board, historyHelper, color, depthFromRoot, DO_SORT, ALL_MOVES);
            }
        }

        if (type == UPPER_BOUND)
        	bestHashMove = Move();

        if (!isMovingSideInCheck && (bestHashMove == Move() || board.isQuietMove(bestHashMove))) {
        	staticEval = evaluator.evaluatePosition(board, color, nnueEvaluator, corrhistHelper);
        	if (type == EXACT || maxEvaluation < staticEval)
        		corrhistHelper.update(color, board, (maxEvaluation - staticEval) * depth / 8);
        }

        if (maxEvaluation == -inf)
        	maxEvaluation = alpha;

        transpositionTable.write(board, currentZobristKey, maxEvaluation, depth, type, boardCurrentAge, bestHashMove, depthFromRoot);
        return maxEvaluation;
    }

    int startSearch(Board &board, int depth, int alpha, int beta) {
        return search<PV>(board, board.boardColor, depth, true, alpha, beta, 0, 0);
        doneSearch = true;
    }

    Move pvLine[256];
    int pvLineSize;

    void getPvLine(Board &board, int color) {
        ull currentZobristKey = board.getZobristKey();

        if (transpositionTable.getNodeType(currentZobristKey) != EXACT)
            return;

        auto [hashTableEvaluation, bestHashMove] = transpositionTable.get(board, currentZobristKey, 0, 0, 0, 0);

        if (bestHashMove != Move()) {
            pvLine[pvLineSize++] = bestHashMove;
            Board boardCopy = board;
            board.makeMove(bestHashMove);
            getPvLine(board, (color == WHITE) ? BLACK : WHITE);
            board = boardCopy;
        }
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
	        	auto ttEntry = transpositionTable.getEntry(board, board.getZobristKey(), 0);
	        	if (ttEntry.bestMove != Move() && moveGenerator.isMoveLegal(board, ttEntry.bestMove))
	        		bestMove = ttEntry.bestMove;
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
	                cout << "info depth " << depth << " seldepth " << seldepth << " score ";
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
	                cout << " nodes " << totalNodes << " nps " << (totalNodes * (long long)(1000)) / (timeThinked + 1) << " time "
	                     << timeThinked << " pv " << bestMove.convertToUCI() << ' ';
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
                    workers[i].killerMovesTable[j][j1] = Move();
                    workers[i].killerMovesCount[j][j1] = 0;
                    workers[i].killerMovesAge[j][j1] = 0;
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