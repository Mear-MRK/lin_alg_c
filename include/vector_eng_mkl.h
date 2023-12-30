#ifndef vector_eng_MKL_H_INCLUDED
#define vector_eng_MKL_H_INCLUDED 1

#ifdef IND_INT64
#define MKL_ILP64 1
#endif
#include "mkl.h"

#ifdef FLD_FLT32
#define COPY cblas_scopy
#define DOT cblas_sdot
#define GEMV cblas_sgemv
#define GEMM cblas_sgemm
#define GER cblas_sger
#define AXPY cblas_saxpy
#define SCAL cblas_sscal
#define ASUM cblas_sasum
#define NRM2 cblas_snrm2
#define VMUL vsMul
#define VMULI vsMulI
#define VDIV vsDiv
#define VDIVI vsDivI
#define VADD vsAdd
#define VADDI vsAddI
#define VSUB vsSub
#define VSUBI vsSubI
#define VSQR vsSqr
#define VSQRI vsSqrI
#define VSQRT vsSqrt
#define VSQRTI vsSqrtI
#define VEXP vsExp
#define VEXPI vsExpI
#define VLNI vsLnI
#define VLOG2I vsLog2I
#define VINVI vsInvI
#define VTANHI vsTanhI
#define VFMAXI vsFmaxI
#define OMAT mkl_somatcopy
#define IMAT mkl_simatcopy
#define VCOPYSIGNI vsCopySignI
#elif defined(FLD_FLT64)
#define COPY cblas_dcopy
#define DOT cblas_ddot
#define GEMV cblas_dgemv
#define GEMM cblas_dgemm
#define GER cblas_dger
#define AXPY cblas_daxpy
#define SCAL cblas_dscal
#define ASUM cblas_dasum
#define NRM2 cblas_dnrm2
#define VMUL vdMul
#define VMULI vdMulI
#define VDIV vdDiv
#define VDIVI vdDivI
#define VADD vdAdd
#define VADDI vdAddI
#define VSUB vdSub
#define VSUBI vdSubI
#define VSQR vdSqr
#define VSQRI vdSqrI
#define VSQRT vdSqrt
#define VSQRTI vdSqrtI
#define VEXP vdExp
#define VEXPI vdExpI
#define VLNI vdLnI
#define VLOG2I vdLog2I
#define VINVI vdInvI
#define VTANHI vdTanhI
#define VFMAXI vdFmaxI
#define OMAT mkl_domatcopy
#define IMAT mkl_dimatcopy
#define VCOPYSIGNI vdCopySignI
#endif 

#endif /* vector_eng_MKL_H_INCLUDED */