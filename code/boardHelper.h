// functions for different board data

#pragma once

#ifndef BITBOARD
#define BITBOARD

#include "bitboard.h"

#endif /* BITBOARD */

struct BoardHelper {
    Bitboard kingMoves[64], knightMoves[64];

    Bitboard pawnMoves[2][64], pawnCaptures[2][64], pawnCaptureLeft[2][64], pawnCaptureRight[2][64];

    Bitboard neighborColumns[64], columns[64];

    Bitboard possiblePawnDefendersWhite[64], possiblePawnDefendersBlack[64]; // squares for checking for passed pawn
    Bitboard possibleOutpostDefendersWhite[64], possibleOutpostDefendersBlack[64]; // squares for checking for outpost
    
    Bitboard rayPair[64][64];
    Bitboard rookRays[64], bishopRays[64];


    Bitboard columnUp[64], columnDown[64];

    int distanceToEdge[64];

    constexpr static int c_abs(int x) {
        return x < 0 ? -x : x;
    }

    constexpr static int c_min(int a, int b) {
        return a < b ? a : b;
    }

    constexpr inline int getColumnNumber(int square) const {
        return square & 7;
    }

    constexpr inline int getRowNumber(int square) const {
        return square >> 3;
    }

    constexpr inline Bitboard getRow(int rowNumber) const {
        return Bitboard(ull(0b11111111) << ((7 - rowNumber) << 3));
    }

    constexpr Bitboard getColumn(int columnNuber) const {
        return Bitboard(ull(0b0000000100000001000000010000000100000001000000010000000100000001) << columnNuber);
    }

    constexpr inline Bitboard generateMask(int start, int end) const { // mask with 1-s from start bit to end bit
        return Bitboard(((1ull << (end - start + 1)) - 1) << start);
    }

    constexpr inline int distanceColumn(
        Bitboard column, int color) const { // distance for the first piece in this column from color's perspective
        if (column == 0)
            return 7;
        if (color == WHITE)
            return 7 - getRowNumber(column.getLastBitNumber());
        else
            return getRowNumber(column.getFirstBitNumber());
    }

    constexpr BoardHelper()
        : kingMoves{}, knightMoves{}, pawnMoves{}, pawnCaptures{}, pawnCaptureLeft{}, pawnCaptureRight{},
          neighborColumns{}, columns{}, possiblePawnDefendersWhite{}, possiblePawnDefendersBlack{},
          possibleOutpostDefendersWhite{}, possibleOutpostDefendersBlack{}, rayPair{}, columnUp{}, columnDown{},
          distanceToEdge{}, rookRays{}, bishopRays{} {
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) {
                int s = i * 8 + j;

                for (int i1 = 0; i1 < 8; i1++)
                    for (int j1 = 0; j1 < 8; j1++) {
                        int s1 = i1 * 8 + j1;

                        int x1 = min(i, i1), y1 = min(j, j1);
                        int x2 = max(i, i1), y2 = max(j, j1);

                        if (i1 == i) {
                            for (int it = y1; it <= y2; it++)
                                rayPair[s][s1] |= (1ull << (i * 8 + it));
                        }

                        if (j1 == j) {
                            for (int it = x1; it <= x2; it++)
                                rayPair[s][s1] |= (1ull << (it * 8 + j));
                        }

                        if (i + j == i1 + j1) {
                            for (int it = x1; it <= x2; it++) {
                                int jt = y2 - (it - x1);
                                rayPair[s][s1] |= (1ull << (it * 8 + jt));
                            }
                        }

                        if (i - j == i1 - j1) {
                            for (int it = x1; it <= x2; it++) {
                                int jt = y1 + (it - x1);
                                rayPair[s][s1] |= (1ull << (it * 8 + jt));
                            }
                        }

                        rayPair[s][s1] &= (~(1ull << s));
                    }

                columns[s] = getColumn(j);

                neighborColumns[s] = 0;
                if (j)
                    neighborColumns[s] |= getColumn(j - 1);
                if (j < 7)
                    neighborColumns[s] |= getColumn(j + 1);

                possiblePawnDefendersWhite[s] = 0;
                for (ll i1 = i - 1; i1 >= 0; i1--) {
                    int s1 = i1 * 8 + j;
                    columnUp[s] |= (1ull << s1);
                    possiblePawnDefendersWhite[s] |= (1ull << s1);
                    if (j)
                        possiblePawnDefendersWhite[s] |= (1ull << (s1 - 1));
                    if (j + 1 < 8)
                        possiblePawnDefendersWhite[s] |= (1ull << (s1 + 1));
                }
                possiblePawnDefendersBlack[s] = 0;
                for (ll i1 = i + 1; i1 < 8; i1++) {
                    int s1 = i1 * 8 + j;
                    columnDown[s] |= (1ull << s1);
                    possiblePawnDefendersBlack[s] |= (1ull << s1);
                    if (j)
                        possiblePawnDefendersBlack[s] |= (1ull << (s1 - 1));
                    if (j + 1 < 8)
                        possiblePawnDefendersBlack[s] |= (1ull << (s1 + 1));
                }

                possibleOutpostDefendersWhite[s] = possiblePawnDefendersWhite[s] ^ columnUp[s];
                possibleOutpostDefendersBlack[s] = possiblePawnDefendersBlack[s] ^ columnDown[s];

                for (int i1 = 0; i1 < 8; i1++)
                    for (int j1 = 0; j1 < 8; j1++) {
                        int s1 = i1 * 8 + j1;
                        if (s == s1)
                            continue;
                        if (c_abs(i1 - i) <= 1 && c_abs(j1 - j) <= 1)
                            kingMoves[s] |= (1ull << s1);
                        if (c_abs(i1 - i) * c_abs(j1 - j) == 2)
                            knightMoves[s] |= (1ull << s1);
                    }

                if (i >= 1) {
                    pawnMoves[WHITE][s] |= (1ull << (s - 8));
                    if (i == 6)
                        pawnMoves[WHITE][s] |= (1ull << (s - 16));
                }

                if (i <= 6) {
                    pawnMoves[BLACK][s] |= (1ull << (s + 8));
                    if (i == 1)
                        pawnMoves[BLACK][s] |= (1ull << (s + 16));
                }

                if (i >= 1) {
                    if (j <= 6) {
                        pawnCaptures[WHITE][s] |= (1ull << (s - 7));
                        pawnCaptureRight[WHITE][s] = (1ull << (s - 7));
                    }
                    if (j >= 1) {
                        pawnCaptures[WHITE][s] |= (1ull << (s - 9));
                        pawnCaptureLeft[WHITE][s] = (1ull << (s - 9));
                    }
                }

                if (i <= 6) {
                    if (j >= 1) {
                        pawnCaptures[BLACK][s] |= (1ull << (s + 7));
                        pawnCaptureLeft[BLACK][s] = (1ull << (s + 7));
                    }
                    if (j <= 6) {
                        pawnCaptures[BLACK][s] |= (1ull << (s + 9));
                        pawnCaptureRight[BLACK][s] = (1ull << (s + 9));
                    }
                }

                for (ll i1 = 0; i1 < 8; i1++)
                    for (ll j1 = 0; j1 < 8; j1++) {
                        ll s1 = i1 * 8 + j1;
                        if (i1 == i || j1 == j)
                            rookRays[s] |= (1ull << s1);
                        if (c_abs(i - i1) == c_abs(j - j1))
                            bishopRays[s] |= (1ull << s1);
                    }

                distanceToEdge[s] = c_min(i, 7 - i) + c_min(j, 7 - j);
            }
    }
};

constexpr BoardHelper boardHelper;
