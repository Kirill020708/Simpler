#pragma once

#define i8s 8
#define i16s 16
#define i32s 32
#define i64s 64

#ifdef __AVX512F__

	#define AVX

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
	#define cmpneq32_mask(a, b) ((unsigned)_mm512_cmpneq_epi32_mask(a, b))

#elif defined(__AVX2__)

	#define AVX

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
	
	inline unsigned cmpneq32_mask(vec a, vec b) {
		return (unsigned)(~_mm256_movemask_ps(_mm256_castsi256_ps(_mm256_cmpeq_epi32(a, b))) & 0xFF);
	}
	

#elif defined(__ARM_NEON)

	#define NEON

	#define vecsize 128
	#define load8 vld1q_s8
	#define loadu8 vld1q_u8
	#define load16 vld1q_s16
	#define loadu16 vld1q_u16
	#define load32 vld1q_s32
	#define loadu32 vld1q_u32

	#define store8 vst1q_s8
	#define storeu8 vst1q_u8
	#define store16 vst1q_s16
	#define storeu16 vst1q_u16
	#define store32 vst1q_s32
	#define storeu32 vst1q_u32

	#define vec8 int8x16_t
	#define vec8half int8x8_t
	#define vec16 int16x8_t
	#define vec16half int16x4_t
	#define vec32 int32x4_t
	#define vec32half int32x2_t

	#define add8 vaddq_s8
	#define add16 vaddq_s16
	#define add32 vaddq_s32

	#define sub8 vsubq_s8
	#define sub16 vsubq_s16
	#define sub32 vsubq_s32

	#define setzero16 vdupq_n_s16(0)
	#define setzero32 vdupq_n_s32(0)
	#define set1_16 vdupq_n_s16
	#define set1_32 vdupq_n_s32
	#define set1u_16 vdupq_n_u16
	#define set1u_32 vdupq_n_u32
	#define min16 vminq_s16
	#define min32 vminq_s32
	#define max16 vmaxq_s16
	#define max32 vmaxq_s32
	#define mulhi16(a, b, shift) vqdmulhq_s16(vshlq_s16(a, vdupq_n_s16(static_cast<int16_t>(shift - 1))), b) 
	#define slli16 vshlq_n_s16
	#define slli32 vshlq_n_s32
	#define srai32 vshrq_n_s32
	#define storehalf vst1_s16
	#define mullo32 vmulq_s32

#else
	#define SCALAR
#endif
