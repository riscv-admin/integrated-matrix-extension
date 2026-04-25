#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <assert.h>

// debug flag
const uint32_t debug = 0;

// helper macros
#define EXPAND(x)  _EXPAND(x)
#define _EXPAND(x) #x

// assembly instructions
#define li(RT, SI)			asm("li " #RT "," EXPAND(SI))
#define lxvp(XT, RA, D)			asm("lxvp " #XT "," #D "(" #RA ")")
#define stxvp(XS, RA, D)		asm("stxvp " #XS "," #D "(" #RA ")")
#define xvf64ger(AT, XA, XB)		asm("xvf64ger " #AT "," #XA "," #XB)
#define xvf64gerpp(AT, XA, XB)		asm("xvf64gerpp " #AT "," #XA "," #XB)
#define xxpermdi(XT, XA, XB, IM)	asm("xxpermdi " #XT "," #XA "," #XB "," #IM)
#define xxmfacc(XT)			asm("xxmfacc " #XT)
#define xxmtacc(XT)			asm("xxmtacc " #XT)
#define xvadddp(XT, XA, XB)		asm("xvadddp " #XT "," #XA "," #XB)
#define addi(RT, RA, SI)		asm("addi " #RT "," #RA "," #SI)
#define xxsetaccz(XT)			asm("xxsetaccz " #XT)
#define xxlxor(XT, XA, XB)		asm("xxlxor " #XT "," #XA "," #XB)
#define xxzero(XT)			xxlxor(XT, XT, XT)

// innermost loop count
#define COUNT 1

#define load_B_packed_4x8(D)                \
    lxvp(48, 4,   0+D); lxvp(50, 4,  32+D); \
    lxvp(52, 4,  64+D); lxvp(54, 4,  96+D); \
    lxvp(56, 4, 128+D); lxvp(58, 4, 160+D); \
    lxvp(60, 4, 192+D); lxvp(62, 4, 224+D);

#define load_A_packed_8x4(D)                \
    lxvp(32, 3,   0+D); lxvp(34, 3,  32+D); \
    lxvp(36, 3,  64+D); lxvp(38, 3,  96+D); \
    lxvp(40, 3, 128+D); lxvp(42, 3, 160+D); \
    lxvp(44, 3, 192+D); lxvp(46, 3, 224+D);

#define zero_8x8() \
    xxsetaccz(0);  \
    xxsetaccz(1);  \
    xxsetaccz(2);  \
    xxsetaccz(3);  \
    xxsetaccz(4);  \
    xxsetaccz(5);  \
    xxsetaccz(6);  \
    xxsetaccz(7);

#define zero_8x8_vsrs() \
    xxzero( 0);		\
    xxzero( 1);		\
    xxzero( 2);		\
    xxzero( 3);		\
    xxzero( 4);		\
    xxzero( 5);		\
    xxzero( 6);		\
    xxzero( 7);		\
    xxzero( 8);		\
    xxzero( 9);		\
    xxzero(10);		\
    xxzero(11);		\
    xxzero(12);		\
    xxzero(13);		\
    xxzero(14);		\
    xxzero(15);		\
    xxzero(16);		\
    xxzero(17);		\
    xxzero(18);		\
    xxzero(19);		\
    xxzero(20);		\
    xxzero(21);		\
    xxzero(22);		\
    xxzero(23);		\
    xxzero(24);		\
    xxzero(25);		\
    xxzero(26);		\
    xxzero(27);		\
    xxzero(28);		\
    xxzero(29);		\
    xxzero(30);		\
    xxzero(31);

#define rank_4_update_8x8_first() \
    xvf64ger(0, 48, 33);          \
    xvf64ger(1, 50, 33);          \
    xvf64ger(2, 48, 32);          \
    xvf64ger(3, 50, 32);          \
    xvf64ger(4, 48, 35);          \
    xvf64ger(5, 50, 35);          \
    xvf64ger(6, 48, 34);          \
    xvf64ger(7, 50, 34);          \
    xvf64gerpp(0, 52, 37);        \
    xvf64gerpp(1, 54, 37);        \
    xvf64gerpp(2, 52, 36);        \
    xvf64gerpp(3, 54, 36);        \
    xvf64gerpp(4, 52, 39);        \
    xvf64gerpp(5, 54, 39);        \
    xvf64gerpp(6, 52, 38);        \
    xvf64gerpp(7, 54, 38);        \
    xvf64gerpp(0, 56, 41);        \
    xvf64gerpp(1, 58, 41);        \
    xvf64gerpp(2, 56, 40);        \
    xvf64gerpp(3, 58, 40);        \
    xvf64gerpp(4, 56, 43);        \
    xvf64gerpp(5, 58, 43);        \
    xvf64gerpp(6, 56, 42);        \
    xvf64gerpp(7, 58, 42);        \
    xvf64gerpp(0, 60, 45);        \
    xvf64gerpp(1, 62, 45);        \
    xvf64gerpp(2, 60, 44);        \
    xvf64gerpp(3, 62, 44);        \
    xvf64gerpp(4, 60, 47);        \
    xvf64gerpp(5, 62, 47);        \
    xvf64gerpp(6, 60, 46);        \
    xvf64gerpp(7, 62, 46);    

#define rank_4_update_8x8()       \
    xvf64gerpp(0, 48, 33);        \
    xvf64gerpp(1, 50, 33);        \
    xvf64gerpp(2, 48, 32);        \
    xvf64gerpp(3, 50, 32);        \
    xvf64gerpp(4, 48, 35);        \
    xvf64gerpp(5, 50, 35);        \
    xvf64gerpp(6, 48, 34);        \
    xvf64gerpp(7, 50, 34);        \
    xvf64gerpp(0, 52, 37);        \
    xvf64gerpp(1, 54, 37);        \
    xvf64gerpp(2, 52, 36);        \
    xvf64gerpp(3, 54, 36);        \
    xvf64gerpp(4, 52, 39);        \
    xvf64gerpp(5, 54, 39);        \
    xvf64gerpp(6, 52, 38);        \
    xvf64gerpp(7, 54, 38);        \
    xvf64gerpp(0, 56, 41);        \
    xvf64gerpp(1, 58, 41);        \
    xvf64gerpp(2, 56, 40);        \
    xvf64gerpp(3, 58, 40);        \
    xvf64gerpp(4, 56, 43);        \
    xvf64gerpp(5, 58, 43);        \
    xvf64gerpp(6, 56, 42);        \
    xvf64gerpp(7, 58, 42);        \
    xvf64gerpp(0, 60, 45);        \
    xvf64gerpp(1, 62, 45);        \
    xvf64gerpp(2, 60, 44);        \
    xvf64gerpp(3, 62, 44);        \
    xvf64gerpp(4, 60, 47);        \
    xvf64gerpp(5, 62, 47);        \
    xvf64gerpp(6, 60, 46);        \
    xvf64gerpp(7, 62, 46);    

#define start_kernel_8x8()     \
    xxmtacc(0);                \
    xxmtacc(1);                \
    xxmtacc(2);                \
    xxmtacc(3);                \
    xxmtacc(4);                \
    xxmtacc(5);                \
    xxmtacc(6);                \
    xxmtacc(7);

#define stop_kernel_8x8()      \
    xxmfacc(0);                \
    xxmfacc(1);                \
    xxmfacc(2);                \
    xxmfacc(3);                \
    xxmfacc(4);                \
    xxmfacc(5);                \
    xxmfacc(6);                \
    xxmfacc(7);

#define accumulate_C_8x8()     \
    lxvp(32+ 0, 5,   0);       \
    lxvp(32+ 2, 5,  32);       \
    lxvp(32+ 4, 5,  64);       \
    lxvp(32+ 6, 5,  96);       \
    lxvp(32+ 8, 5, 128);       \
    lxvp(32+10, 5, 160);       \
    lxvp(32+12, 5, 192);       \
    lxvp(32+14, 5, 224);       \
    lxvp(32+16, 5, 256);       \
    lxvp(32+18, 5, 288);       \
    lxvp(32+20, 5, 320);       \
    lxvp(32+22, 5, 352);       \
    lxvp(32+24, 5, 384);       \
    lxvp(32+26, 5, 416);       \
    lxvp(32+28, 5, 448);       \
    lxvp(32+30, 5, 480);       \
    xvadddp(33,  3, 33);       \
    xvadddp(37,  2, 37);       \
    xvadddp(41,  1, 41);       \
    xvadddp(45,  0, 45);       \
    xvadddp(49,  7, 49);       \
    xvadddp(53,  6, 53);       \
    xvadddp(57,  5, 57);       \
    xvadddp(61,  4, 61);       \
    xvadddp(32, 11, 32);       \
    xvadddp(36, 10, 36);       \
    xvadddp(40,  9, 40);       \
    xvadddp(44,  8, 44);       \
    xvadddp(48, 15, 48);       \
    xvadddp(52, 14, 52);       \
    xvadddp(56, 13, 56);       \
    xvadddp(60, 12, 60);       \
    xvadddp(35, 19, 35);       \
    xvadddp(39, 18, 39);       \
    xvadddp(43, 17, 43);       \
    xvadddp(47, 16, 47);       \
    xvadddp(51, 23, 51);       \
    xvadddp(55, 22, 55);       \
    xvadddp(59, 21, 59);       \
    xvadddp(63, 20, 63);       \
    xvadddp(34, 27, 34);       \
    xvadddp(38, 26, 38);       \
    xvadddp(42, 25, 42);       \
    xvadddp(46, 24, 46);       \
    xvadddp(50, 31, 50);       \
    xvadddp(54, 30, 54);       \
    xvadddp(58, 29, 58);       \
    xvadddp(62, 28, 62);       \
    stxvp(32+ 0, 5,   0);      \
    stxvp(32+ 2, 5,  32);      \
    stxvp(32+ 4, 5,  64);      \
    stxvp(32+ 6, 5,  96);      \
    stxvp(32+ 8, 5, 128);      \
    stxvp(32+10, 5, 160);      \
    stxvp(32+12, 5, 192);      \
    stxvp(32+14, 5, 224);      \
    stxvp(32+16, 5, 256);      \
    stxvp(32+18, 5, 288);      \
    stxvp(32+20, 5, 320);      \
    stxvp(32+22, 5, 352);      \
    stxvp(32+24, 5, 384);      \
    stxvp(32+26, 5, 416);      \
    stxvp(32+28, 5, 448);      \
    stxvp(32+30, 5, 480);

#define add_and_store_8x8()    \
    xxmfacc(0);                \
    xxmfacc(1);                \
    xxmfacc(2);                \
    xxmfacc(3);                \
    xxmfacc(4);                \
    xxmfacc(5);                \
    xxmfacc(6);                \
    xxmfacc(7);                \
    lxvp(32+ 0, 5,   0);       \
    lxvp(32+ 2, 5,  32);       \
    lxvp(32+ 4, 5,  64);       \
    lxvp(32+ 6, 5,  96);       \
    lxvp(32+ 8, 5, 128);       \
    lxvp(32+10, 5, 160);       \
    lxvp(32+12, 5, 192);       \
    lxvp(32+14, 5, 224);       \
    lxvp(32+16, 5, 256);       \
    lxvp(32+18, 5, 288);       \
    lxvp(32+20, 5, 320);       \
    lxvp(32+22, 5, 352);       \
    lxvp(32+24, 5, 384);       \
    lxvp(32+26, 5, 416);       \
    lxvp(32+28, 5, 448);       \
    lxvp(32+30, 5, 480);       \
    xvadddp(33,  3, 33);       \
    xvadddp(37,  2, 37);       \
    xvadddp(41,  1, 41);       \
    xvadddp(45,  0, 45);       \
    xvadddp(49,  7, 49);       \
    xvadddp(53,  6, 53);       \
    xvadddp(57,  5, 57);       \
    xvadddp(61,  4, 61);       \
    xvadddp(32, 11, 32);       \
    xvadddp(36, 10, 36);       \
    xvadddp(40,  9, 40);       \
    xvadddp(44,  8, 44);       \
    xvadddp(48, 15, 48);       \
    xvadddp(52, 14, 52);       \
    xvadddp(56, 13, 56);       \
    xvadddp(60, 12, 60);       \
    xvadddp(35, 19, 35);       \
    xvadddp(39, 18, 39);       \
    xvadddp(43, 17, 43);       \
    xvadddp(47, 16, 47);       \
    xvadddp(51, 23, 51);       \
    xvadddp(55, 22, 55);       \
    xvadddp(59, 21, 59);       \
    xvadddp(63, 20, 63);       \
    xvadddp(34, 27, 34);       \
    xvadddp(38, 26, 38);       \
    xvadddp(42, 25, 42);       \
    xvadddp(46, 24, 46);       \
    xvadddp(50, 31, 50);       \
    xvadddp(54, 30, 54);       \
    xvadddp(58, 29, 58);       \
    xvadddp(62, 28, 62);       \
    stxvp(32+ 0, 5,   0);      \
    stxvp(32+ 2, 5,  32);      \
    stxvp(32+ 4, 5,  64);      \
    stxvp(32+ 6, 5,  96);      \
    stxvp(32+ 8, 5, 128);      \
    stxvp(32+10, 5, 160);      \
    stxvp(32+12, 5, 192);      \
    stxvp(32+14, 5, 224);      \
    stxvp(32+16, 5, 256);      \
    stxvp(32+18, 5, 288);      \
    stxvp(32+20, 5, 320);      \
    stxvp(32+22, 5, 352);      \
    stxvp(32+24, 5, 384);      \
    stxvp(32+26, 5, 416);      \
    stxvp(32+28, 5, 448);      \
    stxvp(32+30, 5, 480);

#define rank_8_update_8x8_first(D) \
    load_B_packed_4x8((D+0));      \
    load_A_packed_8x4((D+0));      \
    rank_4_update_8x8_first();     \
    load_B_packed_4x8((D+256));    \
    load_A_packed_8x4((D+256));    \
    rank_4_update_8x8();

#define rank_8_update_8x8(D)    \
    load_B_packed_4x8((D+0));   \
    load_A_packed_8x4((D+0));   \
    rank_4_update_8x8();        \
    load_B_packed_4x8((D+256)); \
    load_A_packed_8x4((D+256)); \
    rank_4_update_8x8();

#define rank_4_update_8x8_with_loads(D)    \
    load_B_packed_4x8((D+0));   \
    load_A_packed_8x4((D+0));   \
    rank_4_update_8x8();

#define packc_8x8()	    \
    lxvp( 0, 6,   0);       \
    lxvp( 2, 6,  32);       \
    lxvp( 4, 6,  64);       \
    lxvp( 6, 6,  96);       \
    lxvp( 8, 6, 128);       \
    lxvp(10, 6, 160);       \
    lxvp(12, 6, 192);       \
    lxvp(14, 6, 224);       \
    lxvp(16, 6, 256);       \
    lxvp(18, 6, 288);       \
    lxvp(20, 6, 320);       \
    lxvp(22, 6, 352);       \
    lxvp(24, 6, 384);       \
    lxvp(26, 6, 416);       \
    lxvp(28, 6, 448);       \
    lxvp(30, 6, 480);       \
    stxvp( 0, 5,   0);      \
    stxvp( 2, 5,  32);      \
    stxvp( 4, 5,  64);      \
    stxvp( 6, 5,  96);      \
    stxvp( 8, 5, 128);      \
    stxvp(10, 5, 160);      \
    stxvp(12, 5, 192);      \
    stxvp(14, 5, 224);      \
    stxvp(16, 5, 256);      \
    stxvp(18, 5, 288);      \
    stxvp(20, 5, 320);      \
    stxvp(22, 5, 352);      \
    stxvp(24, 5, 384);      \
    stxvp(26, 5, 416);      \
    stxvp(28, 5, 448);      \
    stxvp(30, 5, 480);

#define unpackc_8x8()	    \
    lxvp( 0, 5,   0);       \
    lxvp( 2, 5,  32);       \
    lxvp( 4, 5,  64);       \
    lxvp( 6, 5,  96);       \
    lxvp( 8, 5, 128);       \
    lxvp(10, 5, 160);       \
    lxvp(12, 5, 192);       \
    lxvp(14, 5, 224);       \
    lxvp(16, 5, 256);       \
    lxvp(18, 5, 288);       \
    lxvp(20, 5, 320);       \
    lxvp(22, 5, 352);       \
    lxvp(24, 5, 384);       \
    lxvp(26, 5, 416);       \
    lxvp(28, 5, 448);       \
    lxvp(30, 5, 480);       \
    stxvp( 0, 6,   0);      \
    stxvp( 2, 6,  32);      \
    stxvp( 4, 6,  64);      \
    stxvp( 6, 6,  96);      \
    stxvp( 8, 6, 128);      \
    stxvp(10, 6, 160);      \
    stxvp(12, 6, 192);      \
    stxvp(14, 6, 224);      \
    stxvp(16, 6, 256);      \
    stxvp(18, 6, 288);      \
    stxvp(20, 6, 320);      \
    stxvp(22, 6, 352);      \
    stxvp(24, 6, 384);      \
    stxvp(26, 6, 416);      \
    stxvp(28, 6, 448);      \
    stxvp(30, 6, 480);

void matmul_L_16_hint_256
(
    double *A,
    double *B,
    double *C,
    double *D,
    int     K
)
{
    zero_8x8_vsrs();
    start_kernel_8x8();

    asm("mtctr 7");
    asm("LOOP00:");

    rank_4_update_8x8_with_loads(   0);

    addi(3, 3, 256);
    addi(4, 4, 256);
    asm("bdnz LOOP00");

    stop_kernel_8x8();
    accumulate_C_8x8();
}

void matmul_L_16_hint_512
(
    double *A,
    double *B,
    double *C,
    double *D,
    int     K
)
{
    zero_8x8_vsrs();
    start_kernel_8x8();

    asm("mtctr 7");
    asm("LOOP11:");

    rank_4_update_8x8_with_loads(   0);

    addi(3, 3, 512);
    addi(4, 4, 512);
    asm("bdnz LOOP11");

    stop_kernel_8x8();
    accumulate_C_8x8();
}

void matmul_L_16_hint_768
(
    double *A,
    double *B,
    double *C,
    double *D,
    int     K
)
{
    zero_8x8_vsrs();
    start_kernel_8x8();

    asm("mtctr 7");
    asm("LOOP22:");

    rank_4_update_8x8_with_loads(   0);

    addi(3, 3, 768);
    addi(4, 4, 768);
    asm("bdnz LOOP22");

    stop_kernel_8x8();
    accumulate_C_8x8();
}

void matmul_L_16_hint_1024
(
    double *A,
    double *B,
    double *C,
    double *D,
    int     K
)
{
    zero_8x8_vsrs();
    start_kernel_8x8();

    asm("mtctr 7");
    asm("LOOP33:");

    rank_4_update_8x8_with_loads(   0);

    addi(3, 3, 1024);
    addi(4, 4, 1024);
    asm("bdnz LOOP33");

    stop_kernel_8x8();
    accumulate_C_8x8();
}

void matmul_L_16_hint_8192
(
    double *A,
    double *B,
    double *C,
    double *D,
    int     K
)
{
    zero_8x8_vsrs();
    start_kernel_8x8();

    asm("mtctr 7");
    asm("LOOP44:");

    rank_4_update_8x8_with_loads(   0);

    addi(3, 3, 8192);
    addi(4, 4, 8192);
    asm("bdnz LOOP44");

    stop_kernel_8x8();
    accumulate_C_8x8();
}

void matmul_L_16_nohint
(
    double *A,
    double *B,
    double *C,
    double *D,
    int     K
)
{
    zero_8x8_vsrs();

    asm("mtctr 7");
    asm("LOOP99:");

    rank_4_update_8x8_with_loads(   0);

    addi(3, 3, 256);
    addi(4, 4, 256);
    asm("bdnz LOOP99");

    accumulate_C_8x8();
}

void matmul_8x8xK_col_row
(
    double *A,
    double *B,
    double *C,
    double *D,
    int     K
)
{
    zero_8x8();

    asm("mtctr 7");
    asm("LOOP01:");

    rank_8_update_8x8(   0);
    rank_8_update_8x8( 512);
    rank_8_update_8x8(1024);
    rank_8_update_8x8(1536);
    rank_8_update_8x8(2048);
    rank_8_update_8x8(2560);
    rank_8_update_8x8(3072);
    rank_8_update_8x8(3584);

    addi(3, 3, 4096);
    addi(4, 4, 4096);
    asm("bdnz LOOP01");

    add_and_store_8x8();
}

void matmul_8x8xK_col_row_packc
(
    double *A,
    double *B,
    double *C,
    double *D,
    int     K
)
{
    packc_8x8();
    zero_8x8();

    asm("mtctr 7");
    asm("LOOP02:");

    rank_8_update_8x8(   0);
    rank_8_update_8x8( 512);
    rank_8_update_8x8(1024);
    rank_8_update_8x8(1536);
    rank_8_update_8x8(2048);
    rank_8_update_8x8(2560);
    rank_8_update_8x8(3072);
    rank_8_update_8x8(3584);

    addi(3, 3, 4096);
    addi(4, 4, 4096);
    asm("bdnz LOOP02");

    add_and_store_8x8();

    unpackc_8x8();
}

double now()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1.0e-9;
}

double *A = 0;
double *B = 0;
double *C = 0;

void print8x8
(
    double *A,
    int32_t d1,
    int32_t d2
)
{
    for (int i=0; i<8; i++)
    {
	for (int j=0; j<8; j++)
	    std::cout << std::setw(10) << std::fixed << A[i*d1+j*d2] << " ";
	std::cout << std::endl;
    }
}

double run_kernel
(
    void (kernel)(double*, double*, double*, double*, int),
    int k,
    uint32_t count
)
{
    const uint32_t N = 1024*1024;
    double *A = (double*)aligned_alloc(4096, sizeof(double) * N); for (uint32_t i=0; i<N; i++) A[i] = (i < 4096) ? i%8 : 0; // drand48() - 0.5;
    double *B = (double*)aligned_alloc(4096, sizeof(double) * N); for (uint32_t i=0; i<N; i++) B[i] = (i < 4096) ? i%8 : 0; // drand48() - 0.5;
    double *C = (double*)aligned_alloc(4096, sizeof(double) * N); for (uint32_t i=0; i<N; i++) C[i] = 0; // drand48() - 0.5;
    double *D = (double*)aligned_alloc(4096, sizeof(double) * N); for (uint32_t i=0; i<N; i++) D[i] = 0; // drand48() - 0.5;

    if (debug)
    {
	std::cout << "A = [" << std::endl; print8x8(A, 1, 8); std::cout << "]" << std::endl;
	std::cout << "B = [" << std::endl; print8x8(B, 8, 1); std::cout << "]" << std::endl;
    }

    volatile uint32_t scale = count;
    volatile double start, finish;

    start = now();
    for(; count; count -= COUNT)
    {
	kernel(A,B,C,D,k);
    }
    finish = now();

    if (debug)
    {
	std::cout << "C = [" << std::endl; print8x8(C, 1, 8); std::cout << "]" << std::endl;
    }

    for (int i=0; i<8; i++) for (int j=0; j<8; j++)
    {
	double S = 0;
	for (int n=0; n<k*4; n++) S += A[i+n*8]*B[n*8+j];
	// if (C[i+j*8] != S*scale) { std::cout << "Error! " << "C[" << i << "," << j << "] = " << C[i+j*8] << ", S = " << S*scale << std::endl; exit(-1); }
    }

    free(D);
    free(C);
    free(B);
    free(A);

    return (finish - start);
}

void run_kernel_and_report
(
    void (kernel)(double*, double*, double*, double*, int),
    uint32_t count,
    const char* name,
    uint32_t M,
    uint32_t N,
    uint32_t K,
    uint32_t k
)
{
    assert(0 == K%k);

    volatile double elapsed;
    volatile double flops = 2.0*count*M*N*K;
    elapsed = run_kernel(kernel, K/k, count);
    std::cout << std::setprecision(6);
    std::cout << "Time to run " << std::setw(30) << name << " ( K=" << std::setw(4) << K << " ) " << count << " times = " << std::setw(10) << std::fixed << elapsed << " seconds ( " << std::setw(10) << std::scientific << flops/elapsed << " flops )" << std::endl;
}

#define RUN_KERNEL(kernel, count, M, N, K, k) run_kernel_and_report(kernel, count, #kernel, M, N, K, k)

int main
(
    int		argc,
    char      **argv
)
{
    const uint32_t matmul_8x8xK_col_row_count  =  10000000U;

    std::cout << "=================================================================================================================" << std::endl;

    for (int K=64; K<=512; K+=64)
	RUN_KERNEL(matmul_L_16_hint_256          , matmul_8x8xK_col_row_count ,  8, 8, K,  4);
    for (int K=64; K<=512; K+=64)
	RUN_KERNEL(matmul_L_16_hint_512          , matmul_8x8xK_col_row_count ,  8, 8, K,  4);
    for (int K=64; K<=512; K+=64)
	RUN_KERNEL(matmul_L_16_hint_768          , matmul_8x8xK_col_row_count ,  8, 8, K,  4);
    for (int K=64; K<=512; K+=64)
	RUN_KERNEL(matmul_L_16_hint_1024         , matmul_8x8xK_col_row_count ,  8, 8, K,  4);
    for (int K=64; K<=512; K+=64)
	RUN_KERNEL(matmul_L_16_hint_8192         , matmul_8x8xK_col_row_count ,  8, 8, K,  4);
    /*
    for (int K=64; K<=512; K+=64)
	RUN_KERNEL(matmul_L_16_nohint        , matmul_8x8xK_col_row_count ,  8, 8, K,  4);
    for (int K=64; K<=512; K+=64)
	RUN_KERNEL(matmul_8x8xK_col_row      , matmul_8x8xK_col_row_count ,  8, 8, K, 64);
    for (int K=64; K<=512; K+=64)
	RUN_KERNEL(matmul_8x8xK_col_row_packc, matmul_8x8xK_col_row_count ,  8, 8, K, 64);
    */

    return 0;
}
