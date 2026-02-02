#ifndef SEARCHER
#define SEARCHER

#include "searcher.h"

#endif /* SEARCHER */

struct DataGenerator {
    string outputPath = "/Users/Apple/Desktop/projects/chessEngv2/NNUE/datagen.txt";
    string outputPathBin = "/Users/Apple/Desktop/projects/chessEngv2/NNUE/data.vf";
    int softNodesLimit = 5000, hardNodesLimit = 50000;
    int threadNumber = 1;

    int randomMoveCount = 8;

    int resignMoveCount = 3, resignScore = 1000;

    int drawMoveCount = 8, minDrawMoveCount = 34, drawScore = 20;

    vector<Worker> workers;
    vector<Board> boards;

    vector<int> results;
    vector<vector<char>> resultsBin;

    vector<bool> finished;

    inline int mirroredSquare(int square) {
        int row = 7 - (square >> 3);
        int col = (square & 7);
        return (row << 3) + col;
    }

    void playGame(int workerIdx) {
        int curMoveNumber = 0;
        int result = -10;

        int winningStreakW, winningStreakB;
        int drawStreak;

        winningStreakW = winningStreakB = drawStreak = 0;

        int curRandomMoveCount = randomMoveCount;
        if (rng() % 2 == 0)
            curRandomMoveCount++;

        int resultBinPos = -1;

        while (true) {
            curMoveNumber++;
            if (curMoveNumber <= curRandomMoveCount) {

                workers[workerIdx].moveListGenerator.generateMoves(boards[workerIdx], workers[workerIdx].historyHelper,
                                                                   boards[workerIdx].boardColor, 0, DONT_SORT,
                                                                   ALL_MOVES);
                int movesCount = workers[workerIdx].moveListGenerator.moveListSize[0];
                Move randomMove = workers[workerIdx].moveListGenerator.moveList[0][rngT() % movesCount];
                boards[workerIdx].makeMove(randomMove, workers[workerIdx].nnueEvaluator);

                if (curMoveNumber == curRandomMoveCount) {
                    ull board = (boards[workerIdx].whitePieces | boards[workerIdx].blackPieces).bitboard;
                    for (int i = 7; i >= 0; i--)
                        resultsBin[workerIdx].push_back((board & (ull(0b11111111) << (i * 8))) >> (i * 8));

                    char prevPiece = -1;
                    int nmbOfPieces = 0;
                    for (int row = 7; row >= 0; row--)
                        for (int col = 0; col < 8; col++) {
                            int square = row * 8 + col;
                            int color = boards[workerIdx].occupancy(square);
                            int piece = boards[workerIdx].occupancyPiece(square);
                            if (color != EMPTY) {
                                nmbOfPieces++;
                                char code = (piece - 1);
                                if (piece == ROOK) {
                                    if (square == 0 && boards[workerIdx].castlingBlackQueensideBroke == false)
                                        code = 6;
                                    if (square == 7 && boards[workerIdx].castlingBlackKingsideBroke == false)
                                        code = 6;
                                    if (square == 56 && boards[workerIdx].castlingWhiteQueensideBroke == false)
                                        code = 6;
                                    if (square == 63 && boards[workerIdx].castlingWhiteKingsideBroke == false)
                                        code = 6;
                                }
                                if (color == BLACK)
                                    code += (1 << 3);
                                if (prevPiece == -1)
                                    prevPiece = code;
                                else {
                                    code = (prevPiece) + (code << 4);
                                    resultsBin[workerIdx].push_back(code);
                                    prevPiece = -1;
                                }
                            }
                        }
                    while (nmbOfPieces < 32) {
                        nmbOfPieces++;
                        int code = 0;
                        if (prevPiece == -1)
                            prevPiece = code;
                        else {
                            code = (prevPiece) + (code << 4);
                            resultsBin[workerIdx].push_back(code);
                            prevPiece = -1;
                        }
                    }
                    char enp = 64;
                    if (boards[workerIdx].enPassantColumn != NO_EN_PASSANT) {
                        int col = boards[workerIdx].enPassantColumn;
                        int row = 5;
                        if (boards[workerIdx].boardColor == BLACK)
                            row = 2;
                        enp = row * 8 + col;
                    }
                    if (boards[workerIdx].boardColor == BLACK)
                        enp += (1 << 7);
                    resultsBin[workerIdx].push_back(enp);

                    resultsBin[workerIdx].push_back(0);
                    resultsBin[workerIdx].push_back(0);
                    resultsBin[workerIdx].push_back(0);
                    resultsBin[workerIdx].push_back(0);
                    resultsBin[workerIdx].push_back(0);

                    resultBinPos = resultsBin[workerIdx].size();

                    resultsBin[workerIdx].push_back(0);
                }

            } else {
                results[workerIdx]++;
                workers[workerIdx].IDsearchDatagen(boards[workerIdx], 256, softNodesLimit, hardNodesLimit);
                int score = workers[workerIdx].rootScore;
                if (boards[workerIdx].boardColor == BLACK)
                    score = -score;
                // if(abs(score)<=9000){
                // 	results[workerIdx].push_back(boards[workerIdx].generateFEN()+" | "+to_string(score)+" | ");
                // }

                Move bestMove = workers[workerIdx].bestMove;
                int start = bestMove.getStartSquare();
                int target = bestMove.getTargetSquare();
                int prom = bestMove.getPromotionFlag();

                int type = 0;

                if (prom != 0) {
                    type = 3;
                    prom -= 2;
                }

                if (boards[workerIdx].occupancyPiece(start) == PAWN && (start & 7) != (target & 7) &&
                    boards[workerIdx].occupancyPiece(target) == NOPIECE)
                    type = 1; // en-passant

                if (boards[workerIdx].occupancyPiece(start) == KING && abs((start & 7) - (target & 7)) >= 2) {
                    type = 2; // castling
                    if (target == 2)
                        target = 0;
                    if (target == 6)
                        target = 7;
                    if (target == 58)
                        target = 56;
                    if (target == 62)
                        target = 63;
                }

                int moveCode = Move(mirroredSquare(start), mirroredSquare(target), prom).move;
                moveCode += (type << 14);
                resultsBin[workerIdx].push_back(moveCode & 255);
                resultsBin[workerIdx].push_back((moveCode >> 8) & 255);

                if (score > 32767)
                    score = 32767;
                if (score < -32768)
                    score = -32768;
                resultsBin[workerIdx].push_back(score & 255);
                resultsBin[workerIdx].push_back((score >> 8) & 255);

                boards[workerIdx].makeMove(workers[workerIdx].bestMove, workers[workerIdx].nnueEvaluator);

                if (score >= resignScore)
                    winningStreakW++;
                else
                    winningStreakW = 0;

                if (score <= -resignScore)
                    winningStreakB++;
                else
                    winningStreakB = 0;

                if (abs(score) <= drawScore)
                    drawStreak++;
                else
                    drawStreak = 0;
            }
            // cout<<curMoveNumber<<'\n';

            if (workers[workerIdx].moveListGenerator.isStalled(boards[workerIdx], boards[workerIdx].boardColor) ||
                evaluator.insufficientMaterialDraw(boards[workerIdx])) {

                int resultEval = evaluator.evaluateStalledPosition(boards[workerIdx], boards[workerIdx].boardColor, 0);
                if (boards[workerIdx].boardColor == BLACK)
                    resultEval = -resultEval;

                if (resultEval > 0)
                    result = 1;
                else if (resultEval < 0)
                    result = -1;
                else
                    result = 0;
                break;
            }

            int moves50 = (boards[workerIdx].age - boards[workerIdx].lastIrreversibleMoveAge - 1);
            if (moves50 >= 50) {
                result = 0;
                break;
            }

            if (winningStreakW >= resignMoveCount) {
                result = 1;
                break;
            }

            if (winningStreakB >= resignMoveCount) {
                result = -1;
                break;
            }

            if (drawStreak >= drawMoveCount && curMoveNumber >= minDrawMoveCount) {
                result = 0;
                break;
            }

            if (finished[workerIdx] == true)
                break;
        }

        string resultStr;
        if (result == 1)
            resultStr = "1.0";
        if (result == 0)
            resultStr = "0.5";
        if (result == -1)
            resultStr = "0.0";

        resultsBin[workerIdx].push_back(0);
        resultsBin[workerIdx].push_back(0);
        resultsBin[workerIdx].push_back(0);
        resultsBin[workerIdx].push_back(0);

        if (resultBinPos >= 0)
            resultsBin[workerIdx].insert(resultsBin[workerIdx].begin() + resultBinPos, char(result + 1));
        else
            resultsBin[workerIdx].clear();

        // for(auto &str:results[workerIdx])
        // 	str+=resultStr;
        finished[workerIdx] = true;
    }

    void generateData(int numberOfGames) {
        alwaysReplace = true;
        workers.resize(threadNumber);
        boards.resize(threadNumber, mainBoard);
        results.resize(threadNumber, 0);
        resultsBin.resize(threadNumber);
        finished.resize(threadNumber, false);
        vector<thread> threadPool(threadNumber);

        for (int i = 0; i < threadNumber; i++) {
            workers[i].nnueEvaluator = mainNnueEvaluator;
            mainBoard.initNNUE(workers[i].nnueEvaluator);
            for (ll j = 0; j < 256; j++) {
                for (ll j1 = 0; j1 < 2; j1++) {
                    workers[i].killers[j][j1] = Move();
                    workers[i].killersAge[j][j1] = 0;
                }
            }
        }

        // ofstream out(outputPath);
        ofstream outBin(outputPathBin, ios::binary);

        // for(int i=0;i<threadNumber;i++)
        // 	threadPool[i]=thread(&DataGenerator::playGame,this,i);

        long long curGame = 0, positionsNumber = 0;

        std::chrono::steady_clock::time_point timeStart = std::chrono::steady_clock::now();

        cout << endl;

        while (true) {
            int finishedThread = 0;
            playGame(finishedThread);
            // for(int i=0;i<threadNumber;i++)
            // 	if(finished[i]){
            // 		finishedThread=i;
            // 		break;
            // 	}
            // if(finishedThread==-1){
            // 	std::this_thread::sleep_for(std::chrono::microseconds(100));
            // 	continue;
            // }
            curGame++;
            // threadPool[finishedThread].join();
            positionsNumber += results[finishedThread];

            // for(auto str:results[finishedThread])
            // 	out<<str<<'\n';

            for (auto bt : resultsBin[finishedThread]) {
                // for(int j=0;j<8;j++)
                // 	out<<((bt&(1<<j))!=0);
                // out<<' ';
                outBin.write(reinterpret_cast<char *>(&bt), sizeof(bt));
            }

            boards[finishedThread] = mainBoard;
            results[finishedThread] = 0;
            resultsBin[finishedThread] = vector<char>();
            finished[finishedThread] = false;

            workers[finishedThread].nnueEvaluator = mainNnueEvaluator;
            mainBoard.initNNUE(workers[finishedThread].nnueEvaluator);

            std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
            long long elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - timeStart).count();

            if (curGame > 1) {
                cout << "\033[2K";
                cout << "\033[1A\r";
                cout << "\033[2K";
                cout << "\033[1A\r";
                cout << "\033[2K";
                cout << "\033[1A\r";
                cout << "\033[2K";
                cout << "\033[1A\r";
                cout << "\033[2K";
                cout << "\033[1A\r";
                cout << "\033[2K";
                cout << "\033[1A\r";
                cout << "\033[2K";
                cout.flush();
            }

            cout << "Games played: " << curGame << endl;
            cout << "Positions saved: " << positionsNumber << endl;
            cout << "Positions per game: " << positionsNumber / curGame << endl;
            long long sec = elapsedTime / 1000;
            long long min = sec / 60;
            sec %= 60;
            long long hrs = min / 60;
            min %= 60;
            cout << "Total time: " << hrs << "h " << min << "m " << sec << "s" << endl;
            cout << "Games per sec: " << fixed << setprecision(1) << curGame * 1000.0 / elapsedTime << endl;
            cout << "Positions per sec: " << positionsNumber * 1000 / elapsedTime << endl;
            if (curGame == numberOfGames) {
                // for(int i=0;i<threadNumber;i++)
                // 	if(i!=finishedThread){
                // 		finished[i]=true;
                // 		threadPool[i].join();
                // 	}

                cout << "Done!\n" << endl;
                break;
            }
            // threadPool[finishedThread]=thread(&DataGenerator::playGame,this,finishedThread);
        }
        alwaysReplace = false;
    }
};

DataGenerator dataGenerator;