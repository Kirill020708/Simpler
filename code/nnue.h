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

struct NNUEevaluator {

    bool initialized = false;
    __int16_t w0[inputSize][hl1Size], b0[hl1Size];
    __int8_t w1[outputBuckets][hl1Size / 2][w1BlockSize];
    int b1[outputBuckets][hl2Size];
    int w2[outputBuckets][hl2Size * 2][hl3Size], b2[outputBuckets][hl3Size];
    int w3[outputBuckets][hl3Size], b3[outputBuckets];

    __int16_t hlSumW[hl1Size], hlSumB[hl1Size];

    NNUEevaluator() {
        for (int i = 0; i < hl1Size; i++)
            hlSumW[i] = hlSumB[i] = b0[i];
    }

    void clear() {
        for (int i = 0; i < hl1Size; i += 16) {
            _mm256_storeu_si256((__m256i *)&hlSumW[i], _mm256_loadu_si256((__m256i *)&b0[i]));
            _mm256_storeu_si256((__m256i *)&hlSumB[i], _mm256_loadu_si256((__m256i *)&b0[i]));
        }
    }

    void set0(pair<int, int> neuronIdx) {
        for (int i = 0; i < hl1Size; i += 16) {

            _mm256_storeu_si256((__m256i *)&hlSumW[i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumW[i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[neuronIdx.F][i])));

            _mm256_storeu_si256((__m256i *)&hlSumB[i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumB[i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[neuronIdx.S][i])));

            // hlSumW[i]-=w0[neuronIdx.F][i];
            // hlSumB[i]-=w0[neuronIdx.S][i];
        }
    }

    void set1(pair<int, int> neuronIdx) {
        // cout<<neuronIdx.F<<' '<<neuronIdx.S<<'\n';
        for (int i = 0; i < hl1Size; i += 16) {

            _mm256_storeu_si256((__m256i *)&hlSumW[i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumW[i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[neuronIdx.F][i])));

            _mm256_storeu_si256((__m256i *)&hlSumB[i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumB[i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[neuronIdx.S][i])));

            // hlSumW[i]+=w0[neuronIdx.F][i];
            // hlSumB[i]+=w0[neuronIdx.S][i];
        }
        // for(ll i=0;i<hl1Size;i++)
        // 	cout<<hlSumW[i]<<' ';
        // cout<<'\n';
        // for(ll i=0;i<hl1Size;i++)
        // 	cout<<hlSumB[i]<<' ';
        // cout<<'\n';
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

    int evaluate(int color, int bucket) {
        uint16_t hl1Activations1[hl1Size * 2];
        alignas(64) uint8_t hl1Activations[hl1Size * 2];

        __m256i zerosm = _mm256_set1_epi16(-1);
        __m256i zerol = _mm256_set1_epi16(0);
        __m256i qas = _mm256_set1_epi16(Q0);
        __m256i ql = _mm256_set1_epi16(Q);
        __m256i qql = _mm256_set1_epi32(Q * Q);

        for (int i = 0; i < hl1Size; i += 16) {
            __m256i ac1 = _mm256_loadu_si256((__m256i *)&hlSumW[i]);
            ac1 = _mm256_and_si256(ac1, _mm256_cmpgt_epi16(ac1, zerosm));
            ac1 = _mm256_blendv_epi8(ac1, qas, _mm256_cmpgt_epi16(ac1, qas));

            ac1 = _mm256_mulhi_epi16(_mm256_slli_epi16(ac1, 7), ac1);

            _mm256_storeu_si256((__m256i *)&hl1Activations1[i + hl1Size * (color == BLACK)], ac1);
        }

        for (int i = 0; i < hl1Size; i += 16) {
            __m256i ac1 = _mm256_loadu_si256((__m256i *)&hlSumB[i]);
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
        auto L2_0c = _mm256_and_si256(L2_0, _mm256_cmpgt_epi16(L2_0, zerosm));
        L2_0c = _mm256_blendv_epi8(L2_0c, ql, _mm256_cmpgt_epi16(L2_0c, ql));
        L2_0 = _mm256_mullo_epi32(L2_0, L2_0);
        L2_0 = _mm256_min_epi32(L2_0, qql);

        L2_1 = _mm256_srai_epi32(L2_1, 8);
        L2_1 = _mm256_add_epi32(L2_1, _mm256_loadu_si256((__m256i *)&b1[bucket][hl2Size / 2]));
        auto L2_1c = _mm256_and_si256(L2_1, _mm256_cmpgt_epi16(L2_1, zerosm));
        L2_1c = _mm256_blendv_epi8(L2_1c, ql, _mm256_cmpgt_epi16(L2_1c, ql));
        L2_1 = _mm256_mullo_epi32(L2_1, L2_1);
        L2_1 = _mm256_min_epi32(L2_1, qql);

        // _mm256_storeu_si256((__m256i *)&abc[0], L2_0);
        // for(int i = 0;i<8;i++)
        //     cout<<abc[i]<<' ';
        // _mm256_storeu_si256((__m256i *)&abc[0], L2_1);
        // for(int i = 0;i<8;i++)
        //     cout<<abc[i]<<' ';

        int hl2Activations[hl2Size * 2];
        _mm256_storeu_si256((__m256i *)&hl2Activations[0], _mm256_slli_epi32(L2_0c, 6));
        _mm256_storeu_si256((__m256i *)&hl2Activations[hl2Size / 2], _mm256_slli_epi32(L2_1c, 6));
        _mm256_storeu_si256((__m256i *)&hl2Activations[hl2Size], L2_0);
        _mm256_storeu_si256((__m256i *)&hl2Activations[hl2Size + hl2Size / 2], L2_1);

        // cout<<endl<<endl;

        // cout<<endl;
        // for(int i=0;i<hl2Size;i++)
        //     cout<<hl2Activations[i]<<' ';
        // cout<<endl;

        int hl3Layer[hl3Size];
        memset(hl3Layer, 0, sizeof(hl3Layer));

        for (int i = 0; i < hl2Size * 2; i++) {
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

        clear();
    }
};

NNUEevaluator mainNnueEvaluator;