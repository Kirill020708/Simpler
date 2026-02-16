#ifndef SEARCHER
#define SEARCHER

#include "searcher.h"

#endif /* SEARCHER */

#include <filesystem>

struct DataGenerator {
    string outputDir = "";
    int workerId;
    int seed = 0;

    int softNodesLimit = 5000, hardNodesLimit = 50000;
    int threadNumber = 1;

    int randomMoveCount = 8;

    int resignMoveCount = 3, resignScore = 1000;

    int drawMoveCount = 8, minDrawMoveCount = 34, drawScore = 20;

    int positionsNumber = 0;

    inline int mirroredSquare(int square) {
        int row = 7 - (square >> 3);
        int col = (square & 7);
        return (row << 3) + col;
    }

    vector<char> resultsBin;

    void playGame() {
        resultsBin.clear();

        int curMoveNumber = 0;
        int result = -10;

        int winningStreakW, winningStreakB;
        int drawStreak;

        winningStreakW = winningStreakB = drawStreak = 0;

        int curRandomMoveCount = randomMoveCount;
        if (rng() % 2 == 0)
            curRandomMoveCount++;

        int resultBinPos = -1;

        mainBoard = Board();

        MoveListGenerator moveListGenerator;

        bool firstPos = true;

        while (true) {
            curMoveNumber++;
            if (curMoveNumber <= curRandomMoveCount) {

                moveListGenerator.generateMoves(mainBoard, searcher.workers[0].historyHelper,
                                                                   mainBoard.boardColor, 0, DONT_SORT,
                                                                   ALL_MOVES);
                int movesCount = moveListGenerator.moveListSize[0];
                Move randomMove = moveListGenerator.moveList[0][rngT() % movesCount];
                mainBoard.makeMove(randomMove);

                if (curMoveNumber == curRandomMoveCount) {
                    ull board = (mainBoard.whitePieces | mainBoard.blackPieces).bitboard;
                    for (int i = 7; i >= 0; i--)
                        resultsBin.push_back((board & (ull(0b11111111) << (i * 8))) >> (i * 8));

                    char prevPiece = -1;
                    int nmbOfPieces = 0;
                    for (int row = 7; row >= 0; row--)
                        for (int col = 0; col < 8; col++) {
                            int square = row * 8 + col;
                            int color = mainBoard.occupancy(square);
                            int piece = mainBoard.occupancyPiece(square);
                            if (color != EMPTY) {
                                nmbOfPieces++;
                                char code = (piece - 1);
                                if (piece == ROOK) {
                                    if (square == 0 && mainBoard.castlingBlackQueensideBroke == false)
                                        code = 6;
                                    if (square == 7 && mainBoard.castlingBlackKingsideBroke == false)
                                        code = 6;
                                    if (square == 56 && mainBoard.castlingWhiteQueensideBroke == false)
                                        code = 6;
                                    if (square == 63 && mainBoard.castlingWhiteKingsideBroke == false)
                                        code = 6;
                                }
                                if (color == BLACK)
                                    code += (1 << 3);
                                if (prevPiece == -1)
                                    prevPiece = code;
                                else {
                                    code = (prevPiece) + (code << 4);
                                    resultsBin.push_back(code);
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
                            resultsBin.push_back(code);
                            prevPiece = -1;
                        }
                    }
                    char enp = 64;
                    if (mainBoard.enPassantColumn != NO_EN_PASSANT) {
                        int col = mainBoard.enPassantColumn;
                        int row = 5;
                        if (mainBoard.boardColor == BLACK)
                            row = 2;
                        enp = row * 8 + col;
                    }
                    if (mainBoard.boardColor == BLACK)
                        enp += (1 << 7);
                    resultsBin.push_back(enp);

                    resultsBin.push_back(0);
                    resultsBin.push_back(0);
                    resultsBin.push_back(0);
                    resultsBin.push_back(0);
                    resultsBin.push_back(0);

                    resultBinPos = resultsBin.size();

                    resultsBin.push_back(0);
                }

            } else {
                positionsNumber++;
                searcher.datagenSearch(256, softNodesLimit, hardNodesLimit);
                int score = searcher.workers[0].rootScore;

                if (firstPos && score >= 1000) {
                    resultsBin.clear();
                    resultBinPos = -10;
                    break;
                }
                firstPos = false;

                if (mainBoard.boardColor == BLACK)
                    score = -score;
                // if(abs(score)<=9000){
                // 	results[0].push_back(mainBoard.generateFEN()+" | "+to_string(score)+" | ");
                // }

                Move bestMove = searcher.workers[0].bestMove;
                int start = bestMove.getStartSquare();
                int target = bestMove.getTargetSquare();
                int prom = bestMove.getPromotionFlag();

                int type = 0;

                if (prom != 0) {
                    type = 3;
                    prom -= 2;
                }

                if (mainBoard.occupancyPiece(start) == PAWN && (start & 7) != (target & 7) &&
                    mainBoard.occupancyPiece(target) == NOPIECE)
                    type = 1; // en-passant

                if (mainBoard.occupancyPiece(start) == KING && abs((start & 7) - (target & 7)) >= 2) {
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
                resultsBin.push_back(moveCode & 255);
                resultsBin.push_back((moveCode >> 8) & 255);

                if (score > 32767)
                    score = 32767;
                if (score < -32768)
                    score = -32768;
                resultsBin.push_back(score & 255);
                resultsBin.push_back((score >> 8) & 255);

                mainBoard.makeMove(searcher.workers[0].bestMove, searcher.workers[0].nnueEvaluator);

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

            if (moveListGenerator.isStalled(mainBoard, mainBoard.boardColor) ||
                evaluator.insufficientMaterialDraw(mainBoard)) {

                int resultEval = evaluator.evaluateStalledPosition(mainBoard, mainBoard.boardColor, 0);
                if (mainBoard.boardColor == BLACK)
                    resultEval = -resultEval;

                if (resultEval > 0)
                    result = 1;
                else if (resultEval < 0)
                    result = -1;
                else
                    result = 0;
                break;
            }

            int moves50 = (mainBoard.age - mainBoard.lastIrreversibleMoveAge - 1);
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
        }

        string resultStr;
        if (result == 1)
            resultStr = "1.0";
        if (result == 0)
            resultStr = "0.5";
        if (result == -1)
            resultStr = "0.0";

        resultsBin.push_back(0);
        resultsBin.push_back(0);
        resultsBin.push_back(0);
        resultsBin.push_back(0);

        if (resultBinPos >= 0)
            resultsBin.insert(resultsBin.begin() + resultBinPos, char(result + 1));
        else
            resultsBin.clear();

        // for(auto &str:results[0])
        // 	str+=resultStr;
    }

    void generateData(int numberOfGames) {

        string outputPathBin = outputDir + "/datagenFiles/data/data" + to_string(workerId) + ".vf";
        string outputPathStats = outputDir + "/datagenFiles/stats/stat" + to_string(workerId) + ".txt";
        // searcher.doInfoOutput = false;

        // ofstream out(outputPath);
        ofstream outBin(outputPathBin, ios::binary);

        // for(int i=0;i<threadNumber;i++)
        // 	threadPool[i]=thread(&DataGenerator::playGame,this,i);

        long long curGame = 0;

        std::chrono::steady_clock::time_point timeStart = std::chrono::steady_clock::now();


        while (true) {
            playGame();
            curGame++;

            for (auto bt : resultsBin) {
                outBin.write(reinterpret_cast<char *>(&bt), sizeof(bt));
            }

            std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
            long long elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - timeStart).count();

            ofstream out(outputPathStats);

            out << "seed: " << seed << endl;
            out << "games_number: " << curGame << endl;
            out << "positions_saved: " << positionsNumber << endl;
            out << "positions_per_game: " << positionsNumber / curGame << endl;
            long long sec = elapsedTime / 1000;
            out << "total_time: " << sec << endl;
            out << "games_per_sec: " << fixed << setprecision(1) << curGame * 1000.0 / elapsedTime << endl;
            out << "positions_per_sec: " << positionsNumber * 1000 / elapsedTime << endl;

            if (filesystem::exists(outputDir + "/datagenFiles/exit.txt"))
                break;
            if (curGame == numberOfGames) {
                // for(int i=0;i<threadNumber;i++)
                // 	if(i!=0){
                // 		finished[i]=true;
                // 		threadPool[i].join();
                // 	}

                // cout << "Done!\n" << endl;
                break;
            }
            // threadPool[0]=thread(&DataGenerator::playGame,this,0);
        }
        exit(0);
    }
};

DataGenerator dataGenerator;