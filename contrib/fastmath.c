/*
 * fastmath.c
 * Test for new math-routines
 *
 *  Created on: 24.12.2011
 *      Author: holger
 */

// gcc -o fastmath fastmath.c -lm

#include "stdio.h"
#include <assert.h>
#include <math.h>
#include <time.h>

const float M_PI2 = M_PI*2;

//functions for fastmath without table lookup

/***********************************************************************
 *                       fast sinus implementation                     *
 ***********************************************************************/

// schnelle Sinus Implementierung mit Inline Assembler (MASM-style)
//inline float _fastcall fastSinSSE(float x)
//{
//    static const float _0_16666 = -1/6.0f;
//    static const float _0_00833 = 1/120.0f;
//    static const float _0_00019 = -1/5040.0f;
//    static const float _0_0000027 = 1/362880.0f;

//    _asm
//    {
//        movss   xmm0, x
//        comiss  xmm0, PI    // if (x > PI)
//        jbe SHORT ln1

//        addss   xmm0, PI
//        jmp SHORT ln2

//ln1:
//        movss   xmm1, _PI
//        comiss  xmm1, xmm0  // if ( x < -PI)
//        jbe SHORT ln3

//        subss   xmm0, PI

//ln2:
//        divss   xmm0, PI2

//        cvttss2si ecx, xmm0 // = z ohne rundung!
//        cvtsi2ss xmm1, ecx  // z wieder in float umwandeln

//        movss   xmm0, x
//        mulss   xmm1, PI2
//        subss   xmm0, xmm1
//ln3:
//        // xmm0 beinhaltet immer das aktuelle ergebnis
//        // xmm1 beinhaltet zu jeder zeit x^2
//        // xmm2 beinhaltet zu jeder zeit x
//        // x + x*x*x*(_0_16666 + x*x*(_0_00833 + x*x*(_0_00019f + _0_0000027*x*x))));

//        movss   xmm2, xmm0          // x in xmm0 legen
//        movss   xmm1, xmm0      // x in xmm1 legen
//        mulss   xmm1, xmm1      // xmm1 = x*x
//        movss   xmm0, _0_0000027
//        mulss   xmm0, xmm1      // xmm0 = _0_0000027*x*x
//        addss   xmm0, _0_00019
//        mulss   xmm0, xmm1      // xmm0 = x*x*(_0_00019f + _0_0000027*x*x)
//        addss   xmm0, _0_00833
//        mulss   xmm0, xmm1      // xmm0 = x*x*(_0_00833 + x*x*(_0_00019f + _0_0000027*x*x))
//        addss   xmm0, _0_16666
//        mulss   xmm0, xmm1      // xmm0 = x*x*(_0_16666 + x*x*(_0_00833 + x*x*(_0_00019 + _0_0000027*x*x)))
//        mulss   xmm0, xmm2
//        addss   xmm0, xmm2
//        movss   x, xmm0         // das ergebnis in x rüberschieben
//    }

//    return x;
//}

inline float fastsin(float x) {
    if(M_PI < x) {
        x = x-(int)((x+M_PI)/(M_PI2))*M_PI2;
    }
    else if(x < -M_PI) {
        x = x-(int)((x-M_PI)/(M_PI2))*M_PI2;
    }
    return x*(1 - x*x*(0.16666667f - x*x*(0.00833333f - x*x*(0.0001984f - x*x*0.0000027f))));
}

/***********************************************************************
 *                     fast cosinus implementation                     *
 ***********************************************************************/

// schnelle Kosinus Implementierung mit Inline Assembler  (MASM-style)
//inline float _fastcall fastCosSSE(float x)
//{
//    static const float _0_5 = -1/2.0f;
//    static const float _0_0416 = 1/24.0f;
//    static const float _0_001387 = -1/720.0f;
//    static const float _0_0000248 = 1/40320.0f;
//    static const float _0_000000275 = -1/3629000.0f;
//    static const float _1 = 1.0f;

//    // Wenn x groeßer oder kleiner als PI,
//    // dann wird x auf das Intervall -PI bis PI zurückgerechnet
//    _asm
//    {
//        movss   xmm0, x
//        comiss  xmm0, PI    // if (x > PI)
//        jbe SHORT ln1

//        addss   xmm0, PI
//        jmp SHORT ln2

//ln1:
//        movss   xmm1, _PI
//        comiss  xmm1, xmm0  // if ( x < -PI)
//        jbe SHORT ln3

//        subss   xmm0, PI

//ln2:
//        divss   xmm0, PI2

//        cvttss2si ecx, xmm0 // = z ohne rundung!
//        cvtsi2ss xmm1, ecx  // z wieder in float umwandeln

//        movss   xmm0, x
//        mulss   xmm1, PI2
//        subss   xmm0, xmm1
//ln3:
//        // xmm0 beinhaltet immer das aktuelle ergebnis
//        // xmm1 beinhaltet zu jeder zeit x^2

//        //  1-x*x*(0.5f-x*x*(0.04166667f-x*x*(0.00138889f-x*x*(0.00002480f-x*x*0.000000275f))));

//        movss   xmm1, xmm0      // x in xmm1 legen
//        mulss   xmm1, xmm0      // xmm1 = x*x
//        movss   xmm0, _0_000000275
//        mulss   xmm0, xmm1      // xmm0 = _0_000000275*x*x
//        addss   xmm0, _0_0000248
//        mulss   xmm0, xmm1      // xmm0 = x*x*(_0_0000248 + _0_000000275*x*x)
//        addss   xmm0, _0_001387
//        mulss   xmm0, xmm1      // xmm0 = x*x*(_0_001387 + x*x*(_0_0000248 + _0_000000275*x*x))
//        addss   xmm0, _0_0416
//        mulss   xmm0, xmm1      // xmm0 = x*x*(_0_0416 + x*x*(_0_001387 + x*x*(_0_0000248 + _0_000000275*x*x)))
//        addss   xmm0, _0_5
//        mulss   xmm0, xmm1      // xmm0 = x*x*(_0_5+x*x*(_0_0416 + x*x*(_0_001387 + x*x*(_0_0000248 + _0_000000275*x*x))))
//        addss   xmm0, _1        // xmm0++
//        movss   x, xmm0         // das ergebnis in x rüberschieben
//    }

//    return x;
//}

inline float fastcos(float x) {
    if(M_PI < x) {
        x = x-(int)((x+M_PI)/(M_PI2))*M_PI2;
    }
    else if(x < -M_PI) {
        x = x-(int)((x-M_PI)/(M_PI2))*M_PI2;
    }
    return 1-x*x*(0.5f-x*x*(0.04166667f-x*x*(0.00138889f-x*x*(0.00002480f-x*x*0.000000275f))));
}


const long iMaxTests = 10000000;

int main(int argc,char *argv[])
{
	   long i;
	   float s, c;
	   float sc, scr = 0;
	   unsigned long dwTickStart, dwTickEnd, dwDuration;

	   double d;
	   dwTickStart = clock();
	   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
	   {
	      d = (double)i;
	      s = (float)sin(d);
	      c = (float)cos(d);

	      // This exist only to force optimiser to not delete code
	      sc = s * c;
	      if (sc > scr)
	      {
	         scr = sc;
	      }
	   }
	   dwTickEnd = clock();
	   dwDuration = dwTickEnd - dwTickStart;
	   printf("%d sin and cos computed in %d ticks with standard math funcs\n", iMaxTests, dwDuration);

	   float f;
	   dwTickStart = clock();
	   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
	   {
	      f = (float)i;
	      s = fastsin(f);
	      c = fastcos(f);

	      // This exist only to force optimiser to not delete code
	      sc = s * c;
	      if (sc > scr)
	      {
	         scr = sc;
	      }
	   }
	   dwTickEnd = clock();
	   dwDuration = dwTickEnd - dwTickStart;
	   printf("%d sin and cos computed in %d ticks with fast[cos/sin]\n", iMaxTests, dwDuration);
    return 0;
}
