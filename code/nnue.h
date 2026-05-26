// NNUE

#pragma once

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

#include <array>
#include "simd.h"

#define INCBIN_SILENCE_BITCODE_WARNING
#include "incbin.h"
INCBIN(NETWORK, "code/net.nnue");

const int inputSize = 64 * 12, hl1Size = 1024, hl2Size = 16, hl3Size = 32;
const int w1BlockSize = 4 * hl2Size;
const int inputBuckets = 12;
const int outputBuckets = 8;
const int materialBuckets = 8;
const int Q0 = 255, Q1 = 128, Q = 64, SCALE = 400;

const int DO_HM = 100;

int inputBucketBoard[2][64] = {
    {11, 11, 11, 11, 11, 11, 11, 11, 
    11, 11, 11, 11, 11, 11, 11, 11,
    10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10,
    9, 9, 9, 9, 9, 9, 9, 9,
    8, 8, 8, 8, 8, 8, 8, 8,
    4, 5, 6, 7, 7, 6, 5, 4,
    0, 1, 2, 3, 3, 2, 1, 0},

   {0, 1, 2, 3, 3, 2, 1, 0,
    4, 5, 6, 7, 7, 6, 5, 4,
    8, 8, 8, 8, 8, 8, 8, 8,
    9, 9, 9, 9, 9, 9, 9, 9,
    10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10,
    11, 11, 11, 11, 11, 11, 11, 11, 
    11, 11, 11, 11, 11, 11, 11, 11}
};

bool initialized = false;
alignas(64) __int16_t w0[inputBuckets][inputSize][hl1Size];
alignas(64) __int16_t b0[hl1Size];

#if defined(AVX) || defined(NEON)
alignas(64) __int8_t w1[outputBuckets * inputBuckets][hl1Size / 4][w1BlockSize];
#else
alignas(64) __int8_t w1[outputBuckets * inputBuckets][hl2Size][hl1Size];
#endif
alignas(64) int b1[outputBuckets * inputBuckets][hl2Size];
alignas(64) int w2[outputBuckets * inputBuckets][hl2Size * 2][hl3Size];
alignas(64) int b2[outputBuckets * inputBuckets][hl3Size];
alignas(64) int w3[outputBuckets * inputBuckets][hl3Size];
alignas(64) int b3[outputBuckets * inputBuckets];

// #define PERM_COMP

#ifdef PERM_COMP
struct PermutationComp {

    int nnz[hl1Size / 2];

    void update(uint8_t *hlSum) {
        for (int i = 0; i < hl1Size / 2; i++)
            nnz[i] += (hlSum[i] != 0);
    }

    vector<int> getPermutation() {
        vector<pair<int, int>> idx;
        for (int i = 0; i < hl1Size / 2; i++)
            idx.push_back({-nnz[i], i});

        sort(all(idx));

        vector<int>p(hl1Size / 2);
        for (int i = 0; i < hl1Size / 2; i++)
            p[idx[i].second] = i;

        return p;
    }
};

PermutationComp permComp;
#endif

struct alignas(64) SmallBoard {
    int boardColor;

    Bitboard whitePieces = 0, blackPieces = 0;
    Bitboard pawns = 0, knights = 0, bishops = 0, rooks = 0, queens = 0, kings = 0;

    bool flippedW = true;
    bool flippedB = true;

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

    int getNNUEidx(int color, int square, int piece, int pieceColor) {

        int row = (square >> 3), col = (square & 7);

        if (color == WHITE) {
            int neuronIdxW = 64 * (piece - 1 + pieceColor * 6) + ((7 - row) << 3);
            if (!flippedW)
                neuronIdxW += col;
            else
                neuronIdxW += 7 - col;
            return neuronIdxW;
        }

        int neuronIdxB = 64 * (piece - 1 + (!pieceColor) * 6) + (row << 3);
        if (!flippedB)
            neuronIdxB += col;
        else
            neuronIdxB += 7 - col;
        return neuronIdxB;
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

    bool getMirroring(int color) {

        if (color == WHITE)
            return flippedW;
        return flippedB;
    }

    int getBucket(int color) {

        if (color == WHITE)
            return inputBucketBoard[WHITE][(whitePieces & kings).getFirstBitNumber()];
        return inputBucketBoard[BLACK][(blackPieces & kings).getFirstBitNumber()];
    }

    pair<ll,ll> getBuckets() {
        int whiteKingPos = (whitePieces & kings).getFirstBitNumber();
        int blackKingPos = (blackPieces & kings).getFirstBitNumber();

        return {inputBucketBoard[WHITE][whiteKingPos],
                inputBucketBoard[BLACK][blackKingPos]};
    }
};


static constexpr std::array<std::array<uint16_t, 8>, 256> makeNzTable() {
    std::array<std::array<uint16_t, 8>, 256> t{};
    for (int b = 0; b < 256; b++) {
        int n = 0;
        for (int i = 0; i < 8; i++)
            if (b & (1 << i)) t[b][n++] = i;
    }
    return t;
}
static constexpr auto NZ_TABLE = makeNzTable();


struct FinnyTable {

    SmallBoard board;

    alignas(64) __int16_t accum[hl1Size];

    FinnyTable() {

        #if defined(AVX)
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                store((vec *)&accum[i], load((vec *)&b0[i]));
            }
        #elif defined(NEON)
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                vst1q_s16(&accum[i], vld1q_s16(&b0[i]));
            }
        #else
            for (int i = 0; i < hl1Size; i++)
                accum[i] = b0[i];
        #endif
    }

    void clear() {

        #if defined(AVX)
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                store((vec *)&accum[i], load((vec *)&b0[i]));
            }
        #elif defined(NEON)
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                vst1q_s16(&accum[i], vld1q_s16(&b0[i]));
            }
        #else
            for (int i = 0; i < hl1Size; i++)
                accum[i] = b0[i];
        #endif
    }


    void Sub(int updI, int buckets) {

        #ifdef AVX
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {

                store((vec *)&accum[i],
                                    sub16(load((vec *)&accum[i]),
                                                     load((vec *)&w0[buckets][updI][i])));
            }
        #elif defined(NEON)
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                vst1q_s16(&accum[i], vsubq_s16(vld1q_s16(&accum[i]),
                                               vld1q_s16(&w0[buckets][updI][i])));
            }
        #else
            for (int i = 0; i < hl1Size; i++)
                accum[i] -= w0[buckets][updI][i];
        #endif
    }

    void Add(int updI, int buckets) {

        #ifdef AVX
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {

                store((vec *)&accum[i],
                                    add16(load((vec *)&accum[i]),
                                                     load((vec *)&w0[buckets][updI][i])));
            }
        #elif defined(NEON)
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                vst1q_s16(&accum[i], vaddq_s16(vld1q_s16(&accum[i]),
                                               vld1q_s16(&w0[buckets][updI][i])));
            }
        #else
            for (int i = 0; i < hl1Size; i++)
                accum[i] += w0[buckets][updI][i];
        #endif
    }

    void update(int color, __int16_t *hlSum, SmallBoard &newBoard) {

        int buckets = newBoard.getBucket(color);

        Bitboard changedPieces = 
            (board.whitePieces ^ newBoard.whitePieces) |
            (board.blackPieces ^ newBoard.blackPieces) |
            (board.pawns ^ newBoard.pawns) |
            (board.knights ^ newBoard.knights) |
            (board.bishops ^ newBoard.bishops) |
            (board.rooks ^ newBoard.rooks) |
            (board.queens ^ newBoard.queens) |
            (board.kings ^ newBoard.kings);

        while (changedPieces > 0) {
            int square = changedPieces.getFirstBitNumberAndExclude();

            if ((board.whitePieces | board.blackPieces).getBit(square)) {

                int piece = board.occupancyPiece(square);
                int pieceColor = board.occupancy(square);
                if (pieceColor != EMPTY)
                    Sub(newBoard.getNNUEidx(color, square, piece, pieceColor), buckets);
            }

            if ((newBoard.whitePieces | newBoard.blackPieces).getBit(square)) {

                int piece = newBoard.occupancyPiece(square);
                int pieceColor = newBoard.occupancy(square);
                if (pieceColor != EMPTY)
                    Add(newBoard.getNNUEidx(color, square, piece, pieceColor), buckets);
            }
        }

        board = newBoard;

        #ifdef AVX
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                store((vec *)&hlSum[i], load((vec *)&accum[i]));
            }
        #elif defined(NEON)
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                vst1q_s16(&hlSum[i], vld1q_s16(&accum[i]));
            }
        #else
            for (int i = 0; i < hl1Size; i++)
                hlSum[i] = accum[i];
        #endif

    }
};

int nnzPermutation[] = {
    420, 11, 178, 105, 487, 278, 42, 56, 71, 291, 171, 53, 50, 154, 186, 221, 433, 351, 64, 197, 188, 119, 140, 380, 228, 191, 148, 489, 414, 440, 324, 217, 30, 331, 502, 129, 127, 35, 265, 141, 242, 353, 164, 290, 126, 294, 479, 283, 92, 319, 44, 444, 448, 350, 94, 322, 301, 272, 49, 269, 209, 452, 101, 415, 108, 373, 57, 205, 240, 38, 243, 476, 247, 139, 102, 506, 430, 85, 355, 20, 194, 114, 346, 245, 263, 147, 257, 303, 270, 381, 261, 83, 357, 307, 89, 361, 239, 374, 65, 225, 32, 368, 52, 313, 472, 342, 124, 134, 311, 271, 428, 329, 422, 432, 135, 233, 295, 12, 14, 152, 48, 181, 408, 246, 266, 161, 409, 137, 250, 370, 296, 457, 465, 98, 91, 493, 268, 454, 61, 437, 390, 241, 122, 120, 356, 449, 73, 74, 445, 160, 96, 441, 136, 28, 182, 254, 453, 118, 13, 315, 384, 369, 5, 78, 320, 485, 264, 340, 121, 481, 456, 424, 312, 226, 9, 423, 22, 39, 289, 478, 29, 155, 510, 393, 252, 375, 183, 58, 4, 359, 402, 130, 211, 458, 236, 259, 365, 300, 133, 258, 377, 467, 244, 395, 383, 451, 216, 466, 421, 46, 391, 334, 93, 474, 469, 204, 75, 389, 143, 267, 214, 492, 279, 333, 192, 17, 222, 349, 198, 138, 69, 321, 386, 249, 332, 323, 173, 406, 224, 462, 189, 76, 413, 19, 41, 372, 288, 132, 26, 62, 501, 470, 488, 95, 446, 210, 0, 345, 403, 81, 107, 18, 125, 227, 104, 231, 162, 460, 55, 86, 417, 36, 275, 305, 464, 67, 253, 123, 112, 68, 509, 59, 16, 27, 281, 200, 348, 203, 376, 398, 511, 343, 145, 150, 84, 392, 159, 215, 219, 439, 54, 495, 285, 174, 153, 251, 352, 468, 306, 475, 63, 201, 499, 168, 491, 341, 388, 401, 358, 184, 276, 302, 156, 496, 483, 508, 274, 459, 284, 232, 7, 25, 442, 394, 504, 111, 115, 326, 318, 196, 206, 431, 167, 43, 308, 45, 354, 399, 360, 434, 463, 97, 33, 490, 293, 280, 170, 180, 212, 237, 80, 382, 213, 426, 40, 131, 116, 15, 298, 163, 24, 256, 410, 70, 277, 273, 2, 497, 117, 72, 477, 176, 367, 480, 418, 169, 411, 443, 229, 335, 455, 286, 438, 202, 347, 144, 436, 505, 8, 325, 207, 77, 218, 220, 185, 427, 66, 109, 255, 379, 248, 103, 407, 429, 378, 37, 6, 336, 309, 366, 21, 314, 397, 482, 327, 447, 339, 165, 223, 486, 175, 88, 396, 51, 287, 387, 473, 23, 282, 195, 31, 238, 151, 100, 106, 299, 234, 142, 471, 193, 435, 404, 338, 113, 187, 230, 166, 337, 1, 158, 416, 484, 405, 149, 400, 316, 317, 47, 34, 262, 235, 385, 330, 304, 110, 461, 10, 260, 503, 99, 507, 364, 425, 500, 328, 494, 498, 3, 419, 179, 60, 79, 292, 371, 363, 177, 199, 310, 157, 146, 87, 172, 208, 412, 297, 190, 344, 362, 128, 450, 90, 82
};


int nnzTotal = 0, nnzCount = 0;

struct NNUEevaluator {

    alignas(64) __int16_t hlSumW[maxDepth + 1][hl1Size];
    alignas(64) __int16_t hlSumB[maxDepth + 1][hl1Size];

    FinnyTable finnyTables[2][2][inputBuckets];

    int ply;
    int updateIter[maxDepth + 1];
    int updateW[maxDepth + 1][4];
    int updateB[maxDepth + 1][4];
    SmallBoard boardStack[maxDepth + 1]; // need for lazy eval in hm nodes
    int lastCleanAccumulator[maxDepth + 1];
    pair<int, int> bucketsStack[maxDepth + 1];

    NNUEevaluator() {
        ply = 0;
        updateIter[0] = 0;
        lastCleanAccumulator[0] = 0;
        for (int i = 0; i < hl1Size; i++)
            hlSumW[0][i] = hlSumB[0][i] = b0[i];
    }

    void clear(int idx) {
        #ifdef AVX
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                store((vec *)&hlSumW[idx][i], load((vec *)&b0[i]));
                store((vec *)&hlSumB[idx][i], load((vec *)&b0[i]));
            }
        #elif defined(NEON)
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                vst1q_s16(&hlSumW[idx][i], vld1q_s16(&b0[i]));
                vst1q_s16(&hlSumB[idx][i], vld1q_s16(&b0[i]));
            }
        #else
            for (int i = 0; i < hl1Size; i++)
                hlSumW[idx][i] = hlSumB[idx][i] = b0[i];
        #endif
    }

    void set0(pair<int, int> neuronIdx) {
        updateW[ply][updateIter[ply]] = neuronIdx.F;
        updateB[ply][updateIter[ply]] = neuronIdx.S;
        updateIter[ply]++;
    }

    void set1(pair<int, int> neuronIdx) {
        updateW[ply][updateIter[ply]] = neuronIdx.F;
        updateB[ply][updateIter[ply]] = neuronIdx.S;
        updateIter[ply]++;
    }

    void Add(int idx, pair<int, int>updI, pair<ll, ll>buckets) {
        #ifdef AVX
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {

                store((vec *)&hlSumW[idx][i],
                                    add16(load((vec *)&hlSumW[idx][i]),
                                                     load((vec *)&w0[buckets.F][updI.F][i])));
                store((vec *)&hlSumB[idx][i],
                                    add16(load((vec *)&hlSumB[idx][i]),
                                                     load((vec *)&w0[buckets.S][updI.S][i])));
            }
        #elif defined(NEON)
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                vst1q_s16(&hlSumW[idx][i], vaddq_s16(vld1q_s16(&hlSumW[idx][i]),
                                                     vld1q_s16(&w0[buckets.F][updI.F][i])));

                vst1q_s16(&hlSumB[idx][i], vaddq_s16(vld1q_s16(&hlSumB[idx][i]),
                                                     vld1q_s16(&w0[buckets.S][updI.S][i])));
            }
        #else
            for (int i = 0; i < hl1Size; i++) {
                hlSumW[idx][i] += w0[buckets.F][updI.F][i];
                hlSumB[idx][i] += w0[buckets.S][updI.S][i];
            }
        #endif
    }

    

    void SubAdd(int idx, pair<ll, ll>buckets) {
        #ifdef AVX
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {

                store((vec *)&hlSumW[idx][i],
                                    add16(load((vec *)&hlSumW[idx - 1][i]),
                                        sub16(load((vec *)&w0[buckets.F][updateW[idx][1]][i]),
                                                         load((vec *)&w0[buckets.F][updateW[idx][0]][i]))));

                store((vec *)&hlSumB[idx][i],
                                    add16(load((vec *)&hlSumB[idx - 1][i]),
                                        sub16(load((vec *)&w0[buckets.S][updateB[idx][1]][i]),
                                                         load((vec *)&w0[buckets.S][updateB[idx][0]][i]))));
            }
        #elif defined(NEON)
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                vst1q_s16(&hlSumW[idx][i], vaddq_s16(vld1q_s16(&hlSumW[idx - 1][i]),
                                                     vsubq_s16(vld1q_s16(&w0[buckets.F][updateW[idx][1]][i]),
                                                               vld1q_s16(&w0[buckets.F][updateW[idx][0]][i]))));

                vst1q_s16(&hlSumB[idx][i], vaddq_s16(vld1q_s16(&hlSumB[idx - 1][i]),
                                                     vsubq_s16(vld1q_s16(&w0[buckets.S][updateB[idx][1]][i]),
                                                               vld1q_s16(&w0[buckets.S][updateB[idx][0]][i]))));

            }
        #else
            for (int i = 0; i < hl1Size; i++) {
                hlSumW[idx][i] = hlSumW[idx - 1][i] + w0[buckets.F][updateW[idx][1]][i]
                                                    - w0[buckets.F][updateW[idx][0]][i];

                hlSumB[idx][i] = hlSumB[idx - 1][i] + w0[buckets.S][updateB[idx][1]][i]
                                                    - w0[buckets.S][updateB[idx][0]][i];
            }
        #endif
    }

    void SubSubAdd(int idx, pair<ll, ll>buckets) {
        #ifdef AVX
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {

                store((vec *)&hlSumW[idx][i],
                                    add16(load((vec *)&hlSumW[idx - 1][i]),
                                        sub16(load((vec *)&w0[buckets.F][updateW[idx][2]][i]),
                                            add16(load((vec *)&w0[buckets.F][updateW[idx][0]][i]),
                                                         load((vec *)&w0[buckets.F][updateW[idx][1]][i])))));


                store((vec *)&hlSumB[idx][i],
                                    add16(load((vec *)&hlSumB[idx - 1][i]),
                                        sub16(load((vec *)&w0[buckets.S][updateB[idx][2]][i]),
                                            add16(load((vec *)&w0[buckets.S][updateB[idx][0]][i]),
                                                         load((vec *)&w0[buckets.S][updateB[idx][1]][i])))));

            }
        #elif defined(NEON)
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                vst1q_s16(&hlSumW[idx][i], vaddq_s16(vld1q_s16(&hlSumW[idx - 1][i]),
                                                     vsubq_s16(vld1q_s16(&w0[buckets.F][updateW[idx][2]][i]),
                                                               vaddq_s16(vld1q_s16(&w0[buckets.F][updateW[idx][0]][i]),
                                                                         vld1q_s16(&w0[buckets.F][updateW[idx][1]][i])))));

                vst1q_s16(&hlSumB[idx][i], vaddq_s16(vld1q_s16(&hlSumB[idx - 1][i]),
                                                     vsubq_s16(vld1q_s16(&w0[buckets.S][updateB[idx][2]][i]),
                                                               vaddq_s16(vld1q_s16(&w0[buckets.S][updateB[idx][0]][i]),
                                                                         vld1q_s16(&w0[buckets.S][updateB[idx][1]][i])))));


            }
        #else
            for (int i = 0; i < hl1Size; i++) {
                hlSumW[idx][i] = hlSumW[idx - 1][i] + w0[buckets.F][updateW[idx][2]][i]
                                                    - w0[buckets.F][updateW[idx][1]][i]
                                                    - w0[buckets.F][updateW[idx][0]][i];

                hlSumB[idx][i] = hlSumB[idx - 1][i] + w0[buckets.S][updateB[idx][2]][i]
                                                    - w0[buckets.S][updateB[idx][1]][i]
                                                    - w0[buckets.S][updateB[idx][0]][i];
            }
        #endif
    }

    void SubAddSubAdd(int idx, pair<ll, ll>buckets) {
        #ifdef AVX
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {

                store((vec *)&hlSumW[idx][i],
                                    add16(load((vec *)&hlSumW[idx - 1][i]),
                                        add16(
                                            sub16(load((vec *)&w0[buckets.F][updateW[idx][1]][i]),
                                                             load((vec *)&w0[buckets.F][updateW[idx][0]][i])),
                                            sub16(load((vec *)&w0[buckets.F][updateW[idx][3]][i]),
                                                             load((vec *)&w0[buckets.F][updateW[idx][2]][i])))));

                store((vec *)&hlSumB[idx][i],
                                    add16(load((vec *)&hlSumB[idx - 1][i]),
                                        add16(
                                            sub16(load((vec *)&w0[buckets.S][updateB[idx][1]][i]),
                                                             load((vec *)&w0[buckets.S][updateB[idx][0]][i])),
                                            sub16(load((vec *)&w0[buckets.S][updateB[idx][3]][i]),
                                                             load((vec *)&w0[buckets.S][updateB[idx][2]][i])))));

            }
        #elif defined(NEON)
            for (int i = 0; i < hl1Size; i += vecsize / i16s) {
                vst1q_s16(&hlSumW[idx][i], vaddq_s16(vld1q_s16(&hlSumW[idx - 1][i]),
                vaddq_s16(vsubq_s16(
                    vld1q_s16(&w0[buckets.F][updateW[idx][1]][i]),
                    vld1q_s16(&w0[buckets.F][updateW[idx][0]][i])
                ),
                vsubq_s16(
                    vld1q_s16(&w0[buckets.F][updateW[idx][3]][i]),
                    vld1q_s16(&w0[buckets.F][updateW[idx][2]][i])
                )
            )));

            
            vst1q_s16(&hlSumB[idx][i], vaddq_s16(vld1q_s16(&hlSumB[idx - 1][i]),
                vaddq_s16(vsubq_s16(
                    vld1q_s16(&w0[buckets.S][updateB[idx][1]][i]),
                    vld1q_s16(&w0[buckets.S][updateB[idx][0]][i])
                ),
                vsubq_s16(
                    vld1q_s16(&w0[buckets.S][updateB[idx][3]][i]),
                    vld1q_s16(&w0[buckets.S][updateB[idx][2]][i])
                )
            )));
            }
        #else
            for (int i = 0; i < hl1Size; i++) {
                hlSumW[idx][i] = hlSumW[idx - 1][i] + w0[buckets.F][updateW[idx][1]][i]
                                                    + w0[buckets.F][updateW[idx][3]][i]
                                                    - w0[buckets.F][updateW[idx][0]][i]
                                                    - w0[buckets.F][updateW[idx][2]][i];

                hlSumB[idx][i] = hlSumB[idx - 1][i] + w0[buckets.S][updateB[idx][1]][i]
                                                    + w0[buckets.S][updateB[idx][3]][i]
                                                    - w0[buckets.S][updateB[idx][0]][i]
                                                    - w0[buckets.S][updateB[idx][2]][i];
            }
        #endif
    }

    int screlu(int x) {
        x = clamp(x, 0, Q0);
        return x * x;
    }

#if defined(AVX)
    inline vechalf pack(vec activations) {
        return packus16(activations);
    }
#elif defined(NEON)
    inline vec8half pack(vec16 activations) {
        return vqmovun_s16(activations);
    }
#endif

    inline void activateAcc(const __int16_t *accumulator, uint8_t *out) {
        #ifdef AVX

            const vec zero = setzero();
            const vec one = set1_16(Q0);

            for (int i = 0; i < hl1Size / 2; i += vecsize / i16s) {
                vec ac1 = load((const vec *)(accumulator + i));
                ac1 = max16(ac1, zero);
                ac1 = min16(ac1, one);

                vec ac2 = load((const vec *)(accumulator + hl1Size / 2 + i));
                ac2 = max16(ac2, zero);
                ac2 = min16(ac2, one);

                vec ac = mulhi16(slli16(ac1, 7), ac2);
                storehalf((vechalf *)(out + i), pack(ac));
            }
        
        #elif defined(NEON)
            
            const vec16 zero = setzero16;
            const vec16 one = set1_16(Q0);

            for (int i = 0; i < hl1Size / 2; i += vecsize / i16s) {
                vec16 ac1 = load16((accumulator + i));
                ac1 = max16(ac1, zero);
                ac1 = min16(ac1, one);

                vec16 ac2 = load16((accumulator + hl1Size / 2 + i));
                ac2 = max16(ac2, zero);
                ac2 = min16(ac2, one);

                vec16 ac = mulhi16(ac1, ac2, 7);
                vst1_u8((out + i), pack(ac));
            }
            
        #else

            for (int i = 0; i < hl1Size / 2; i++) {

                int ac1 = clamp(int(accumulator[i]), 0, int(Q0));
                int ac2 = clamp(int(accumulator[hl1Size / 2 + i]), 0, int(Q0));

                out[i] = (ac1 * ac2) >> 9;
            }
        #endif
    }

#if defined(AVX)
    inline vec dpbusdx2(vec sum, uint32_t packed0, vec weights0, uint32_t packed1, vec weights1,
                            vec ones) {
        vec partial0 = maddubs16(set1_32(packed0), weights0);
        vec partial1 = maddubs16(set1_32(packed1), weights1);
        return add32(sum, maddwd16(add16(partial0, partial1), ones));
    }

#elif defined(NEON)

    inline vec32 dpbusd(vec32 accum, vec8 a, vec8 b) {
        #ifdef __ARM_FEATURE_DOTPROD
            return vdotq_s32(accum, a, b);
        #else
            const auto low = vmull_s8(vget_low_s8(a), vget_low_s8(b));
            const auto high = vmull_high_s8(a, b);
            const auto p = vpaddq_s16(low, high);
            return vpadalq_s16(accum, p);
        #endif
    }

    inline vec32 dpbusdx2(vec32 sum, uint32_t packed0, vec8 weights0, uint32_t packed1, vec8 weights1) {
        vec32 partial0 = dpbusd(sum, vreinterpretq_u8_s8(set1u_32(packed0)), weights0);
        vec32 partial1 = dpbusd(setzero32, vreinterpretq_u8_s8(set1u_32(packed1)), weights1);
        return add32(partial0, partial1);
    }

#endif

#if defined(AVX)
    inline int findNonZeroIndices(const uint32_t *packedFt, uint16_t *indices) {
        int count = 0;
        constexpr int elems = vecsize / i32s;
        constexpr int bytes = elems / 8;
        for (int i = 0; i < hl1Size / 4; i += elems) {
            unsigned mask = cmpneq32_mask(load((const vec *)(packedFt + i)), setzero());
            for (int b = 0; b < bytes; b++) {
                uint8_t byte = (mask >> (8 * b)) & 0xFF;
                __m128i lut  = _mm_loadu_si128((const __m128i *)NZ_TABLE[byte].data());
                __m128i base = _mm_set1_epi16((short)(i + b * 8));
                _mm_storeu_si128((__m128i *)(indices + count), _mm_add_epi16(lut, base));
                count += __builtin_popcount(byte);
            }
        }
        return count;
    }

#elif defined(NEON)

    inline int findNonZeroIndices(const uint32_t *packedFt, uint16_t *indices) {
        int count = 0;
        constexpr int elems = vecsize / i32s;

        uint32_t kMask[4] = {1, 2, 4, 8};
        vec32 vmask = loadu32(kMask);

        for (int i = 0; i < hl1Size / 4; i += elems) {
            vec32 v = loadu32(packedFt + i);
            uint8_t byte = vaddvq_u32(vandq_u32(vtstq_u32(v, v), vmask));
            vec16 lut  = loadu16(NZ_TABLE[byte].data());
            vec16 base = set1_16((short)(i));
            storeu16((indices + count), add16(lut, base));
            
            count += __builtin_popcount(byte);
        }
        return count;
    }
    
#endif  

    void printAccum() {
        for (ll i = 0; i < hl1Size; i++)
            cout << hlSumW[i] << ' ';
        cout << '\n';
        for (ll i = 0; i < hl1Size; i++)
            cout << hlSumB[i] << ' ';
        cout << '\n';
    }

    void cleanAccumulators() {
        int cleanAcc = lastCleanAccumulator[ply];
        for (int i = cleanAcc + 1; i <= ply; i++) {
            lastCleanAccumulator[i] = i;
            if (updateIter[i] == 5) { // Reevaluate accumulators

                pair<int, int> buckets = boardStack[i].getBuckets();
                bucketsStack[i] = buckets;

                finnyTables[WHITE][boardStack[i].flippedW][buckets.F].update(
                    WHITE, hlSumW[i], boardStack[i]);

                finnyTables[BLACK][boardStack[i].flippedB][buckets.S].update(
                    BLACK, hlSumB[i], boardStack[i]);

            } else {
                bucketsStack[i] = bucketsStack[i - 1];
                pair<int, int> buckets = bucketsStack[i];
                if(updateIter[i] == 2) {
                    SubAdd(i, buckets);
                } else if(updateIter[i] == 3) {
                    SubSubAdd(i, buckets);
                } else if(updateIter[i] == 4) {
                    SubAddSubAdd(i, buckets);
                }
            }
        }
    }

    int evaluate(int color, int bucket) {

        cleanAccumulators();


        alignas(64) uint8_t activatedFt[hl1Size];

        const auto stm_acc = color == WHITE ? &hlSumW[ply][0] : &hlSumB[ply][0];
        const auto ntm_acc = color == WHITE ? &hlSumB[ply][0] : &hlSumW[ply][0];
        
        activateAcc(stm_acc, &activatedFt[0]);
        activateAcc(ntm_acc, &activatedFt[hl1Size / 2]);

        auto bck = bucketsStack[ply];
        int inbucket;
        if(color == WHITE)
            inbucket = bck.F;
        else
            inbucket = bck.S;

        bucket = inbucket * materialBuckets + bucket;

    #ifdef AVX

        #ifdef PERM_COMP
        permComp.update(activatedFt);
        #endif

        const vec zero = setzero();
        const vec q = set1_16(Q);
        const vec q32 = set1_32(Q);
        const vec qq = set1_32(Q * Q);
        const vec ones = set1_16(1);
        const vec zerosm = set1_16(-1);


        const uint32_t *packedFt = (const uint32_t *)activatedFt;

        alignas(64) int hl2Activations[hl2Size * 2];

        // number of vectors needed to store the hl2Size values
        constexpr int L2_VECS = hl2Size * i32s / vecsize;
        // to fit the 16 outputs we need 1 vector on avx512, 2 on avx2
        constexpr int L2_UNROLL = 4;

        vec accum[L2_VECS][L2_UNROLL];
        for (int v = 0; v < L2_VECS; v++)
            for (int u = 0; u < L2_UNROLL; u++)
                accum[v][u] = setzero();

        
        alignas(64) uint16_t nzIndices[hl1Size / 4 + 8];
        int nzCount = findNonZeroIndices(packedFt, nzIndices);

        nnzTotal += nzCount;
        nnzCount++;

        int nzi = 0;
        for (; nzi + 2 * L2_UNROLL <= nzCount; nzi += 2 * L2_UNROLL) {
            for (int u = 0; u < L2_UNROLL; u++) {
                const uint32_t ft1 = packedFt[nzIndices[nzi + 2 * u]];
                const uint32_t ft2 = packedFt[nzIndices[nzi + 2 * u + 1]];

                for (int v = 0; v < L2_VECS; v++) {
                    const vec w1_v = load((const vec *)&w1[bucket][nzIndices[nzi + 2 * u]][v * (vecsize / i8s)]);
                    const vec w2_v = load((const vec *)&w1[bucket][nzIndices[nzi + 2 * u + 1]][v * (vecsize / i8s)]);
                    accum[v][u] = dpbusdx2(accum[v][u], ft1, w1_v, ft2, w2_v, ones);
                }
            }
        }

        for (; nzi < nzCount; nzi++) {
            const uint32_t ft = packedFt[nzIndices[nzi]];
            for (int v = 0; v < L2_VECS; v++) {
                const vec w_v = load((const vec *)&w1[bucket][nzIndices[nzi]][v * (vecsize / i8s)]);
                const vec partial = maddubs16(set1_32(ft), w_v);
                accum[v][0] = add32(accum[v][0], maddwd16(partial, ones));
            }
        }

        for (int v = 0; v < L2_VECS; v++) {
            vec L2 = setzero();
            for (int u = 0; u < L2_UNROLL; u++) L2 = add32(L2, accum[v][u]);
            L2 = srai32(L2, 8);
            L2 = add32(L2, load((const vec *)&b1[bucket][v * (vecsize / i32s)]));
            auto L2c = max32(L2, zero);
            L2c = min32(L2c, q32);
            L2 = mullo32(L2, L2);
            L2 = min32(L2, qq);
            store((vec *)&hl2Activations[v * (vecsize / i32s)], slli32(L2c, 6));
            store((vec *)&hl2Activations[hl2Size + v * (vecsize / i32s)], L2);
        }

        alignas(64) int hl3Layer[hl3Size];
        memset(hl3Layer, 0, sizeof(hl3Layer));

        for (int i = 0; i < hl2Size * 2; i++) {
            vec act = set1_32(hl2Activations[i]);
            for(int j = 0; j < hl3Size; j += vecsize / i32s) {
                store((vec *)&hl3Layer[j], 
                    add32(load((vec *)&hl3Layer[j]), 
                        mullo32(
                            act, 
                            load((vec *)&w2[bucket][i][j]))));
            }
        }

        int sum = 0;
        for (int i = 0; i < 32; i++) {
            sum += clamp(hl3Layer[i] + b2[bucket][i], 0, Q*Q*Q) * w3[bucket][i];
        }
        sum += b3[bucket];

        sum = int64_t(sum) * SCALE / (Q*Q*Q*Q);
        return sum;
    
    #elif defined(NEON)

        #ifdef PERM_COMP
        permComp.update(activatedFt);
        #endif

        const vec32 zero = setzero32;
        // const vec q = set1_16(Q);
        const vec32 q32 = set1_32(Q);
        const vec32 qq = set1_32(Q * Q);
        // const vec zerosm = set1_16(-1);


        const uint32_t *packedFt = (const uint32_t *)activatedFt;

        alignas(64) int hl2Activations[hl2Size * 2];

        // number of vectors needed to store the hl2Size values
        constexpr int L2_VECS = hl2Size * i32s / vecsize;
        // to fit the 16 outputs we need 1 vector on avx512, 2 on avx2, 4 on neon
        constexpr int L2_UNROLL = 4;

        vec32 accum[L2_VECS][L2_UNROLL];
        for (int v = 0; v < L2_VECS; v++)
            for (int u = 0; u < L2_UNROLL; u++)
                accum[v][u] = setzero32;

        
        alignas(64) uint16_t nzIndices[hl1Size / 4 + 8];
        int nzCount = findNonZeroIndices(packedFt, nzIndices);

        nnzTotal += nzCount;
        nnzCount++;

        int nzi = 0;
        for (; nzi + 2 * L2_UNROLL <= nzCount; nzi += 2 * L2_UNROLL) {
            for (int u = 0; u < L2_UNROLL; u++) {
                const uint32_t ft1 = packedFt[nzIndices[nzi + 2 * u]];
                const uint32_t ft2 = packedFt[nzIndices[nzi + 2 * u + 1]];

                for (int v = 0; v < L2_VECS; v++) {
                    const vec8 w1_v = load8(&w1[bucket][nzIndices[nzi + 2 * u]][v * (vecsize / i8s)]);
                    const vec8 w2_v = load8(&w1[bucket][nzIndices[nzi + 2 * u + 1]][v * (vecsize / i8s)]);
                    
                    accum[v][u] = dpbusdx2(accum[v][u], ft1, w1_v, ft2, w2_v);
                }
            }
        }

        for (; nzi < nzCount; nzi++) {
            const uint32_t ft = packedFt[nzIndices[nzi]];
            for (int v = 0; v < L2_VECS; v++) {
                const vec8 w_v = load8(&w1[bucket][nzIndices[nzi]][v * (vecsize / i8s)]);
                accum[v][0] = dpbusd(accum[v][0], vreinterpretq_u8_u32(set1u_32(ft)), w_v);
            }
        }
        
        for (int v = 0; v < L2_VECS; v++) {
            vec32 L2 = setzero32;
            for (int u = 0; u < L2_UNROLL; u++) L2 = add32(L2, accum[v][u]);
            L2 = srai32(L2, 8);
            L2 = add32(L2, load32(&b1[bucket][v * (vecsize / i32s)]));
            auto L2c = max32(L2, zero);
            L2c = min32(L2c, q32);
            L2 = mullo32(L2, L2);
            L2 = min32(L2, qq);
            store32(&hl2Activations[v * (vecsize / i32s)], slli32(L2c, 6));
            store32(&hl2Activations[hl2Size + v * (vecsize / i32s)], L2);
        }

        alignas(64) int hl3Layer[hl3Size];
        memset(hl3Layer, 0, sizeof(hl3Layer));

        for (int i = 0; i < hl2Size * 2; i++) {
            vec32 act = set1_32(hl2Activations[i]);
            for(int j = 0; j < hl3Size; j += vecsize / i32s) {
                store32(&hl3Layer[j], 
                    add32(load32(&hl3Layer[j]), 
                        mullo32(
                            act, 
                            load32(&w2[bucket][i][j]))));
            }
        }

        int sum = 0;
        for (int i = 0; i < 32; i++) {
            sum += clamp(hl3Layer[i] + b2[bucket][i], 0, int(Q * Q * Q)) * w3[bucket][i];
        }
        sum += b3[bucket];

        sum = int64_t(sum) * SCALE / (Q*Q*Q*Q);

        return sum;


    #else

        uint16_t nzIndices[hl1Size / 4 + 8];
        int nzCount = 0;
        for (int i = 0; i < hl1Size; i++)
            if (activatedFt[i] != 0)
                nzIndices[nzCount++] = i;

        int L2[hl2Size];
        memset(L2, 0, sizeof(L2));

        for (int j = 0; j < hl2Size; j++)
            for (int i = 0; i < nzCount; i++)
                L2[j] += activatedFt[nzIndices[i]] * w1[bucket][j][nzIndices[i]];

        int L2_act[2 * hl2Size];
        for (int j = 0; j < hl2Size; j++) {
            int x = L2[j];
            x >>= 8;
            x += b1[bucket][j];
            L2_act[j] = clamp(x, 0, int(Q)) << 6;
            L2_act[j + hl2Size] = clamp(x * x, 0, int(Q * Q));
        }

        int L3[hl3Size];
        memset(L3, 0, sizeof(L3));

        for(int i = 0; i < 2 * hl2Size; i++)
            for (int j = 0; j < hl3Size; j++)
                L3[j] += L2_act[i] * w2[bucket][i][j];

        int sum = 0;
        for (int i = 0; i < 32; i++) {
            sum += clamp(L3[i] + b2[bucket][i], 0, int(Q * Q * Q)) * w3[bucket][i];
        }
        sum += b3[bucket];

        sum = int64_t(sum) * SCALE / (Q*Q*Q*Q);

        return sum;
    #endif
    }

    int getValue(vector<int8_t>&data, int &iter, int btCount) {
        if (btCount == 8)
            return data[iter++];
        if (btCount == 16) {
            int x = *(int16_t*)&data[iter];
            iter += 2;
            return x;
        }
        // 32
        int x = *(int32_t*)&data[iter];
        iter += 4;
        return x;
    }

    void initFromFile() {
        // ifstream file(path,ios::binary);

        // if(!file){
        // 	cout<<"Failed to open NNUE file\n";
        // 	return;
        // }

        initialized = true;

        vector<int8_t> data;
        size_t file_size = gNETWORKSize;
        size_t num_elements = file_size / sizeof(int8_t);

        // cout<<num_elements<<endl;
        data.resize(num_elements);
        // file.read(reinterpret_cast<char*>(data.data()), file_size);

        const int8_t *src_data = reinterpret_cast<const int8_t *>(gNETWORKData);
        std::copy(src_data, src_data + num_elements, data.begin());

        #ifdef PERM_COMP
        for (int i = 0; i < hl1Size / 2; i++)
            nnzPermutation[i] = i;
        #endif


        int iter = 0;
        for (int bucket = 0; bucket < inputBuckets; bucket++)
            for (int i = 0; i < inputSize; i++)
                for (int j = 0; j < hl1Size; j++) {

                    int nj = j, dj = 0;
                    if (j >= hl1Size / 2) {
                        nj -= hl1Size / 2;
                        dj = hl1Size / 2;
                    }

                    w0[bucket][i][nnzPermutation[nj] + dj] = getValue(data, iter, 16);
                }

        for (int j = 0; j < hl1Size; j++) {

            int nj = j, dj = 0;
            if (j >= hl1Size / 2) {
                nj -= hl1Size / 2;
                dj = hl1Size / 2;
            }

            b0[nnzPermutation[nj] + dj] = getValue(data, iter, 16);
            // cout<<b0[j]<<' ';
        }
        // cout<<'\n';

        for (int i = 0; i < hl1Size; i++)
            for (int bucket = 0; bucket < outputBuckets * inputBuckets; bucket++)
                for (int j = 0; j < hl2Size; j++) {
                    int ni = i;
                    if (i < hl1Size / 2)
                        ni = nnzPermutation[i];
                    else
                        ni = nnzPermutation[i - hl1Size / 2] + hl1Size / 2;

                    #ifdef AVX
                    int i0 = ni / 4;
                    int j0 = j * 4 + (ni % 4);
                    #elif defined(NEON)
                    int i0 = ni / 4;
                    int j0 = j * 4 + (ni % 4);
                    #else
                    int i0 = j;
                    int j0 = ni;
                    #endif

                    w1[bucket][i0][j0] = getValue(data, iter, 8);
                }


        for (int bucket = 0; bucket < outputBuckets * inputBuckets; bucket++)
            for (int i = 0; i < hl2Size; i++)
                b1[bucket][i] = getValue(data, iter, 32);

        for (int i = 0; i < hl2Size * 2; i++)
            for (int bucket = 0; bucket < outputBuckets * inputBuckets; bucket++)
                for (int j = 0; j < hl3Size; j++)
                    w2[bucket][i][j] = getValue(data, iter, 32);

        for (int bucket = 0; bucket < outputBuckets * inputBuckets; bucket++)
            for (int i = 0; i < hl3Size; i++)
                b2[bucket][i] = getValue(data, iter, 32);

        for (int i = 0; i < hl3Size; i++)
            for (int bucket = 0; bucket < outputBuckets * inputBuckets; bucket++)
                w3[bucket][i] = getValue(data, iter, 32);

        for (int bucket = 0; bucket < outputBuckets * inputBuckets; bucket++)
            b3[bucket] = getValue(data, iter, 32);

        clear(0);
    }
};

NNUEevaluator mainNnueEvaluator;
