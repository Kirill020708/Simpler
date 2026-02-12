// NNUE

#pragma once

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

#define INCBIN_SILENCE_BITCODE_WARNING
#include "incbin.h"
INCBIN(NETWORK, "code/multilayer.nnue");

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
    __int16_t w0[inputSize][hl1Size], b0[hl1Size];
    __int8_t w1[outputBuckets][hl1Size / 2][w1BlockSize];
    int b1[outputBuckets][hl2Size];
    int w2[outputBuckets][hl2Size][hl3Size], b2[outputBuckets][hl3Size];
    int w3[outputBuckets][hl3Size], b3[outputBuckets];

    __int16_t hlSumW[maxDepth + 1][hl1Size];
    __int16_t hlSumB[maxDepth + 1][hl1Size];

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
            _mm256_storeu_si256((__m256i *)&hlSumW[idx][i], _mm256_loadu_si256((__m256i *)&b0[i]));
            _mm256_storeu_si256((__m256i *)&hlSumB[idx][i], _mm256_loadu_si256((__m256i *)&b0[i]));
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

            _mm256_storeu_si256((__m256i *)&hlSumW[idx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumW[idx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[updI.F][i])));
            _mm256_storeu_si256((__m256i *)&hlSumB[idx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumB[idx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[updI.S][i])));
        }
    }

    

    void SubAdd(int idx) {
        for (int i = 0; i < hl1Size; i += 16) {

            _mm256_storeu_si256((__m256i *)&hlSumW[idx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumW[idx - 1][i]),
                                    _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&w0[updateW[idx][1]][i]),
                                                     _mm256_loadu_si256((__m256i *)&w0[updateW[idx][0]][i]))));

            _mm256_storeu_si256((__m256i *)&hlSumB[idx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumB[idx - 1][i]),
                                    _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&w0[updateB[idx][1]][i]),
                                                     _mm256_loadu_si256((__m256i *)&w0[updateB[idx][0]][i]))));
        }
    }

    void SubSubAdd(int idx) {
        for (int i = 0; i < hl1Size; i += 16) {

            _mm256_storeu_si256((__m256i *)&hlSumW[idx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumW[idx - 1][i]),
                                    _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&w0[updateW[idx][2]][i]),
                                        _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&w0[updateW[idx][0]][i]),
                                                     _mm256_loadu_si256((__m256i *)&w0[updateW[idx][1]][i])))));


            _mm256_storeu_si256((__m256i *)&hlSumB[idx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumB[idx - 1][i]),
                                    _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&w0[updateB[idx][2]][i]),
                                        _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&w0[updateB[idx][0]][i]),
                                                     _mm256_loadu_si256((__m256i *)&w0[updateB[idx][1]][i])))));

        }
    }

    void SubAddSubAdd(int idx) {
        for (int i = 0; i < hl1Size; i += 16) {

            _mm256_storeu_si256((__m256i *)&hlSumW[idx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumW[idx - 1][i]),
                                    _mm256_add_epi16(
                                        _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&w0[updateW[idx][1]][i]),
                                                         _mm256_loadu_si256((__m256i *)&w0[updateW[idx][0]][i])),
                                        _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&w0[updateW[idx][3]][i]),
                                                         _mm256_loadu_si256((__m256i *)&w0[updateW[idx][2]][i])))));

            _mm256_storeu_si256((__m256i *)&hlSumB[idx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumB[idx - 1][i]),
                                    _mm256_add_epi16(
                                        _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&w0[updateB[idx][1]][i]),
                                                         _mm256_loadu_si256((__m256i *)&w0[updateB[idx][0]][i])),
                                        _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&w0[updateB[idx][3]][i]),
                                                         _mm256_loadu_si256((__m256i *)&w0[updateB[idx][2]][i])))));

        }
    }

    int screlu(int x) {
        x = clamp(x, 0, Q0);
        return x * x;
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
        uint16_t hl1Activations1[hl1Size * 2];
        alignas(64) uint8_t hl1Activations[hl1Size * 2];

        __m256i zerosm = _mm256_set1_epi16(-1);
        __m256i qas = _mm256_set1_epi16(Q0);
        __m256i ql = _mm256_set1_epi16(Q);

        for (int i = 0; i < hl1Size; i += 16) {
            __m256i ac1 = _mm256_loadu_si256((__m256i *)&hlSumW[ply][i]);
            ac1 = _mm256_and_si256(ac1, _mm256_cmpgt_epi16(ac1, zerosm));
            ac1 = _mm256_blendv_epi8(ac1, qas, _mm256_cmpgt_epi16(ac1, qas));

            ac1 = _mm256_mulhi_epi16(_mm256_slli_epi16(ac1, 7), ac1);

            _mm256_storeu_si256((__m256i *)&hl1Activations1[i + hl1Size * (color == BLACK)], ac1);
        }

        for (int i = 0; i < hl1Size; i += 16) {
            __m256i ac1 = _mm256_loadu_si256((__m256i *)&hlSumB[ply][i]);
            ac1 = _mm256_and_si256(ac1, _mm256_cmpgt_epi16(ac1, zerosm));
            ac1 = _mm256_blendv_epi8(ac1, qas, _mm256_cmpgt_epi16(ac1, qas));
            
            ac1 = _mm256_mulhi_epi16(_mm256_slli_epi16(ac1, 7), ac1);

            _mm256_storeu_si256((__m256i *)&hl1Activations1[i + hl1Size * (color == WHITE)], ac1);
        }

        // for (int i = 0; i < hl1Size; i += 16) {
        //     __m256i ac1 = _mm256_loadu_si256((__m256i *)&hlSumW[i]);
        //     ac1 = _mm256_and_si256(ac1, _mm256_cmpgt_epi16(ac1, zerosm));
        //     ac1 = _mm256_blendv_epi8(ac1, qas, _mm256_cmpgt_epi16(ac1, qas));
        //     ac1 = _mm256_slli_epi16(ac1, 7);

        //     __m256i ac2 = _mm256_loadu_si256((__m256i *)&hlSumW[i + hl1Size / 2]);
        //     ac2 = _mm256_and_si256(ac2, _mm256_cmpgt_epi16(ac2, zerosm));
        //     ac2 = _mm256_blendv_epi8(ac2, qas, _mm256_cmpgt_epi16(ac2, qas));

        //     ac1 = _mm256_mulhi_epi16(ac1, ac2);

        //     _mm256_storeu_si256((__m256i *)&hl1Activations1[i + hl1Size / 2 * (color == BLACK)], ac1);
        // }

        // for (int i = 0; i < hl1Size / 2; i += 16) {
        //     __m256i ac1 = _mm256_loadu_si256((__m256i *)&hlSumB[i]);
        //     ac1 = _mm256_and_si256(ac1, _mm256_cmpgt_epi16(ac1, zerosm));
        //     ac1 = _mm256_blendv_epi8(ac1, qas, _mm256_cmpgt_epi16(ac1, qas));
        //     ac1 = _mm256_slli_epi16(ac1, 7);

        //     __m256i ac2 = _mm256_loadu_si256((__m256i *)&hlSumB[i + hl1Size / 2]);
        //     ac2 = _mm256_and_si256(ac2, _mm256_cmpgt_epi16(ac2, zerosm));
        //     ac2 = _mm256_blendv_epi8(ac2, qas, _mm256_cmpgt_epi16(ac2, qas));

        //     ac1 = _mm256_mulhi_epi16(ac1, ac2);

        //     _mm256_storeu_si256((__m256i *)&hl1Activations1[i + hl1Size / 2 * (color == WHITE)], ac1);
        // }

        for (int i = 0; i < hl1Size * 2; i++)
            hl1Activations[i] = hl1Activations1[i];

        __m256i L2_0 = _mm256_setzero_si256();
        __m256i L2_1 = _mm256_setzero_si256();
        // ll sm0=0;

        for (int i = 0; i < hl1Size / 2; i++) {

            // ll j0 = i * 4;
            // ll sm1=0;
            // for(int j=0;j<4;j++) {
            //     sm1+=int(hl1Activations[i*4+j])*w1[bucket][i][j];
            //     cout<<int(hl1Activations[i*4+j])<<' '<<int(w1[bucket][i][j])<<endl;
            // }
            // sm0+=sm1;
            __m256 act = _mm256_set1_epi32(*(int32_t*)&hl1Activations[i * 4]);

            auto p = _mm256_maddubs_epi16(act,_mm256_loadu_si256((__m256i *)&w1[bucket][i][0]));
            // cout<<_mm256_extract_epi16(p, 0)<<endl;
            // cout<<_mm256_extract_epi16(p, 1)<<endl;
            // cout<<_mm256_extract_epi16(p, 2)<<endl;
            // cout<<_mm256_extract_epi16(p, 3)<<endl;
            // cout<<"! " <<_mm256_extract_epi8(act, 2)<<endl;
            // cout<<sm1<<' '<<_mm256_extract_epi32(p, 0)<<endl;
            // cout<<endl<<endl;


            L2_0 = _mm256_add_epi32(L2_0, _mm256_madd_epi16(_mm256_maddubs_epi16(act, 
                                                                _mm256_loadu_si256((__m256i *)&w1[bucket][i][0])), _mm256_set1_epi16(1)));
            L2_1 = _mm256_add_epi32(L2_1, _mm256_madd_epi16(_mm256_maddubs_epi16(act, 
                                                                _mm256_loadu_si256((__m256i *)&w1[bucket][i][2 * hl2Size])), _mm256_set1_epi16(1)));

            // cout<<_mm256_extract_epi32(L2_0, 0)<<' '<<sm0<<endl;
        }
        // int abc[8];
        // cout<<sm0<<endl;
        // _mm256_storeu_si256((__m256i *)&abc[0], L2_0);
        // for(int i = 0;i<8;i++)
        //     cout<<abc[i]<<' ';
        // 1167/(255*255/(2**9)*128/64)
        L2_0 = _mm256_srai_epi32(L2_0, 8);
        L2_0 = _mm256_add_epi32(L2_0, _mm256_loadu_si256((__m256i *)&b1[bucket][0]));
        L2_0 = _mm256_and_si256(L2_0, _mm256_cmpgt_epi16(L2_0, zerosm));
        L2_0 = _mm256_blendv_epi8(L2_0, ql, _mm256_cmpgt_epi16(L2_0, ql));
        L2_0 = _mm256_mullo_epi32(L2_0, L2_0);

        L2_1 = _mm256_srai_epi32(L2_1, 8);
        L2_1 = _mm256_add_epi32(L2_1, _mm256_loadu_si256((__m256i *)&b1[bucket][hl2Size / 2]));
        L2_1 = _mm256_and_si256(L2_1, _mm256_cmpgt_epi16(L2_1, zerosm));
        L2_1 = _mm256_blendv_epi8(L2_1, ql, _mm256_cmpgt_epi16(L2_1, ql));
        L2_1 = _mm256_mullo_epi32(L2_1, L2_1);

        // _mm256_storeu_si256((__m256i *)&abc[0], L2_0);
        // for(int i = 0;i<8;i++)
        //     cout<<abc[i]<<' ';
        // _mm256_storeu_si256((__m256i *)&abc[0], L2_1);
        // for(int i = 0;i<8;i++)
        //     cout<<abc[i]<<' ';

        int hl2Activations[hl2Size];
        _mm256_storeu_si256((__m256i *)&hl2Activations[0], L2_0);
        _mm256_storeu_si256((__m256i *)&hl2Activations[hl2Size / 2], L2_1);

        // cout<<endl<<endl;

        // cout<<endl;
        // for(int i=0;i<hl2Size;i++)
        //     cout<<hl2Activations[i]<<' ';
        // cout<<endl;

        int hl3Layer[hl3Size];
        memset(hl3Layer, 0, sizeof(hl3Layer));

        for (int i = 0; i < hl2Size; i++) {
            __m256 act = _mm256_set1_epi32(*(int32_t*)&hl2Activations[i]);
            for(int j = 0; j < hl3Size; j += 8) {
                // hl3Layer[j] += hl2Activations[i] * w2[bucket][i][j];
                // cout<<w2[bucket][i][j]<<'\n';
                _mm256_storeu_si256((__m256i *)&hl3Layer[j], 
                    _mm256_add_epi32(_mm256_loadu_si256((__m256i *)&hl3Layer[j]), 
                        _mm256_mullo_epi32(
                            act, 
                            _mm256_loadu_si256((__m256i *)&w2[bucket][i][j]))));
            }
        }

        // cout<<endl;
        // for(int i=0;i<hl3Size;i++)
        //     cout<<hl3Layer[i]<<' ';
        // cout<<endl;

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
    //         __m256i hl = _mm256_loadu_si256((__m256i *)&hlSumW[i]);
    //         hl = _mm256_and_si256(hl, _mm256_cmpgt_epi16(hl, zerosm));
    //         hl = _mm256_blendv_epi8(hl, qas, _mm256_cmpgt_epi16(hl, qas));
    //         __m256i hl0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 0));
    //         hl0 = _mm256_mullo_epi32(hl0, hl0);
    //         __m256i hl1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 1));
    //         hl1 = _mm256_mullo_epi32(hl1, hl1);
    //         __m256i w1v = _mm256_loadu_si256((__m256i *)&w1[bucket][i + hl1Size * (color == BLACK)]);
    //         outputV = _mm256_add_epi32(
    //             outputV, _mm256_mullo_epi32(hl0, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 0))));
    //         outputV = _mm256_add_epi32(
    //             outputV, _mm256_mullo_epi32(hl1, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 1))));

    //         hl = _mm256_loadu_si256((__m256i *)&hlSumB[i]);
    //         hl = _mm256_and_si256(hl, _mm256_cmpgt_epi16(hl, zerosm));
    //         hl = _mm256_blendv_epi8(hl, qas, _mm256_cmpgt_epi16(hl, qas));
    //         hl0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 0));
    //         hl0 = _mm256_mullo_epi32(hl0, hl0);
    //         hl1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 1));
    //         hl1 = _mm256_mullo_epi32(hl1, hl1);
    //         w1v = _mm256_loadu_si256((__m256i *)&w1[bucket][i + hl1Size * (color == WHITE)]);
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

        for (int i = 0; i < hl2Size; i++)
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