// A unit for uci communication

#pragma once

#ifndef SEARCHER
#define SEARCHER

#include "searcher.h"

#endif /* SEARCHER */

#ifndef BOARD
#define BOARD

#include "mainBoard.h"

#endif /* BOARD */

#ifndef EVALUATOR
#define EVALUATOR

#include "evaluation.h"

#endif /* EVALUATOR */

#ifndef PERFT
#define PERFT

#include "perft.h"

#endif /* PERFT */

#ifndef NNUE
#define NNUE

#include "nnue.h"

#endif /* NNUE */

#ifndef DATAGEN
#define DATAGEN

#include "datagen.h"

#endif /* DATAGEN */

void waitAndEndSearch(int timeToThink) {
    // searcher.stopSearch=false;
    // thread th(&Searcher::iterativeDeepeningSearch,&searcher,mainBoard.boardColor,256);
    // this_thread::sleep_for(std::chrono::milliseconds(timeToThink));
    // searcher.stopSearch=true;
    // th.join();
}
// position startpos moves b1c3 b8c6 g1f3 d7d5 e2e3 e7e6 f1b5 c8d7 d2d4 g8f6 b5c6 d7c6 f3e5 f8d6 e5c6 b7c6 e1g1 e8g8
// d1f3 e6e5 c1d2 e5e4 f3f5 d8b8 a1b1 b8c8 f5c8 a8c8 f2f3 c6c5 f3e4 d5e4 c3b5 a7a6 b5d6 c7d6 b1c1 f8e8 f1e1 d6d5 a2a3
// c5c4 d2c3 c8d8 c1d1 d8b8 a3a4 b8d8 c3b4 d8a8 b4c3 a8d8 a4a5 d8a8 d1d2 a8d8 d2f2 d8a8 f2d2

void printDesk01(ull x) {
    for (ll i = 0; i < 64; i++) {
        cout << ((x & (1ull << i)) > 0);
        if (i % 8 == 7)
            cout << '\n';
    }
}
#ifndef PERFT
#define PERFT

#include "perft.h"

#endif /* PERFT */

struct UCIcommunicationHepler {

    thread searcherThread;

    int hardNodesOpt = 1e9;

    void reallocateHashMemory(int MBsize) {
        long long bSize = ll(MBsize) * 1024 * 1024;
        int TTEntrySize = sizeof(TableEntry);

        transpositionTable.table.resize(bSize / TTEntrySize, TableEntry());
        transpositionTable.table.shrink_to_fit();
        transpositionTable.tableSize = transpositionTable.table.size();
    }

    void clearHash() {
        transpositionTable.table = vector<TableEntry>(transpositionTable.table.size(), TableEntry());
    }

    void parseCommand(string command) {
        if (command == "")
            return;
        vector<string> tokens = splitStr(command, " ");
        string mainCommand = tokens[0];
        if (mainCommand == "quit")
            exit(0);
        if (mainCommand == "dbg") {
            // printDesk01(mainBoard.rooks);
            // perftester.perfTest(256);
            // for(ll i=0;i<mainBoard.age;i++)
            // cout<<occuredPositionsHelper.occuredPositions[i]<<'\n';
            // inline int sseEval(int square,int color,int firstAttacker){
            // cout<<evaluator.evaluateDoubledPawns()<<'\n';
            // nnueEvaluator.printAccum();
            // return;
        }
        if (mainCommand == "uci") {
            cout << "id name Simpler 1" << endl;
            cout << "id author Kirill020708\n" << endl;

            cout << "option name HardNodesLimit type spin default 1000000000 min 1 max 1000000000" << endl;
            cout << "option name Normalize type check default true" << endl;
            cout << "option name Threads type spin default 1 min 1 max 1024" << endl;
            cout << "option name Hash type spin default 256 min 1 max 33554432" << endl;
            cout << "option name Minimal type check default false" << endl;
            cout << "option name NNUEpath type string default quantisedv2.bin" << endl;
            cout << "uciok" << endl;
            return;
        }
        if (mainCommand == "isready") {
            cout << "readyok" << endl;
            return;
        }
        if (mainCommand == "makemove") {
            mainOccuredPositionsHelper.occuredPositions[mainBoard.age] = mainBoard.getZobristKey();
            mainBoard.makeMove(Move(tokens[1]));
            return;
        }
        if (mainCommand == "eval") {
            cout << "endgame weight: " << mainBoard.endgameWeight() << '\n';
            if (tokens.size() > 1 && tokens[1] == "info")
                evaluator.showInfo = true;
            evaluator.uciOutput = true;
            cout << evaluator.evaluatePositionDeterministic(mainBoard) << " cp (white's perspective)" << endl;
            mainBoard.initNNUE(mainNnueEvaluator);
            int nnueEval = mainNnueEvaluator.evaluate(mainBoard.boardColor, mainBoard.getOutputBucket());
            if (mainBoard.boardColor == BLACK)
                nnueEval = -nnueEval;
            cout << nnueEval << " cp (NNUE, white's perspective)" << endl;

            char pieceChar[2][7] = {{' ', 'P', 'N', 'B', 'R', 'Q', 'K'}, {' ', 'p', 'n', 'b', 'r', 'q', 'k'}};

            int scaledNNUEeval = normalizeNNUEscore(nnueEval, mainBoard.getNormalizeMaterial());

            cout << scaledNNUEeval << " cp (scaled NNUE, white's perspective)" << endl;

            cout << "+-------+-------+-------+-------+-------+-------+-------+-------+\n";

            for (int row = 0; row < 8; row++) {
                cout << '|';
                for (int col = 0; col < 8; col++) {
                    int square = row * 8 + col;
                    int color = mainBoard.occupancy(square);
                    int piece = mainBoard.occupancyPiece(square);
                    cout << "   ";
                    if (color == EMPTY)
                        cout << ' ';
                    else
                        cout << pieceChar[color][piece];
                    cout << "   |";
                }
                cout << '\n';
                cout << '|';
                for (int col = 0; col < 8; col++) {
                    int square = row * 8 + col;
                    int color = mainBoard.occupancy(square);
                    int piece = mainBoard.occupancyPiece(square);
                    cout << " ";
                    if (color == EMPTY)
                        cout << "     ";
                    else {
                        mainBoard.clearPosition(square, mainNnueEvaluator);
                        int newNnueEval = mainNnueEvaluator.evaluate(mainBoard.boardColor, mainBoard.getOutputBucket());
                        if (mainBoard.boardColor == BLACK)
                            newNnueEval = -newNnueEval;
                        newNnueEval = normalizeNNUEscore(newNnueEval, mainBoard.getNormalizeMaterial());
                        mainBoard.putPiece(square, color, piece, mainNnueEvaluator);
                        int pieceValue = scaledNNUEeval - newNnueEval;
                        cout << intTo5symbFormat(pieceValue);
                    }
                    cout << " |";
                }
                cout << "\n+-------+-------+-------+-------+-------+-------+-------+-------+\n";
            }
            cout << endl;

            evaluator.showInfo = false;
            evaluator.uciOutput = false;
            return;
        }
        if (mainCommand == "position") {
            int movesIter = tokens.size();
            if (tokens[1] == "startpos") {
                movesIter = 3;
                mainBoard = Board();
            }
            if (tokens[1] == "fen") {
                string fen;
                for (int i = 2; i < tokens.size(); i++) {
                    if (tokens[i] == "moves") {
                        movesIter = i + 1;
                        break;
                    }
                    fen += tokens[i] + " ";
                }
                mainBoard.initFromFEN(fen);
            }
            mainOccuredPositionsHelper.occuredPositions[mainBoard.age] = mainBoard.getZobristKey();
            for (; movesIter < tokens.size(); movesIter++) {
                mainBoard.makeMove(Move(tokens[movesIter]));
                mainOccuredPositionsHelper.occuredPositions[mainBoard.age] = mainBoard.getZobristKey();
            }
        }
        if (mainCommand == "go") {
            if (searcherThread.joinable())
                searcherThread.join();
            int wtime = -1, btime = -1, winc = -1, binc = 0;
            int movetime = -1;
            int depth = 256;
            int nodes = 1e9;
            int nodesh = 1e9;
            for (int i = 1; i < tokens.size(); i++) {
                if (tokens[i] == "wtime")
                    wtime = stoi(tokens[i + 1]);
                if (tokens[i] == "btime")
                    btime = stoi(tokens[i + 1]);
                if (tokens[i] == "winc")
                    winc = stoi(tokens[i + 1]);
                if (tokens[i] == "binc")
                    binc = stoi(tokens[i + 1]);

                if (tokens[i] == "movetime")
                    movetime = stoi(tokens[i + 1]);

                if (tokens[i] == "depth")
                    depth = stoi(tokens[i + 1]);

                if (tokens[i] == "nodes")
                    nodes = stoi(tokens[i + 1]);

                if (tokens[i] == "nodesh")
                    nodesh = stoi(tokens[i + 1]);
            }
            int timeToThink = 1e9;
            int basetime = 0;
            bool timeBound = 0;
            if (mainBoard.boardColor == WHITE && wtime != -1) {
                timeToThink = wtime * 0.025 + winc;
                basetime = wtime;
                timeBound = true;
            }
            if (mainBoard.boardColor == BLACK && btime != -1) {
                timeToThink = btime * 0.025 + binc;
                basetime = btime;
                timeBound = true;
            }
            if (movetime != -1) {
                timeToThink = movetime;
                timeBound = true;
            }
            // cout<<timeToThink<<'\n';
            int softBound = inf, hardBound = inf;
            if (timeBound)
                searcher.workers[0].basetime = basetime;
            else
                searcher.workers[0].basetime = 1e9;
            if (wtime != -1) {
                softBound = timeToThink;
                hardBound = max(min(basetime / 2, basetime - 10), 1);
            }
            if (movetime != -1) {
                softBound = hardBound = timeToThink = movetime;
            }
            // searcherThread = thread(&Searcher::iterativeDeepeningSearch, &searcher, depth, softBound, hardBound, nodes, hardNodesOpt);
            searcher.iterativeDeepeningSearch(depth, softBound, hardBound, nodes, hardNodesOpt);
        }
        if (mainCommand == "perft") {
            perftester.perfTest(stoi(tokens[1]));
        }
        if (mainCommand == "stop") {
            searcher.workers[0].stopSearch = true;
            if (searcherThread.joinable())
                searcherThread.join();
        }
        if (mainCommand == "setoption") {
            if (tokens[2] == "HardNodesLimit") {
                hardNodesOpt = stoi(tokens[4]);
            }
            if (tokens[2] == "Normalize") {
                if (tokens[4] == "true")
                    searcher.workers[0].doNormalization = true;
                else
                    searcher.workers[0].doNormalization = false;
            }
            if (tokens[2] == "Minimal") {
                if (tokens[4] == "true")
                    searcher.minimal = true;
                else
                    searcher.minimal = false;
            }
            if (tokens[2] == "Threads") {
                int thn = stoi(tokens[4]);
                searcher.threadNumber = thn;
                searcher.workers.resize(thn);
            }
            if (tokens[2] == "Hash") {
                int sz = stoi(tokens[4]);
                reallocateHashMemory(sz);
            }
            if (tokens[2] == "NNUEpath") {
                mainNnueEvaluator.initFromFile(tokens[4]);
            }
        }
        if (mainCommand == "ucinewgame") {
            clearHash();
        }
        if (mainCommand == "d") {
            cout << mainBoard.generateFEN() << endl;
        }
        if (mainCommand == "datagen") {
            int gamesNumber = 1;
            for (int i = 1; i < tokens.size(); i++) {
                if (tokens[i] == "softnodes")
                    dataGenerator.softNodesLimit = stoi(tokens[i + 1]);
                if (tokens[i] == "hardnodes")
                    dataGenerator.hardNodesLimit = stoi(tokens[i + 1]);
                if (tokens[i] == "threads")
                    dataGenerator.threadNumber = stoi(tokens[i + 1]);
                if (tokens[i] == "file")
                    dataGenerator.outputPathBin = tokens[i + 1];
                if (tokens[i] == "games")
                    gamesNumber = stoi(tokens[i + 1]);
            }
            dataGenerator.generateData(gamesNumber);
        }
    }

    bool stopListening;

    void listenInput() {
        stopListening = 0;
        while (true) {
            string command;
            getline(cin, command);
            parseCommand(command);
        }
    }
};

UCIcommunicationHepler uciHelper;