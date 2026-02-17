// generator of bitboards of moves for a piece

#pragma once

#ifndef BOARD
#define BOARD

#include "board.h"

#endif /* BOARD */

#ifndef BOARDHELP
#define BOARDHELP

#include "boardHelper.h"

#endif /* BOARDHELP */

#ifndef MAGIC
#define MAGIC

#include "magicNumbers.h"

#endif /* MAGIC */

struct MoveGeneration {

    inline Bitboard pawnMoves(Board &board, int square) {
        Bitboard moves;
        int col = board.occupancy(square);
        Bitboard pieces = board.whitePieces | board.blackPieces;
        Bitboard shiftedPieces =
            ((col == WHITE) ? ((pieces ^ (1ull << square)) >> 8) : ((pieces ^ (1ull << square)) << 8));
        Bitboard opponentPieces = (col == BLACK) ? board.whitePieces : board.blackPieces;

        moves |= (boardHelper.pawnMoves[col][square] & (~(pieces | shiftedPieces)));

        moves |= (boardHelper.pawnCaptures[col][square] & (opponentPieces));

        int column = boardHelper.getColumnNumber(square);

        if (boardHelper.getRowNumber(square) == (4 - (col == WHITE))) {
            if (board.enPassantColumn == column - 1)
                moves |= boardHelper.pawnCaptureLeft[col][square];

            if (board.enPassantColumn == column + 1)
                moves |= boardHelper.pawnCaptureRight[col][square];
        }

        return moves;
        /*


                int occup=board.occupancy(square);
                Bitboard moves;
                if(occup==WHITE){
                    if(board.occupancy(square-8)==EMPTY){
                        moves|=(1ull<<(square-8));
                        if(row==6&&board.occupancy(square-16)==EMPTY)
                            moves|=(1ull<<(square-16));
                    }
                    if(column>0&&(board.occupancy(square-9)==BLACK||(row==3&&board.enPassantColumn==column-1)))
                        moves|=(1ull<<(square-9));
                    if(column<7&&(board.occupancy(square-7)==BLACK||(row==3&&board.enPassantColumn==column+1)))
                        moves|=(1ull<<(square-7));
                }
                if(occup==BLACK){
                    if(board.occupancy(square+8)==EMPTY){
                        moves|=(1ull<<(square+8));
                        if(row==1&&board.occupancy(square+16)==EMPTY)
                            moves|=(1ull<<(square+16));
                    }
                    if(column>0&&(board.occupancy(square+7)==WHITE||(row==4&&board.enPassantColumn==column-1)))
                        moves|=(1ull<<(square+7));
                    if(column<7&&(board.occupancy(square+9)==WHITE||(row==4&&board.enPassantColumn==column+1)))
                        moves|=(1ull<<(square+9));
                }
                return moves;
                */
    }

    inline Bitboard knightMoves(Board &board, int square) {
        int occup = board.occupancy(square);
        Bitboard moves = boardHelper.knightMoves[square];
        return moves;
    }

    inline Bitboard bishopMoves(Board &board, int square) {
        ull key = (((magic.bishopAttackPruned[square] & ull(board.whitePieces | board.blackPieces)) *
                    magic.bishopsMagics[square]) >>
                   magic.bishopsShifts[square]);
        return magic.bishopCaptures[square][key];
    }

    inline Bitboard rookMoves(Board &board, int square) {
        ull key = (((magic.rookAttackPruned[square] & ull(board.whitePieces | board.blackPieces)) *
                    magic.rooksMagics[square]) >>
                   magic.rooksShifts[square]);
        return magic.rookCaptures[square][key];
    }

    inline Bitboard queenMoves(Board &board, int square) {
        return bishopMoves(board, square) | rookMoves(board, square);
    }

    inline bool isSquareAttackedByWhite();
    inline bool isSquareAttackedByBlack();

    inline bool isWhiteInCheck(Board &board) {
        return isSquareAttackedByBlack(board, (board.kings & board.whitePieces).getFirstBitNumber());
    }

    inline bool isBlackInCheck(Board &board) {
        return isSquareAttackedByWhite(board, (board.kings & board.blackPieces).getFirstBitNumber());
    }

    inline bool isInCheck(Board &board, int color) {
        if (color == WHITE)
            return isWhiteInCheck(board);
        return isBlackInCheck(board);
    }

    inline bool canWhiteCastleQueenside(Board &board) {
        return (!board.castlingWhiteQueensideBroke) &&
               ((boardHelper.generateMask(57, 59) & (board.whitePieces | board.blackPieces)) == 0) &&
               !isSquareAttackedByBlack(board, 58) && !isSquareAttackedByBlack(board, 59) &&
               !isSquareAttackedByBlack(board, 60);
    }

    inline bool canWhiteCastleKingside(Board &board) {
        return (!board.castlingWhiteKingsideBroke) &&
               ((boardHelper.generateMask(61, 62) & (board.whitePieces | board.blackPieces)) == 0) &&
               !isSquareAttackedByBlack(board, 60) && !isSquareAttackedByBlack(board, 61) &&
               !isSquareAttackedByBlack(board, 62);
    }

    inline bool canBlackCastleQueenside(Board &board) {
        return (!board.castlingBlackQueensideBroke) &&
               ((boardHelper.generateMask(1, 3) & (board.whitePieces | board.blackPieces)) == 0) &&
               !isSquareAttackedByWhite(board, 2) && !isSquareAttackedByWhite(board, 3) &&
               !isSquareAttackedByWhite(board, 4);
    }

    inline bool canBlackCastleKingside(Board &board) {
        return (!board.castlingBlackKingsideBroke) &&
               ((boardHelper.generateMask(5, 6) & (board.whitePieces | board.blackPieces)) == 0) &&
               !isSquareAttackedByWhite(board, 4) && !isSquareAttackedByWhite(board, 5) &&
               !isSquareAttackedByWhite(board, 6);
    }

    inline Bitboard kingMoves(Board &board, int square) {
        // this_thread::sleep_for(std::chrono::milliseconds(1));
        int occup = board.occupancy(square);
        Bitboard moves = boardHelper.kingMoves[square];
        if (occup == WHITE) {
            if (canWhiteCastleQueenside(board))
                moves |= (1ull << 58);
            if (canWhiteCastleKingside(board))
                moves |= (1ull << 62);
            moves &= (~board.whitePieces);
        }
        if (occup == BLACK) {
            if (canBlackCastleQueenside(board))
                moves |= (1ull << 2);
            if (canBlackCastleKingside(board))
                moves |= (1ull << 6);
            moves &= (~board.blackPieces);
        }
        return moves;
    }

    inline Bitboard moves(Board &board, int square) {
        if (board.pawns.getBit(square))
            return pawnMoves(board, square);
        if (board.knights.getBit(square))
            return knightMoves(board, square);
        if (board.bishops.getBit(square))
            return bishopMoves(board, square);
        if (board.rooks.getBit(square))
            return rookMoves(board, square);
        if (board.queens.getBit(square))
            return queenMoves(board, square);
        if (board.kings.getBit(square))
            return kingMoves(board, square);
        return 0;
    }

    inline bool isSquareAttackedByWhite(Board &board, int square) {
        Bitboard whitePawns = board.pawns & board.whitePieces;
        Bitboard pawnsAttack =
            (((whitePawns & (~boardHelper.getColumn(7))) >> 7) | ((whitePawns & (~boardHelper.getColumn(0))) >> 9)) &
            (~board.whitePieces);
        if (pawnsAttack.getBit(square))
            return true;
        if (board.whitePieces & board.knights & boardHelper.knightMoves[square])
            return true;
        if (board.whitePieces & board.kings & boardHelper.kingMoves[square])
            return true;

        Bitboard bishopRays = bishopMoves(board, square);
        if (board.whitePieces & board.bishops & bishopRays)
            return true;

        Bitboard rookRays = rookMoves(board, square);
        if (board.whitePieces & board.rooks & rookRays)
            return true;

        Bitboard queenRays = bishopRays | rookRays;
        if (board.whitePieces & board.queens & queenRays)
            return true;

        return false;
    }

    inline bool isSquareAttackedByBlack(Board &board, int square) {
        Bitboard blackPawns = board.pawns & board.blackPieces;
        Bitboard pawnsAttack =
            (((blackPawns & (~boardHelper.getColumn(0))) << 7) | ((blackPawns & (~boardHelper.getColumn(7))) << 9)) &
            (~board.blackPieces);
        if (pawnsAttack.getBit(square))
            return true;
        if (board.blackPieces & board.knights & boardHelper.knightMoves[square])
            return true;
        if (board.blackPieces & board.kings & boardHelper.kingMoves[square])
            return true;

        Bitboard bishopRays = bishopMoves(board, square);
        if (board.blackPieces & board.bishops & bishopRays)
            return true;

        Bitboard rookRays = rookMoves(board, square);
        if (board.blackPieces & board.rooks & rookRays)
            return true;

        Bitboard queenRays = bishopRays | rookRays;
        if (board.blackPieces & board.queens & queenRays)
            return true;
        return false;
    }

    inline bool isSquareAttacked(Board &board, int square, int color) {
        if (color == WHITE)
            return isSquareAttackedByWhite(board, square);
        return isSquareAttackedByBlack(board, square);
    }

    inline int getSmallestAttacker(Board &board, int square, int color) {
        int oppositeColor = (color == WHITE) ? BLACK : WHITE;
        Bitboard friendPieces = (color == WHITE) ? board.whitePieces : board.blackPieces;
        Bitboard opponentPieces = (color == WHITE) ? board.blackPieces : board.whitePieces;

        if (boardHelper.pawnCaptureLeft[oppositeColor][square] & board.pawns & friendPieces)
            return boardHelper.pawnCaptureLeft[oppositeColor][square].getFirstBitNumber();
        if (boardHelper.pawnCaptureRight[oppositeColor][square] & board.pawns & friendPieces)
            return boardHelper.pawnCaptureRight[oppositeColor][square].getFirstBitNumber();

        if (friendPieces & board.knights & boardHelper.knightMoves[square])
            return (friendPieces & board.knights & boardHelper.knightMoves[square]).getFirstBitNumber();

        Bitboard bishopRays = bishopMoves(board, square);
        if (friendPieces & board.bishops & bishopRays)
            return (friendPieces & board.bishops & bishopRays).getFirstBitNumber();

        Bitboard rookRays = rookMoves(board, square);
        if (friendPieces & board.rooks & rookRays)
            return (friendPieces & board.rooks & rookRays).getFirstBitNumber();

        Bitboard queenRays = bishopRays | rookRays;
        if (friendPieces & board.queens & queenRays)
            return (friendPieces & board.queens & queenRays).getFirstBitNumber();

        if ((friendPieces & board.kings & boardHelper.kingMoves[square]))
            return (friendPieces & board.kings & boardHelper.kingMoves[square]).getFirstBitNumber();

        return -1;
    }

    int pieceMaterial[7] = {0, 100, 300, 300, 500, 1000, 100000};
    int evalStack[32];

    inline int sseEval(Board &board, int square, int color, int firstAttacker) {
        Board boardCopy = board;
        int captureNumber = 1;
        int eval = 0;
        evalStack[0] = 0;
        while (true) {
            int attacker;
            if (captureNumber == 1)
                attacker = firstAttacker;
            else
                attacker = getSmallestAttacker(board, square, color);

            // cout<<attacker<<'\n';

            int mult = (captureNumber & 1) ? 1 : -1;

            if (attacker == -1) {
                eval *= mult;
                break;
            }

            int capturedPiece = board.occupancyPiece(square);
            int attackingPiece = board.occupancyPiece(attacker);

            bool isPromotion = 0;
            if (attackingPiece == PAWN) {
                if (color == WHITE)
                    isPromotion = (square < 8);
                else
                    isPromotion = (square >= 56);
            }

            board.makeMove(Move(attacker, square, isPromotion * QUEEN));
            if (attackingPiece == KING) {
                if (isInCheck(board, color))
                    break;
            }

            evalStack[captureNumber++] =
                pieceMaterial[capturedPiece] + isPromotion * (pieceMaterial[QUEEN] - pieceMaterial[PAWN]);

            eval += evalStack[captureNumber - 1];
            if (attackingPiece == KING) {
                break;
            }
            eval = -eval;
            color = (color == WHITE) ? BLACK : WHITE;
        }
        board = boardCopy;
        evalStack[captureNumber] = 0;

        // for(ll i=0;i<captureNumber;i++)
        // 	cout<<evalStack[i]<<' ';
        // cout<<'\n';

        for (int i = captureNumber - 2; i >= 2; i--) {
            evalStack[i] = max(0, -evalStack[i + 1] + evalStack[i]);
        }
        // for(ll i=0;i<captureNumber;i++)
        // 	cout<<evalStack[i]<<' ';
        // cout<<'\n';

        return evalStack[1] - evalStack[2];
    }

    bool isMoveLegal(Board &board, Move move) {
        int startSquare = move.getStartSquare();
        int targetSquare = move.getTargetSquare();
        if (!moves(board, startSquare).getBit(targetSquare))
            return false;
        Board boardCopy = board;
        board.makeMove(move);
        if (isInCheck(board, boardCopy.boardColor)) {
            board = boardCopy;
            return false;
        }
        board = boardCopy;
        return true;
    }

    Bitboard computeAttackBitboardsW(Board &board) {
    	Bitboard attacks, pieces, whitePawns = board.pawns & board.whitePieces;

        attacks |= (((whitePawns & (~boardHelper.getColumn(7))) >> 7) | ((whitePawns & (~boardHelper.getColumn(0))) >> 9));

    	pieces = board.knights & board.whitePieces;
    	while (pieces > 0)
    		attacks |= knightMoves(board, pieces.getFirstBitNumberAndExclude());

    	pieces = board.bishops & board.whitePieces;
    	while (pieces > 0)
    		attacks |= bishopMoves(board, pieces.getFirstBitNumberAndExclude());

    	pieces = board.rooks & board.whitePieces;
    	while (pieces > 0)
    		attacks |= rookMoves(board, pieces.getFirstBitNumberAndExclude());

    	pieces = board.queens & board.whitePieces;
    	while (pieces > 0)
    		attacks |= queenMoves(board, pieces.getFirstBitNumberAndExclude());

    	pieces = board.kings & board.whitePieces;
    	while (pieces > 0)
    		attacks |= kingMoves(board, pieces.getFirstBitNumberAndExclude());

    	return attacks & (~board.whitePieces);
    }

    Bitboard computeAttackBitboardsB(Board &board) {
    	Bitboard attacks, pieces, blackPawns = board.pawns & board.blackPieces;

    	attacks |= (((blackPawns & (~boardHelper.getColumn(0))) << 7) | ((blackPawns & (~boardHelper.getColumn(7))) << 9));
    	
    	pieces = board.knights & board.blackPieces;
    	while (pieces > 0)
    		attacks |= knightMoves(board, pieces.getFirstBitNumberAndExclude());

    	pieces = board.bishops & board.blackPieces;
    	while (pieces > 0)
    		attacks |= bishopMoves(board, pieces.getFirstBitNumberAndExclude());

    	pieces = board.rooks & board.blackPieces;
    	while (pieces > 0)
    		attacks |= rookMoves(board, pieces.getFirstBitNumberAndExclude());

    	pieces = board.queens & board.blackPieces;
    	while (pieces > 0)
    		attacks |= queenMoves(board, pieces.getFirstBitNumberAndExclude());

    	pieces = board.kings & board.blackPieces;
    	while (pieces > 0)
    		attacks |= kingMoves(board, pieces.getFirstBitNumberAndExclude());

    	return attacks & (~board.blackPieces);
    }
};

MoveGeneration moveGenerator;