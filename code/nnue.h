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
const int inputBuckets = 8;
const int outputBuckets = 8;
const int materialBuckets = 8;
const int Q0 = 255, Q1 = 128, Q = 64, SCALE = 400;

const int DO_HM = 100;

int inputBucketBoard[2][64] = {
    {7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,
    6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,
    4,4,5,5,5,5,4,4,
    0,1,2,3,3,2,1,0},

   {0,1,2,3,3,2,1,0,
    4,4,5,5,5,5,4,4,
    6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7}
};

bool initialized = false;
alignas(64) __int16_t w0[inputBuckets][inputSize][hl1Size];
alignas(64) __int16_t b0[hl1Size];

alignas(64) __int8_t w1[outputBuckets * inputBuckets][hl1Size / 4][w1BlockSize];
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

    void clear() {
        for (int i = 0; i < hl1Size; i += vecsize / i16s) {
            store((vec *)&accum[i], load((vec *)&b0[i]));
        }
    }


    void Sub(int updI, int buckets) {

        for (int i = 0; i < hl1Size; i += vecsize / i16s) {

            store((vec *)&accum[i],
                                sub16(load((vec *)&accum[i]),
                                                 load((vec *)&w0[buckets][updI][i])));
        }
    }

    void Add(int updI, int buckets) {

        for (int i = 0; i < hl1Size; i += vecsize / i16s) {

            store((vec *)&accum[i],
                                add16(load((vec *)&accum[i]),
                                                 load((vec *)&w0[buckets][updI][i])));
        }
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


        for (int i = 0; i < hl1Size; i += vecsize / i16s) {
            store((vec *)&hlSum[i], load((vec *)&accum[i]));
        }
    }
};

FinnyTable finnyTables[2][2][inputBuckets];

int nnzPermutation[] = {
    175, 166, 500, 137, 440, 355, 334, 236, 386, 404, 308, 416, 165, 119, 97, 48, 403, 457, 406, 460, 320, 364, 443, 21, 141, 185, 413, 110, 503, 101, 34, 446, 122, 420, 358, 273, 474, 424, 238, 415, 108, 448, 384, 14, 159, 201, 75, 77, 42, 407, 360, 338, 45, 376, 23, 268, 484, 55, 284, 497, 401, 411, 295, 331, 487, 463, 126, 176, 311, 139, 445, 304, 337, 418, 362, 346, 50, 145, 27, 146, 96, 350, 291, 431, 459, 24, 200, 356, 56, 270, 85, 157, 480, 379, 58, 491, 400, 265, 507, 223, 187, 429, 226, 205, 90, 232, 206, 449, 98, 88, 41, 149, 249, 92, 465, 63, 297, 153, 194, 374, 322, 399, 436, 373, 184, 490, 239, 252, 174, 73, 257, 452, 17, 272, 22, 37, 408, 254, 52, 135, 240, 288, 164, 78, 292, 366, 385, 242, 196, 472, 112, 478, 251, 231, 405, 388, 245, 328, 392, 369, 134, 430, 433, 26, 394, 310, 302, 104, 221, 18, 47, 40, 432, 7, 16, 347, 2, 62, 204, 44, 105, 499, 51, 81, 208, 299, 233, 442, 229, 163, 387, 132, 220, 340, 455, 197, 250, 275, 256, 222, 344, 298, 371, 489, 481, 53, 438, 195, 475, 315, 207, 93, 168, 427, 13, 361, 225, 262, 155, 171, 79, 330, 246, 193, 410, 306, 219, 107, 397, 390, 425, 102, 354, 391, 210, 398, 25, 393, 326, 462, 332, 336, 170, 59, 38, 283, 70, 230, 64, 243, 468, 422, 148, 227, 293, 316, 133, 10, 106, 203, 30, 286, 470, 218, 11, 189, 486, 235, 115, 57, 267, 82, 124, 447, 381, 4, 280, 188, 466, 266, 130, 492, 359, 277, 244, 357, 167, 103, 29, 150, 0, 485, 72, 382, 269, 216, 123, 142, 234, 209, 214, 6, 258, 294, 54, 8, 28, 83, 46, 501, 419, 396, 483, 31, 349, 264, 493, 473, 5, 253, 12, 506, 109, 324, 140, 498, 313, 467, 61, 260, 183, 180, 363, 127, 458, 389, 151, 116, 36, 213, 80, 111, 247, 1, 156, 211, 439, 169, 182, 161, 383, 199, 32, 84, 435, 33, 228, 172, 76, 281, 144, 339, 143, 118, 312, 479, 343, 241, 290, 303, 309, 414, 329, 353, 314, 377, 509, 437, 202, 198, 87, 323, 237, 454, 342, 9, 154, 434, 212, 100, 421, 341, 215, 287, 261, 378, 89, 511, 117, 74, 296, 327, 138, 395, 495, 409, 68, 131, 471, 181, 69, 456, 177, 504, 318, 502, 365, 375, 444, 114, 94, 86, 186, 162, 451, 152, 3, 217, 66, 321, 412, 49, 345, 402, 190, 352, 276, 300, 464, 450, 39, 426, 158, 128, 147, 367, 191, 248, 129, 35, 477, 65, 67, 173, 20, 160, 259, 285, 43, 441, 351, 423, 263, 428, 508, 319, 224, 505, 325, 305, 279, 317, 496, 71, 274, 370, 372, 348, 113, 179, 95, 301, 19, 125, 510, 271, 136, 255, 494, 178, 476, 121, 469, 282, 488, 192, 380, 368, 91, 461, 99, 120, 15, 335, 60, 453, 289, 417, 278, 307, 333, 482
};


int nnzTotal = 0, nnzCount = 0;

struct NNUEevaluator {

    alignas(64) __int16_t hlSumW[maxDepth + 1][hl1Size];
    alignas(64) __int16_t hlSumB[maxDepth + 1][hl1Size];

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
        for (int i = 0; i < hl1Size; i += vecsize / i16s) {
            store((vec *)&hlSumW[idx][i], load((vec *)&b0[i]));
            store((vec *)&hlSumB[idx][i], load((vec *)&b0[i]));
        }
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
        for (int i = 0; i < hl1Size; i += vecsize / i16s) {

            store((vec *)&hlSumW[idx][i],
                                add16(load((vec *)&hlSumW[idx][i]),
                                                 load((vec *)&w0[buckets.F][updI.F][i])));
            store((vec *)&hlSumB[idx][i],
                                add16(load((vec *)&hlSumB[idx][i]),
                                                 load((vec *)&w0[buckets.S][updI.S][i])));
        }
    }

    

    void SubAdd(int idx, pair<ll, ll>buckets) {
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
    }

    void SubSubAdd(int idx, pair<ll, ll>buckets) {
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
    }

    void SubAddSubAdd(int idx, pair<ll, ll>buckets) {
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
    }

    int screlu(int x) {
        x = clamp(x, 0, Q0);
        return x * x;
    }

    inline vechalf pack(vec activations) {
        return packus16(activations);
    }

    inline void activateAcc(const __int16_t *accumulator, uint8_t *out) {
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
    }

#if defined(__AVX2__) || defined(__AVX512F__)
    inline vec dpbusdx2(vec sum, uint32_t packed0, vec weights0, uint32_t packed1, vec weights1,
                            vec ones) {
        vec partial0 = maddubs16(set1_32(packed0), weights0);
        vec partial1 = maddubs16(set1_32(packed1), weights1);
        return add32(sum, maddwd16(add16(partial0, partial1), ones));
    }
#endif

    
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

        const vec zero = setzero();
        const vec q = set1_16(Q);
        const vec q32 = set1_32(Q);
        const vec qq = set1_32(Q * Q);
        const vec ones = set1_16(1);
        const vec zerosm = set1_16(-1);

        alignas(64) uint8_t activatedFt[hl1Size];

        const auto stm_acc = color == WHITE ? &hlSumW[ply][0] : &hlSumB[ply][0];
        const auto ntm_acc = color == WHITE ? &hlSumB[ply][0] : &hlSumW[ply][0];
        
        activateAcc(stm_acc, &activatedFt[0]);
        activateAcc(ntm_acc, &activatedFt[hl1Size / 2]);

        #ifdef PERM_COMP
        permComp.update(activatedFt);
        #endif

        auto bck = bucketsStack[ply];
        int inbucket;
        if(color == WHITE)
            inbucket = bck.F;
        else
            inbucket = bck.S;

        bucket = inbucket * materialBuckets + bucket;


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
        

        // int i = 0;
        // for (; i + 2 * L2_UNROLL <= hl1Size / 4; i += 2 * L2_UNROLL) {
        //     for (int u = 0; u < L2_UNROLL; u++) {
        //         const uint32_t ft1 = packedFt[i + 2 * u];
        //         const uint32_t ft2 = packedFt[i + 2 * u + 1];

        //         for (int v = 0; v < L2_VECS; v++) {
        //             const vec w1_v = load((const vec *)&w1[bucket][i + 2 * u][v * (vecsize / i8s)]);
        //             const vec w2_v = load((const vec *)&w1[bucket][i + 2 * u + 1][v * (vecsize / i8s)]);
        //             accum[v][u] = dpbusdx2(accum[v][u], ft1, w1_v, ft2, w2_v, ones);
        //         }
        //     }
        // }

        // for (; i < hl1Size / 4; i++) {
        //     const uint32_t ft = packedFt[i];
        //     for (int v = 0; v < L2_VECS; v++) {
        //         const vec w_v = load((const vec *)&w1[bucket][i][v * (vecsize / i8s)]);
        //         const vec partial = maddubs16(set1_32(ft), w_v);
        //         accum[v][0] = add32(accum[v][0], maddwd16(partial, ones));
        //     }
        // }

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
    }

    // int evaluate1(int color, int bucket) {
    //     int output = 0;

    //     vec outputV = setzero();

    //     vec zerosm = set1_16(-1);
    //     vec qas = set1_16(QA);

    //     for (int i = 0; i < hl1Size; i += 16) {
    //         vec hl = load((vec *)&hlSumW[i]);
    //         hl = _mm256_and_si256(hl, _mm256_cmpgt_epi16(hl, zerosm));
    //         hl = _mm256_blendv_epi8(hl, qas, _mm256_cmpgt_epi16(hl, qas));
    //         vec hl0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 0));
    //         hl0 = mullo32(hl0, hl0);
    //         vec hl1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 1));
    //         hl1 = mullo32(hl1, hl1);
    //         vec w1v = load((vec *)&w1[bucket][i + hl1Size * (color == BLACK)]);
    //         outputV = add32(
    //             outputV, mullo32(hl0, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 0))));
    //         outputV = add32(
    //             outputV, mullo32(hl1, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 1))));

    //         hl = load((vec *)&hlSumB[i]);
    //         hl = _mm256_and_si256(hl, _mm256_cmpgt_epi16(hl, zerosm));
    //         hl = _mm256_blendv_epi8(hl, qas, _mm256_cmpgt_epi16(hl, qas));
    //         hl0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 0));
    //         hl0 = mullo32(hl0, hl0);
    //         hl1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 1));
    //         hl1 = mullo32(hl1, hl1);
    //         w1v = load((vec *)&w1[bucket][i + hl1Size * (color == WHITE)]);
    //         outputV = add32(
    //             outputV, mullo32(hl0, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 0))));
    //         outputV = add32(
    //             outputV, mullo32(hl1, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 1))));

    //         // output+=screlu(hlSumW[i])*w1[i];
    //         // output+=screlu(hlSumB[i])*w1[i+hl1Size];
    //     }
    //     vec hadd1 = _mm256_hadd_epi32(outputV, outputV);
    //     vec hadd2 = _mm256_hadd_epi32(hadd1, hadd1);
    //     vechalf sum128 = _mm_add_epi32(_mm256_castsi256_si128(hadd2), _mm256_extractf128_si256(hadd2, 1));
    //     output = _mm_extract_epi32(sum128, 0);

    //     output /= QA;
    //     output += b1[bucket];
    //     output *= SCALE;
    //     output /= (QA * QB);
    //     return output;
    // }

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

    void initFromFile(string path) {
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

                    int i0 = ni / 4;
                    int j0 = j * 4 + (ni % 4);
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
