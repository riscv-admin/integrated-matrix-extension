#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <iostream>
#include <iomanip>

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

const u32 COUNT = 1;

const u32 debug = 0;

class RV_t
{
    public:
        virtual void vfmmacc(u32 vd, u32 vs1, u32 vs2) = 0;
        virtual void vfmacc(u32 vl, u32 vd, double alpha, u32 vs2) = 0;
        virtual void vxor(u32 vd, u32 vs1, u32 vs2) = 0;
        virtual void vle64(u32 vl, u32 vd, double *A) = 0;
        virtual void vse64(u32 vl, u32 vs, double *A) = 0;
        virtual u32& SEW() = 0;
        virtual u32& LMUL() = 0;
        virtual s64& X(u32 rs) = 0;
        virtual u32  VLEN() const = 0;
        virtual u32& VL() = 0;
        virtual u32  lambda() const = 0;
        virtual u32  VLENE() const = 0;
        virtual u32  sigma() const = 0;
};

template<u32 VLEN_, u32 lambda_>
class RVIME_t : public RV_t
{
    private:
        u32     SEW_;
        u32     LMUL_;
        u32     RMUL_;
        u32     CMUL_;
        u32     VL_;
        bool    altfmt_;
        union
        {
            double f64[VLEN_/64];
            float  f32[VLEN_/32];
            u64    i64[VLEN_/64];
            u32    i32[VLEN_/32];
            u16    i16[VLEN_/16];
        } VR[32];
        s64 X_[32];

        void vfmmacc_fp64(u32 vd, u32 vs1, u32 vs2)
        {
            u32 L = VLEN_/SEW_;
            u32 m, n = m = L/lambda_;
            u32 mul = m/lambda_;
            if (debug > 0)
            {
                std::cout << "Executing double-precision vfmmacc : " << "vd = " << vd << ", vs1 = " << vs1 << ", vs2 = " << vs2 << ", L = " << L << ", m = " << m << ", n = " << n << ", lambda = " << lambda_ << std::endl;
            }

            for (u32 k=0; k<lambda_; k++)
            {
                for (u32 i=0; i<m; i++)
                    for (u32 j=0; j<n; j++)
                    {
                        if (debug > 0)
                        {
                            std::cout << "Computing VR[" << vd + j/lambda_ << "].f64[" << j%lambda_ + i*lambda_ << "] += VR[" << vs1 << "].f64[" << k + i*lambda_ << "] * VR[" << vs2 << "].f64[" << k + j*lambda_ << "]" << std::endl;
                        }
                        VR[vd + j/lambda_].f64[j%lambda_ + i*lambda_] += VR[vs1].f64[k + i*lambda_] * VR[vs2].f64[k + j*lambda_]; 
                        if (debug > 0)
                        {
                            std::cout << "VR[" << vs1 << "].f64[" << k + i*lambda_ << "] = " << VR[vs1].f64[k + i*lambda_] << std::endl;
                            std::cout << "VR[" << vs2 << "].f64[" << k + j*lambda_ << "] = " << VR[vs2].f64[k + j*lambda_] << std::endl;
                            std::cout << "VR[" << vd + j/lambda_ << "].f64[" << j%lambda_ + i*lambda_ << "] = " << VR[vd + j/lambda_].f64[j%lambda_ + i*lambda_] << std::endl;
                        }
                    }
            }
        }

        void vxor_i64(u32 vd, u32 vs1, u32 vs2)
        {
            u32 L = VLEN_/SEW_;
            for (u32 i=0; i<L; i++) VR[vd].i64[i] = VR[vs1].i64[i] ^ VR[vs2].i64[i];
        }

    public:
        u32& SEW()
        {
            return SEW_;
        }

        u32& LMUL()
        {
            return LMUL_;
        }

        u32& RMUL()
        {
            return RMUL_;
        }

        u32& CMUL()
        {
            return CMUL_;
        }

        u32 VLEN() const
        {
            return VLEN_;
        }

        u32& VL()
        {
            return VL_;
        }

        u32 lambda() const
        {
            return lambda_;
        }

        u32 sigma() const
        {
            u32 L = VLEN_/SEW_;
            return L/lambda_;
        }

        s64& X(u32 rs)
        {
            assert(rs < 32);
            return X_[rs];
        }

        void vfmmacc(u32 vd, u32 vs1, u32 vs2)
        {
            u32 B = VLENE()/(lambda() * lambda());
            switch(SEW_)
            {
                case 64:
                    if (debug > 0)
                    {
                        std::cout << "Each basic vfmmacc produces " << B << " vector registers of output" << std::endl;
                    }
                    //for (u32 i=0; i<RMUL(); i++) for (u32 j=0; j<CMUL(); j++)
                    //    vfmmacc_fp64(vd + B*i + B*j*RMUL(), vs1 + i, vs2 + j);
                    vfmmacc_fp64(vd, vs1, vs2);
                    break;
                default:
                    assert(false);
            }
        }

        void vxor(u32 vd, u32 vs1, u32 vs2)
        {
            switch(SEW_)
            {
                case 64:
                    vxor_i64(vd, vs1, vs2);
                    break;
                default:
                    assert(false);
            }
        }

        void vfmacc(u32 vl, u32 vd, double alpha, u32 vs2)
        {
            assert(vl <= VLENE());
            if (debug > 1) { std::cout << "Computing VR[" << vd << "] += " << alpha << " * VR[" << vs2 << "]" << std::endl; }
            for (u32 i=0; i<vl; i++) VR[vd].f64[i] += alpha * VR[vs2].f64[i];
        }

        void vle64(u32 vl, u32 vd, double *A)
        {
            assert(vl <= VLENE());
            if (debug > 2) { std::cout << "Loading VR[" << vd << "], vl = " << vl << std::endl; }
            for (u32 i=0; i<vl; i++) VR[vd].f64[i] = A[i];
        }

        void vse64(u32 vl, u32 vs, double *A)
        {
            assert(vl <= VLENE());
            if (debug > 2) { std::cout << "Storing VR[" << vs << "], vl = " << vl << std::endl; }
            for (u32 i=0; i<vl; i++) A[i] = VR[vs].f64[i];
        }

        u32 VLENE() const
        {
            return VLEN_/SEW_;
        }
};

RV_t *RV = nullptr;

class vsetvli_t
{
    public:
        void operator()(u32 rd, u32 rs1, u32 sew, u32 lmul, u32 ta, u32 ma)
        {
            if (0 == rs1)
            {
                assert(0 == rs1);
                RV->SEW() = sew;
                RV->LMUL() = lmul;
                RV->VL() = lmul*RV->VLENE();
                RV->X(rd) = RV->VL();
            }
            else
            {
                assert(rs1 <= lmul*RV->VLENE());
                RV->SEW() = sew;
                RV->LMUL() = lmul;
                RV->VL() = rs1;
                RV->X(rd) = RV->VL();
            }
        }
};

class vsetvl_t
{
    public:
        void operator()(u32 rd, u32 rs1, u32 sew, u32 lmul, u32 ta, u32 ma)
        {
            assert(0 == rs1);
            RV->SEW() = sew;
            RV->LMUL() = lmul;
            RV->VL() = lmul*RV->VLENE();
            RV->X(rd) = RV->VL();
        }
};

class vfmmacc_t
{
    public:
        void vv(u32 vd, u32 vs1, u32 vs2)
        {
            return RV->vfmmacc(vd, vs1, vs2);
        }
};

class vxor_t
{
    public:
        void vv(u32 vd, u32 vs1, u32 vs2)
        {
            return RV->vxor(vd, vs1, vs2);
        }
};

u32 min(u32 a, u32 b)
{
    return (a < b) ? a : b;
}

class vfmacc_t
{
    public:
        void vf(u32 vd, double alpha, u32 vs2)
        {
            assert(64 == RV->SEW());
            assert(RV->VL() <= RV->VLENE() * RV->LMUL());
            u32 vl = RV->VL();
            for (u32 i=0; i<RV->LMUL(); i++)
            {
                RV->vfmacc(min(vl, RV->VLENE()), vd + i, alpha, vs2 + i);
                vl = (vl < RV->VLENE()) ? 0 : vl - RV->VLENE();
            }
        }
};

class vle64_t
{
    public:
        void v(u32 vd, double *A)
        {
            assert(64 == RV->SEW());
            assert(RV->VL() <= RV->VLENE() * RV->LMUL());
            u32 vl = RV->VL();
            for (u32 i=0; i<RV->LMUL(); i++)
            {
                RV->vle64(min(vl, RV->VLENE()), vd + i, A + i*RV->VLENE());
                vl = (vl < RV->VLENE()) ? 0 : vl - RV->VLENE();
            }
            return;
        }
};

class vse64_t
{
    public:
        void v(u32 vs, double *A)
        {
            assert(64 == RV->SEW());
            assert(RV->VL() <= RV->VLENE() * RV->LMUL());
            u32 vl = RV->VL();
            for (u32 i=0; i<RV->LMUL(); i++)
            {
                RV->vse64(min(vl, RV->VLENE()), vs + i, A + i*RV->VLENE());
                vl = (vl < RV->VLENE()) ? 0 : vl - RV->VLENE();
            }
            return;
        }
};

vfmmacc_t vfmmacc;
vfmacc_t  vfmacc;
vsetvli_t vsetvli;
vsetvl_t  vsetvl;
vxor_t    vxor;
vle64_t   vle64;
vse64_t   vse64;

void microgemm
(
    u32     K,
    double *A,
    double *B,
    double  alpha,
    double *C,
    u32     M,
    u32     N,
    u32     rmul,
    u32     cmul
)
{
    assert(0 == K % RV->lambda());                      // For simplicty, K must be a multiple of lambda
    u32 mul = RV->VLENE()/(RV->lambda() * RV->lambda());  // mul = # of output registers in a basic vfmmacc instruction

    vsetvli(5, 0, 64, 1, true, true);                   // Initialize the vtype register
    for (u32 r=16; r<32; r++) vxor.vv(r, r, r);         // T = 0

    // We have 5 possible variants of the code, depending on "mul"
    // (the number of output registers per basic vfmmacc)
    // mul = 1, 2, 4, 8, 16

    vsetvli(5, 0, 64, 1, true, true);
    if (mul == 16) {
        assert(rmul == 1);
        assert(cmul == 1);
        u64 stepA = M * RV->lambda();                 // Precompute the A panel step
        u64 stepB = N * RV->lambda();
        for (u32 k=0; k<K; k=k+RV->lambda())
        {
            vle64.v(0, A); A += stepA;
            vle64.v(8, B); B += stepB;
            vfmmacc.vv(16, 0, 8);
        }
    } else if (mul == 8) {
        assert(rmul == 2);
        assert(cmul == 1);
        u64 stepA = M/2 * RV->lambda();
        u64 stepB = N * RV->lambda();
        for (u32 k=0; k<K; k=k+RV->lambda())
        {
            vle64.v(0, A); A += stepA;
            vle64.v(8, B); B += stepB;
            vfmmacc.vv(16, 0, 8);
            vle64.v(1, A); A += stepA;
            vfmmacc.vv(24, 1, 8);
        }
    } else if (mul == 4) {
        assert(rmul == 2);
        assert(cmul == 2);
        u64 stepA = M/2 * RV->lambda();
        u64 stepB = N/2 * RV->lambda();
        for (u32 k=0; k<K; k=k+RV->lambda())
        {
            vle64.v(0, A); A += stepA;
            vle64.v(8, B); B += stepB;
            vfmmacc.vv(16, 0, 8);
            vle64.v(1, A); A += stepA;
            vfmmacc.vv(20, 1, 8);
            vle64.v(9, B); B += stepB;
            vfmmacc.vv(24, 0, 9);
            vfmmacc.vv(28, 1, 9);
        }
    } else if (mul == 2) {
        assert(rmul == 4);
        assert(cmul == 2);
        u64 stepA = M/4 * RV->lambda();
        u64 stepB = N/2 * RV->lambda();
        for (u32 k=0; k<K; k=k+RV->lambda())
        {
            vle64.v(0, A); A += stepA;
            vle64.v(8, B); B += stepB;
            vfmmacc.vv(16, 0, 8);
            vle64.v(1, A); A += stepA;
            vfmmacc.vv(18, 1, 8);
            vle64.v(2, A); A += stepA;
            vfmmacc.vv(20, 2, 8);
            vle64.v(3, A); A += stepA;
            vfmmacc.vv(22, 3, 8);
            vle64.v(9, B); B += stepB;
            vfmmacc.vv(24, 0, 9);
            vfmmacc.vv(26, 1, 9);
            vfmmacc.vv(28, 2, 9);
            vfmmacc.vv(30, 3, 9);
        }
    } else if (mul == 1) {
        assert(rmul == 4);
        assert(cmul == 4);
        u64 stepA = M/4 * RV->lambda();
        u64 stepB = N/4 * RV->lambda();
        for (u32 k=0; k<K; k=k+RV->lambda())
        {
            vle64.v(0, A); A += stepA;
            vle64.v(8, B); B += stepB;
            vfmmacc.vv(16, 0, 8);
            vle64.v(1, A); A += stepA;
            vfmmacc.vv(17, 1, 8);
            vle64.v(2, A); A += stepA;
            vfmmacc.vv(18, 2, 8);
            vle64.v(3, A); A += stepA;
            vfmmacc.vv(19, 3, 8);
            vle64.v(9, B); B += stepB;
            vfmmacc.vv(20, 0, 9);
            vfmmacc.vv(21, 1, 9);
            vfmmacc.vv(22, 2, 9);
            vfmmacc.vv(23, 3, 9);
            vle64.v(10, B); B += stepB;
            vfmmacc.vv(24, 0, 10);
            vfmmacc.vv(25, 1, 10);
            vfmmacc.vv(26, 2, 10);
            vfmmacc.vv(27, 3, 10);
            vle64.v(11, B); B += stepB;
            vfmmacc.vv(28, 0, 11);
            vfmmacc.vv(29, 1, 11);
            vfmmacc.vv(30, 2, 11);
            vfmmacc.vv(31, 3, 11);
        }
    } else {
        assert(false);
    }

    vsetvli(5, 0, 64, 1, true, true);
    for (u32 r=0; r<16; r++)
    {
        u32 block = r/mul;
        u32 col = block/rmul;
        u32 row = block%rmul;
        u32 displ = (col*rmul*mul + row + (r%mul)*rmul)*RV->VL();
        if (debug > 1) std::cout << "Reading VR[" << r << "] from C[" << displ << "]" << std::endl;
        vle64.v(r, C + displ);
        vfmacc.vf(r, alpha, r+16);
        if (debug > 1) std::cout << "Writing VR[" << r << "]  to  C[" << displ << "]" << std::endl;
        vse64.v(r, C + displ);
    }
}

double now()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1.0e-9;
}

void packfp64
(
    double *P,
    double *A,
    u32    sigma,
    u32    lambda,
    u32    K,
    u32    mul
)
{
    assert(0 == K % lambda);                            // For simplicity, K must be a multiple of lambda

    u32 mu = sigma*mul;
    vsetvli(5, lambda, 64, 1, true, true);
    for (u32 k=0; k<K; k+=lambda)
    {
        for (u32 i=0; i<mu; i++)
        {
            vle64.v(0, A + k + i*K);
            vse64.v(0, P + k*mul*sigma + i*lambda); 
        }
    }
}

void unpackfp64
(
     double *A,
     double *P,
     u32    sigma,
     u32    lambda,
     u32    K,
     u32    mul
)
{
    assert(0 == K % lambda);                            // For simplicity, K must be a multiple of lambda

    u32 mu = sigma*mul;
    vsetvli(5, lambda, 64, 1, true, true);
    for (u32 k=0; k<K; k+=lambda)
    {
        for (u32 i=0; i<mu; i++)
        {
            vle64.v(0, P + k*mul*sigma + i*lambda);
            vse64.v(0, A + k + i*K); 
        }
    }
}

template<u32 VLEN, u32 lambda>
bool run_microgemm
(
    u32 K
)
{
    RVIME_t<VLEN, lambda> RVIME; RV = &RVIME;

    // Find the geometry of the microgemm
    vsetvli(5, 0, 64, 1, true, true);
    u32 L = RV->VLENE();
    assert(L >= lambda);
    assert(L/(lambda*lambda) >=  1);
    assert(L/(lambda*lambda) <= 16);

    u32 rmul = 1;
    u32 cmul = 1;
    while (rmul * cmul * L/(lambda*lambda) < 16)
    {
        if (rmul > cmul) cmul = cmul*2;
        else rmul = rmul * 2;
    }
    std::cout << "L = " << std::setw(2) << L << ", lambda = " << std::setw(2) << RV->lambda() << ", sigma = " << std::setw(2) << RV->sigma() << ", RMUL = " << rmul << ", CMUL = " << cmul;

    u32 mu = rmul*RV->sigma();
    u32 nu = cmul*RV->sigma();

    std::cout << ", microgemm geometry : " << std::setw(2) << mu << " x " << std::setw(2) << nu << std::endl;

    u32 M = mu;
    u32 N = nu;

    double alpha = 3.141592654;

    // Allocate A, B, and C panels
    double *A = new double[M*K]; for (u32 i=0; i<M*K; i++) A[i] = drand48() - 0.5;
    double *B = new double[K*N]; for (u32 i=0; i<K*N; i++) B[i] = drand48() - 0.5;
    double *C = new double[M*N]; for (u32 i=0; i<M*N; i++) C[i] = drand48() - 0.5;
    double *D = new double[M*N]; for (u32 i=0; i<M*N; i++) D[i] = 0.0;

    // Allocate the packed panels
    double *Ap = new double[M*K];
    double *Bp = new double[K*N];
    double *Cp = new double[M*N];

    // Pack the A and B panels
    //vsetmul(rmul,cmul);
    packfp64(Ap, A, RV->sigma(), RV->lambda(), K, rmul);
    packfp64(Bp, B, RV->sigma(), RV->lambda(), K, cmul);

    // We also need to pack the C panel
    packfp64(Cp, C, RV->sigma(), RV->lambda(), N, rmul);

    // Invoke the microgemm kernel
    microgemm(K, Ap, Bp, alpha, Cp, M, N, rmul, cmul);

    // Unpack the results
    unpackfp64(D, Cp, RV->sigma(), RV->lambda(), N, rmul);

    // Check the result
    for (u32 j=0; j<N; j++)
        for (u32 i=0; i<M; i++)
        {
            double S = 0;
            for (u32 k=0; k<K; k++)
            {
                if (debug > 1)
                {
                    if ((2 == i) && (0 == j))
                        std::cout << "A[" << i << ", " << k << "] = " << A[k+i*K] << ", B[" << k << ", " << j << "] = " << B[k+j*K] << std::endl;
                }
                S += A[k+i*K]*B[k+j*K];
            }
            if ((alpha*S + C[j+i*N]) != D[j+i*N])
            {
                std::cout << "Error for D[" << i << "," << j << "] = " << D[i+j*M] << " != " << S << std::endl;
                exit(-1);
            }
        }

    // Free the panels
    delete [] Cp;
    delete [] Bp;
    delete [] Ap;
    delete [] D;
    delete [] C;
    delete [] B;
    delete [] A;

    // Successful execution
    return true;
}

int main
(
    int         argc,
    char      **argv
)
{
    std::cout << "=========================================================================================================================" << std::endl;
    run_microgemm<  64, 1>(1);
    run_microgemm<  64, 1>(2);
    run_microgemm<  64, 1>(4);
    run_microgemm<  64, 1>(8);
    run_microgemm< 128, 1>(1);
    run_microgemm< 128, 1>(2);
    run_microgemm< 128, 1>(4);
    run_microgemm< 128, 1>(8);
    run_microgemm< 256, 1>(1);
    run_microgemm< 256, 1>(2);
    run_microgemm< 256, 1>(4);
    run_microgemm< 256, 1>(8);
    run_microgemm< 256, 2>(2);
    run_microgemm< 256, 2>(4);
    run_microgemm< 256, 2>(8);
    run_microgemm< 512, 1>(1);
    run_microgemm< 512, 1>(2);
    run_microgemm< 512, 1>(4);
    run_microgemm< 512, 1>(8);
    run_microgemm< 512, 2>(2);
    run_microgemm< 512, 2>(4);
    run_microgemm< 512, 2>(8);
    run_microgemm<1024, 1>(1);
    run_microgemm<1024, 1>(2);
    run_microgemm<1024, 1>(4);
    run_microgemm<1024, 1>(8);
    run_microgemm<1024, 2>(2);
    run_microgemm<1024, 2>(4);
    run_microgemm<1024, 2>(8);
    run_microgemm<1024, 4>(4);
    run_microgemm<1024, 4>(8);
    run_microgemm<16384, 4>(8);

    return 0;
}
