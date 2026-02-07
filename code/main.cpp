#ifndef SEARCHER
#define SEARCHER

#include "searcher.h"

#endif /* SEARCHER */

#ifndef UCI
#define UCI

#include "uciProtocol.h"

#endif /* UCI */

#ifndef PERFT
#define PERFT

#include "perft.h"

#endif /* PERFT */

#ifndef TEXEL
#define TEXEL

#include "texel.h"

#endif /* TEXEL */

#ifndef NNUE
#define NNUE

#include "nnue.h"

#endif /* NNUE */

#ifndef DATAGEN
#define DATAGEN

#include "datagen.h"

#endif /* DATAGEN */

#ifndef BENCH
#define BENCH

#include "bench.h"

#endif /* BENCH */

string squareToString(int square) {
    return string(1, char((square & 7) + 'a')) + string(1, char(7 - (square >> 3) + '1'));
}

int main(int argc, char *argv[]) {
	initLmrTable();
    // texelTuner.checkData();
    // evaluator.writeToFile("/Users/Apple/Desktop/projects/chessEngv2/apps/evalbase.txt");
    // evaluator.initFromFile("/Users/Apple/Desktop/projects/chessEngv2/apps/evalbase.txt");
    // evaluator.initFromFile("/Users/Apple/Desktop/projects/chessEngv2/Simple-chess-engine/code/evaluationWeights.txt");
    string evaluationWeights =
        "mobility mg: 0 0 1.66818 2.58412 3.0677 -1.76086 -11.6306 0 |mobility eg: 0 0 9.26372 12.6373 11.9937 12.1001 "
        "1.48715 0 |king attack mg: 0 8 19.217 20.5776 23.8801 29.4941 20.3715 0 |king attack eg: 0 0 0.165305 1.76775 "
        "0.501133 9.75261 2.38228 0 |doubled mg: 12|doubled eg: 7|isolated mg: 0.746226 13.8841 20.3863 24.1883 "
        "25.1588 21.9609 14.1906 15.8822 |isolated eg: -0.731248 3.71009 14.0723 10.6767 15.1742 10.7892 5.82794 "
        "4.35728 |passed mg: 0 4.17851 6.7106 15.9533 31.5766 57.2377 98.5963 0 |passed eg: 0 12.5789 20.5657 44.247 "
        "71.6173 112.118 183.411 0 |islands mg: 8.69405|islands eg: -11.4291|shield dist: 0 0 0 5 10 15 20 25 |tempo: "
        "1.98761|bishop pair mg: 37.7756|bishop pair eg: 64.6135";
    evaluator.init(splitStr(evaluationWeights, "|"));

    mainNnueEvaluator.initFromFile("/Users/Apple/Desktop/projects/chessEngv2/Simple-chess-engine/code/quantisedv2.bin");
    mainBoard = Board();

    if (argc >= 2) {
    	string args = string(argv[1]);

	    if (args == "bench") {
	        benchmarker.benchmark(8, 10000);
	        exit(0);
	    }

	    if(args.substr(0, 7) == "genfens") {

            uciHelper.reallocateHashMemory(8);
	    	auto tokens = splitStr(args, " ");
	    	int N = stoi(tokens[1]);
	    	int seed = stoi(tokens[3]);
            bool printInfo = 1;
            if (tokens.size() == 5 && tokens[4] == "noprintinfo")
                printInfo = 0;
	    	mt19937_64 rngS(seed);

	    	MoveListGenerator moveListGenerator;
	    	HistoryHelper historyHelper;

            searcher.doInfoOutput = false;

	    	while (N--) {
	    		auto boardCopy = mainBoard;
	    		int nmbOfMoves = 8 + (rngS() % 2);
                bool isOk = 1;

	    		while (nmbOfMoves--) {
	                moveListGenerator.generateMoves(mainBoard, historyHelper,mainBoard.boardColor, 0, DONT_SORT, ALL_MOVES);
	                int movesCount = moveListGenerator.moveListSize[0];
                    if (movesCount == 0) {
                        isOk = 0;
                        break;
                    }
	                Move randomMove = moveListGenerator.moveList[0][rngS() % movesCount];
	                mainBoard.makeMove(randomMove);
	            }

                if (moveListGenerator.isStalled(mainBoard, mainBoard.boardColor))
                    isOk = 0;
                else {
                    searcher.iterativeDeepeningSearch(256, 1e6, 1e6, 500, 500);
                    if (abs(searcher.workers[0].rootScore) >= 500) {
                        searcher.iterativeDeepeningSearch(256, 1e6, 1e6, 5000, 5000);
                        if (abs(searcher.workers[0].rootScore) >= 1000)
                            isOk = 0;
                    }
                }

                if (isOk) {
                    if (printInfo)
	                    cout << "info string genfens ";
                    cout << mainBoard.generateFEN() << endl;
                }
                else
                    N++;
                mainBoard = boardCopy;
	    	}

	    	exit(0);
	    }
    }

    // mainBoard.initNNUE(mainNnueEvaluator);
    // dataGenerator.generateData(1'000'000);
    // mainBoard.initFromFEN("k7/8/8/8/8/8/8/K7 w - - 0 1");
    // cout<<nnueEvaluator.evaluate(WHITE)<<'\n';

    // cout<<'\n';
    // return 0;

    // texelTuner.checkData();
    // texelTuner.train("/Users/Apple/Desktop/projects/chessEngv2/texelBigGoodData.txt");
    // for(ll depth=1;depth<=15;depth++){
    // cout<<depth<<": ";
    // for(ll j=1;j<=40;j++)
    // cout<<int(floor(log(float(depth)) * log(float(j)) / 3))<<' ';
    // cout<<'\n';
    // }
    // cout<<sizeof(Board)<<'\n';
    // board.initFromFEN("4rrk1/1pqb1pb1/p2p3p/4pPp1/2P1B1P1/1P3N1P/P3Q3/1R2K2R w K - 5 22");

    // cout<<pieceSquareTable.getPiecePositionEval(KING,52,WHITE,32-3)<<'\n';
    // cout<<pieceSquareTable.getPiecePositionEval(KING,54,WHITE,32-3)<<'\n';
    // cout<<pieceSquareTable.getPiecePositionEval(KING,52-8,WHITE,32-3)<<'\n';
    // cout<<board.numberOfPieces()<<'\n';

    // moveListGenerator.generateMoves(WHITE,0,1,0);
    // 	for(int currentMove=0;currentMove<moveListGenerator.moveListSize[0];currentMove++){
    // 		Move move=moveListGenerator.moveList[0][currentMove];
    // 		cout<<move.convertToUCI()<<' '<<move.score<<'\n';
    // 		}
    // cout<<board.evaluation<<'\n';
    // board.makeMove(Move("a1d1"));
    // cout<<board.evaluation<<'\n';
    // cout<<pieceSquareTable.getPieceEval(ROOK,56,WHITE)<<' '<<pieceSquareTable.getPieceEval(ROOK,59,WHITE)<<'\n';
    // return 0;
    // board.makeMove(Move("e2e4"));
    // board.makeMove(Move("e7e5"));
    // cout<<board.evaluation<<'\n';
    // cout<<ttt.tableSize<<'\n';
    // perftester.perfTest(256);
    // searcher.iterativeDeepeningSearch(WHITE,100);
    // system("/Users/Apple/Desktop/projects/chesscpp/app");
    // printDesk01(boardHelper.neighborColumns[34].bitboard);
    // printDesk01(boardHelper.possiblePawnDefendersBlack[31]);
    // cout<<boardHelper.distanceColumn(board.pawns&board.whitePieces&boardHelper.getColumn(0),BLACK);
    uciHelper.reallocateHashMemory(256);
    uciHelper.listenInput();
    // 64
}

/*

cd /Users/Apple/Desktop/projects/chessEngv2/apps
c++ ../Simple-chess-engine/code/main.cpp -o appV17_SEE -std=c++2a -O2
c++ main.cpp -o appPVS -std=c++2a -O2

./fastchess -recover -repeat -games 2 -rounds 50000 -ratinginterval 1 -scoreinterval 1 -autosaveinterval 0\
            -report penta=true -pgnout results.pgn\
            -srand 5895699939700649196 -resign movecount=3 score=600\
            -draw movenumber=34 movecount=8 score=20 -variant standard -concurrency 2\
            -openings file=UHO_Lichess_4852_v1.epd format=epd order=random plies=16 start=15619\
            -engine name=New tc=10+0.1 cmd=./Simple-chess-engine/code/appPVS restart=on dir=.\
            -engine name=Base tc=10+0.1 cmd=./Simple-chess-engine/code/app restart=on dir=.\
            -each proto=uci -pgnout result.pgn -sprt elo0=0 elo1=10 alpha=0.05 beta=0.05

go movetime 1000
position startpos moves e2e4 e7e5
go movetime 1000

position fen 5r2/4k2P/R7/8/8/5PK1/8/8 w - - 0 82
go movetime 100

./fastchess -recover -repeat -games 2 -rounds 50000 -ratinginterval 1 -scoreinterval 1 -autosaveinterval 0\
            -report penta=true -pgnout results.pgn\
            -srand 5895699939700649196 -resign movecount=3 score=600\
            -draw movenumber=34 movecount=8 score=20 -variant standard -concurrency 2\
            -openings file=8moves_v3.pgn format=pgn order=random\
            -engine name=New tc=10+0.1 cmd=./apps/appV30_BSHP restart=on dir=.\
            -engine name=Base tc=10+0.1 cmd=./apps/appV29_PST restart=on dir=.\
            -each proto=uci -pgnout result.pgn -sprt elo0=0 elo1=10 alpha=0.05 beta=0.05




*/