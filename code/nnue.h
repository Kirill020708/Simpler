// NNUE

#pragma once

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

#include "simd.h"

#define INCBIN_SILENCE_BITCODE_WARNING
#include "incbin.h"
INCBIN(NETWORK, "code/net.nnue");

const int inputSize = 64 * 12, hl1Size = 1024, hl2Size = 16, hl3Size = 32;
const int w1BlockSize = 4 * hl2Size;
const int inputBuckets = 8;
const int outputBuckets = 8;
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

struct alignas(64) SmallBoard {
    Bitboard whitePieces, blackPieces;
    Bitboard pawns, knights, bishops, rooks, queens, kings;

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

    pair<ll,ll> getBuckets() {
        int whiteKingPos = (whitePieces & kings).getFirstBitNumber();
        int blackKingPos = (blackPieces & kings).getFirstBitNumber();

        return {inputBucketBoard[WHITE][whiteKingPos],
                inputBucketBoard[BLACK][blackKingPos]};
    }
};

struct NNUEevaluator {

    bool initialized = false;
    alignas(64) __int16_t w0[inputBuckets][inputSize][hl1Size];
    alignas(64) __int16_t b0[hl1Size];

    alignas(64) __int8_t w1[outputBuckets][hl1Size / 4][w1BlockSize];
    alignas(64) int b1[outputBuckets][hl2Size];
    alignas(64) int w2[outputBuckets][hl2Size * 2][hl3Size];
    alignas(64) int b2[outputBuckets][hl3Size];
    alignas(64) int w3[outputBuckets][hl3Size];
    alignas(64) int b3[outputBuckets];

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
            if (updateIter[i] == 5) { // HM
                clear(i);
                pair<int, int> buckets = boardStack[i].getBuckets();
                bucketsStack[i] = buckets;
                Bitboard pieces = boardStack[i].whitePieces | boardStack[i].blackPieces;
                while (pieces > 0) {
                    int square = pieces.getFirstBitNumberAndExclude();
                    int piece = boardStack[i].occupancyPiece(square);
                    int pieceColor = boardStack[i].occupancy(square);
                    if (pieceColor != EMPTY)
                        Add(i, boardStack[i].getNNUEidx(square, piece, pieceColor), buckets);
                }
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

        const uint32_t *packedFt = (const uint32_t *)activatedFt;

        alignas(64) int hl2Activations[hl2Size * 2];

        #ifdef __AVX512F__

            vec L2_0 = setzero();
            vec L2_1 = setzero();
            vec L2_2 = setzero();
            vec L2_3 = setzero();

            for (int i = 0; i < hl1Size / 4; i += 8) {
                vec wg0 = load((const vec *)&w1[bucket][i][0]);
                vec wg1 = load((const vec *)&w1[bucket][i + 1][0]);
                vec wg2 = load((const vec *)&w1[bucket][i + 2][0]);
                vec wg3 = load((const vec *)&w1[bucket][i + 3][0]);
                vec wg4 = load((const vec *)&w1[bucket][i + 4][0]);
                vec wg5 = load((const vec *)&w1[bucket][i + 5][0]);
                vec wg6 = load((const vec *)&w1[bucket][i + 6][0]);
                vec wg7 = load((const vec *)&w1[bucket][i + 7][0]);
                L2_0 = dpbusdx2(L2_0, packedFt[i],     wg0, packedFt[i + 1], wg1, ones);
                L2_1 = dpbusdx2(L2_1, packedFt[i + 2], wg2, packedFt[i + 3], wg3, ones);
                L2_2 = dpbusdx2(L2_2, packedFt[i + 4], wg4, packedFt[i + 5], wg5, ones);
                L2_3 = dpbusdx2(L2_3, packedFt[i + 6], wg6, packedFt[i + 7], wg7, ones);
            }

            vec L2 = add32(add32(L2_0, L2_1), add32(L2_2, L2_3));
            L2 = srai32(L2, 8);
            L2 = add32(L2, load((vec *)&b1[bucket][0]));
            auto L2c = max32(L2, zero);
            L2c = min32(L2c, q32);
            L2 = mullo32(L2, L2);
            L2 = min32(L2, qq);

            store((vec *)&hl2Activations[0], slli32(L2c, 6));
            store((vec *)&hl2Activations[hl2Size], L2);

        #elif defined(__AVX2__)

            vec L2_0 = setzero();
            vec L2_1 = setzero();

            for (int i = 0; i < hl1Size / 4; i += 2) {
                vec w10 = load((const vec *)&w1[bucket][i][0]);
                vec w11 = load((const vec *)&w1[bucket][i + 1][0]);
                vec w10sq = load((const vec *)&w1[bucket][i][2 * hl2Size]);
                vec w11sq = load((const vec *)&w1[bucket][i + 1][2 * hl2Size]);

                L2_0 = dpbusdx2(L2_0, packedFt[i], w10, packedFt[i + 1], w11, ones);
                L2_1 = dpbusdx2(L2_1, packedFt[i], w10sq, packedFt[i + 1], w11sq, ones);
            }

            L2_0 = srai32(L2_0, 8);
            L2_0 = add32(L2_0, load((vec *)&b1[bucket][0]));
            auto L2_0c = max32(L2_0, zero);
            L2_0c = min32(L2_0c, q32);
            L2_0 = mullo32(L2_0, L2_0);
            L2_0 = min32(L2_0, qq);

            L2_1 = srai32(L2_1, 8);
            L2_1 = add32(L2_1, load((vec *)&b1[bucket][hl2Size / 2]));
            auto L2_1c = max32(L2_1, zero);
            L2_1c = min32(L2_1c, q32);
            L2_1 = mullo32(L2_1, L2_1);
            L2_1 = min32(L2_1, qq);

            store((vec *)&hl2Activations[0], slli32(L2_0c, 6));
            store((vec *)&hl2Activations[hl2Size / 2], slli32(L2_1c, 6));
            store((vec *)&hl2Activations[hl2Size], L2_0);
            store((vec *)&hl2Activations[hl2Size + hl2Size / 2], L2_1);

        #endif

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


        int iter = 0;
        for (int bucket = 0; bucket < inputBuckets; bucket++)
            for (int i = 0; i < inputSize; i++)
                for (int j = 0; j < hl1Size; j++)
                    w0[bucket][i][j] = getValue(data, iter, 16);

        for (int j = 0; j < hl1Size; j++) {
            b0[j] = getValue(data, iter, 16);
            // cout<<b0[j]<<' ';
        }
        // cout<<'\n';

        for (int i = 0; i < hl1Size; i++)
            for (int bucket = 0; bucket < outputBuckets; bucket++)
                for (int j = 0; j < hl2Size; j++) {
                    int i0 = i / 4;
                    int j0 = j * 4 + (i % 4);
                    w1[bucket][i0][j0] = getValue(data, iter, 8);
                }

        for (int bucket = 0; bucket < outputBuckets; bucket++)
            for (int i = 0; i < hl2Size; i++)
                b1[bucket][i] = getValue(data, iter, 32);

        for (int i = 0; i < hl2Size * 2; i++)
            for (int bucket = 0; bucket < outputBuckets; bucket++)
                for (int j = 0; j < hl3Size; j++)
                    w2[bucket][i][j] = getValue(data, iter, 32);

        for (int bucket = 0; bucket < outputBuckets; bucket++)
            for (int i = 0; i < hl3Size; i++)
                b2[bucket][i] = getValue(data, iter, 32);

        for (int i = 0; i < hl3Size; i++)
            for (int bucket = 0; bucket < outputBuckets; bucket++)
                w3[bucket][i] = getValue(data, iter, 32);

        for (int bucket = 0; bucket < outputBuckets; bucket++)
            b3[bucket] = getValue(data, iter, 32);

        clear(0);
    }
};

NNUEevaluator mainNnueEvaluator;
