// Copyright (C) 2019. Huawei Technologies Co., Ltd. All rights reserved.

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "tensor_computing.h"
#ifdef _USE_GENERAL
#include "cpu/general/tensor_computing_general.h"
#endif
#ifdef _USE_NEON
#include "cpu/arm/tensor_computing_arm.h"
#endif
#ifdef _USE_MALI 
#include "gpu/mali/tensor_computing_mali.h"
#endif

EE softmax(TensorDesc inputDesc, const void* input, int axis, TensorDesc outputDesc, void* output, Arch arch, ExtInfo_t extInfo)
{
    EE ret = NOT_SUPPORTED;
    if (arch == CPU_GENERAL) {
#ifdef _USE_GENERAL
        ret = softmax_general(inputDesc, input, axis, outputDesc, output);
#endif
#ifdef _USE_NEON
    } else if (arch == ARM_A55 || arch == ARM_A76 || arch == ARM_V8 || arch == ARM_V7) {
        ret = softmax_arm(inputDesc, input, axis, outputDesc, output);
#endif
#ifdef _USE_MALI
    } else if (arch == MALI) {
        ret = softmax_mali(extInfo->maliInfo.handle, inputDesc, (GCLMem_t)input, axis, outputDesc, (GCLMem_t)output);
#endif
    }
    return ret;
}

inline EE softmax_infer_output_size_cpu(TensorDesc inputDesc, TensorDesc *outputDesc)
{
    if (nullptr == outputDesc) CHECK_STATUS(NULL_POINTER);
    *outputDesc = inputDesc;
    if (DF_NCHWC8 == (*outputDesc).df) {
         (*outputDesc).df = DF_NCHW;
    }
    return SUCCESS;
}

EE softmax_infer_output_size(TensorDesc inputDesc, TensorDesc *outputDesc, Arch arch, ExtInfo_t extInfo)
{
    EE ret = NOT_SUPPORTED;
    if(arch == MALI){
#ifdef _USE_MALI
        ret = softmax_infer_output_size_mali(inputDesc, outputDesc, extInfo->maliInfo.gclmemInputDesc, extInfo->maliInfo.gclmemOutputDesc);
#endif
    } else {
        ret = softmax_infer_output_size_cpu(inputDesc, outputDesc);
    }    
    return ret;
}
