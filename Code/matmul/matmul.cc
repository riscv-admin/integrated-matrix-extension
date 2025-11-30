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
#define lxvp(XT, RA, D)			asm("lxvp " #XT "," #D "(" #RA ")")
#define stxvp(XS, RA, D)		asm("stxvp " #XS "," #D "(" #RA ")")
#define xvf64ger(AT, XA, XB)		asm("xvf64ger " #AT "," #XA "," #XB)
#define xvf64gerpp(AT, XA, XB)		asm("xvf64gerpp " #AT "," #XA "," #XB)
#define xxpermdi(XT, XA, XB, IM)	asm("xxpermdi " #XT "," #XA "," #XB "," #IM)
#define xxmfacc(XT)			asm("xxmfacc " #XT)
#define xvadddp(XT, XA, XB)		asm("xvadddp " #XT "," #XA "," #XB)

// innermost loop count
#define COUNT 1000

void matmul_8x8x8_col_col
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP02:");

    xxpermdi(48, 32, 34, 0b00);
    xxpermdi(52, 32, 34, 0b11);
    xxpermdi(49, 36, 38, 0b00);
    xxpermdi(53, 36, 38, 0b11);

    xxpermdi(50, 40, 42, 0b00);
    xxpermdi(54, 40, 42, 0b11);
    xxpermdi(51, 44, 46, 0b00);
    xxpermdi(55, 44, 46, 0b11);

    xxpermdi(56, 33, 35, 0b00);
    xxpermdi(60, 33, 35, 0b11);
    xxpermdi(57, 37, 39, 0b00);
    xxpermdi(61, 37, 39, 0b11);

    xxpermdi(58, 41, 43, 0b00);
    xxpermdi(62, 41, 43, 0b11);
    xxpermdi(59, 45, 47, 0b00);
    xxpermdi(63, 45, 47, 0b11);

    xvf64ger(0, 32, 48);
    xvf64ger(1, 32, 49);
    xvf64ger(2, 32, 50);
    xvf64ger(3, 32, 51);
    xvf64ger(4, 34, 48);
    xvf64ger(5, 34, 49);
    xvf64ger(6, 34, 50);
    xvf64ger(7, 34, 51);

    xvf64gerpp(0, 36, 52);
    xvf64gerpp(1, 36, 53);
    xvf64gerpp(2, 36, 54);
    xvf64gerpp(3, 36, 55);
    xvf64gerpp(4, 38, 52);
    xvf64gerpp(5, 38, 53);
    xvf64gerpp(6, 38, 54);
    xvf64gerpp(7, 38, 55);

    xvf64gerpp(0, 40, 56);
    xvf64gerpp(1, 40, 57);
    xvf64gerpp(2, 40, 58);
    xvf64gerpp(3, 40, 59);
    xvf64gerpp(4, 42, 56);
    xvf64gerpp(5, 42, 57);
    xvf64gerpp(6, 42, 58);
    xvf64gerpp(7, 42, 59);

    xvf64gerpp(0, 44, 60);
    xvf64gerpp(1, 44, 61);
    xvf64gerpp(2, 44, 62);
    xvf64gerpp(3, 44, 63);
    xvf64gerpp(4, 46, 60);
    xvf64gerpp(5, 46, 61);
    xvf64gerpp(6, 46, 62);
    xvf64gerpp(7, 46, 63);

    xxpermdi(48, 32, 34, 0b00);
    xxpermdi(52, 32, 34, 0b11);
    xxpermdi(49, 36, 38, 0b00);
    xxpermdi(53, 36, 38, 0b11);

    xxpermdi(50, 40, 42, 0b00);
    xxpermdi(54, 40, 42, 0b11);
    xxpermdi(51, 44, 46, 0b00);
    xxpermdi(55, 44, 46, 0b11);

    xxpermdi(56, 33, 35, 0b00);
    xxpermdi(60, 33, 35, 0b11);
    xxpermdi(57, 37, 39, 0b00);
    xxpermdi(61, 37, 39, 0b11);

    xxpermdi(58, 41, 43, 0b00);
    xxpermdi(62, 41, 43, 0b11);
    xxpermdi(59, 45, 47, 0b00);
    xxpermdi(63, 45, 47, 0b11);

    xvf64gerpp(0, 32, 48);
    xvf64gerpp(1, 32, 49);
    xvf64gerpp(2, 32, 50);
    xvf64gerpp(3, 32, 51);
    xvf64gerpp(4, 34, 48);
    xvf64gerpp(5, 34, 49);
    xvf64gerpp(6, 34, 50);
    xvf64gerpp(7, 34, 51);

    xvf64gerpp(0, 36, 52);
    xvf64gerpp(1, 36, 53);
    xvf64gerpp(2, 36, 54);
    xvf64gerpp(3, 36, 55);
    xvf64gerpp(4, 38, 52);
    xvf64gerpp(5, 38, 53);
    xvf64gerpp(6, 38, 54);
    xvf64gerpp(7, 38, 55);

    xvf64gerpp(0, 40, 56);
    xvf64gerpp(1, 40, 57);
    xvf64gerpp(2, 40, 58);
    xvf64gerpp(3, 40, 59);
    xvf64gerpp(4, 42, 56);
    xvf64gerpp(5, 42, 57);
    xvf64gerpp(6, 42, 58);
    xvf64gerpp(7, 42, 59);

    xvf64gerpp(0, 44, 60);
    xvf64gerpp(1, 44, 61);
    xvf64gerpp(2, 44, 62);
    xvf64gerpp(3, 44, 63);
    xvf64gerpp(4, 46, 60);
    xvf64gerpp(5, 46, 61);
    xvf64gerpp(6, 46, 62);
    xvf64gerpp(7, 46, 63);

    asm("bdnz LOOP02");
}

void matmul_8x8x8_col_row
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP01:");

    xvf64ger(0, 32, 48);
    xvf64ger(1, 32, 49);
    xvf64ger(2, 32, 50);
    xvf64ger(3, 32, 51);
    xvf64ger(4, 34, 48);
    xvf64ger(5, 34, 49);
    xvf64ger(6, 34, 50);
    xvf64ger(7, 34, 51);

    xvf64gerpp(0, 36, 52);
    xvf64gerpp(1, 36, 53);
    xvf64gerpp(2, 36, 54);
    xvf64gerpp(3, 36, 55);
    xvf64gerpp(4, 38, 52);
    xvf64gerpp(5, 38, 53);
    xvf64gerpp(6, 38, 54);
    xvf64gerpp(7, 38, 55);

    xvf64gerpp(0, 40, 56);
    xvf64gerpp(1, 40, 57);
    xvf64gerpp(2, 40, 58);
    xvf64gerpp(3, 40, 59);
    xvf64gerpp(4, 42, 56);
    xvf64gerpp(5, 42, 57);
    xvf64gerpp(6, 42, 58);
    xvf64gerpp(7, 42, 59);

    xvf64gerpp(0, 44, 60);
    xvf64gerpp(1, 44, 61);
    xvf64gerpp(2, 44, 62);
    xvf64gerpp(3, 44, 63);
    xvf64gerpp(4, 46, 60);
    xvf64gerpp(5, 46, 61);
    xvf64gerpp(6, 46, 62);
    xvf64gerpp(7, 46, 63);

    xvf64gerpp(0, 32, 48);
    xvf64gerpp(1, 32, 49);
    xvf64gerpp(2, 32, 50);
    xvf64gerpp(3, 32, 51);
    xvf64gerpp(4, 34, 48);
    xvf64gerpp(5, 34, 49);
    xvf64gerpp(6, 34, 50);
    xvf64gerpp(7, 34, 51);

    xvf64gerpp(0, 36, 52);
    xvf64gerpp(1, 36, 53);
    xvf64gerpp(2, 36, 54);
    xvf64gerpp(3, 36, 55);
    xvf64gerpp(4, 38, 52);
    xvf64gerpp(5, 38, 53);
    xvf64gerpp(6, 38, 54);
    xvf64gerpp(7, 38, 55);

    xvf64gerpp(0, 40, 56);
    xvf64gerpp(1, 40, 57);
    xvf64gerpp(2, 40, 58);
    xvf64gerpp(3, 40, 59);
    xvf64gerpp(4, 42, 56);
    xvf64gerpp(5, 42, 57);
    xvf64gerpp(6, 42, 58);
    xvf64gerpp(7, 42, 59);

    xvf64gerpp(0, 44, 60);
    xvf64gerpp(1, 44, 61);
    xvf64gerpp(2, 44, 62);
    xvf64gerpp(3, 44, 63);
    xvf64gerpp(4, 46, 60);
    xvf64gerpp(5, 46, 61);
    xvf64gerpp(6, 46, 62);
    xvf64gerpp(7, 46, 63);

    asm("bdnz LOOP01");
}

void matmul_8x8x8_col_row_with_loads
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP03:");

    lxvp(32, 3,   0); lxvp(34, 3,  32);
    lxvp(48, 4,   0); lxvp(50, 4,  32);
    lxvp(36, 3,  64); lxvp(38, 3,  96);
    lxvp(52, 4,  64); lxvp(54, 4,  96);
    lxvp(40, 3, 128); lxvp(42, 3, 160);
    lxvp(56, 4, 128); lxvp(58, 4, 160);
    lxvp(44, 3, 192); lxvp(46, 3, 224);
    lxvp(60, 4, 192); lxvp(62, 4, 224);

    xvf64ger(0, 32, 48);
    xvf64ger(1, 32, 49);
    xvf64ger(2, 32, 50);
    xvf64ger(3, 32, 51);
    xvf64ger(4, 34, 48);
    xvf64ger(5, 34, 49);
    xvf64ger(6, 34, 50);
    xvf64ger(7, 34, 51);

    xvf64gerpp(0, 36, 52);
    xvf64gerpp(1, 36, 53);
    xvf64gerpp(2, 36, 54);
    xvf64gerpp(3, 36, 55);
    xvf64gerpp(4, 38, 52);
    xvf64gerpp(5, 38, 53);
    xvf64gerpp(6, 38, 54);
    xvf64gerpp(7, 38, 55);

    xvf64gerpp(0, 40, 56);
    xvf64gerpp(1, 40, 57);
    xvf64gerpp(2, 40, 58);
    xvf64gerpp(3, 40, 59);
    xvf64gerpp(4, 42, 56);
    xvf64gerpp(5, 42, 57);
    xvf64gerpp(6, 42, 58);
    xvf64gerpp(7, 42, 59);

    xvf64gerpp(0, 44, 60);
    xvf64gerpp(1, 44, 61);
    xvf64gerpp(2, 44, 62);
    xvf64gerpp(3, 44, 63);
    xvf64gerpp(4, 46, 60);
    xvf64gerpp(5, 46, 61);
    xvf64gerpp(6, 46, 62);
    xvf64gerpp(7, 46, 63);

    lxvp(32, 3,   0+256); lxvp(34, 3,  32+256);
    lxvp(48, 4,   0+256); lxvp(50, 4,  32+256);
    lxvp(36, 3,  64+256); lxvp(38, 3,  96+256);
    lxvp(52, 4,  64+256); lxvp(54, 4,  96+256);
    lxvp(40, 3, 128+256); lxvp(42, 3, 160+256);
    lxvp(56, 4, 128+256); lxvp(58, 4, 160+256);
    lxvp(44, 3, 192+256); lxvp(46, 3, 224+256);
    lxvp(60, 4, 192+256); lxvp(62, 4, 224+256);

    xvf64gerpp(0, 32, 48);
    xvf64gerpp(1, 32, 49);
    xvf64gerpp(2, 32, 50);
    xvf64gerpp(3, 32, 51);
    xvf64gerpp(4, 34, 48);
    xvf64gerpp(5, 34, 49);
    xvf64gerpp(6, 34, 50);
    xvf64gerpp(7, 34, 51);

    xvf64gerpp(0, 36, 52);
    xvf64gerpp(1, 36, 53);
    xvf64gerpp(2, 36, 54);
    xvf64gerpp(3, 36, 55);
    xvf64gerpp(4, 38, 52);
    xvf64gerpp(5, 38, 53);
    xvf64gerpp(6, 38, 54);
    xvf64gerpp(7, 38, 55);

    xvf64gerpp(0, 40, 56);
    xvf64gerpp(1, 40, 57);
    xvf64gerpp(2, 40, 58);
    xvf64gerpp(3, 40, 59);
    xvf64gerpp(4, 42, 56);
    xvf64gerpp(5, 42, 57);
    xvf64gerpp(6, 42, 58);
    xvf64gerpp(7, 42, 59);

    xvf64gerpp(0, 44, 60);
    xvf64gerpp(1, 44, 61);
    xvf64gerpp(2, 44, 62);
    xvf64gerpp(3, 44, 63);
    xvf64gerpp(4, 46, 60);
    xvf64gerpp(5, 46, 61);
    xvf64gerpp(6, 46, 62);
    xvf64gerpp(7, 46, 63);

    asm("bdnz LOOP03");
}

void matmul_8x8x8_col_row_just_loads
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP17:");

    lxvp(32, 3,   0); lxvp(34, 3,  32);
    lxvp(48, 4,   0); lxvp(50, 4,  32);
    lxvp(36, 3,  64); lxvp(38, 3,  96);
    lxvp(52, 4,  64); lxvp(54, 4,  96);
    lxvp(40, 3, 128); lxvp(42, 3, 160);
    lxvp(56, 4, 128); lxvp(58, 4, 160);
    lxvp(44, 3, 192); lxvp(46, 3, 224);
    lxvp(60, 4, 192); lxvp(62, 4, 224);

    lxvp(32, 3,   0+256); lxvp(34, 3,  32+256);
    lxvp(48, 4,   0+256); lxvp(50, 4,  32+256);
    lxvp(36, 3,  64+256); lxvp(38, 3,  96+256);
    lxvp(52, 4,  64+256); lxvp(54, 4,  96+256);
    lxvp(40, 3, 128+256); lxvp(42, 3, 160+256);
    lxvp(56, 4, 128+256); lxvp(58, 4, 160+256);
    lxvp(44, 3, 192+256); lxvp(46, 3, 224+256);
    lxvp(60, 4, 192+256); lxvp(62, 4, 224+256);

    asm("bdnz LOOP17");
}

void matmul_8x8x8_col_row_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP05:");

    lxvp(32, 3,   0); lxvp(34, 3,  32);
    lxvp(48, 4,   0); lxvp(50, 4,  32);
    lxvp(36, 3,  64); lxvp(38, 3,  96);
    lxvp(52, 4,  64); lxvp(54, 4,  96);
    lxvp(40, 3, 128); lxvp(42, 3, 160);
    lxvp(56, 4, 128); lxvp(58, 4, 160);
    lxvp(44, 3, 192); lxvp(46, 3, 224);
    lxvp(60, 4, 192); lxvp(62, 4, 224);

    xvf64ger(0, 32, 48);
    xvf64ger(1, 32, 49);
    xvf64ger(2, 32, 50);
    xvf64ger(3, 32, 51);
    xvf64ger(4, 34, 48);
    xvf64ger(5, 34, 49);
    xvf64ger(6, 34, 50);
    xvf64ger(7, 34, 51);

    xvf64gerpp(0, 36, 52);
    xvf64gerpp(1, 36, 53);
    xvf64gerpp(2, 36, 54);
    xvf64gerpp(3, 36, 55);
    xvf64gerpp(4, 38, 52);
    xvf64gerpp(5, 38, 53);
    xvf64gerpp(6, 38, 54);
    xvf64gerpp(7, 38, 55);

    xvf64gerpp(0, 40, 56);
    xvf64gerpp(1, 40, 57);
    xvf64gerpp(2, 40, 58);
    xvf64gerpp(3, 40, 59);
    xvf64gerpp(4, 42, 56);
    xvf64gerpp(5, 42, 57);
    xvf64gerpp(6, 42, 58);
    xvf64gerpp(7, 42, 59);

    xvf64gerpp(0, 44, 60);
    xvf64gerpp(1, 44, 61);
    xvf64gerpp(2, 44, 62);
    xvf64gerpp(3, 44, 63);
    xvf64gerpp(4, 46, 60);
    xvf64gerpp(5, 46, 61);
    xvf64gerpp(6, 46, 62);
    xvf64gerpp(7, 46, 63);

    lxvp(32, 3,   0+256); lxvp(34, 3,  32+256);
    lxvp(48, 4,   0+256); lxvp(50, 4,  32+256);
    lxvp(36, 3,  64+256); lxvp(38, 3,  96+256);
    lxvp(52, 4,  64+256); lxvp(54, 4,  96+256);
    lxvp(40, 3, 128+256); lxvp(42, 3, 160+256);
    lxvp(56, 4, 128+256); lxvp(58, 4, 160+256);
    lxvp(44, 3, 192+256); lxvp(46, 3, 224+256);
    lxvp(60, 4, 192+256); lxvp(62, 4, 224+256);

    xvf64gerpp(0, 32, 48);
    xvf64gerpp(1, 32, 49);
    xvf64gerpp(2, 32, 50);
    xvf64gerpp(3, 32, 51);
    xvf64gerpp(4, 34, 48);
    xvf64gerpp(5, 34, 49);
    xvf64gerpp(6, 34, 50);
    xvf64gerpp(7, 34, 51);

    xvf64gerpp(0, 36, 52);
    xvf64gerpp(1, 36, 53);
    xvf64gerpp(2, 36, 54);
    xvf64gerpp(3, 36, 55);
    xvf64gerpp(4, 38, 52);
    xvf64gerpp(5, 38, 53);
    xvf64gerpp(6, 38, 54);
    xvf64gerpp(7, 38, 55);

    xvf64gerpp(0, 40, 56);
    xvf64gerpp(1, 40, 57);
    xvf64gerpp(2, 40, 58);
    xvf64gerpp(3, 40, 59);
    xvf64gerpp(4, 42, 56);
    xvf64gerpp(5, 42, 57);
    xvf64gerpp(6, 42, 58);
    xvf64gerpp(7, 42, 59);

    xvf64gerpp(0, 44, 60);
    xvf64gerpp(1, 44, 61);
    xvf64gerpp(2, 44, 62);
    xvf64gerpp(3, 44, 63);
    xvf64gerpp(4, 46, 60);
    xvf64gerpp(5, 46, 61);
    xvf64gerpp(6, 46, 62);
    xvf64gerpp(7, 46, 63);

    xxmfacc(0);
    xxmfacc(1);
    xxmfacc(2);
    xxmfacc(3);
    xxmfacc(4);
    xxmfacc(5);
    xxmfacc(6);
    xxmfacc(7);

    lxvp(32+ 0, 5,   0);
    lxvp(32+ 2, 5,  32);
    lxvp(32+ 4, 5,  64);
    lxvp(32+ 6, 5,  96);
    lxvp(32+ 8, 5, 128);
    lxvp(32+10, 5, 160);
    lxvp(32+12, 5, 192);
    lxvp(32+14, 5, 224);
    lxvp(32+16, 5, 256);
    lxvp(32+18, 5, 288);
    lxvp(32+20, 5, 320);
    lxvp(32+22, 5, 352);
    lxvp(32+24, 5, 384);
    lxvp(32+26, 5, 416);
    lxvp(32+28, 5, 448);
    lxvp(32+30, 5, 480);

    xvadddp(32+ 0,  0, 32+ 0);
    xvadddp(32+ 2,  2, 32+ 2);
    xvadddp(32+ 4,  4, 32+ 4);
    xvadddp(32+ 6,  6, 32+ 6);
    xvadddp(32+ 8,  8, 32+ 8);
    xvadddp(32+10, 10, 32+10);
    xvadddp(32+12, 12, 32+12);
    xvadddp(32+14, 14, 32+14);
    xvadddp(32+16, 16, 32+16);
    xvadddp(32+18, 18, 32+18);
    xvadddp(32+20, 20, 32+20);
    xvadddp(32+22, 22, 32+22);
    xvadddp(32+24, 24, 32+24);
    xvadddp(32+26, 26, 32+26);
    xvadddp(32+28, 28, 32+28);
    xvadddp(32+30, 30, 32+30);

    stxvp(32+ 0, 5,   0);
    stxvp(32+ 2, 5,  32);
    stxvp(32+ 4, 5,  64);
    stxvp(32+ 6, 5,  96);
    stxvp(32+ 8, 5, 128);
    stxvp(32+10, 5, 160);
    stxvp(32+12, 5, 192);
    stxvp(32+14, 5, 224);
    stxvp(32+16, 5, 256);
    stxvp(32+18, 5, 288);
    stxvp(32+20, 5, 320);
    stxvp(32+22, 5, 352);
    stxvp(32+24, 5, 384);
    stxvp(32+26, 5, 416);
    stxvp(32+28, 5, 448);
    stxvp(32+30, 5, 480);

    asm("bdnz LOOP05");
}

void matmul_8x8x8_col_col_with_loads
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP04:");

    lxvp(32, 4,   0); 
    lxvp(34, 4,  32);
    lxvp(36, 4,  64); 
    lxvp(38, 4,  96);
    lxvp(40, 4, 128); 
    lxvp(42, 4, 160);
    lxvp(44, 4, 192); 
    lxvp(46, 4, 224);

    xxpermdi(48, 32, 34, 0b00);
    xxpermdi(52, 32, 34, 0b11);
    xxpermdi(49, 36, 38, 0b00);
    xxpermdi(53, 36, 38, 0b11);

    xxpermdi(50, 40, 42, 0b00);
    xxpermdi(54, 40, 42, 0b11);
    xxpermdi(51, 44, 46, 0b00);
    xxpermdi(55, 44, 46, 0b11);

    xxpermdi(56, 33, 35, 0b00);
    xxpermdi(60, 33, 35, 0b11);
    xxpermdi(57, 37, 39, 0b00);
    xxpermdi(61, 37, 39, 0b11);

    xxpermdi(58, 41, 43, 0b00);
    xxpermdi(62, 41, 43, 0b11);
    xxpermdi(59, 45, 47, 0b00);
    xxpermdi(63, 45, 47, 0b11);

    lxvp(32, 3,   0); lxvp(34, 3,  32);
    lxvp(36, 3,  64); lxvp(38, 3,  96);
    lxvp(40, 3, 128); lxvp(42, 3, 160);
    lxvp(44, 3, 192); lxvp(46, 3, 224);

    xvf64ger(0, 32, 48);
    xvf64ger(1, 32, 49);
    xvf64ger(2, 32, 50);
    xvf64ger(3, 32, 51);
    xvf64ger(4, 34, 48);
    xvf64ger(5, 34, 49);
    xvf64ger(6, 34, 50);
    xvf64ger(7, 34, 51);

    xvf64gerpp(0, 36, 52);
    xvf64gerpp(1, 36, 53);
    xvf64gerpp(2, 36, 54);
    xvf64gerpp(3, 36, 55);
    xvf64gerpp(4, 38, 52);
    xvf64gerpp(5, 38, 53);
    xvf64gerpp(6, 38, 54);
    xvf64gerpp(7, 38, 55);

    xvf64gerpp(0, 40, 56);
    xvf64gerpp(1, 40, 57);
    xvf64gerpp(2, 40, 58);
    xvf64gerpp(3, 40, 59);
    xvf64gerpp(4, 42, 56);
    xvf64gerpp(5, 42, 57);
    xvf64gerpp(6, 42, 58);
    xvf64gerpp(7, 42, 59);

    xvf64gerpp(0, 44, 60);
    xvf64gerpp(1, 44, 61);
    xvf64gerpp(2, 44, 62);
    xvf64gerpp(3, 44, 63);
    xvf64gerpp(4, 46, 60);
    xvf64gerpp(5, 46, 61);
    xvf64gerpp(6, 46, 62);
    xvf64gerpp(7, 46, 63);

    lxvp(32, 4,   0+256); 
    lxvp(34, 4,  32+256);
    lxvp(36, 4,  64+256); 
    lxvp(38, 4,  96+256);
    lxvp(40, 4, 128+256); 
    lxvp(42, 4, 160+256);
    lxvp(44, 4, 192+256); 
    lxvp(46, 4, 224+256);

    xxpermdi(48, 32, 34, 0b00);
    xxpermdi(52, 32, 34, 0b11);
    xxpermdi(49, 36, 38, 0b00);
    xxpermdi(53, 36, 38, 0b11);

    xxpermdi(50, 40, 42, 0b00);
    xxpermdi(54, 40, 42, 0b11);
    xxpermdi(51, 44, 46, 0b00);
    xxpermdi(55, 44, 46, 0b11);

    xxpermdi(56, 33, 35, 0b00);
    xxpermdi(60, 33, 35, 0b11);
    xxpermdi(57, 37, 39, 0b00);
    xxpermdi(61, 37, 39, 0b11);

    xxpermdi(58, 41, 43, 0b00);
    xxpermdi(62, 41, 43, 0b11);
    xxpermdi(59, 45, 47, 0b00);
    xxpermdi(63, 45, 47, 0b11);

    lxvp(32, 3,   0+256); lxvp(34, 3,  32+256);
    lxvp(36, 3,  64+256); lxvp(38, 3,  96+256);
    lxvp(40, 3, 128+256); lxvp(42, 3, 160+256);
    lxvp(44, 3, 192+256); lxvp(46, 3, 224+256);

    xvf64gerpp(0, 32, 48);
    xvf64gerpp(1, 32, 49);
    xvf64gerpp(2, 32, 50);
    xvf64gerpp(3, 32, 51);
    xvf64gerpp(4, 34, 48);
    xvf64gerpp(5, 34, 49);
    xvf64gerpp(6, 34, 50);
    xvf64gerpp(7, 34, 51);

    xvf64gerpp(0, 36, 52);
    xvf64gerpp(1, 36, 53);
    xvf64gerpp(2, 36, 54);
    xvf64gerpp(3, 36, 55);
    xvf64gerpp(4, 38, 52);
    xvf64gerpp(5, 38, 53);
    xvf64gerpp(6, 38, 54);
    xvf64gerpp(7, 38, 55);

    xvf64gerpp(0, 40, 56);
    xvf64gerpp(1, 40, 57);
    xvf64gerpp(2, 40, 58);
    xvf64gerpp(3, 40, 59);
    xvf64gerpp(4, 42, 56);
    xvf64gerpp(5, 42, 57);
    xvf64gerpp(6, 42, 58);
    xvf64gerpp(7, 42, 59);

    xvf64gerpp(0, 44, 60);
    xvf64gerpp(1, 44, 61);
    xvf64gerpp(2, 44, 62);
    xvf64gerpp(3, 44, 63);
    xvf64gerpp(4, 46, 60);
    xvf64gerpp(5, 46, 61);
    xvf64gerpp(6, 46, 62);
    xvf64gerpp(7, 46, 63);

    asm("bdnz LOOP04");
}

void matmul_8x8x8_col_col_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP06:");

    lxvp(32, 4,   0); 
    lxvp(34, 4,  32);
    lxvp(36, 4,  64); 
    lxvp(38, 4,  96);
    lxvp(40, 4, 128); 
    lxvp(42, 4, 160);
    lxvp(44, 4, 192); 
    lxvp(46, 4, 224);

    xxpermdi(48, 32, 34, 0b00);
    xxpermdi(52, 32, 34, 0b11);
    xxpermdi(49, 36, 38, 0b00);
    xxpermdi(53, 36, 38, 0b11);

    xxpermdi(50, 40, 42, 0b00);
    xxpermdi(54, 40, 42, 0b11);
    xxpermdi(51, 44, 46, 0b00);
    xxpermdi(55, 44, 46, 0b11);

    xxpermdi(56, 33, 35, 0b00);
    xxpermdi(60, 33, 35, 0b11);
    xxpermdi(57, 37, 39, 0b00);
    xxpermdi(61, 37, 39, 0b11);

    xxpermdi(58, 41, 43, 0b00);
    xxpermdi(62, 41, 43, 0b11);
    xxpermdi(59, 45, 47, 0b00);
    xxpermdi(63, 45, 47, 0b11);

    lxvp(32, 3,   0); lxvp(34, 3,  32);
    lxvp(36, 3,  64); lxvp(38, 3,  96);
    lxvp(40, 3, 128); lxvp(42, 3, 160);
    lxvp(44, 3, 192); lxvp(46, 3, 224);

    xvf64ger(0, 32, 48);
    xvf64ger(1, 32, 49);
    xvf64ger(2, 32, 50);
    xvf64ger(3, 32, 51);
    xvf64ger(4, 34, 48);
    xvf64ger(5, 34, 49);
    xvf64ger(6, 34, 50);
    xvf64ger(7, 34, 51);

    xvf64gerpp(0, 36, 52);
    xvf64gerpp(1, 36, 53);
    xvf64gerpp(2, 36, 54);
    xvf64gerpp(3, 36, 55);
    xvf64gerpp(4, 38, 52);
    xvf64gerpp(5, 38, 53);
    xvf64gerpp(6, 38, 54);
    xvf64gerpp(7, 38, 55);

    xvf64gerpp(0, 40, 56);
    xvf64gerpp(1, 40, 57);
    xvf64gerpp(2, 40, 58);
    xvf64gerpp(3, 40, 59);
    xvf64gerpp(4, 42, 56);
    xvf64gerpp(5, 42, 57);
    xvf64gerpp(6, 42, 58);
    xvf64gerpp(7, 42, 59);

    xvf64gerpp(0, 44, 60);
    xvf64gerpp(1, 44, 61);
    xvf64gerpp(2, 44, 62);
    xvf64gerpp(3, 44, 63);
    xvf64gerpp(4, 46, 60);
    xvf64gerpp(5, 46, 61);
    xvf64gerpp(6, 46, 62);
    xvf64gerpp(7, 46, 63);

    lxvp(32, 4,   0+256); 
    lxvp(34, 4,  32+256);
    lxvp(36, 4,  64+256); 
    lxvp(38, 4,  96+256);
    lxvp(40, 4, 128+256); 
    lxvp(42, 4, 160+256);
    lxvp(44, 4, 192+256); 
    lxvp(46, 4, 224+256);

    xxpermdi(48, 32, 34, 0b00);
    xxpermdi(52, 32, 34, 0b11);
    xxpermdi(49, 36, 38, 0b00);
    xxpermdi(53, 36, 38, 0b11);

    xxpermdi(50, 40, 42, 0b00);
    xxpermdi(54, 40, 42, 0b11);
    xxpermdi(51, 44, 46, 0b00);
    xxpermdi(55, 44, 46, 0b11);

    xxpermdi(56, 33, 35, 0b00);
    xxpermdi(60, 33, 35, 0b11);
    xxpermdi(57, 37, 39, 0b00);
    xxpermdi(61, 37, 39, 0b11);

    xxpermdi(58, 41, 43, 0b00);
    xxpermdi(62, 41, 43, 0b11);
    xxpermdi(59, 45, 47, 0b00);
    xxpermdi(63, 45, 47, 0b11);

    lxvp(32, 3,   0+256); lxvp(34, 3,  32+256);
    lxvp(36, 3,  64+256); lxvp(38, 3,  96+256);
    lxvp(40, 3, 128+256); lxvp(42, 3, 160+256);
    lxvp(44, 3, 192+256); lxvp(46, 3, 224+256);

    xvf64gerpp(0, 32, 48);
    xvf64gerpp(1, 32, 49);
    xvf64gerpp(2, 32, 50);
    xvf64gerpp(3, 32, 51);
    xvf64gerpp(4, 34, 48);
    xvf64gerpp(5, 34, 49);
    xvf64gerpp(6, 34, 50);
    xvf64gerpp(7, 34, 51);

    xvf64gerpp(0, 36, 52);
    xvf64gerpp(1, 36, 53);
    xvf64gerpp(2, 36, 54);
    xvf64gerpp(3, 36, 55);
    xvf64gerpp(4, 38, 52);
    xvf64gerpp(5, 38, 53);
    xvf64gerpp(6, 38, 54);
    xvf64gerpp(7, 38, 55);

    xvf64gerpp(0, 40, 56);
    xvf64gerpp(1, 40, 57);
    xvf64gerpp(2, 40, 58);
    xvf64gerpp(3, 40, 59);
    xvf64gerpp(4, 42, 56);
    xvf64gerpp(5, 42, 57);
    xvf64gerpp(6, 42, 58);
    xvf64gerpp(7, 42, 59);

    xvf64gerpp(0, 44, 60);
    xvf64gerpp(1, 44, 61);
    xvf64gerpp(2, 44, 62);
    xvf64gerpp(3, 44, 63);
    xvf64gerpp(4, 46, 60);
    xvf64gerpp(5, 46, 61);
    xvf64gerpp(6, 46, 62);
    xvf64gerpp(7, 46, 63);

    xxmfacc(0);
    xxmfacc(1);
    xxmfacc(2);
    xxmfacc(3);
    xxmfacc(4);
    xxmfacc(5);
    xxmfacc(6);
    xxmfacc(7);

    lxvp(32+ 0, 5,   0);
    lxvp(32+ 2, 5,  32);
    lxvp(32+ 4, 5,  64);
    lxvp(32+ 6, 5,  96);
    lxvp(32+ 8, 5, 128);
    lxvp(32+10, 5, 160);
    lxvp(32+12, 5, 192);
    lxvp(32+14, 5, 224);
    lxvp(32+16, 5, 256);
    lxvp(32+18, 5, 288);
    lxvp(32+20, 5, 320);
    lxvp(32+22, 5, 352);
    lxvp(32+24, 5, 384);
    lxvp(32+26, 5, 416);
    lxvp(32+28, 5, 448);
    lxvp(32+30, 5, 480);

    xvadddp(32+ 0,  0, 32+ 0);
    xvadddp(32+ 2,  2, 32+ 2);
    xvadddp(32+ 4,  4, 32+ 4);
    xvadddp(32+ 6,  6, 32+ 6);
    xvadddp(32+ 8,  8, 32+ 8);
    xvadddp(32+10, 10, 32+10);
    xvadddp(32+12, 12, 32+12);
    xvadddp(32+14, 14, 32+14);
    xvadddp(32+16, 16, 32+16);
    xvadddp(32+18, 18, 32+18);
    xvadddp(32+20, 20, 32+20);
    xvadddp(32+22, 22, 32+22);
    xvadddp(32+24, 24, 32+24);
    xvadddp(32+26, 26, 32+26);
    xvadddp(32+28, 28, 32+28);
    xvadddp(32+30, 30, 32+30);

    stxvp(32+ 0, 5,   0);
    stxvp(32+ 2, 5,  32);
    stxvp(32+ 4, 5,  64);
    stxvp(32+ 6, 5,  96);
    stxvp(32+ 8, 5, 128);
    stxvp(32+10, 5, 160);
    stxvp(32+12, 5, 192);
    stxvp(32+14, 5, 224);
    stxvp(32+16, 5, 256);
    stxvp(32+18, 5, 288);
    stxvp(32+20, 5, 320);
    stxvp(32+22, 5, 352);
    stxvp(32+24, 5, 384);
    stxvp(32+26, 5, 416);
    stxvp(32+28, 5, 448);
    stxvp(32+30, 5, 480);

    asm("bdnz LOOP06");
}

#define load_B_packed_4x8(D)   \
    lxvp(32, 4,   0+D);        \
    lxvp(34, 4,  32+D);        \
    lxvp(36, 4,  64+D);        \
    lxvp(38, 4,  96+D);        \
    lxvp(40, 4, 128+D);        \
    lxvp(42, 4, 160+D);        \
    lxvp(44, 4, 192+D);        \
    lxvp(46, 4, 224+D);

#define transpose_B_4x8()           \
    xxpermdi(48, 32, 34, 0b00); \
    xxpermdi(52, 32, 34, 0b11); \
    xxpermdi(49, 36, 38, 0b00); \
    xxpermdi(53, 36, 38, 0b11); \
    xxpermdi(50, 40, 42, 0b00); \
    xxpermdi(54, 40, 42, 0b11); \
    xxpermdi(51, 44, 46, 0b00); \
    xxpermdi(55, 44, 46, 0b11); \
    xxpermdi(56, 33, 35, 0b00); \
    xxpermdi(60, 33, 35, 0b11); \
    xxpermdi(57, 37, 39, 0b00); \
    xxpermdi(61, 37, 39, 0b11); \
    xxpermdi(58, 41, 43, 0b00); \
    xxpermdi(62, 41, 43, 0b11); \
    xxpermdi(59, 45, 47, 0b00); \
    xxpermdi(63, 45, 47, 0b11);

#define store_B_packed_4x8(D)   \
    stxvp(48, 6,   0+D);        \
    stxvp(50, 6,  32+D);        \
    stxvp(52, 6,  64+D);        \
    stxvp(54, 6,  96+D);        \
    stxvp(56, 6, 128+D);        \
    stxvp(58, 6, 160+D);        \
    stxvp(60, 6, 192+D);        \
    stxvp(62, 6, 224+D);

#define load_A_packed_8x4(D)                    \
    lxvp(32, 3,   0+D); lxvp(34, 3,  32+D); \
    lxvp(36, 3,  64+D); lxvp(38, 3,  96+D); \
    lxvp(40, 3, 128+D); lxvp(42, 3, 160+D); \
    lxvp(44, 3, 192+D); lxvp(46, 3, 224+D);

#define rank_4_update_8x8_first() \
    xvf64ger(0, 32, 48);          \
    xvf64ger(1, 32, 49);          \
    xvf64ger(2, 32, 50);          \
    xvf64ger(3, 32, 51);          \
    xvf64ger(4, 34, 48);          \
    xvf64ger(5, 34, 49);          \
    xvf64ger(6, 34, 50);          \
    xvf64ger(7, 34, 51);          \
    xvf64gerpp(0, 36, 52);        \
    xvf64gerpp(1, 36, 53);        \
    xvf64gerpp(2, 36, 54);        \
    xvf64gerpp(3, 36, 55);        \
    xvf64gerpp(4, 38, 52);        \
    xvf64gerpp(5, 38, 53);        \
    xvf64gerpp(6, 38, 54);        \
    xvf64gerpp(7, 38, 55);        \
    xvf64gerpp(0, 40, 56);        \
    xvf64gerpp(1, 40, 57);        \
    xvf64gerpp(2, 40, 58);        \
    xvf64gerpp(3, 40, 59);        \
    xvf64gerpp(4, 42, 56);        \
    xvf64gerpp(5, 42, 57);        \
    xvf64gerpp(6, 42, 58);        \
    xvf64gerpp(7, 42, 59);        \
    xvf64gerpp(0, 44, 60);        \
    xvf64gerpp(1, 44, 61);        \
    xvf64gerpp(2, 44, 62);        \
    xvf64gerpp(3, 44, 63);        \
    xvf64gerpp(4, 46, 60);        \
    xvf64gerpp(5, 46, 61);        \
    xvf64gerpp(6, 46, 62);        \
    xvf64gerpp(7, 46, 63);    

#define rank_4_update_8x8() \
    xvf64gerpp(0, 32, 48); \
    xvf64gerpp(1, 32, 49); \
    xvf64gerpp(2, 32, 50); \
    xvf64gerpp(3, 32, 51); \
    xvf64gerpp(4, 34, 48); \
    xvf64gerpp(5, 34, 49); \
    xvf64gerpp(6, 34, 50); \
    xvf64gerpp(7, 34, 51); \
    xvf64gerpp(0, 36, 52); \
    xvf64gerpp(1, 36, 53); \
    xvf64gerpp(2, 36, 54); \
    xvf64gerpp(3, 36, 55); \
    xvf64gerpp(4, 38, 52); \
    xvf64gerpp(5, 38, 53); \
    xvf64gerpp(6, 38, 54); \
    xvf64gerpp(7, 38, 55); \
    xvf64gerpp(0, 40, 56); \
    xvf64gerpp(1, 40, 57); \
    xvf64gerpp(2, 40, 58); \
    xvf64gerpp(3, 40, 59); \
    xvf64gerpp(4, 42, 56); \
    xvf64gerpp(5, 42, 57); \
    xvf64gerpp(6, 42, 58); \
    xvf64gerpp(7, 42, 59); \
    xvf64gerpp(0, 44, 60); \
    xvf64gerpp(1, 44, 61); \
    xvf64gerpp(2, 44, 62); \
    xvf64gerpp(3, 44, 63); \
    xvf64gerpp(4, 46, 60); \
    xvf64gerpp(5, 46, 61); \
    xvf64gerpp(6, 46, 62); \
    xvf64gerpp(7, 46, 63);

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
    xvadddp(32+ 0,  0, 32+ 0); \
    xvadddp(32+ 2,  2, 32+ 2); \
    xvadddp(32+ 4,  4, 32+ 4); \
    xvadddp(32+ 6,  6, 32+ 6); \
    xvadddp(32+ 8,  8, 32+ 8); \
    xvadddp(32+10, 10, 32+10); \
    xvadddp(32+12, 12, 32+12); \
    xvadddp(32+14, 14, 32+14); \
    xvadddp(32+16, 16, 32+16); \
    xvadddp(32+18, 18, 32+18); \
    xvadddp(32+20, 20, 32+20); \
    xvadddp(32+22, 22, 32+22); \
    xvadddp(32+24, 24, 32+24); \
    xvadddp(32+26, 26, 32+26); \
    xvadddp(32+28, 28, 32+28); \
    xvadddp(32+30, 30, 32+30); \
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
    stxvp(32+30, 5, 480);      \

#define rank_8_update_8x8_transpose_B_first(D) \
    load_B_packed_4x8((D+0));                  \
    transpose_B_4x8();                         \
    load_A_packed_8x4((D+0));                  \
    rank_4_update_8x8_first();                 \
    load_B_packed_4x8((D+256));                \
    transpose_B_4x8();                         \
    load_A_packed_8x4((D+256));                \
    rank_4_update_8x8();

#define rank_8_update_8x8_transpose_B(D) \
    load_B_packed_4x8((D+0));            \
    transpose_B_4x8();                   \
    load_A_packed_8x4((D+0));            \
    rank_4_update_8x8();                 \
    load_B_packed_4x8((D+256));          \
    transpose_B_4x8();                   \
    load_A_packed_8x4((D+256));          \
    rank_4_update_8x8();

#define rank_8_update_8x8_transpose_and_store_B_first(D) \
    load_B_packed_4x8((D+0));                  \
    transpose_B_4x8();                         \
    store_B_packed_4x8((D+0));		       \
    load_A_packed_8x4((D+0));                  \
    rank_4_update_8x8_first();                 \
    load_B_packed_4x8((D+256));                \
    transpose_B_4x8();                         \
    store_B_packed_4x8((D+256));	       \
    load_A_packed_8x4((D+256));                \
    rank_4_update_8x8();

#define rank_8_update_8x8_transpose_and_store_B(D) \
    load_B_packed_4x8((D+0));            \
    transpose_B_4x8();                   \
    load_A_packed_8x4((D+0));            \
    rank_4_update_8x8();                 \
    load_B_packed_4x8((D+256));          \
    transpose_B_4x8();                   \
    load_A_packed_8x4((D+256));          \
    rank_4_update_8x8();

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

void matmul_8x8x16_col_row_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP13:");

    rank_8_update_8x8_first(0);
    rank_8_update_8x8( 512);

    add_and_store_8x8();

    asm("bdnz LOOP13");
}

void matmul_8x8x24_col_row_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP14:");

    rank_8_update_8x8_first(0);
    rank_8_update_8x8( 512);
    rank_8_update_8x8(1024);

    add_and_store_8x8();

    asm("bdnz LOOP14");
}

void matmul_8x8x32_col_row_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP11:");

    rank_8_update_8x8_first(0);
    rank_8_update_8x8( 512);
    rank_8_update_8x8(1024);
    rank_8_update_8x8(1536);

    add_and_store_8x8();

    asm("bdnz LOOP11");
}

void matmul_8x8x40_col_row_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP12:");

    rank_8_update_8x8_first(0);
    rank_8_update_8x8( 512);
    rank_8_update_8x8(1024);
    rank_8_update_8x8(1536);
    rank_8_update_8x8(2048);

    add_and_store_8x8();

    asm("bdnz LOOP12");
}

void matmul_8x8x48_col_row_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP09:");

    rank_8_update_8x8_first(0);
    rank_8_update_8x8( 512);
    rank_8_update_8x8(1024);
    rank_8_update_8x8(1536);
    rank_8_update_8x8(2048);
    rank_8_update_8x8(2560);

    add_and_store_8x8();

    asm("bdnz LOOP09");
}

void matmul_8x8x56_col_row_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP10:");

    rank_8_update_8x8_first(0);
    rank_8_update_8x8( 512);
    rank_8_update_8x8(1024);
    rank_8_update_8x8(1536);
    rank_8_update_8x8(2048);
    rank_8_update_8x8(2560);
    rank_8_update_8x8(3072);

    add_and_store_8x8();

    asm("bdnz LOOP10");
}

void matmul_8x8x64_col_row_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP08:");

    load_B_packed_4x8(0);
    load_A_packed_8x4(0);
    rank_4_update_8x8_first();
    load_B_packed_4x8(256);
    load_A_packed_8x4(256);
    rank_4_update_8x8();

    load_B_packed_4x8(512);
    load_A_packed_8x4(512);
    rank_4_update_8x8();
    load_B_packed_4x8(768);
    load_A_packed_8x4(768);
    rank_4_update_8x8();

    load_B_packed_4x8(1024);
    load_A_packed_8x4(1024);
    rank_4_update_8x8();
    load_B_packed_4x8(1280);
    load_A_packed_8x4(1280);
    rank_4_update_8x8();

    load_B_packed_4x8(1536);
    load_A_packed_8x4(1536);
    rank_4_update_8x8();
    load_B_packed_4x8(1792);
    load_A_packed_8x4(1792);
    rank_4_update_8x8();

    load_B_packed_4x8(2048);
    load_A_packed_8x4(2048);
    rank_4_update_8x8();
    load_B_packed_4x8(2304);
    load_A_packed_8x4(2304);
    rank_4_update_8x8();

    load_B_packed_4x8(2560);
    load_A_packed_8x4(2560);
    rank_4_update_8x8();
    load_B_packed_4x8(2816);
    load_A_packed_8x4(2816);
    rank_4_update_8x8();

    load_B_packed_4x8(3072);
    load_A_packed_8x4(3072);
    rank_4_update_8x8();
    load_B_packed_4x8(3328);
    load_A_packed_8x4(3328);
    rank_4_update_8x8();

    load_B_packed_4x8(3584);
    load_A_packed_8x4(3584);
    rank_4_update_8x8();
    load_B_packed_4x8(3840);
    load_A_packed_8x4(3840);
    rank_4_update_8x8();

    add_and_store_8x8();

    asm("bdnz LOOP08");
}

void matmul_8x8x64_col_col_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP15:");

    rank_8_update_8x8_transpose_B_first(0);
    rank_8_update_8x8_transpose_B( 512);
    rank_8_update_8x8_transpose_B(1024);
    rank_8_update_8x8_transpose_B(1536);
    rank_8_update_8x8_transpose_B(2048);
    rank_8_update_8x8_transpose_B(2560);
    rank_8_update_8x8_transpose_B(3072);
    rank_8_update_8x8_transpose_B(3584);

    add_and_store_8x8();

    asm("bdnz LOOP15");
}

void matmul_8x8x64_col_col_with_loads_and_stores_store_B
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    li(7, COUNT);
    asm("mtctr 7");
    asm("LOOP16:");

    rank_8_update_8x8_transpose_and_store_B_first(0);
    rank_8_update_8x8_transpose_and_store_B( 512);
    rank_8_update_8x8_transpose_and_store_B(1024);
    rank_8_update_8x8_transpose_and_store_B(1536);
    rank_8_update_8x8_transpose_and_store_B(2048);
    rank_8_update_8x8_transpose_and_store_B(2560);
    rank_8_update_8x8_transpose_and_store_B(3072);
    rank_8_update_8x8_transpose_and_store_B(3584);

    add_and_store_8x8();

    asm("bdnz LOOP16");
}

void matmul_16x8x64_col_col_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    matmul_8x8x64_col_col_with_loads_and_stores_store_B(A+   0, B, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+ 512, D, C, D);
}

void matmul_24x8x64_col_col_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    matmul_8x8x64_col_col_with_loads_and_stores_store_B(A+   0, B, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+ 512, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+1024, D, C, D);
}

void matmul_32x8x64_col_col_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    matmul_8x8x64_col_col_with_loads_and_stores_store_B(A+   0, B, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+ 512, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+1024, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+1536, D, C, D);
}

void matmul_40x8x64_col_col_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    matmul_8x8x64_col_col_with_loads_and_stores_store_B(A+   0, B, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+ 512, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+1024, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+1536, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+2048, D, C, D);
}

void matmul_48x8x64_col_col_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    matmul_8x8x64_col_col_with_loads_and_stores_store_B(A+   0, B, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+ 512, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+1024, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+1536, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+2048, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+2560, D, C, D);
}

void matmul_56x8x64_col_col_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    matmul_8x8x64_col_col_with_loads_and_stores_store_B(A+   0, B, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+ 512, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+1024, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+1536, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+2048, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+2560, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+3072, D, C, D);
}

void matmul_64x8x64_col_col_with_loads_and_stores
(
    double *A,
    double *B,
    double *C,
    double *D
)
{
    matmul_8x8x64_col_col_with_loads_and_stores_store_B(A+   0, B, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+ 512, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+1024, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+1536, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+2048, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+2560, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+3072, D, C, D);
    matmul_8x8x64_col_row_with_loads_and_stores        (A+3584, D, C, D);
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

double run_kernel
(
    void (kernel)(double*, double*, double*, double*),
    uint32_t count
)
{
    int nthreads = omp_get_max_threads();
    std::vector<double*> A(nthreads);
    std::vector<double*> B(nthreads);
    std::vector<double*> C(nthreads);
    std::vector<double*> D(nthreads);

    const uint32_t N = 1024*1024;
#pragma omp parallel for schedule(static,1)
    for (int j=0; j<nthreads; j++)
    {
	A[j] = (double*)aligned_alloc(4096, sizeof(double) * N); for (uint32_t i=0; i<N; i++) A[j][i] = drand48() - 0.5;
	B[j] = (double*)aligned_alloc(4096, sizeof(double) * N); for (uint32_t i=0; i<N; i++) B[j][i] = drand48() - 0.5;
	C[j] = (double*)aligned_alloc(4096, sizeof(double) * N); for (uint32_t i=0; i<N; i++) C[j][i] = drand48() - 0.5;
	D[j] = (double*)aligned_alloc(4096, sizeof(double) * N); for (uint32_t i=0; i<N; i++) D[j][i] = drand48() - 0.5;
    }

    volatile double start, finish;

    start = now();
#pragma omp parallel for schedule(static,1)
    for (int j=0; j<nthreads; j++)
    {
	for(uint32_t reps = count; reps; reps -= COUNT)
	{
	    kernel(A[j],B[j],C[j],D[j]);
	}
    }
    finish = now();

    for (int j=0; j<nthreads; j++)
    {
	free(D[j]);
	free(C[j]);
	free(B[j]);
	free(A[j]);
    }

    return (finish - start);
}

void run_kernel_and_report
(
    void (kernel)(double*, double*, double*, double*),
    uint32_t count,
    const char* name,
    uint32_t M,
    uint32_t N,
    uint32_t K
)
{
    volatile double elapsed;
    volatile double flops = 2.0*count*M*N*K*omp_get_max_threads();
    elapsed = run_kernel(kernel, count);
    std::cout << std::setprecision(6);
    std::cout << "Time to run " << std::setw(51) << name << " " << count << " times = " << std::setw(10) << std::fixed << elapsed << " seconds (" << std::setw(10) << std::scientific << flops/elapsed << " flops)" << std::endl;
}

#define RUN_KERNEL(kernel, count, M, N, K) run_kernel_and_report(kernel, count, #kernel, M, N, K)

int main
(
    int		argc,
    char      **argv
)
{
    const uint32_t matmul_8x8x8_col_row_count   =  100000000U;
    const uint32_t matmul_8x8x16_col_row_count  =  100000000U;
    const uint32_t matmul_8x8x24_col_row_count  =  100000000U;
    const uint32_t matmul_8x8x32_col_row_count  =  100000000U;
    const uint32_t matmul_8x8x40_col_row_count  =  100000000U;
    const uint32_t matmul_8x8x48_col_row_count  =  100000000U;
    const uint32_t matmul_8x8x56_col_row_count  =  100000000U;
    const uint32_t matmul_8x8x64_col_row_count  =  100000000U;

    const uint32_t matmul_8x8x8_col_col_count   =  100000000U;
    const uint32_t matmul_8x8x64_col_col_count  =  100000000U;
    const uint32_t matmul_64x8x64_col_col_count =  100000000U;

    volatile double elapsed;
    
    int nthreads = omp_get_max_threads();
    std::cout << "=========================================================================================================================" << std::endl;
    std::cout << "Running on " << nthreads << ((nthreads > 1) ? " threads" : " thread") << std::endl;
    std::cout << "OMP_NUM_THREADS=" << getenv("OMP_NUM_THREADS") << std::endl;
    std::cout << "GOMP_CPU_AFFINITY=" << getenv("GOMP_CPU_AFFINITY") << std::endl;

    RUN_KERNEL(matmul_8x8x8_col_row                               , matmul_8x8x8_col_row_count  ,  8, 8,  8);
    RUN_KERNEL(matmul_8x8x8_col_col                               , matmul_8x8x8_col_col_count  ,  8, 8,  8);
    RUN_KERNEL(matmul_8x8x8_col_row_just_loads                    , matmul_8x8x8_col_row_count  ,  8, 8,  8);
    RUN_KERNEL(matmul_8x8x8_col_row_with_loads                    , matmul_8x8x8_col_row_count  ,  8, 8,  8);
    RUN_KERNEL(matmul_8x8x8_col_col_with_loads                    , matmul_8x8x8_col_col_count  ,  8, 8,  8);
    RUN_KERNEL(matmul_8x8x8_col_row_with_loads_and_stores         , matmul_8x8x8_col_row_count  ,  8, 8,  8);
    RUN_KERNEL(matmul_8x8x8_col_col_with_loads_and_stores         , matmul_8x8x8_col_col_count  ,  8, 8,  8);
    RUN_KERNEL(matmul_8x8x16_col_row_with_loads_and_stores        , matmul_8x8x16_col_row_count ,  8, 8, 16);
    RUN_KERNEL(matmul_8x8x24_col_row_with_loads_and_stores        , matmul_8x8x24_col_row_count ,  8, 8, 24);
    RUN_KERNEL(matmul_8x8x32_col_row_with_loads_and_stores        , matmul_8x8x32_col_row_count ,  8, 8, 32);
    RUN_KERNEL(matmul_8x8x40_col_row_with_loads_and_stores        , matmul_8x8x40_col_row_count ,  8, 8, 40);
    RUN_KERNEL(matmul_8x8x48_col_row_with_loads_and_stores        , matmul_8x8x48_col_row_count ,  8, 8, 48);
    RUN_KERNEL(matmul_8x8x56_col_row_with_loads_and_stores        , matmul_8x8x56_col_row_count ,  8, 8, 56);
    RUN_KERNEL(matmul_8x8x64_col_row_with_loads_and_stores        , matmul_8x8x64_col_row_count ,  8, 8, 64);
    RUN_KERNEL(matmul_8x8x64_col_col_with_loads_and_stores        , matmul_8x8x64_col_col_count ,  8, 8, 64);
    RUN_KERNEL(matmul_8x8x64_col_col_with_loads_and_stores_store_B, matmul_8x8x64_col_col_count ,  8, 8, 64);
    RUN_KERNEL(matmul_16x8x64_col_col_with_loads_and_stores       , matmul_64x8x64_col_col_count, 16, 8, 64);
    RUN_KERNEL(matmul_24x8x64_col_col_with_loads_and_stores       , matmul_64x8x64_col_col_count, 24, 8, 64);
    RUN_KERNEL(matmul_32x8x64_col_col_with_loads_and_stores       , matmul_64x8x64_col_col_count, 32, 8, 64);
    RUN_KERNEL(matmul_40x8x64_col_col_with_loads_and_stores       , matmul_64x8x64_col_col_count, 40, 8, 64);
    RUN_KERNEL(matmul_48x8x64_col_col_with_loads_and_stores       , matmul_64x8x64_col_col_count, 48, 8, 64);
    RUN_KERNEL(matmul_56x8x64_col_col_with_loads_and_stores       , matmul_64x8x64_col_col_count, 56, 8, 64);
    RUN_KERNEL(matmul_64x8x64_col_col_with_loads_and_stores       , matmul_64x8x64_col_col_count, 64, 8, 64);

    return 0;
}
