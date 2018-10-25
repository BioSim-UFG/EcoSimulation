#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cuda_runtime.h>

#define CUDA_ERROR_CHECK
//undefine to remove error checking

//static void CheckCudaErrorAux (const char *, unsigned, const char *, cudaError_t);



/* faz chamada de função da API CUDA com checagem de erro
 * ex: CudaSafeCall(cudamalloc(&addr, sizeof(float)*150));
 */
#define CudaSafeCall( err ) __cudaSafeCall( err, __FILE__, __LINE__ )


/* faz checagem de erro após uma chamada de kernel
 * ex: my_gpuFuction(arg1, arg2);
 * CudaCheckError();
 */
#define CudaCheckError()    __cudaCheckError( __FILE__, __LINE__ )


/* faz o mesmo que CudaSafeCall, porém o output é ligeiramente diferente
 */
#define CudaSafeCall2(value) CheckCudaErrorAux(__FILE__,__LINE__, #value, value)



inline void __cudaSafeCall( cudaError err, const char *file, const int line )
{
#ifdef CUDA_ERROR_CHECK
    if ( cudaSuccess != err ) {
        fprintf( stderr, "cudaSafeCall() failed at %s:%i : %s\n",
                 file, line, cudaGetErrorString( err ) );
        exit( -1 );
    }
#endif

    return;
}

inline void __cudaCheckError( const char *file, const int line )
{
#ifdef CUDA_ERROR_CHECK
    cudaError err = cudaGetLastError();
    if ( cudaSuccess != err ) {
        fprintf( stderr, "cudaCheckError() failed at %s:%i : %s\n",
                 file, line, cudaGetErrorString( err ) );
        exit( -1 );
    }

    // More careful checking. However, this will affect performance.
    // Comment away if needed.
    /*
    err = cudaDeviceSynchronize();
    if( cudaSuccess != err )
    {
        fprintf( stderr, "cudaCheckError() with sync failed at %s:%i : %s\n",
                 file, line, cudaGetErrorString( err ) );
        exit( -1 );
    }*/
#endif

    return;
}

/**
 * Check the return value of the CUDA runtime API call and exit
 * the application if the call has failed.
 */
static void CheckCudaErrorAux (const char *file, unsigned line, const char *statement, cudaError_t err)
{
    if (err == cudaSuccess)
        return;
    std::cerr << statement<<" returned " << cudaGetErrorString(err) << "("<<err<< ") at "<<file<<":"<<line << std::endl;
    exit (1);
}


//use with gpuErrchk(cudaPeekAtLastError()); ,  gpuErrchk( cudaDeviceSynchronize() );
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }

inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}