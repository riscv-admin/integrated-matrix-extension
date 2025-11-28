#include <stdint.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <omp.h>
#include <vector>

// helper macros
#define EXPAND(x)  _EXPAND(x)
#define _EXPAND(x) #x

// assembly instructions
#define li(RT, SI)			asm("li " #RT "," EXPAND(SI))
#define addi(RT, RA, SI)		asm("addi " #RT ", " #RA ", " EXPAND(SI))
#define lxvp(XT, RA, D)			asm("lxvp " #XT "," #D "(" #RA ")")
#define stxvp(XS, RA, D)		asm("stxvp " #XS "," #D "(" #RA ")")
#define xvf64ger(AT, XA, XB)		asm("xvf64ger " #AT "," #XA "," #XB)
#define xvf64gerpp(AT, XA, XB)		asm("xvf64gerpp " #AT "," #XA "," #XB)
#define xxpermdi(XT, XA, XB, IM)	asm("xxpermdi " #XT "," #XA "," #XB "," #IM)
#define xxmfacc(XT)			asm("xxmfacc " #XT)
#define xvadddp(XT, XA, XB)		asm("xvadddp " #XT "," #XA "," #XB)
#define nop()				asm("nop ")

void memory_load_1KiB
(
    double   *A,
    uint32_t  count
)
{
    asm("mtctr 4");
    asm("nop");
    asm("LOOP17:");

    lxvp( 0+ 0, 3,   0+  0); lxvp( 0+ 2, 3,  32+  0);
    lxvp( 0+ 4, 3,  64+  0); lxvp( 0+ 6, 3,  96+  0);
    lxvp( 0+ 8, 3, 128+  0); lxvp( 0+10, 3, 160+  0);
    lxvp( 0+12, 3, 192+  0); lxvp( 0+14, 3, 224+  0);
    
    lxvp(16+ 0, 3,   0+256); lxvp(16+ 2, 3,  32+256);
    lxvp(16+ 4, 3,  64+256); lxvp(16+ 6, 3,  96+256);
    lxvp(16+ 8, 3, 128+256); lxvp(16+10, 3, 160+256);
    lxvp(16+12, 3, 192+256); lxvp(16+14, 3, 224+256);
    
    lxvp(32+ 0, 3,   0+512); lxvp(32+ 2, 3,  32+512);
    lxvp(32+ 4, 3,  64+512); lxvp(32+ 6, 3,  96+512);
    lxvp(32+ 8, 3, 128+512); lxvp(32+10, 3, 160+512);
    lxvp(32+12, 3, 192+512); lxvp(32+14, 3, 224+512);
    
    lxvp(48+ 0, 3,   0+768); lxvp(48+ 2, 3,  32+768);
    lxvp(48+ 4, 3,  64+768); lxvp(48+ 6, 3,  96+768);
    lxvp(48+ 8, 3, 128+768); lxvp(48+10, 3, 160+768);
    lxvp(48+12, 3, 192+768); lxvp(48+14, 3, 224+768);

    addi(3, 3, 1024);
    asm("bdnz LOOP17");
}

double now()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1.0e-9;
}

double run_kernel
(
    void (kernel)(double*, uint32_t),
    volatile uint32_t count,
    volatile uint32_t N,
    volatile uint32_t n
)
{
    int nthreads = omp_get_max_threads();
    std::vector<double*> A(nthreads);
    for (int j=0; j<nthreads; j++)
    {
	A[j] = (double*)aligned_alloc(4096, sizeof(double) * N); 
	for (uint32_t i=0; i<N; i++) A[j][i] = drand48() - 0.5;
    }
    uint32_t r = N/n;

    volatile double start, finish;

    start = now();
#pragma omp parallel for
    for (int j=0; j<nthreads; j++)
    {
	for(uint32_t reps = count; reps; reps--)
	{
	    kernel(A[j], r);
	}
    }
    finish = now();

    for (int j=0; j<nthreads; j++)
    {
	free(A[j]);
    }

    return (finish - start);
}

void run_kernel_and_report
(
    void (kernel)(double*, uint32_t),
    volatile uint32_t count,
    const char* name,
    volatile uint32_t N,
    volatile uint32_t n
)
{
    volatile double elapsed;
    volatile double GB = (1.0e-09)*N*sizeof(double)*count*omp_get_max_threads();
    elapsed = run_kernel(kernel, count, N, n);
    std::cout << std::setprecision(6);
    std::cout << "Time to run " << std::setw(30) << name << " (" << std::setw(9) << N << " doubles) " << std::setw(9) << count << " times = " << std::setw(10) << std::fixed << elapsed << " seconds (" << std::setw(10) << std::scientific << GB/elapsed << " GB/s)" << std::endl;
}

#define RUN_KERNEL(kernel, count, N, n) run_kernel_and_report(kernel, count, #kernel, N, n)

int main
(
    int		argc,
    char      **argv
)
{
    const uint32_t memory_load_count   =  1024*1024*128;

    volatile double elapsed;
    
    int nthreads = omp_get_max_threads();
    std::cout << "=========================================================================================================================" << std::endl;
    std::cout << "Running on " << nthreads << ((nthreads > 1) ? " threads" : " thread") << std::endl;

    for (uint32_t i=1; i<1000000; i *= 2) 
	RUN_KERNEL(memory_load_1KiB, memory_load_count/i, i*1024, 128);

    return 0;
}
