#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <iostream>
#include <iomanip>

typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t  s32;
typedef int64_t  s64;

const u32 COUNT = 1;

const u32 debug = 0;

void print(u32, u32, double*);

class RV_t
{
    public:
        virtual void vfmmacc(u32 vd, u32 vs1, u32 vs2) = 0;
        virtual void vfmmacc_v0(u32 vd, u32 vs1, u32 vs2) = 0;
        virtual void vfmacc(u32 vl, u32 vd, double alpha, u32 vs2) = 0;
        virtual void vxor(u32 vd, u32 vs1, u32 vs2) = 0;
        virtual void vle64(u32 vl, u32 vd, double *A) = 0;
        virtual void vlte64(u32 vl, u32 vd, double *A, u32 stride) = 0;
        virtual void vse64(u32 vl, u32 vs, double *A) = 0;
        virtual void vste64(u32 vl, u32 vs, double *A, u32 stride) = 0;
        virtual void vmtlfre64(u32 vd, double *A, s32 stride) = 0;
        virtual void vmtsfre64(u32 vs, double *A, s32 stride) = 0;
        virtual void vmrotate(u32 vd, u32 vs1) = 0;
        virtual u32& SEW() = 0;
        virtual u32& LMUL() = 0;
        virtual u32& RMUL() = 0;
        virtual u32& CMUL() = 0;
        virtual s64& X(u32 rs) = 0;
        virtual u32  VLEN() const = 0;
        virtual u32& VL() = 0;
        virtual u32  lambda() const = 0;
        virtual u32  VLENE() const = 0;
        virtual u32  sigma() const = 0;
        virtual void printVRf64(u32 vs) = 0;
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
        } VR[32];
        s64 X_[32];

        void printVRf64(u32 vs)
        {
            assert(32 > vs);
            std::cout << "[" << std::endl;
            for (u32 i=0; i<sigma(); i++)
            {
                for (u32 j=0; j<lambda(); j++)
                {
                    std::cout << " " << std::setw(8) << VR[vs].f64[i*lambda() + j];
                }
                std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        void vmrotate(u32 vd, u32 vs1)
        {
            switch(SEW_)
            {
                case 64:
                    double f64[VLEN_/64];
                    for (u32 i=0; i<VLENE(); i++) f64[i] = VR[vs1].f64[(i + lambda()*lambda()) % VLENE()];
                    for (u32 i=0; i<VLENE(); i++) VR[vd].f64[i] = f64[i];
                    break;
                default:
                    assert(false);
            }
        }

        void vfmmacc_v0_fp64(u32 vd, u32 vs1, u32 vs2)
        {
            for (u32 i=0; i<sigma(); i++)
                for (u32 j=0; j<lambda(); j++)
                {
                    double S = 0.0;
                    for (u32 k=0; k<lambda(); k++) S += VR[vs1].f64[i*lambda() + k] * VR[vs2].f64[j*lambda() + k];
                    VR[vd].f64[i*lambda() + j] += S;
                }
        }

        void vfmmacc_fp64(u32 vd, u32 vs1, u32 vs2)
        {
            u32 L = VLEN_/SEW_;
            u32 m = L/lambda_;
            u32 n = m/lambda_;
            if (debug > 0)
            {
                std::cout << "Executing double-precision vfmmacc : " << "vd = " << vd << ", vs1 = " << vs1 << ", vs2 = " << vs2 << ", L = " << L << ", m = " << m << ", n = " << n << std::endl;
            }

            for (u32 k=0; k<lambda_; k++)
            {
                for (u32 i=0; i<m; i++)
                    for (u32 j=0; j<m; j++)
                    {
                        if (debug > 0)
                        {
                            std::cout << "Computing VR[" << vd + j/lambda_ << "].f64[" << i + m*(j%lambda_) << "] += VR[" << vs1 << "].f64[" << i + m*k << "] * VR[" << vs2 << "].f64[" << j + m*k << "]" << std::endl;
                        }
                        VR[vd + j/lambda_].f64[i + m*(j%lambda_)] += VR[vs1].f64[i + m*k] * VR[vs2].f64[j + m*k]; 
                        if (debug > 0)
                        {
                            std::cout << "VR[" << vs1 << "].f64[" << i + m*k << "] = " << VR[vs1].f64[i + m*k] << std::endl;
                            std::cout << "VR[" << vs2 << "].f64[" << j + m*k << "] = " << VR[vs2].f64[j + m*k] << std::endl;
                            std::cout << "VR[" << vd + j/lambda_ << "].f64[" << i + m*(j%lambda_) << "] = " << VR[vd + j/lambda_].f64[i + m*(j%lambda_)] << std::endl;
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
                    for (u32 i=0; i<RMUL(); i++) for (u32 j=0; j<CMUL(); j++)
                        vfmmacc_fp64(vd + B*i + B*j*RMUL(), vs1 + i, vs2 + j);
                    break;
                default:
                    assert(false);
            }
        }

        void vfmmacc_v0(u32 vd, u32 vs1, u32 vs2)
        {
            switch(SEW_)
            {
                case 64:
                    vfmmacc_v0_fp64(vd, vs1, vs2);
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

        void vlte64(u32 vl, u32 vd, double *A, u32 stride)
        {
            assert(vl == VLENE());
            if (debug > 2) { std::cout << "Loading VR[" << vd << "], vl = " << vl << ", sigma = " << sigma() << ", stride = " << stride << std::endl; }
            for (u32 col=0; col < lambda(); col++) for (u32 row=0; row < sigma(); row++) VR[vd].f64[col*sigma() + row] = A[col*stride + row];
        }

        void vse64(u32 vl, u32 vs, double *A)
        {
            assert(vl <= VLENE());
            if (debug > 2) { std::cout << "Storing VR[" << vs << "], vl = " << vl << std::endl; }
            for (u32 i=0; i<vl; i++) A[i] = VR[vs].f64[i];
        }

        void vste64(u32 vl, u32 vs, double *A, u32 stride)
        {
            assert(vl == VLENE());
            if (debug > 2) { std::cout << "Storing VR[" << vs << "], vl = " << vl << ", sigma = " << sigma() << ", stride = " << stride << std::endl; }
            for (u32 col=0; col < lambda(); col++) for (u32 row=0; row < sigma(); row++) A[col*stride + row] = VR[vs].f64[col*sigma() + row];
        }

        void vmtlfre64(u32 vd, double *A, s32 stride)
        {
            if (debug > 2) { std::cout << "Loading VR[" << vd << "], vl = " << VLENE() << ", sigma = " << sigma() << ", lambda = " << lambda() << ", stride = " << stride << std::endl; }
            for (u32 row=0; row < sigma(); row++) for (u32 col=0; col < lambda(); col++) VR[vd].f64[row*lambda() + col] = A[row*stride + col];
        }

        void vmtsfre64(u32 vs, double *A, s32 stride)
        {
            if (debug > 2) { std::cout << "Storing VR[" << vs << "], vl = " << VLENE() << ", sigma = " << sigma() << ", lambda = " << lambda() << ", stride = " << stride << std::endl; }
            for (u32 row=0; row < sigma(); row++) for (u32 col=0; col < lambda(); col++) A[row*stride + col] = VR[vs].f64[row*lambda() + col];
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

class vsetmul_t
{
    public:
        void operator()(u32 rmul, u32 cmul)
        {
            RV->CMUL() = cmul;
            RV->RMUL() = rmul;
        }
};

class vfmmacc_t
{
    public:
        void vv(u32 vd, u32 vs1, u32 vs2)
        {
            return RV->vfmmacc(vd, vs1, vs2);
        }

        void v0(u32 vd, u32 vs1, u32 vs2)
        {
            assert(64 == RV->SEW());
            assert(RV->VL() == RV->lambda() * RV->lambda());
            vs1 = (vs1 / RV->LMUL()) * RV->LMUL();
            vs2 = (vs2 / RV->LMUL()) * RV->LMUL();
            for (u32 i=0; i<RV->LMUL(); i++)
            {
                RV->vfmmacc_v0(vd, vs1+i, vs2+i);
            }
        }
};

class vmrotate_t
{
    public:
        void vv(u32 vd, u32 vs1)
        {
            assert(64 == RV->SEW());
            assert(RV->VL() == RV->lambda() * RV->lambda());
            vd  = (vd  / RV->LMUL()) * RV->LMUL();
            vs1 = (vs1 / RV->LMUL()) * RV->LMUL();
            for (u32 i=0; i<RV->LMUL(); i++)
            {
                RV->vmrotate(vd+i, vs1+i);
            }
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

class vlte64_t
{
    public:
        void v(u32 vd, double *A, u32 stride)
        {
            assert(64 == RV->SEW());
            assert(1 == RV->LMUL());
            assert(RV->VL() == RV->VLENE() * RV->LMUL());

            u32 vl = RV->VL();
            for (u32 i=0; i<RV->LMUL(); i++)
            {
                RV->vlte64(min(vl, RV->VLENE()), vd + i, A + i*RV->lambda()*stride, stride);
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

class vste64_t
{
    public:
        void v(u32 vs, double *A, u32 stride)
        {
            assert(64 == RV->SEW());
            assert(1 == RV->LMUL());
            assert(RV->VL() == RV->VLENE() * RV->LMUL());

            u32 vl = RV->VL();
            for (u32 i=0; i<RV->LMUL(); i++)
            {
                RV->vste64(min(vl, RV->VLENE()), vs + i, A + i*RV->lambda()*stride, stride);
                vl = (vl < RV->VLENE()) ? 0 : vl - RV->VLENE();
            }
            return;
        }
};

class vmtlfre64_t
{
    public:
        void v(u32 vd, double *A, s32 stride)
        {
            assert(64 == RV->SEW());
            assert( 4 >= RV->LMUL());

            if (vd % RV->LMUL()) return;        // nop if not LMUL aligned
            for (u32 i=0; i<RV->LMUL(); i++)
            {
                RV->vmtlfre64(vd+i, A + i*RV->lambda(), stride);
            }
            return;
        }
};

class vmtsfre64_t
{
    public:
        void v(u32 vs, double *A, s32 stride)
        {
            assert(64 == RV->SEW());
            assert( 4 >= RV->LMUL());

            if (vs % RV->LMUL()) return;        // nop if not LMUL aligned
            for (u32 i=0; i<RV->LMUL(); i++)
            {
                RV->vmtsfre64(vs+i, A + i*RV->lambda(), stride);
            }
            return;
        }
};

vfmmacc_t       vfmmacc;
vfmacc_t        vfmacc;
vsetvli_t       vsetvli;
vsetvl_t        vsetvl;
vsetmul_t       vsetmul;
vxor_t          vxor;
vle64_t         vle64;
vlte64_t        vlte64;
vse64_t         vse64;
vste64_t        vste64;
vmtlfre64_t     vmtlfre64;
vmtsfre64_t     vmtsfre64;
vmrotate_t      vmrotate;

u32 LisSquare()
{
    double rootL = sqrt(RV->VLENE());
    if ((rootL*rootL) == RV->VLENE()) return 1;
    else return 0;
}

template<typename T>
void microgemm
(
    u32     M,
    u32     N,
    u32     K,
    T      *A,
    T      *B,
    T       alpha,
    T      *C,
    s32     gamma,
    u32     lmul
);

template<>
void microgemm<double>
(
    u32     M,
    u32     N,
    u32     K,
    double *A,
    double *B,
    double  alpha,
    double *C,
    s32     gamma,
    u32     lmul
)
{
    u32 L = RV->VLENE();                                                // L is number of elements per vector register
    u32 lambda_eff = RV->lambda() * lmul;                               // lambda_eff is the maximum lambda for this L
    assert(0 == K % lambda_eff);                                        // for simplicty, K must be a multiple of lambda_eff

    vsetvl(5, 0, 64, 1, true, true);                                    // double-precision kernel, set VL to VLENE and LMUL to 1
    for (u32 r=16; r<32; r++) vxor.vv(r, r, r);                         // T = 0

    vsetvl(5, RV->lambda() * RV->lambda(), 64, lmul, true, true);       // double-precision kernel, set VL to lambda^2 and LMUL accordingly
    s32 INCA = M*lambda_eff; s32 INCB = N*lambda_eff;                   // iteration increments for A and B panels

    // the following setup for the A and B register load pointers works because not all loads are active for all values of lmul
    double *A0 = A; double *A1 = A0 + LisSquare() * L; double *A2 = A1 + ((2 == lmul) ? LisSquare() * L : L); double *A3 = A2 + LisSquare() * L;        // pointers for loads to the A registers
    double *B0 = B; double *B1 = B0 + L;               double *B2 = B1 + L;                                   double *B3 = B2 + L;                      // pointers for loads to the B registers

    // the computation loop
    for (u32 k=0; k<K; k+=lambda_eff)
    {
        if (debug > 1) { std::cout << "k = " << k << std::endl; }

        // load the 4 A registers
        vmtlfre64.v( 0, A0, lambda_eff); if (debug > 1) { std::cout << "VR[ 0] = "; RV->printVRf64( 0); }
        vmtlfre64.v( 1, A1, lambda_eff); if (debug > 1) { std::cout << "VR[ 1] = "; RV->printVRf64( 1); }
        vmtlfre64.v( 2, A2, lambda_eff); if (debug > 1) { std::cout << "VR[ 2] = "; RV->printVRf64( 2); }
        vmtlfre64.v( 3, A3, lambda_eff); if (debug > 1) { std::cout << "VR[ 3] = "; RV->printVRf64( 3); }

        // load the 4 B registers
        vmtlfre64.v( 8, B0, lambda_eff); if (debug > 1) { std::cout << "VR[ 8] = "; RV->printVRf64( 8); }
        vmtlfre64.v( 9, B1, lambda_eff); if (debug > 1) { std::cout << "VR[ 9] = "; RV->printVRf64( 9); }
        vmtlfre64.v(10, B2, lambda_eff); if (debug > 1) { std::cout << "VR[10] = "; RV->printVRf64(10); }
        vmtlfre64.v(11, B3, lambda_eff); if (debug > 1) { std::cout << "VR[11] = "; RV->printVRf64(11); }

        A0 = A0 + INCA ; A1 = A1 + INCA ; A2 = A2 + INCA ; A3 = A3 + INCA;  // increment pointers for the A registers
        B0 = B0 + INCB ; B1 = B1 + INCB ; B2 = B2 + INCB ; B3 = B3 + INCB;  // increment pointers for the B registers

        // perform 16 vmmacc's, one for each target register
        vfmmacc.v0(16,  0,  8); vmrotate.vv( 8,  8); if (debug > 1) { std::cout << "VR[16] = "; RV->printVRf64(16); }
        vfmmacc.v0(17,  0,  9); vmrotate.vv( 9,  9); if (debug > 1) { std::cout << "VR[17] = "; RV->printVRf64(17); }
        vfmmacc.v0(18,  1,  8); vmrotate.vv( 8,  8); if (debug > 1) { std::cout << "VR[18] = "; RV->printVRf64(18); }
        vfmmacc.v0(19,  1,  9); vmrotate.vv( 9,  9); if (debug > 1) { std::cout << "VR[19] = "; RV->printVRf64(19); }
        vfmmacc.v0(20,  0, 10); vmrotate.vv(10, 10); if (debug > 1) { std::cout << "VR[20] = "; RV->printVRf64(20); }
        vfmmacc.v0(21,  0, 11); vmrotate.vv(11, 11); if (debug > 1) { std::cout << "VR[21] = "; RV->printVRf64(21); }
        vfmmacc.v0(22,  1, 10); vmrotate.vv(10, 10); if (debug > 1) { std::cout << "VR[22] = "; RV->printVRf64(22); }
        vfmmacc.v0(23,  1, 11); vmrotate.vv(11, 11); if (debug > 1) { std::cout << "VR[23] = "; RV->printVRf64(23); }
        vfmmacc.v0(24,  2,  8); vmrotate.vv( 8,  8); if (debug > 1) { std::cout << "VR[24] = "; RV->printVRf64(24); }
        vfmmacc.v0(25,  2,  9); vmrotate.vv( 9,  9); if (debug > 1) { std::cout << "VR[25] = "; RV->printVRf64(25); }
        vfmmacc.v0(26,  3,  8); vmrotate.vv( 8,  8); if (debug > 1) { std::cout << "VR[26] = "; RV->printVRf64(26); }
        vfmmacc.v0(27,  3,  9); vmrotate.vv( 9,  9); if (debug > 1) { std::cout << "VR[27] = "; RV->printVRf64(27); }
        vfmmacc.v0(28,  2, 10); vmrotate.vv(10, 10); if (debug > 1) { std::cout << "VR[28] = "; RV->printVRf64(28); }
        vfmmacc.v0(29,  2, 11); vmrotate.vv(11, 11); if (debug > 1) { std::cout << "VR[29] = "; RV->printVRf64(29); }
        vfmmacc.v0(30,  3, 10); vmrotate.vv(10, 10); if (debug > 1) { std::cout << "VR[30] = "; RV->printVRf64(30); }
        vfmmacc.v0(31,  3, 11); vmrotate.vv(11, 11); if (debug > 1) { std::cout << "VR[31] = "; RV->printVRf64(31); }
    }

    // compute the store offsets for each result register - this only has to be done once per <L,lambda,gamma> configuration
    // that is, it can be done on entry to BLAS-level GEMM routine
    // the offset vector only needs 16 elements - we use 32 for convenience and will cleanup later
    u32 offset[32];
    offset[16] = 0;
    offset[17] = offset[16] + (((!LisSquare()) && (1 == lmul)) ? 2 * RV->lambda() : RV->lambda());
    offset[18] = (1 != lmul) ? (offset[17] + RV->lambda()) : (LisSquare() ? offset[16] + RV->lambda() * gamma : offset[16] + RV->lambda());
    offset[19] = ((!LisSquare()) && (1 == lmul)) ? offset[17] + RV->lambda() : offset[18] + RV->lambda();
    offset[20] = (4 == lmul) ? (offset[16] + 4*RV->lambda()) : offset[16] + (2*RV->sigma())/lmul;
    offset[21] = offset[20] + (((!LisSquare()) && (1 == lmul)) ? 2 * RV->lambda() : RV->lambda());
    offset[22] = (1 != lmul) ? (offset[21] + RV->lambda()) : (LisSquare() ? offset[20] + RV->lambda() * gamma : offset[20] + RV->lambda());
    offset[23] = ((!LisSquare()) && (1 == lmul)) ? offset[21] + RV->lambda() : offset[22] + RV->lambda();
    offset[24] = (4 == lmul) ? (offset[16] + 8*RV->lambda()) : ((LisSquare() || (1 == lmul)) ? gamma * (M/2) : offset[16] + 4*RV->lambda());
    offset[25] = offset[24] + (((!LisSquare()) && (1 == lmul)) ? 2 * RV->lambda() : RV->lambda());
    offset[26] = (1 != lmul) ? (offset[25] + RV->lambda()) : (LisSquare() ? offset[24] + RV->lambda() * gamma : offset[24] + RV->lambda());
    offset[27] = ((!LisSquare()) && (1 == lmul)) ? offset[25] + RV->lambda() : offset[26] + RV->lambda();
    offset[28] = (4 == lmul) ? (offset[24] + 4*RV->lambda()) : offset[24] + (2*RV->sigma())/lmul;
    offset[29] = offset[28] + (((!LisSquare()) && (1 == lmul)) ? 2 * RV->lambda() : RV->lambda());
    offset[30] = (1 != lmul) ? (offset[29] + RV->lambda()) : (LisSquare() ? offset[28] + RV->lambda() * gamma : offset[28] + RV->lambda());
    offset[31] = ((!LisSquare()) && (1 == lmul)) ? offset[29] + RV->lambda() : offset[30] + RV->lambda();

    if (debug > 1) { for (u32 i=16; i<32; i++) std::cout << "offset[" << i << "] = " << offset[i] << std::endl; }

    // do the scaling by alpha and update C
    vsetvl(5, 0, 64, 1, true, true);                                    // double-precision kernel, set VL to VLENE and LMUL to 1
    for (u32 vd=0; vd<16; vd++)
    {
        vmtlfre64.v(vd, C+offset[vd+16], N);                            // C[i,j] = alpha * T[i,j] + C[i,j]
        vfmacc.vf(vd, alpha, vd+16);
        vmtsfre64.v(vd, C+offset[vd+16], N);
    }
}

void microdgemm
(
    u32 M,
    u32 N,
    u32 K,
    double *A,
    double *B,
    double  alpha,
    double *C,
    s32     gamma,
    u32     lmul
)
{
    u32 lambda_eff = RV->lambda() * lmul;
    assert(0 == K % lambda_eff);                                        // For simplicty, K must be a multiple of lambda_eff

   double *S = new double[M*N]; for (u32 i=0; i<M*N; i++) S[i] = 0;     // drand48() - 0.5;

    vsetvl(5, 0, 64, 1, true, true);                                    // double-precision kernel, set VL to VLENE and LMUL to 1
    for (u32 r=16; r<32; r++) vxor.vv(r, r, r);                         // T = 0

    vsetvl(5, RV->lambda() * RV->lambda(), 64, lmul, true, true);       // double-precision kernel, set VL to lambda^2 and LMUL accordingly
    s32 INCA = M*lambda_eff; s32 INCB = N*lambda_eff;
    double *A0, *A1, *A2, *A3;
    A0 = A; 
    if (LisSquare())
    {
        if      (1 == lmul) { A1 = A0 + RV->VLENE() ; A2 = A1 + RV->VLENE()  ; A3 = A2 + RV->VLENE() ; }
        else if (2 == lmul) { A1 = A0 + RV->lambda(); A2 = A0 + 2*RV->VLENE(); A3 = A2 + RV->lambda(); }
        else if (4 == lmul) { A1 = A0 + RV->lambda(); A2 = A1 + RV->lambda() ; A3 = A2 + RV->lambda(); }
        else assert(false);
    }
    else
    {
        if      (1 == lmul) { A1 = A0               ; A2 = A0 + RV->VLENE(); A3 = A2               ; }
        else if (2 == lmul) { A1 = A0 + RV->lambda(); A2 = A0              ; A3 = A2 + RV->lambda(); }
        else assert(false);
    }
    double *B0 = B; double *B1 = B0 + RV->VLENE();             double *B2 = B1 + RV->VLENE(); double *B3 = B2 + RV->VLENE();
    for (u32 k=0; k<K; k+=lambda_eff)
    {
        if (debug > 1) { std::cout << "k = " << k << std::endl; }

        vmtlfre64.v( 0, A0, lambda_eff); if (debug > 1) { std::cout << "VR[ 0] = "; RV->printVRf64( 0); }
        vmtlfre64.v( 1, A1, lambda_eff); if (debug > 1) { std::cout << "VR[ 1] = "; RV->printVRf64( 1); }
        vmtlfre64.v( 2, A2, lambda_eff); if (debug > 1) { std::cout << "VR[ 2] = "; RV->printVRf64( 2); }
        vmtlfre64.v( 3, A3, lambda_eff); if (debug > 1) { std::cout << "VR[ 3] = "; RV->printVRf64( 3); }
        vmtlfre64.v( 8, B0, lambda_eff); if (debug > 1) { std::cout << "VR[ 8] = "; RV->printVRf64( 8); }
        vmtlfre64.v( 9, B1, lambda_eff); if (debug > 1) { std::cout << "VR[ 9] = "; RV->printVRf64( 9); }
        vmtlfre64.v(10, B2, lambda_eff); if (debug > 1) { std::cout << "VR[10] = "; RV->printVRf64(10); }
        vmtlfre64.v(11, B3, lambda_eff); if (debug > 1) { std::cout << "VR[11] = "; RV->printVRf64(11); }

        A0 = A0 + INCA ; A1 = A1 + INCA ; A2 = A2 + INCA ; A3 = A3 + INCA;
        B0 = B0 + INCB ; B1 = B1 + INCB ; B2 = B2 + INCB ; B3 = B3 + INCB;

        vfmmacc.v0(16,  0,  8); vmrotate.vv( 8,  8); if (debug > 1) { std::cout << "VR[16] = "; RV->printVRf64(16); }
        vfmmacc.v0(17,  0,  9); vmrotate.vv( 9,  9); if (debug > 1) { std::cout << "VR[17] = "; RV->printVRf64(17); }
        vfmmacc.v0(18,  1,  8); vmrotate.vv( 8,  8); if (debug > 1) { std::cout << "VR[18] = "; RV->printVRf64(18); }
        vfmmacc.v0(19,  1,  9); vmrotate.vv( 9,  9); if (debug > 1) { std::cout << "VR[19] = "; RV->printVRf64(19); }
        vfmmacc.v0(20,  0, 10); vmrotate.vv(10, 10); if (debug > 1) { std::cout << "VR[20] = "; RV->printVRf64(20); }
        vfmmacc.v0(21,  0, 11); vmrotate.vv(11, 11); if (debug > 1) { std::cout << "VR[21] = "; RV->printVRf64(21); }
        vfmmacc.v0(22,  1, 10); vmrotate.vv(10, 10); if (debug > 1) { std::cout << "VR[22] = "; RV->printVRf64(22); }
        vfmmacc.v0(23,  1, 11); vmrotate.vv(11, 11); if (debug > 1) { std::cout << "VR[23] = "; RV->printVRf64(23); }
        vfmmacc.v0(24,  2,  8); vmrotate.vv( 8,  8); if (debug > 1) { std::cout << "VR[24] = "; RV->printVRf64(24); }
        vfmmacc.v0(25,  2,  9); vmrotate.vv( 9,  9); if (debug > 1) { std::cout << "VR[25] = "; RV->printVRf64(25); }
        vfmmacc.v0(26,  3,  8); vmrotate.vv( 8,  8); if (debug > 1) { std::cout << "VR[26] = "; RV->printVRf64(26); }
        vfmmacc.v0(27,  3,  9); vmrotate.vv( 9,  9); if (debug > 1) { std::cout << "VR[27] = "; RV->printVRf64(27); }
        vfmmacc.v0(28,  2, 10); vmrotate.vv(10, 10); if (debug > 1) { std::cout << "VR[28] = "; RV->printVRf64(28); }
        vfmmacc.v0(29,  2, 11); vmrotate.vv(11, 11); if (debug > 1) { std::cout << "VR[29] = "; RV->printVRf64(29); }
        vfmmacc.v0(30,  3, 10); vmrotate.vv(10, 10); if (debug > 1) { std::cout << "VR[30] = "; RV->printVRf64(30); }
        vfmmacc.v0(31,  3, 11); vmrotate.vv(11, 11); if (debug > 1) { std::cout << "VR[31] = "; RV->printVRf64(31); }
    }

    u32 offset[32];
    offset[16] = 0;
    offset[17] = offset[16] + (((!LisSquare()) && (1 == lmul)) ? 2 * RV->lambda() : RV->lambda());
    offset[18] = (1 != lmul) ? (offset[17] + RV->lambda()) : (LisSquare() ? offset[16] + RV->lambda() * gamma : offset[16] + RV->lambda());
    offset[19] = ((!LisSquare()) && (1 == lmul)) ? offset[17] + RV->lambda() : offset[18] + RV->lambda();
    offset[20] = (4 == lmul) ? (offset[16] + 4*RV->lambda()) : offset[16] + (2*RV->sigma())/lmul;
    offset[21] = offset[20] + (((!LisSquare()) && (1 == lmul)) ? 2 * RV->lambda() : RV->lambda());
    offset[22] = (1 != lmul) ? (offset[21] + RV->lambda()) : (LisSquare() ? offset[20] + RV->lambda() * gamma : offset[20] + RV->lambda());
    offset[23] = ((!LisSquare()) && (1 == lmul)) ? offset[21] + RV->lambda() : offset[22] + RV->lambda();
    offset[24] = (4 == lmul) ? (offset[16] + 8*RV->lambda()) : ((LisSquare() || (1 == lmul)) ? gamma * (M/2) : offset[16] + 4*RV->lambda());
    offset[25] = offset[24] + (((!LisSquare()) && (1 == lmul)) ? 2 * RV->lambda() : RV->lambda());
    offset[26] = (1 != lmul) ? (offset[25] + RV->lambda()) : (LisSquare() ? offset[24] + RV->lambda() * gamma : offset[24] + RV->lambda());
    offset[27] = ((!LisSquare()) && (1 == lmul)) ? offset[25] + RV->lambda() : offset[26] + RV->lambda();
    offset[28] = (4 == lmul) ? (offset[24] + 4*RV->lambda()) : offset[24] + (2*RV->sigma())/lmul;
    offset[29] = offset[28] + (((!LisSquare()) && (1 == lmul)) ? 2 * RV->lambda() : RV->lambda());
    offset[30] = (1 != lmul) ? (offset[29] + RV->lambda()) : (LisSquare() ? offset[28] + RV->lambda() * gamma : offset[28] + RV->lambda());
    offset[31] = ((!LisSquare()) && (1 == lmul)) ? offset[29] + RV->lambda() : offset[30] + RV->lambda();

    if (debug > 1) { for (u32 i=16; i<32; i++) std::cout << "offset[" << i << "] = " << offset[i] << std::endl; }

    double *D = new double[M*N];
    vsetvl(5, 0, 64, 1, true, true);                                    // double-precision kernel, set VL to VLENE and LMUL to 1
    for (u32 vs=16; vs<32; vs++)
        vmtsfre64.v(vs, D+offset[vs], N);

    double *E = new double[M*N]; for (u32 i=0; i<M*N; i++) E[i] = C[i];
    for (u32 vd=0; vd<16; vd++)
    {
        vmtlfre64.v(vd, C+offset[vd+16], N);
        vfmacc.vf(vd, alpha, vd+16);
        vmtsfre64.v(vd, E+offset[vd+16], N);
    }

    for (u32 k=0; k<K; k+=lambda_eff)
    {
        for (u32 i=0; i<M; i++) for (u32 j=0; j<N; j++)
        {
            for (u32 k=0; k<lambda_eff; k++)
            {
                S[i*N + j] += A[i*lambda_eff + k] * B[j*lambda_eff + k];
            }
        }
        A += INCA;
        B += INCB;
    }
    for (u32 i=0; i<M; i++) for (u32 j=0; j<N; j++) C[i*gamma + j] += alpha * S[i*N + j];

    if (debug > 1) { std::cout << "S = "; print(M, N, S); }
    if (debug > 1) { std::cout << "D = "; print(M, N, D); }

    for (u32 i=0; i<M; i++) for (u32 j=0; j<N; j++) if (D[i*N+j] != S[i*N+j]) { std::cout << "Error for D[" << i << "," << j << "] = " << D[i*N+j] << " != " << S[i*N+j] << std::endl; exit(-1); }
    for (u32 i=0; i<M; i++) for (u32 j=0; j<N; j++) if (E[i*N+j] != C[i*N+j]) { std::cout << "Error for E[" << i << "," << j << "] = " << E[i*N+j] << " != " << C[i*N+j] << std::endl; exit(-1); }

    delete [] E;
    delete [] S;
    delete [] D;
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
    const char *Pstr,
    double *A,
    const char *Astr,
    u32    mu,
    u32    lambda,
    u32    K,
    u32    mul
)
{
    assert(0 == K % lambda);                            // For simplicity, K must be a multiple of lambda

    for (u32 k=0; k<K; k+=lambda)
    {
        for (u32 i=0; i<mu; i++)
        {
            for (u32 j=0; j<lambda; j++)
            {
                P[i*lambda + j] = A[i*K + j];
            }
        }
        A += lambda;
        P += mu*lambda;
    }
}

void print
(
    u32           M,
    u32           N,
    double       *A
)
{
    std::cout << "[" << std::endl;
    for (u32 i=0; i<M; i++)
    {
        for (u32 j=0; j<N; j++)
        {
            std::cout << " " << std::setw(9) << std::fixed << std::setprecision(0) << A[i*N + j];
        }
        std::cout << std::endl;
    }
    std::cout << "]" << std::endl;
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
        if (cmul > rmul) rmul = rmul*2;
        else cmul = cmul * 2;
    }

    u32 lambda_eff = 1;
    while ((2*lambda_eff)*(2*lambda_eff) <= L) lambda_eff *= 2;
    u32 LMUL = lambda_eff / lambda;
    std::cout << "L = " << std::setw(4) << L << ", lambda = " << std::setw(2) << RV->lambda() << ", sigma = " << std::setw(3) << RV->sigma() << ", lambda_eff = " << std::setw(2) << lambda_eff << ", LMUL = " << LMUL;
    std::cout << ", RMUL = " << std::setw(2) << rmul << ", CMUL = " << std::setw(2) << cmul;

    u32 mu = rmul*RV->sigma();
    u32 nu = cmul*RV->sigma();

    std::cout << ", mu = " << std::setw(3) << mu << ", nu =  " << std::setw(3) << nu << ", K = " << std::setw(3) << K << std::endl;

    u32 M = mu;
    u32 N = nu;

    double alpha = 3.141592654; // alpha = 1.0;

    // Allocate A, B, and C panels
    double *A = new double[M*K]; for (u32 i=0; i<M*K; i++) A[i] = i; // drand48() - 0.5;
    double *B = new double[K*N]; for (u32 i=0; i<K*N; i++) B[i] = i; // drand48() - 0.5;
    double *C = new double[M*N]; for (u32 i=0; i<M*N; i++) C[i] = 0; // drand48() - 0.5;
    double *D = new double[M*N]; for (u32 i=0; i<M*N; i++) D[i] = C[i];

    if (debug > 1) { std::cout << "A = "; print(M, K, A); }
    if (debug > 1) { std::cout << "B = "; print(N, K, B); }

    // Allocate the packed panels
    double *Ap = new double[M*K];
    double *Bp = new double[K*N];
    double *Cp = new double[M*N];

    // Pack the A and B panels
    vsetmul(rmul,cmul);
    packfp64(Ap, "Ap", A, "A", mu, lambda_eff, K, rmul);
    packfp64(Bp, "Bp", B, "B", nu, lambda_eff, K, cmul);

    for (u32 k=0; k<K; k+=lambda_eff)
    {
        if (debug > 1) { std::cout << "Ap[" << k/lambda_eff << "] = "; print(M, lambda_eff, Ap+k*mu); }
        if (debug > 1) { std::cout << "Bp[" << k/lambda_eff << "] = "; print(N, lambda_eff, Bp+k*nu); }
    }

    microgemm<double>(M, N, K, Ap, Bp, alpha, D, N, LMUL);

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
                        std::cout << "A[" << i << ", " << k << "] = " << A[i*K+k] << ", B[" << k << ", " << j << "] = " << B[j*K+k] << std::endl;
                }
                S += A[i*K+k]*B[j*K+k];
            }
            if ((alpha*S + C[i*N+j]) != D[i*N+j])
            {
                std::cout << "Error for D[" << i << "," << j << "] = " << D[i*N+j] << " != " << alpha*S + C[i*N+j] << std::endl;
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
    std::cout << "=================================================================================================================" << std::endl;
    run_microgemm<   64, 1>( 1);
    run_microgemm<   64, 1>( 2);
    run_microgemm<   64, 1>( 4);
    run_microgemm<   64, 1>( 8);
    run_microgemm<  128, 1>( 1);
    run_microgemm<  128, 1>( 2);
    run_microgemm<  128, 1>( 4);
    run_microgemm<  128, 1>( 8);
    run_microgemm<  256, 1>( 2);
    run_microgemm<  256, 1>( 4);
    run_microgemm<  256, 1>( 8);
    run_microgemm<  256, 2>( 2);
    run_microgemm<  256, 2>( 4);
    run_microgemm<  256, 2>( 8);
    run_microgemm<  512, 1>( 2);
    run_microgemm<  512, 1>( 4);
    run_microgemm<  512, 1>( 8);
    run_microgemm<  512, 2>( 2);
    run_microgemm<  512, 2>( 4);
    run_microgemm<  512, 2>( 8);
    run_microgemm< 1024, 1>( 4);
    run_microgemm< 1024, 1>( 8);
    run_microgemm< 1024, 1>(16);
    run_microgemm< 1024, 2>( 4);
    run_microgemm< 1024, 2>( 8);
    run_microgemm< 1024, 2>(16);
    run_microgemm< 1024, 4>( 4);
    run_microgemm< 1024, 4>( 8);
    run_microgemm< 1024, 4>(16);
    run_microgemm< 2048, 2>( 8);
    run_microgemm< 2048, 2>(16);
    run_microgemm< 2048, 4>( 8);
    run_microgemm< 2048, 4>(64);
    run_microgemm< 4096, 2>(64);
    run_microgemm< 4096, 4>(64);
    run_microgemm< 4096, 8>(64);
    run_microgemm< 8192, 4>(64);
    run_microgemm< 8192, 8>(64);
    run_microgemm<16384, 4>(64);
    run_microgemm<16384, 8>(64);
    run_microgemm<16384,16>(64);
    run_microgemm<32768, 8>(64);
    run_microgemm<32768,16>(64);
    run_microgemm<65536, 8>(64);
    run_microgemm<65536,16>(64);
    run_microgemm<65536,32>(64);

    return 0;
}
