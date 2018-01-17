/*
* Synet Framework (http://github.com/ermig1979/Synet).
*
* Copyright (c) 2018-2018 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "Synet/Math.h"

namespace Synet
{
    static void CpuGemmNN(size_t M, size_t N, size_t K, float alpha, const float * A, const float * B, float * C)
    {
        for (size_t i = 0; i < M; ++i) 
        {
            for (size_t k = 0; k < K; ++k) 
            {
                register float a = alpha * A[i*K + k];
                for (size_t j = 0; j < N; ++j) 
                    C[i*N + j] += a * B[k*N + j];
            }
        }
    }

    static void CpuGemmNT(size_t M, size_t N, size_t K, float alpha, const float * A, const float * B, float * C)
    {
        for (size_t i = 0; i < M; ++i) 
        {
            for (size_t j = 0; j < N; ++j) 
            {
                register float sum = 0;
                for (size_t k = 0; k < K; ++k) 
                    sum += alpha * A[i*K + k] * B[j*K + k];
                C[i*N + j] += sum;
            }
        }
    }

    static void CpuGemmTN(size_t M, size_t N, size_t K, float alpha, const float * A, const float * B, float * C)
    {
        for (size_t i = 0; i < M; ++i) 
        {
            for (size_t k = 0; k < K; ++k) 
            {
                register float a = alpha * A[k*M + i];
                for (size_t j = 0; j < N; ++j) 
                    C[i*N + j] += a * B[k*N + j];
            }
        }
    }

    static void CpuGemmTT(size_t M, size_t N, size_t K, float alpha, const float * A, const float * B, float * C)
    {
        for (size_t i = 0; i < M; ++i) 
        {
            for (size_t j = 0; j < N; ++j) 
            {
                register float sum = 0;
                for (size_t k = 0; k < K; ++k) 
                    sum += alpha * A[i + k * M] * B[k + j * K];
                C[i*N + j] += sum;
            }
        }
    }

    template <> void CpuGemm<float>(CblasTranspose transA, CblasTranspose transB,
        size_t M, size_t N, size_t K, float alpha, const float * A, const float * B, float beta, float * C)
    {
        for (size_t i = 0; i < M; ++i)
            for (size_t j = 0; j < N; ++j)
                C[i*N + j] *= beta;

        if (transA == CblasNoTrans && transB == CblasNoTrans)
            CpuGemmNN(M, N, K, alpha, A, B, C);
        if (transA == CblasTrans && transB == CblasNoTrans)
            CpuGemmTN(M, N, K, alpha, A, B, C);
        if (transA == CblasNoTrans && transB == CblasTrans)
            CpuGemmNT(M, N, K, alpha, A, B, C);
        if (transA == CblasTrans && transB == CblasTrans)
            CpuGemmTT(M, N, K, alpha, A, B, C);
    }
}