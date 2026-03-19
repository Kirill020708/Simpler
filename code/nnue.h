// NNUE

#pragma once

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

#define INCBIN_SILENCE_BITCODE_WARNING
#include "incbin.h"
INCBIN(NETWORK, "code/dualact.nnue");

const int inputSize = 64 * 12, hl1Size = 512, hl2Size = 16, hl3Size = 32;
const int w1BlockSize = 4 * hl2Size;
const int outputBuckets = 8;
const int Q0 = 255, Q1 = 128, Q = 64, SCALE = 400;

const int DO_HM = 100;

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
};

struct NNUEevaluator {

    bool initialized = false;
    alignas(64) __int16_t w0[inputSize][hl1Size];
    alignas(64) __int16_t b0[hl1Size];

    alignas(64) __int8_t w1[outputBuckets][hl1Size / 2][w1BlockSize];
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

    NNUEevaluator() {
        ply = 0;
        updateIter[0] = 0;
        lastCleanAccumulator[0] = 0;
        for (int i = 0; i < hl1Size; i++)
            hlSumW[0][i] = hlSumB[0][i] = b0[i];
    }

    void clear(int idx) {
        for (int i = 0; i < hl1Size; i += 16) {
            _mm256_store_si256((__m256i *)&hlSumW[idx][i], _mm256_load_si256((__m256i *)&b0[i]));
            _mm256_store_si256((__m256i *)&hlSumB[idx][i], _mm256_load_si256((__m256i *)&b0[i]));
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

    void Add(int idx, pair<int, int>updI) {
        for (int i = 0; i < hl1Size; i += 16) {

            _mm256_store_si256((__m256i *)&hlSumW[idx][i],
                                _mm256_add_epi16(_mm256_load_si256((__m256i *)&hlSumW[idx][i]),
                                                 _mm256_load_si256((__m256i *)&w0[updI.F][i])));
            _mm256_store_si256((__m256i *)&hlSumB[idx][i],
                                _mm256_add_epi16(_mm256_load_si256((__m256i *)&hlSumB[idx][i]),
                                                 _mm256_load_si256((__m256i *)&w0[updI.S][i])));
        }
    }

    

    void SubAdd(int idx) {
        for (int i = 0; i < hl1Size; i += 16) {

            _mm256_store_si256((__m256i *)&hlSumW[idx][i],
                                _mm256_add_epi16(_mm256_load_si256((__m256i *)&hlSumW[idx - 1][i]),
                                    _mm256_sub_epi16(_mm256_load_si256((__m256i *)&w0[updateW[idx][1]][i]),
                                                     _mm256_load_si256((__m256i *)&w0[updateW[idx][0]][i]))));

            _mm256_store_si256((__m256i *)&hlSumB[idx][i],
                                _mm256_add_epi16(_mm256_load_si256((__m256i *)&hlSumB[idx - 1][i]),
                                    _mm256_sub_epi16(_mm256_load_si256((__m256i *)&w0[updateB[idx][1]][i]),
                                                     _mm256_load_si256((__m256i *)&w0[updateB[idx][0]][i]))));
        }
    }

    void SubSubAdd(int idx) {
        for (int i = 0; i < hl1Size; i += 16) {

            _mm256_store_si256((__m256i *)&hlSumW[idx][i],
                                _mm256_add_epi16(_mm256_load_si256((__m256i *)&hlSumW[idx - 1][i]),
                                    _mm256_sub_epi16(_mm256_load_si256((__m256i *)&w0[updateW[idx][2]][i]),
                                        _mm256_add_epi16(_mm256_load_si256((__m256i *)&w0[updateW[idx][0]][i]),
                                                     _mm256_load_si256((__m256i *)&w0[updateW[idx][1]][i])))));


            _mm256_store_si256((__m256i *)&hlSumB[idx][i],
                                _mm256_add_epi16(_mm256_load_si256((__m256i *)&hlSumB[idx - 1][i]),
                                    _mm256_sub_epi16(_mm256_load_si256((__m256i *)&w0[updateB[idx][2]][i]),
                                        _mm256_add_epi16(_mm256_load_si256((__m256i *)&w0[updateB[idx][0]][i]),
                                                     _mm256_load_si256((__m256i *)&w0[updateB[idx][1]][i])))));

        }
    }

    void SubAddSubAdd(int idx) {
        for (int i = 0; i < hl1Size; i += 16) {

            _mm256_store_si256((__m256i *)&hlSumW[idx][i],
                                _mm256_add_epi16(_mm256_load_si256((__m256i *)&hlSumW[idx - 1][i]),
                                    _mm256_add_epi16(
                                        _mm256_sub_epi16(_mm256_load_si256((__m256i *)&w0[updateW[idx][1]][i]),
                                                         _mm256_load_si256((__m256i *)&w0[updateW[idx][0]][i])),
                                        _mm256_sub_epi16(_mm256_load_si256((__m256i *)&w0[updateW[idx][3]][i]),
                                                         _mm256_load_si256((__m256i *)&w0[updateW[idx][2]][i])))));

            _mm256_store_si256((__m256i *)&hlSumB[idx][i],
                                _mm256_add_epi16(_mm256_load_si256((__m256i *)&hlSumB[idx - 1][i]),
                                    _mm256_add_epi16(
                                        _mm256_sub_epi16(_mm256_load_si256((__m256i *)&w0[updateB[idx][1]][i]),
                                                         _mm256_load_si256((__m256i *)&w0[updateB[idx][0]][i])),
                                        _mm256_sub_epi16(_mm256_load_si256((__m256i *)&w0[updateB[idx][3]][i]),
                                                         _mm256_load_si256((__m256i *)&w0[updateB[idx][2]][i])))));

        }
    }

    int screlu(int x) {
        x = clamp(x, 0, Q0);
        return x * x;
    }

    inline __m128i pack(__m256i activations) {
        return _mm_packus_epi16(_mm256_castsi256_si128(activations), _mm256_extracti128_si256(activations, 1));
    }

    inline void activateAcc(const __int16_t *accumulator, uint8_t *out) {
        const __m256i zero = _mm256_setzero_si256();
        const __m256i one = _mm256_set1_epi16(Q0);

        for (int i = 0; i < hl1Size; i += 16) {
            __m256i ac = _mm256_load_si256((const __m256i *)(accumulator + i));
            ac = _mm256_max_epi16(ac, zero);
            ac = _mm256_min_epi16(ac, one);
            ac = _mm256_mulhi_epi16(_mm256_slli_epi16(ac, 7), ac);
            _mm_store_si128((__m128i *)(out + i), pack(ac));
        }
    }

    inline __m256i maddubs(__m256i u, __m256i i) {
        return _mm256_maddubs_epi16(u, i);
    }

    inline __m256i maddwd(__m256i a, __m256i b) {
        return _mm256_madd_epi16(a, b);
    }

    inline __m256i dpbusdx2(__m256i sum, uint32_t packed0, __m256i weights0, uint32_t packed1, __m256i weights1,
                            __m256i ones) {
        __m256i partial0 = maddubs(_mm256_set1_epi32(packed0), weights0);
        __m256i partial1 = maddubs(_mm256_set1_epi32(packed1), weights1);
        return _mm256_add_epi32(sum, maddwd(_mm256_add_epi16(partial0, partial1), ones));
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
            if (updateIter[i] == 5) { // HM
                clear(i);
                Bitboard pieces = boardStack[i].whitePieces | boardStack[i].blackPieces;
                while (pieces > 0) {
                    int square = pieces.getFirstBitNumberAndExclude();
                    int piece = boardStack[i].occupancyPiece(square);
                    int pieceColor = boardStack[i].occupancy(square);
                    if (pieceColor != EMPTY)
                        Add(i, boardStack[i].getNNUEidx(square, piece, pieceColor));
                }
            } else if(updateIter[i] == 2) {
                SubAdd(i);
            } else if(updateIter[i] == 3) {
                SubSubAdd(i);
            } else if(updateIter[i] == 4) {
                SubAddSubAdd(i);
            }
        }
    }

    int evaluate(int color, int bucket) {
        cleanAccumulators();

        const __m256i q = _mm256_set1_epi16(Q);
        const __m256i qq = _mm256_set1_epi32(Q * Q);
        const __m256i ones = _mm256_set1_epi16(1);
        const __m256i zerosm = _mm256_set1_epi16(-1);

        __m256i L2_0 = _mm256_setzero_si256();
        __m256i L2_1 = _mm256_setzero_si256();
        alignas(64) uint8_t activatedFt[hl1Size * 2];

        const auto stm_acc = color == WHITE ? &hlSumW[ply][0] : &hlSumB[ply][0];
        const auto ntm_acc = color == WHITE ? &hlSumB[ply][0] : &hlSumW[ply][0];
        
        activateAcc(stm_acc, &activatedFt[0]);
        activateAcc(ntm_acc, &activatedFt[hl1Size]);

        const uint32_t *packedFt = (const uint32_t *)activatedFt;
        for (int i = 0; i < hl1Size / 2; i += 2) {
            __m256i w10 = _mm256_load_si256((const __m256i *)&w1[bucket][i][0]);
            __m256i w11 = _mm256_load_si256((const __m256i *)&w1[bucket][i + 1][0]);
            __m256i w10sq = _mm256_load_si256((const __m256i *)&w1[bucket][i][2 * hl2Size]);
            __m256i w11sq = _mm256_load_si256((const __m256i *)&w1[bucket][i + 1][2 * hl2Size]);

            L2_0 = dpbusdx2(L2_0, packedFt[i], w10, packedFt[i + 1], w11, ones);
            L2_1 = dpbusdx2(L2_1, packedFt[i], w10sq, packedFt[i + 1], w11sq, ones);
        }

        L2_0 = _mm256_srai_epi32(L2_0, 8);
        L2_0 = _mm256_add_epi32(L2_0, _mm256_load_si256((__m256i *)&b1[bucket][0]));
        auto L2_0c = _mm256_and_si256(L2_0, _mm256_cmpgt_epi16(L2_0, zerosm));
        L2_0c = _mm256_blendv_epi8(L2_0c, q, _mm256_cmpgt_epi16(L2_0c, q));
        L2_0 = _mm256_mullo_epi32(L2_0, L2_0);
        L2_0 = _mm256_min_epi32(L2_0, qq);

        L2_1 = _mm256_srai_epi32(L2_1, 8);
        L2_1 = _mm256_add_epi32(L2_1, _mm256_load_si256((__m256i *)&b1[bucket][hl2Size / 2]));
        auto L2_1c = _mm256_and_si256(L2_1, _mm256_cmpgt_epi16(L2_1, zerosm));
        L2_1c = _mm256_blendv_epi8(L2_1c, q, _mm256_cmpgt_epi16(L2_1c, q));
        L2_1 = _mm256_mullo_epi32(L2_1, L2_1);
        L2_1 = _mm256_min_epi32(L2_1, qq);

        alignas(64) int hl2Activations[hl2Size * 2];
        _mm256_store_si256((__m256i *)&hl2Activations[0], _mm256_slli_epi32(L2_0c, 6));
        _mm256_store_si256((__m256i *)&hl2Activations[hl2Size / 2], _mm256_slli_epi32(L2_1c, 6));
        _mm256_store_si256((__m256i *)&hl2Activations[hl2Size], L2_0);
        _mm256_store_si256((__m256i *)&hl2Activations[hl2Size + hl2Size / 2], L2_1);

        alignas(64) int hl3Layer[hl3Size];
        memset(hl3Layer, 0, sizeof(hl3Layer));

        for (int i = 0; i < hl2Size * 2; i++) {
            __m256i act = _mm256_set1_epi32(hl2Activations[i]);
            for(int j = 0; j < hl3Size; j += 8) {
                _mm256_store_si256((__m256i *)&hl3Layer[j], 
                    _mm256_add_epi32(_mm256_load_si256((__m256i *)&hl3Layer[j]), 
                        _mm256_mullo_epi32(
                            act, 
                            _mm256_load_si256((__m256i *)&w2[bucket][i][j]))));
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

    //     __m256i outputV = _mm256_setzero_si256();

    //     __m256i zerosm = _mm256_set1_epi16(-1);
    //     __m256i qas = _mm256_set1_epi16(QA);

    //     for (int i = 0; i < hl1Size; i += 16) {
    //         __m256i hl = _mm256_load_si256((__m256i *)&hlSumW[i]);
    //         hl = _mm256_and_si256(hl, _mm256_cmpgt_epi16(hl, zerosm));
    //         hl = _mm256_blendv_epi8(hl, qas, _mm256_cmpgt_epi16(hl, qas));
    //         __m256i hl0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 0));
    //         hl0 = _mm256_mullo_epi32(hl0, hl0);
    //         __m256i hl1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 1));
    //         hl1 = _mm256_mullo_epi32(hl1, hl1);
    //         __m256i w1v = _mm256_load_si256((__m256i *)&w1[bucket][i + hl1Size * (color == BLACK)]);
    //         outputV = _mm256_add_epi32(
    //             outputV, _mm256_mullo_epi32(hl0, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 0))));
    //         outputV = _mm256_add_epi32(
    //             outputV, _mm256_mullo_epi32(hl1, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 1))));

    //         hl = _mm256_load_si256((__m256i *)&hlSumB[i]);
    //         hl = _mm256_and_si256(hl, _mm256_cmpgt_epi16(hl, zerosm));
    //         hl = _mm256_blendv_epi8(hl, qas, _mm256_cmpgt_epi16(hl, qas));
    //         hl0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 0));
    //         hl0 = _mm256_mullo_epi32(hl0, hl0);
    //         hl1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 1));
    //         hl1 = _mm256_mullo_epi32(hl1, hl1);
    //         w1v = _mm256_load_si256((__m256i *)&w1[bucket][i + hl1Size * (color == WHITE)]);
    //         outputV = _mm256_add_epi32(
    //             outputV, _mm256_mullo_epi32(hl0, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 0))));
    //         outputV = _mm256_add_epi32(
    //             outputV, _mm256_mullo_epi32(hl1, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 1))));

    //         // output+=screlu(hlSumW[i])*w1[i];
    //         // output+=screlu(hlSumB[i])*w1[i+hl1Size];
    //     }
    //     __m256i hadd1 = _mm256_hadd_epi32(outputV, outputV);
    //     __m256i hadd2 = _mm256_hadd_epi32(hadd1, hadd1);
    //     __m128i sum128 = _mm_add_epi32(_mm256_castsi256_si128(hadd2), _mm256_extractf128_si256(hadd2, 1));
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
        for (int i = 0; i < inputSize; i++)
            for (int j = 0; j < hl1Size; j++)
                w0[i][j] = getValue(data, iter, 16);

        for (int j = 0; j < hl1Size; j++) {
            b0[j] = getValue(data, iter, 16);
            // cout<<b0[j]<<' ';
        }
        // cout<<'\n';

        for (int i = 0; i < hl1Size * 2; i++)
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
