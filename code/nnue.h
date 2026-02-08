// NNUE

#pragma once

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

#define INCBIN_SILENCE_BITCODE_WARNING
#include "incbin.h"
INCBIN(NETWORK, "code/quantised1024.bin");

const int inputSize = 64 * 12, hiddenLayerSize = 1024;
const int outputBuckets = 8;
const int QA = 255, QB = 64, SCALE = 400;

struct NNUEevaluator {

    bool initialized = false;
    __int16_t w0[inputSize][hiddenLayerSize], b0[hiddenLayerSize];
    __int16_t w1[outputBuckets][hiddenLayerSize * 2], b1[outputBuckets];

    __int16_t hlSumW[hiddenLayerSize], hlSumB[hiddenLayerSize];

    NNUEevaluator() {
        for (int i = 0; i < hiddenLayerSize; i++)
            hlSumW[i] = hlSumB[i] = b0[i];
    }

    void clear() {
        for (int i = 0; i < hiddenLayerSize; i += 16) {
            _mm256_storeu_si256((__m256i *)&hlSumW[i], _mm256_loadu_si256((__m256i *)&b0[i]));
            _mm256_storeu_si256((__m256i *)&hlSumB[i], _mm256_loadu_si256((__m256i *)&b0[i]));
        }
    }

    void set0(pair<int, int> neuronIdx) {
        for (int i = 0; i < hiddenLayerSize; i += 16) {

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
        for (int i = 0; i < hiddenLayerSize; i += 16) {

            _mm256_storeu_si256((__m256i *)&hlSumW[i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumW[i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[neuronIdx.F][i])));

            _mm256_storeu_si256((__m256i *)&hlSumB[i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumB[i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[neuronIdx.S][i])));

            // hlSumW[i]+=w0[neuronIdx.F][i];
            // hlSumB[i]+=w0[neuronIdx.S][i];
        }
        // for(ll i=0;i<hiddenLayerSize;i++)
        // 	cout<<hlSumW[i]<<' ';
        // cout<<'\n';
        // for(ll i=0;i<hiddenLayerSize;i++)
        // 	cout<<hlSumB[i]<<' ';
        // cout<<'\n';
    }

    int screlu(int x) {
        x = clamp(x, 0, QA);
        return x * x;
    }

    void printAccum() {
        for (ll i = 0; i < hiddenLayerSize; i++)
            cout << hlSumW[i] << ' ';
        cout << '\n';
        for (ll i = 0; i < hiddenLayerSize; i++)
            cout << hlSumB[i] << ' ';
        cout << '\n';
    }

    int evaluate(int color, int bucket) {
        int output = 0;

        __m256i outputV = _mm256_setzero_si256();

        __m256i zerosm = _mm256_set1_epi16(-1);
        __m256i qas = _mm256_set1_epi16(QA);

        for (int i = 0; i < hiddenLayerSize; i += 16) {
            __m256i hl = _mm256_loadu_si256((__m256i *)&hlSumW[i]);
            hl = _mm256_and_si256(hl, _mm256_cmpgt_epi16(hl, zerosm));
            hl = _mm256_blendv_epi8(hl, qas, _mm256_cmpgt_epi16(hl, qas));
            __m256i hl0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 0));
            hl0 = _mm256_mullo_epi32(hl0, hl0);
            __m256i hl1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 1));
            hl1 = _mm256_mullo_epi32(hl1, hl1);
            __m256i w1v = _mm256_loadu_si256((__m256i *)&w1[bucket][i + hiddenLayerSize * (color == BLACK)]);
            outputV = _mm256_add_epi32(
                outputV, _mm256_mullo_epi32(hl0, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 0))));
            outputV = _mm256_add_epi32(
                outputV, _mm256_mullo_epi32(hl1, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 1))));

            hl = _mm256_loadu_si256((__m256i *)&hlSumB[i]);
            hl = _mm256_and_si256(hl, _mm256_cmpgt_epi16(hl, zerosm));
            hl = _mm256_blendv_epi8(hl, qas, _mm256_cmpgt_epi16(hl, qas));
            hl0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 0));
            hl0 = _mm256_mullo_epi32(hl0, hl0);
            hl1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 1));
            hl1 = _mm256_mullo_epi32(hl1, hl1);
            w1v = _mm256_loadu_si256((__m256i *)&w1[bucket][i + hiddenLayerSize * (color == WHITE)]);
            outputV = _mm256_add_epi32(
                outputV, _mm256_mullo_epi32(hl0, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 0))));
            outputV = _mm256_add_epi32(
                outputV, _mm256_mullo_epi32(hl1, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 1))));

            // output+=screlu(hlSumW[i])*w1[i];
            // output+=screlu(hlSumB[i])*w1[i+hiddenLayerSize];
        }
        __m256i hadd1 = _mm256_hadd_epi32(outputV, outputV);
        __m256i hadd2 = _mm256_hadd_epi32(hadd1, hadd1);
        __m128i sum128 = _mm_add_epi32(_mm256_castsi256_si128(hadd2), _mm256_extractf128_si256(hadd2, 1));
        output = _mm_extract_epi32(sum128, 0);

        output /= QA;
        output += b1[bucket];
        output *= SCALE;
        output /= (QA * QB);
        return output;
    }

    void initFromFile(string path) {
        // ifstream file(path,ios::binary);

        // if(!file){
        // 	cout<<"Failed to open NNUE file\n";
        // 	return;
        // }

        initialized = true;

        vector<int16_t> data;
        size_t file_size = gNETWORKSize;
        size_t num_elements = file_size / sizeof(int16_t);

        // cout<<num_elements<<endl;
        data.resize(num_elements);
        // file.read(reinterpret_cast<char*>(data.data()), file_size);

        const int16_t *src_data = reinterpret_cast<const int16_t *>(gNETWORKData);
        std::copy(src_data, src_data + num_elements, data.begin());

        int iter = 0;
        for (int i = 0; i < inputSize; i++)
            for (int j = 0; j < hiddenLayerSize; j++)
                w0[i][j] = data[iter++];

        for (int j = 0; j < hiddenLayerSize; j++) {
            b0[j] = data[iter++];
            // cout<<b0[j]<<' ';
        }
        // cout<<'\n';
        for (int bucket = 0; bucket < outputBuckets; bucket++) {
            for (int j = 0; j < hiddenLayerSize * 2; j++)
                w1[bucket][j] = data[iter++];

        }
        for (int bucket = 0; bucket < outputBuckets; bucket++)
            b1[bucket] = data[iter++];
        // cout<<b1<<'\n';

        clear();
    }
};

NNUEevaluator mainNnueEvaluator;