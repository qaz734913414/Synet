/*
* Synet Framework (http://github.com/ermig1979/Synet).
*
* Copyright (c) 2018-2019 Yermalayeu Ihar.
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

#pragma once

#include "Synet/Common.h"
#include "Synet/Layer.h"

namespace Synet
{
    namespace Detail
    {
        template <class T> void ShuffleLayerForwardCpu(const T * src0, size_t srcC0, const T * src1, size_t srcC1, size_t spatial, T * dst0, T * dst1, size_t dstC, TensorFormat format)
        {
            if (format == TensorFormatNhwc)
            {
                for (size_t s = 0; s < spatial; ++s)
                {
                    size_t cd = 0;
                    for (size_t cs = 0; cs < srcC0; cs += 2, cd += 1)
                    {
                        dst0[cd] = src0[cs + 0];
                        dst1[cd] = src0[cs + 1];
                    }
                    for (size_t cs = 0; cs < srcC1; cs += 2, cd += 1)
                    {
                        dst0[cd] = src1[cs + 0];
                        dst1[cd] = src1[cs + 1];
                    }
                    src0 += srcC0;
                    src1 += srcC1;
                    dst0 += dstC;
                    dst1 += dstC;
                }
            }
            else
            {
                size_t cd = 0;
                for (size_t cs = 0; cs < srcC0; cs += 2, cd += 1)
                {
                    memcpy(dst0, src0 + 0 * spatial, sizeof(T) * spatial);
                    memcpy(dst1, src0 + 1 * spatial, sizeof(T) * spatial);
                    src0 += 2 * spatial;
                    dst0 += spatial;
                    dst1 += spatial;
                }
                for (size_t cs = 0; cs < srcC1; cs += 2, cd += 1)
                {
                    memcpy(dst0, src1 + 0 * spatial, sizeof(T) * spatial);
                    memcpy(dst1, src1 + 1 * spatial, sizeof(T) * spatial);
                    src1 += 2 * spatial;
                    dst0 += spatial;
                    dst1 += spatial;
                }
            }
        }

#ifdef SYNET_SIMD_LIBRARY_ENABLE
        template <> SYNET_INLINE void ShuffleLayerForwardCpu<float>(const float * src0, size_t srcC0, const float * src1, size_t srcC1, size_t spatial, float * dst0, float * dst1, size_t dstC, TensorFormat format)
        {
            ::SimdSynetShuffleLayerForward(src0, srcC0, src1, srcC1, spatial, dst0, dst1, dstC, (::SimdTensorFormatType)format);
        }
#endif
    }

    template <class T> class ShuffleLayer : public Synet::Layer<T>
    {
    public:
        typedef T Type;
        typedef Layer<T> Base;
        typedef typename Base::TensorPtrs TensorPtrs;

        ShuffleLayer(const LayerParam & param)
            : Base(param)
        {
        }

        virtual void Reshape(const TensorPtrs & src, const TensorPtrs & buf, const TensorPtrs & dst)
        {
            assert(src.size() == 2 && src[0]->Count() == 4 && src[1]->Count() == 4 && src[0]->Format() == src[1]->Format());
            _format = src[0]->Format();
            const Shape & srcShape0 = src[0]->Shape();
            const Shape & srcShape1 = src[1]->Shape();
            _batch = srcShape0[0];
            assert(srcShape0[0] == srcShape1[0]);
            Shape dstShape = srcShape0;
            if (_format == TensorFormatNhwc)
            {
                _srcC0 = srcShape0[3];
                _srcC1 = srcShape1[3];
                _dstC = (_srcC0 + _srcC1) / 2;
                assert(_srcC0  + _srcC1 == _dstC*2);
                dstShape[3] = _dstC;
                _spatial = srcShape0[1] * srcShape0[2];
                assert(srcShape0[1] == srcShape1[1] && srcShape0[2] == srcShape1[2]);
            }
            else
            {
                _srcC0 = srcShape0[1];
                _srcC1 = srcShape1[1];
                _dstC = (_srcC0 + _srcC1) / 2;
                assert(_srcC0 + _srcC1 == _dstC * 2);
                dstShape[1] = _dstC;
                _spatial = srcShape0[2] * srcShape0[3];
                assert(srcShape0[2] == srcShape1[2] && srcShape0[3] == srcShape1[3]);
            }
            dst[0]->Reshape(dstShape, _format);
            dst[1]->Reshape(dstShape, _format);
        }

    protected:
        virtual void ForwardCpu(const TensorPtrs & src, const TensorPtrs & buf, const TensorPtrs & dst)
        {
            SYNET_PERF_FUNC();

            const Type * src0 = src[0]->CpuData();
            const Type * src1 = src[1]->CpuData();
            Type * dst0 = dst[0]->CpuData();
            Type * dst1 = dst[1]->CpuData();
            for(size_t b = 0; b < _batch; ++b)
            {
                Detail::ShuffleLayerForwardCpu(src0, _srcC0, src1, _srcC1, _spatial, dst0, dst1, _dstC, _format);
                src0 += _srcC0*_spatial;
                src1 += _srcC1*_spatial;
                dst0 += _dstC*_spatial;
                dst1 += _dstC*_spatial;
            }
        }
    private:
        TensorFormat _format;
        size_t _batch, _srcC0, _srcC1, _dstC, _spatial;
    };
}