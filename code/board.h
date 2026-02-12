// struct for board

#pragma once

#ifndef MOVE
#define MOVE

#include "move.h"

#endif /* MOVE */

#ifndef BITBOARD
#define BITBOARD

#include "bitboard.h"

#endif /* BITBOARD */

#ifndef BOARDHELP
#define BOARDHELP

#include "boardHelper.h"

#endif /* BOARDHELP */

#ifndef PIECESQTABLE
#define PIECESQTABLE

#include "pieceSquareTable.h"

#endif /* PIECESQTABLE */

#ifndef ZOBRIST
#define ZOBRIST

#include "zobrist.h"

#endif /* ZOBRIST */

#ifndef NNUE
#define NNUE

#include "nnue.h"

#endif /* NNUE */

struct OccuredPositionsHelper {
    ull occuredPositions[2560]; // positions occured in current variation; for testing repetition
};

OccuredPositionsHelper mainOccuredPositionsHelper;

int material[8] = {0, 0, 3, 3, 5, 10, 0};

struct alignas(64) Board {
    char boardColor;
    int materialCount;
    int evaluation;
    bool doNNUEupdates = false;

    Bitboard whitePieces, blackPieces;
    Bitboard pawns, knights, bishops, rooks, queens, kings;

    bool castlingWhiteQueensideBroke, castlingWhiteKingsideBroke, castlingBlackQueensideBroke,
        castlingBlackKingsideBroke;

    char enPassantColumn;

    ull zobristKey;

    ull zobristKeyPawn;
    ull zobristKeyMinor;

    ull zobristKeyWhite;
    ull zobristKeyBlack;

    int ply1Sq = 0, ply1Ps = 0;
    int ply2Sq = 0, ply2Ps = 0;

    int age = 0;

    int lastIrreversibleMoveAge = -1; // age of last irreversible move (capture/pawn move), for testing repetition

    bool flippedW = true;
    bool flippedB = true;

    inline int numberOfPieces() {
        return (whitePieces | blackPieces).popcnt();
    }

    inline int getOutputBucket() {
        return (numberOfPieces() - 2) / 4;
    }

    inline float endgameWeight() {
        return 1 - float(materialCount) / 64.0;
    }

    inline int occupancy(int square) {
        return WHITE * (whitePieces.getBit(square)) + BLACK * (blackPieces.getBit(square)) +
               EMPTY * (!((whitePieces | blackPieces).getBit(square)));

        if (square < 0 || square > 63)
            return ERROR;
        if (whitePieces.getBit(square))
            return WHITE;
        if (blackPieces.getBit(square))
            return BLACK;
        return EMPTY;
    }

    inline int occupancyPiece(int square) {
        // return ERROR * (square < 0 || square > 63) + PAWN * (pawns.getBit(square)) + KNIGHT * (knights.getBit(square)) +
        //        BISHOP * (bishops.getBit(square)) + ROOK * (rooks.getBit(square)) + QUEEN * (queens.getBit(square)) +
        //        KING * (kings.getBit(square)) + NOPIECE;

        if (square < 0 || square > 63)
            return ERROR;
        if (pawns.getBit(square))
            return PAWN;
        if (knights.getBit(square))
            return KNIGHT;
        if (bishops.getBit(square))
            return BISHOP;
        if (rooks.getBit(square))
            return ROOK;
        if (queens.getBit(square))
            return QUEEN;
        if (kings.getBit(square))
            return KING;
        return NOPIECE;
    }

    inline bool isQuietMove(Move move) { // is move quiet (no capture/promotion)
        return !(occupancy(move.getTargetSquare()) != EMPTY || move.getPromotionFlag() != 0);
    }

    void initZobristKey() {
        zobristKey = 0;
        zobristKeyPawn = 0;
        zobristKeyMinor = 0;
        zobristKeyWhite = 0;
        zobristKeyBlack = 0;
        for (int square = 0; square < 64; square++) {
            int piece = occupancyPiece(square);
            int pieceColor = occupancy(square);
            if (pieceColor != EMPTY)
                zobristKey ^= zobristKeys.pieceKeys[square][(pieceColor << 3) + piece];
            if (piece == PAWN)
                zobristKeyPawn ^= zobristKeys.pieceKeys[square][(pieceColor << 3) + piece];
            if (piece == KNIGHT || piece == BISHOP)
                zobristKeyMinor ^= zobristKeys.pieceKeys[square][(pieceColor << 3) + piece];
            if(piece != PAWN) {
                if (pieceColor == WHITE)
                    zobristKeyWhite ^= zobristKeys.pieceKeys[square][(pieceColor << 3) + piece];
                if (pieceColor == BLACK)
                    zobristKeyBlack ^= zobristKeys.pieceKeys[square][(pieceColor << 3) + piece];
            }
        }
    }

    ull getZobristKey() {
        return zobristKey ^

               ((boardColor == WHITE) ? 0 : zobristKeys.colorKey) ^

               zobristKeys.canCastle[castlingWhiteQueensideBroke + (castlingWhiteKingsideBroke << 1) +
                                     (castlingBlackQueensideBroke << 2) + (castlingBlackKingsideBroke << 3)] ^

               zobristKeys.enPassant[enPassantColumn];
    }

    pair<int, int> getNNUEidx(int square, int piece, int pieceColor) {

        int row = (square >> 3), col = (square & 7);

        int neuronIdxW = 64 * (piece - 1 + pieceColor * 6) + ((7 - row) << 3);
        if (!flippedW)
            neuronIdxW += col;
        else
            neuronIdxW += 7 - col;

        int neuronIdxB = 64 * (piece - 1 + (!pieceColor) * 6) + (row << 3);
        if (!flippedB)
            neuronIdxB += col;
        else
            neuronIdxB += 7 - col;
        return {neuronIdxW, neuronIdxB};
    }

    inline void clearPosition(int square) {
        int piece = occupancyPiece(square);
        int pieceColor = occupancy(square);

        #if defined DO_HCE
        if (pieceColor != EMPTY)
            materialCount -= material[piece];
        #endif

        if (pieceColor != EMPTY) {

            ull pieceKey = zobristKeys.pieceKeys[square][(pieceColor << 3) + piece];

            zobristKey ^= pieceKey;


            if (piece == KNIGHT || piece == BISHOP){
                zobristKeyMinor ^= pieceKey;
            }
            
            if (piece == PAWN) {
                zobristKeyPawn ^= pieceKey;
            } else {
                if (pieceColor == WHITE){
                    zobristKeyWhite ^= pieceKey;
                }

                if (pieceColor == BLACK){
                    zobristKeyBlack ^= pieceKey;
                }
            }
        }

        // evaluation -= pieceSquareTable.getPieceEval(piece, square, pieceColor, endgameWeight());
        whitePieces &= (~(1ull << square));
        blackPieces &= (~(1ull << square));
        pawns &= (~(1ull << square));
        knights &= (~(1ull << square));
        bishops &= (~(1ull << square));
        rooks &= (~(1ull << square));
        queens &= (~(1ull << square));
        kings &= (~(1ull << square));
    }

    inline void clearPosition(int square, NNUEevaluator &nnueEvaluator) {
        int piece = occupancyPiece(square);
        int pieceColor = occupancy(square);

        #if !defined DO_HCE
        if (pieceColor != EMPTY)
            nnueEvaluator.set0(getNNUEidx(square, piece, pieceColor));
        #endif

        clearPosition(square);
    }

    inline void putPiece(int square, int color, int pieceType) {

        // evaluation += pieceSquareTable.getPieceEval(pieceType, square, color, endgameWeight());

        #if defined DO_HCE
        materialCount += material[pieceType];
        #endif

        if (color == WHITE)
            whitePieces |= (1ull << square);
        if (color == BLACK)
            blackPieces |= (1ull << square);
        if (pieceType == PAWN)
            pawns |= (1ull << square);
        if (pieceType == KNIGHT)
            knights |= (1ull << square);
        if (pieceType == BISHOP)
            bishops |= (1ull << square);
        if (pieceType == ROOK)
            rooks |= (1ull << square);
        if (pieceType == QUEEN)
            queens |= (1ull << square);
        if (pieceType == KING)
            kings |= (1ull << square);

        ull pieceKey = zobristKeys.pieceKeys[square][(color << 3) + pieceType];

        zobristKey ^= pieceKey;

        if (pieceType == KNIGHT || pieceType == BISHOP)
            zobristKeyMinor ^= pieceKey;


        if (pieceType == PAWN)
            zobristKeyPawn ^= pieceKey;
        else {
            if (color == WHITE)
                zobristKeyWhite ^= pieceKey;

            if (color == BLACK)
                zobristKeyBlack ^= pieceKey;
        }
    }

    inline void putPiece(int square, int color, int pieceType, NNUEevaluator &nnueEvaluator) {

        #if !defined DO_HCE
        nnueEvaluator.set1(getNNUEidx(square, pieceType, color));
        #endif

        putPiece(square, color, pieceType);
    }

    inline void movePiece(int startSquare, int targetSquare) {
        int color = occupancy(startSquare);
        int movingPiece = occupancyPiece(startSquare);
        clearPosition(startSquare);
        clearPosition(targetSquare);
        putPiece(targetSquare, color, movingPiece);
    }

    inline void movePiece(int startSquare, int targetSquare, NNUEevaluator &nnueEvaluator) {
        int color = occupancy(startSquare);
        int movingPiece = occupancyPiece(startSquare);
        clearPosition(startSquare, nnueEvaluator);
        clearPosition(targetSquare, nnueEvaluator);
        putPiece(targetSquare, color, movingPiece, nnueEvaluator);
    }

    inline int makeNullMove() {
        boardColor = (boardColor == WHITE) ? BLACK : WHITE;
        int enPassantColumnPrev = enPassantColumn;
        enPassantColumn = NO_EN_PASSANT;
        return enPassantColumnPrev;
    }

    inline void makeMove(Move move) {
        if ((whitePieces | blackPieces).getBit(move.getTargetSquare()) ||
            pawns.getBit(move.getStartSquare())) // check if move is irreversible
            lastIrreversibleMoveAge = age;

        age++;
        boardColor = (boardColor == WHITE) ? BLACK : WHITE;
        int startSquare = move.getStartSquare();
        int targetSquare = move.getTargetSquare();
        int color = occupancy(startSquare);
        int movingPiece = occupancyPiece(startSquare);

        ply2Sq = ply1Sq;
        ply2Ps = ply1Ps;
        ply1Sq = targetSquare;
        ply1Ps = movingPiece;

        enPassantColumn = NO_EN_PASSANT;
        if (movingPiece == PAWN) {
            if ((abs(targetSquare - startSquare) & 1) && occupancy(targetSquare) == EMPTY) { // enPassant capture
                if (color == WHITE)
                    clearPosition(targetSquare + 8);
                if (color == BLACK)
                    clearPosition(targetSquare - 8);
            }
            clearPosition(startSquare);
            clearPosition(targetSquare);
            if (move.getPromotionFlag() != NOPIECE)
                movingPiece = move.getPromotionFlag();
            putPiece(targetSquare, color, movingPiece);
            if (abs(targetSquare - startSquare) == 16) // updEnPassant
                enPassantColumn = boardHelper.getColumnNumber(startSquare);
        } else if (movingPiece == KING) {
            movePiece(startSquare, targetSquare);
            if (((startSquare & 7) <= 3) != ((targetSquare & 7) <= 3)) {
                if (color == WHITE)
                    flippedW ^= 1;
                else
                    flippedB ^= 1;
            }
            if (startSquare == 60 && targetSquare == 58) // white left castling
                movePiece(56, 59);
            if (startSquare == 60 && targetSquare == 62) // white right castling
                movePiece(63, 61);
            if (startSquare == 4 && targetSquare == 2) // black left castling
                movePiece(0, 3);
            if (startSquare == 4 && targetSquare == 6) // black right castling
                movePiece(7, 5);
            if (color == WHITE)
                castlingWhiteQueensideBroke = castlingWhiteKingsideBroke = 1;
            if (color == BLACK)
                castlingBlackQueensideBroke = castlingBlackKingsideBroke = 1;
        } else {
            movePiece(startSquare, targetSquare);
        }
        if (startSquare == 56 || targetSquare == 56)
            castlingWhiteQueensideBroke = 1;
        if (startSquare == 63 || targetSquare == 63)
            castlingWhiteKingsideBroke = 1;
        if (startSquare == 0 || targetSquare == 0)
            castlingBlackQueensideBroke = 1;
        if (startSquare == 7 || targetSquare == 7)
            castlingBlackKingsideBroke = 1;
    }

    void initNNUE(NNUEevaluator &nnueEvaluator) {
        nnueEvaluator.ply = 0;
        nnueEvaluator.clear(0);
        for (int square = 0; square < 64; square++) {
            int piece = occupancyPiece(square);
            int pieceColor = occupancy(square);
            if (pieceColor != EMPTY)
                nnueEvaluator.Add(0, getNNUEidx(square, piece, pieceColor));
        }
    }

    inline void makeMove(Move move, NNUEevaluator &nnueEvaluator) {
        if ((whitePieces | blackPieces).getBit(move.getTargetSquare()) ||
            pawns.getBit(move.getStartSquare())) // check if move is irreversible
            lastIrreversibleMoveAge = age;

        age++;
        boardColor = (boardColor == WHITE) ? BLACK : WHITE;
        int startSquare = move.getStartSquare();
        int targetSquare = move.getTargetSquare();
        int color = occupancy(startSquare);
        int movingPiece = occupancyPiece(startSquare);

        ply2Sq = ply1Sq;
        ply2Ps = ply1Ps;
        ply1Sq = targetSquare;
        ply1Ps = movingPiece;

        bool flipRecalc = false;

        nnueEvaluator.ply++;
        nnueEvaluator.lastCleanAccumulator[nnueEvaluator.ply] = nnueEvaluator.lastCleanAccumulator[nnueEvaluator.ply - 1];
        nnueEvaluator.updateIter[nnueEvaluator.ply] = 0;
        
        enPassantColumn = NO_EN_PASSANT;
        if (movingPiece == PAWN) {
            clearPosition(startSquare, nnueEvaluator);
            if ((abs(targetSquare - startSquare) & 1) && occupancy(targetSquare) == EMPTY) { // enPassant capture
                if (color == WHITE)
                    clearPosition(targetSquare + 8, nnueEvaluator);
                if (color == BLACK)
                    clearPosition(targetSquare - 8, nnueEvaluator);
            }
            clearPosition(targetSquare, nnueEvaluator);
            if (move.getPromotionFlag() != NOPIECE)
                movingPiece = move.getPromotionFlag();
            putPiece(targetSquare, color, movingPiece, nnueEvaluator);
            if (abs(targetSquare - startSquare) == 16) // updEnPassant
                enPassantColumn = boardHelper.getColumnNumber(startSquare);
        } else if (movingPiece == KING) {
            movePiece(startSquare, targetSquare, nnueEvaluator);
            if (((startSquare & 7) <= 3) != ((targetSquare & 7) <= 3)) {
                flipRecalc = true;
                if (color == WHITE)
                    flippedW ^= 1;
                else
                    flippedB ^= 1;
            }
            if (startSquare == 60 && targetSquare == 58) // white left castling
                movePiece(56, 59, nnueEvaluator);
            if (startSquare == 60 && targetSquare == 62) // white right castling
                movePiece(63, 61, nnueEvaluator);
            if (startSquare == 4 && targetSquare == 2) // black left castling
                movePiece(0, 3, nnueEvaluator);
            if (startSquare == 4 && targetSquare == 6) // black right castling
                movePiece(7, 5, nnueEvaluator);
            if (color == WHITE)
                castlingWhiteQueensideBroke = castlingWhiteKingsideBroke = 1;
            if (color == BLACK)
                castlingBlackQueensideBroke = castlingBlackKingsideBroke = 1;
        } else {
            movePiece(startSquare, targetSquare, nnueEvaluator);
        }
        if (startSquare == 56 || targetSquare == 56)
            castlingWhiteQueensideBroke = 1;
        if (startSquare == 63 || targetSquare == 63)
            castlingWhiteKingsideBroke = 1;
        if (startSquare == 0 || targetSquare == 0)
            castlingBlackQueensideBroke = 1;
        if (startSquare == 7 || targetSquare == 7)
            castlingBlackKingsideBroke = 1;
        if (flipRecalc) {
            nnueEvaluator.updateIter[nnueEvaluator.ply] = 5;
            int ply = nnueEvaluator.ply;
            nnueEvaluator.boardStack[ply].whitePieces = whitePieces;
            nnueEvaluator.boardStack[ply].blackPieces = blackPieces;
            nnueEvaluator.boardStack[ply].pawns = pawns;
            nnueEvaluator.boardStack[ply].knights = knights;
            nnueEvaluator.boardStack[ply].bishops = bishops;
            nnueEvaluator.boardStack[ply].rooks = rooks;
            nnueEvaluator.boardStack[ply].queens = queens;
            nnueEvaluator.boardStack[ply].kings = kings;
            nnueEvaluator.boardStack[ply].flippedW = flippedW;
            nnueEvaluator.boardStack[ply].flippedB = flippedB;
        }
    }

    inline void clearPositionZbr(int square) {
        int piece = occupancyPiece(square);
        int pieceColor = occupancy(square);

        if (pieceColor != EMPTY)
            zobristKey ^= zobristKeys.pieceKeys[square][(pieceColor << 3) + piece];
    }

    inline void putPieceZbr(int square, int color, int pieceType) {
        if (color != EMPTY)
            zobristKey ^= zobristKeys.pieceKeys[square][(color << 3) + pieceType];
    }

    inline void movePieceZbr(int startSquare, int targetSquare) {
        int color = occupancy(startSquare);
        int movingPiece = occupancyPiece(startSquare);
        clearPositionZbr(startSquare);
        clearPositionZbr(targetSquare);
        putPieceZbr(targetSquare, color, movingPiece);
    }

    inline void calculateZobristAfterMove(Move move) {
        boardColor = (boardColor == WHITE) ? BLACK : WHITE;
        int startSquare = move.getStartSquare();
        int targetSquare = move.getTargetSquare();
        int color = occupancy(startSquare);
        int movingPiece = occupancyPiece(startSquare);
        enPassantColumn = NO_EN_PASSANT;
        if (movingPiece == PAWN) {
            if ((abs(targetSquare - startSquare) & 1) && occupancy(targetSquare) == EMPTY) { // enPassant capture
                if (color == WHITE)
                    clearPositionZbr(targetSquare + 8);
                if (color == BLACK)
                    clearPositionZbr(targetSquare - 8);
            }
            clearPositionZbr(startSquare);
            clearPositionZbr(targetSquare);
            if (move.getPromotionFlag() != NOPIECE)
                movingPiece = move.getPromotionFlag();
            putPieceZbr(targetSquare, color, movingPiece);
            if (abs(targetSquare - startSquare) == 16) // updEnPassant
                enPassantColumn = boardHelper.getColumnNumber(startSquare);
        } else if (movingPiece == KING) {
            movePieceZbr(startSquare, targetSquare);
            if (startSquare == 60 && targetSquare == 58) // white left castling
                movePieceZbr(56, 59);
            if (startSquare == 60 && targetSquare == 62) // white right castling
                movePieceZbr(63, 61);
            if (startSquare == 4 && targetSquare == 2) // black left castling
                movePieceZbr(0, 3);
            if (startSquare == 4 && targetSquare == 6) // black right castling
                movePieceZbr(7, 5);
            if (color == WHITE)
                castlingWhiteQueensideBroke = castlingWhiteKingsideBroke = 1;
            if (color == BLACK)
                castlingBlackQueensideBroke = castlingBlackKingsideBroke = 1;
        } else {
            movePieceZbr(startSquare, targetSquare);
        }
        if (startSquare == 56 || targetSquare == 56)
            castlingWhiteQueensideBroke = 1;
        if (startSquare == 63 || targetSquare == 63)
            castlingWhiteKingsideBroke = 1;
        if (startSquare == 0 || targetSquare == 0)
            castlingBlackQueensideBroke = 1;
        if (startSquare == 7 || targetSquare == 7)
            castlingBlackKingsideBroke = 1;
    }

    void initFromFEN(string fen) {
        age = 0;
        vector<string> tokens = splitStr(fen, " ");
        string position = tokens[0], currentColor = tokens[1], castlingAvailability = tokens[2],
               enPassantSquare = tokens[3], ageDiff = tokens[4];
        whitePieces = blackPieces = pawns = knights = bishops = rooks = queens = kings = 0;
        castlingWhiteQueensideBroke = castlingWhiteKingsideBroke = castlingBlackQueensideBroke =
            castlingBlackKingsideBroke = 1;
        enPassantColumn = NO_EN_PASSANT;

        int currentSquare = 0;
        for (auto piece : position) {
            if (piece == '/')
                continue;
            if (piece >= '0' && piece <= '9') {
                currentSquare += piece - '0';
                continue;
            }
            ull currentSquareBit = (1ull << currentSquare);
            if (piece >= 'A' && piece <= 'Z') {
                whitePieces |= currentSquareBit;
                piece += 'a' - 'A';
            } else
                blackPieces |= currentSquareBit;
            if (piece == 'p')
                pawns |= currentSquareBit;
            if (piece == 'n')
                knights |= currentSquareBit;
            if (piece == 'b')
                bishops |= currentSquareBit;
            if (piece == 'r')
                rooks |= currentSquareBit;
            if (piece == 'q')
                queens |= currentSquareBit;
            if (piece == 'k')
                kings |= currentSquareBit;
            currentSquare++;
        }

        if (currentColor == "w")
            boardColor = WHITE;
        else
            boardColor = BLACK;

        for (auto cst : castlingAvailability) {
            if (cst == 'Q')
                castlingWhiteQueensideBroke = 0;
            if (cst == 'K')
                castlingWhiteKingsideBroke = 0;
            if (cst == 'q')
                castlingBlackQueensideBroke = 0;
            if (cst == 'k')
                castlingBlackKingsideBroke = 0;
        }

        if (enPassantSquare != "-")
            enPassantColumn = enPassantSquare[0] - 'a';

        lastIrreversibleMoveAge = age - stoi(ageDiff) - 1;

        int wKingPos = (whitePieces & kings).getFirstBitNumber();
        flippedW = (wKingPos & 7) >= 4;
        int bKingPos = (blackPieces & kings).getFirstBitNumber();
        flippedB = (bKingPos & 7) >= 4;

        evaluation = 0;
        materialCount = 0;
        for (int square = 0; square < 64; square++)
            if (occupancy(square) != EMPTY)
                materialCount += material[occupancyPiece(square)];
        for (int square = 0; square < 64; square++)
            if (occupancy(square) != EMPTY)
                evaluation +=
                    pieceSquareTable.getPieceEval(occupancyPiece(square), square, occupancy(square), endgameWeight());
        initZobristKey();
    }

    void initFromFEN(string fen, NNUEevaluator &nnueEvaluator) {
        initFromFEN(fen);
        initNNUE(nnueEvaluator);
    }

    string generateFEN() {
        char pieceChar[7] = {'.', 'p', 'n', 'b', 'r', 'q', 'k'};
        string fen;
        int emptySq = 0;
        for (int i = 0; i < 64; i++) {
            int color = occupancy(i);
            int piece = occupancyPiece(i);
            if (color == EMPTY)
                emptySq++;
            if (color != EMPTY || i % 8 == 7) {
                if (emptySq)
                    fen += to_string(emptySq);
                emptySq = 0;
            }
            if (color != EMPTY) {
                char pc = pieceChar[piece];
                if (color == WHITE)
                    pc += 'A' - 'a';
                fen.push_back(pc);
            }
            if (i % 8 == 7 && i < 63)
                fen += "/";
        }

        if (boardColor == WHITE)
            fen += " w";
        else
            fen += " b";

        string cst;
        if (!castlingWhiteKingsideBroke)
            cst += "K";
        if (!castlingWhiteQueensideBroke)
            cst += "Q";
        if (!castlingBlackKingsideBroke)
            cst += "k";
        if (!castlingBlackQueensideBroke)
            cst += "q";
        if (cst == "")
            cst = "-";
        fen += " " + cst;

        string enp = "-";
        if (enPassantColumn != NO_EN_PASSANT) {
            int row = 3, col = enPassantColumn;
            if (boardColor == WHITE)
                row = 6;
            enp = "";
            enp.push_back(char(col + 'a'));
            enp.push_back(char(row + '0'));
        }
        fen += " " + enp;

        fen += " " + to_string(age - lastIrreversibleMoveAge - 1);

        fen += " " + to_string(age / 2 + 1);

        return fen;
    }

    Board() {
        age = 0;
        boardColor = WHITE;
        whitePieces = boardHelper.generateMask(48, 63);
        blackPieces = boardHelper.generateMask(0, 15);
        pawns = boardHelper.generateMask(8, 15) | boardHelper.generateMask(48, 55);
        knights = (1ull << 1) | (1ull << 6) | (1ull << 57) | (1ull << 62);
        bishops = (1ull << 2) | (1ull << 5) | (1ull << 58) | (1ull << 61);
        rooks = (1ull << 0) | (1ull << 7) | (1ull << 56) | (1ull << 63);
        queens = (1ull << 3) | (1ull << 59);
        kings = (1ull << 4) | (1ull << 60);
        castlingWhiteQueensideBroke = castlingWhiteKingsideBroke = castlingBlackQueensideBroke =
            castlingBlackKingsideBroke = 0;
        enPassantColumn = NO_EN_PASSANT;
        evaluation = 0;
        initZobristKey();

        materialCount = 0;
        for (int square = 0; square < 64; square++)
            if (occupancy(square) != EMPTY)
                materialCount += material[occupancyPiece(square)];
    }

    int getNormalizeMaterial() {
        int mat = 0;
        int materialNNUE[] = {0, 1, 3, 3, 5, 9, 0};

        for (int square = 0; square < 64; square++)
            if (occupancy(square) != EMPTY)
                mat += materialNNUE[occupancyPiece(square)];
        return mat;
    }
};

Board mainBoard;