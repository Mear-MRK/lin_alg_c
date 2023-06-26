#ifndef LIN_ALG_CONFIG_H_INCLUDED
#define LIN_ALG_CONFIG_H_INCLUDED 1

#include <stddef.h>
#include <stdint.h>

#ifndef FEILD
#define FLT_TYPE float
#elif FEILD == FLT32
#define FLT_TYPE float
#elif FEILD == FLT64
#define FLT_TYPE double
#else
#define FLT_TYPE float
#endif /* FEILD */

#define MKL_ILP64 1
#include "mkl.h"

#define ALIGNED_MALLOC(sz) mkl_malloc(sz, 64)
#define ALIGNED_CALLOC(nbr, sz) mkl_calloc(nbr, sz, 64)
#define ALIGNED_FREE(ptr) mkl_free(ptr)

#if FLT_TYPE == float
#define DOT cblas_sdot
#define GEMV cblas_sgemv
#define GEMM cblas_sgemm
#define GER cblas_sger
#define AXPY cblas_saxpy
#define SCAL cblas_sscal
#define ASUM cblas_sasum
#define NRM2 cblas_snrm2
#define VMUL vsMul
#define VADD vsAdd
#define VSUB vsSub
#define VEXP vsExp
#define VINV vsInv
#define VTANH vsTanh
#define VFMAXI vsFmaxI
#define OMAT mkl_somatcopy
#define IMAT mkl_simatcopy 
#elif FLT_TYPE == double
#define DOT cblas_ddot
#define GEMV cblas_dgemv
#define GEMM cblas_dgemm
#define GER cblas_dger
#define AXPY cblas_daxpy
#define SCAL cblas_dscal
#define ASUM cblas_dasum
#define NRM2 cblas_dnrm2
#define VMUL vdMul
#define VADD vdAdd
#define VSUB vdSub
#define VEXP vdExp
#define VINV vdInv
#define VTANH vdTanh
#define VFMAXI vdFmaxI
#define OMAT mkl_domatcopy
#define IMAT mkl_dimatcopy 
#endif /* FLT_TYPE */



#endif /* LIN_ALG_CONFIG_H_INCLUDED */