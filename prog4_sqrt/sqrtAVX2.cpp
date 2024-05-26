#include <immintrin.h>

const int VECTOR_WIDTH = 8;

void sqrtAVX2(int N, float initialGuess, float values[], float output[]) {

    static const __m256 kThreshold = _mm256_set1_ps(0.00001f);

    for (int i = 0; i < N; i += VECTOR_WIDTH) {
        __m256 x = _mm256_loadu_ps(values + i);  // float x = values[i];
        __m256 guess = _mm256_set1_ps(initialGuess);      // float guess = initialGuess;

        // float error = fabs(guess * guess * x - 1.f);
        __m256 error = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), _mm256_sub_ps(_mm256_mul_ps(_mm256_mul_ps(guess, guess), x), _mm256_set1_ps(1.f)));

        // while (error > kThreshold)
        while (_mm256_movemask_ps(_mm256_cmp_ps(error, kThreshold, _CMP_GT_OS))) {
            // guess = (3.f * guess - x * guess * guess * guess) * 0.5f;
            guess = _mm256_mul_ps(_mm256_sub_ps(_mm256_mul_ps(_mm256_set1_ps(3.0f), guess), _mm256_mul_ps(_mm256_mul_ps(x, guess), _mm256_mul_ps(guess, guess))), _mm256_set1_ps(0.5f));
            // error = fabs(guess * guess * x - 1.f);
            error = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), _mm256_sub_ps(_mm256_mul_ps(_mm256_mul_ps(guess, guess), x), _mm256_set1_ps(1.f)));
        }

        // output[i] = x * guess;
        _mm256_storeu_ps(output + i, _mm256_mul_ps(x, guess));
    }
}