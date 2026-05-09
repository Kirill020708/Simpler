#pragma once

#define i8s 8
#define i16s 16
#define i32s 32
#define i64s 64

#ifdef __AVX512F__

	#define vecsize 512
	#define load _mm512_load_si512
	#define store _mm512_store_si512
	#define vec __m512i
	#define vechalf __m256i
	#define add16 _mm512_add_epi16
	#define add32 _mm512_add_epi32
	#define sub16 _mm512_sub_epi16
	#define packus16(x) _mm512_cvtepi16_epi8(x)
	#define setzero _mm512_setzero_si512
	#define set1_16 _mm512_set1_epi16
	#define set1_32 _mm512_set1_epi32
	#define min16 _mm512_min_epi16
	#define min32 _mm512_min_epi32
	#define max16 _mm512_max_epi16
	#define max32 _mm512_max_epi32
	#define mulhi16 _mm512_mulhi_epi16
	#define slli16 _mm512_slli_epi16
	#define slli32 _mm512_slli_epi32
	#define srai32 _mm512_srai_epi32
	#define storehalf _mm256_store_si256
	#define mullo32 _mm512_mullo_epi32
	#define maddubs16 _mm512_maddubs_epi16
	#define maddwd16 _mm512_madd_epi16

#elif defined(__AVX2__)

	#define vecsize 256
	#define load _mm256_load_si256
	#define store _mm256_store_si256
	#define vec __m256i
	#define vechalf __m128i
	#define add16 _mm256_add_epi16
	#define add32 _mm256_add_epi32
	#define sub16 _mm256_sub_epi16
	#define packus16(x) _mm_packus_epi16(_mm256_castsi256_si128(x), _mm256_extracti128_si256(x, 1))
	#define setzero _mm256_setzero_si256
	#define set1_16 _mm256_set1_epi16
	#define set1_32 _mm256_set1_epi32
	#define min16 _mm256_min_epi16
	#define min32 _mm256_min_epi32
	#define max16 _mm256_max_epi16
	#define max32 _mm256_max_epi32
	#define mulhi16 _mm256_mulhi_epi16
	#define slli16 _mm256_slli_epi16
	#define slli32 _mm256_slli_epi32
	#define srai32 _mm256_srai_epi32
	#define storehalf _mm_store_si128
	#define mullo32 _mm256_mullo_epi32
	#define maddubs16 _mm256_maddubs_epi16
	#define maddwd16 _mm256_madd_epi16

#endif
