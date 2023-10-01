#ifndef BLASENG_MKL_H_INCLUDED
#define BLASENG_MKL_H_INCLUDED 1

#if INDEX_T == INT64
#define MKL_ILP64 1
#endif
#include "mkl.h"
#define ALIGNED_MALLOC(sz) mkl_malloc(sz, 64)
#define ALIGNED_CALLOC(nbr, sz) mkl_calloc(nbr, sz, 64)
#define ALIGNED_FREE(ptr) mkl_free(ptr)

#if FIELD_T == FLT32
#define DOT cblas_sdot
#define GEMV cblas_sgemv
#define GEMM cblas_sgemm
#define GER cblas_sger
#define AXPY cblas_saxpy
#define SCAL cblas_sscal
#define ASUM cblas_sasum
#define NRM2 cblas_snrm2
#define VMUL vsMul
#define VDIV vsDiv
#define VMULI vsMulI
#define VADD vsAdd
#define VSUB vsSub
#define VSUBI vsSubI
#define VSQR vsSqr
#define VSQRT vsSqrt
#define VEXP vsExp
#define VINV vsInv
#define VTANH vsTanh
#define VFMAXI vsFmaxI
#define OMAT mkl_somatcopy
#define IMAT mkl_simatcopy
#define VCOPYSIGNI vsCopySignI
#elif FIELD_T == FLT64
#define DOT cblas_ddot
#define GEMV cblas_dgemv
#define GEMM cblas_dgemm
#define GER cblas_dger
#define AXPY cblas_daxpy
#define SCAL cblas_dscal
#define ASUM cblas_dasum
#define NRM2 cblas_dnrm2
#define VMUL vdMul
#define VDIV vdDiv
#define VMULI vdMulI
#define VADD vdAdd
#define VSUB vdSub
#define VSUBI vdSubI
#define VSQR vdSqr
#define VSQRT vdSqrt
#define VEXP vdExp
#define VINV vdInv
#define VTANH vdTanh
#define VFMAXI vdFmaxI
#define OMAT mkl_domatcopy
#define IMAT mkl_dimatcopy
#define VCOPYSIGNI vdCopySignI
#endif /* FEILD_T */

#endif /* BLASENG_MKL_H_INCLUDED */