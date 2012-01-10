/*
 * fastmath2.c
 * Test for new math-routines in another way
 * uses lookup tables
 *
 *  Created on: 24.12.2011
 *      Author: holger
 */

// gcc -o fastmath2 fastmath2.c -lm -ffast-math
// if this is better in speed as with -ffast-math , then use it

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

/// fast exp implementation

static union{
    double d;
    struct{
        int j,i;
        } n;
} d2i;

#ifndef M_LN2
#define M_LN2 0.69314718055994530942
#endif

#define EXP_A (1048576/M_LN2)
#define EXP_C 60801
//#define fastexp(y) (d2i.n.i = EXP_A*(y)+(1072693248-EXP_C),d2i.d)

inline double fastexp(const double y){
	return (d2i.n.i = EXP_A*(y)+(1072693248-EXP_C),d2i.d);
}

// fast fabs routine

//inline float fastfabs(const float f) {
//  int i=((*(int*)&f)&0x7fffffff);
//  return (*(float*)&i);
//}

inline float fastfabs(const float n){
    if(n >= 0.0f)return n; //if positive, return without any change
    else return 0.0f - n; //if negative, return a positive version
    }

inline float fastneg(const float f) {
 int i=((*(int*)&f)^0x80000000);
 return (*(float*)&i);
}

inline int fastsgn(const float f) {
 return 1+(((*(int*)&f)>>31)<<1);
}


// fast sqrt with table lookup (from Nvidia)

//typedef unsigned char      BYTE;       // better: uint8_t out <stdint.h>
//typedef unsigned short     WORD;       // better: uint16_t out <stdint.h>
//typedef unsigned long      DWORD;      // better: uint32_t out <stdint.h>
//typedef unsigned long      QWORD;      // better: uint64_t out <stdint.h>

//#define FP_BITS(fp) (*(DWORD *)&(fp))
#define FP_BITS(fp) (*(uint32_t *)&(fp))
#define FP_ABS_BITS(fp) (FP_BITS(fp)&0x7FFFFFFF)
#define FP_SIGN_BIT(fp) (FP_BITS(fp)&0x80000000)
#define FP_ONE_BITS 0x3F800000

static unsigned int fast_sqrt_table[0x10000];  // declare table of square roots

typedef union FastSqrtUnion
{
  float f;
  unsigned int i;
} FastSqrtUnion;

void  build_sqrt_table()
{
  unsigned int i;
  FastSqrtUnion s;

  for (i = 0; i <= 0x7FFF; i++)
  {

    // Build a float with the bit pattern i as mantissa
    //  and an exponent of 0, stored as 127

    s.i = (i << 8) | (0x7F << 23);
    s.f = (float)sqrt(s.f);

    // Take the square root then strip the first 7 bits of
    //  the mantissa into the table

    fast_sqrt_table[i + 0x8000] = (s.i & 0x7FFFFF);

    // Repeat the process, this time with an exponent of 1,
    //  stored as 128

    s.i = (i << 8) | (0x80 << 23);
    s.f = (float)sqrt(s.f);

    fast_sqrt_table[i] = (s.i & 0x7FFFFF);
  }
}


inline float fastsqrt(float n)
{

  if (FP_BITS(n) == 0)
    return 0.0;                 // check for square root of 0

  FP_BITS(n) = fast_sqrt_table[(FP_BITS(n) >> 8) & 0xFFFF] | ((((FP_BITS(n) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);

  return n;
}



//fast pow only with double (float won't work)

double fastpow(double a, double b) {
    int tmp = (*(1 + (int *)&a));
    int tmp2 = (int)(b * (tmp - 1072632447) + 1072632447);
    double p = 0.0;
    //*(1 + (int * )&p) = tmp2;
    //return p;
    union { double d; int x[2]; } u = { p }; u.x[1] = tmp2;
    return u.d;
}

// fast atan only with float

float fastatan(float x)
{
    return M_PI_4*x - x*(fabs(x) - 1)*(0.2447 + 0.0663*fabs(x));
}


// fast atan2 only with float

float fastatan2(float y, float x) {
	float coeff_1 = M_PI / 4.0f;
	float coeff_2 = 3.0f * coeff_1;
	float abs_y = abs(y);
	float angle;
	if (x > 0.0f) {
		float r = (x - abs_y) / (x + abs_y);
		angle = coeff_1 - coeff_1 * r;
	} else {
		float r = (x + abs_y) / (abs_y - x);
		angle = coeff_2 - coeff_1 * r;
	}
	return y < 0.0f ? -angle : angle;
}


#define MAX_CIRCLE_ANGLE      512
#define HALF_MAX_CIRCLE_ANGLE (MAX_CIRCLE_ANGLE/2)
#define QUARTER_MAX_CIRCLE_ANGLE (MAX_CIRCLE_ANGLE/4)
#define MASK_MAX_CIRCLE_ANGLE (MAX_CIRCLE_ANGLE - 1)

float fast_cossin_table[MAX_CIRCLE_ANGLE];           // Declare table of fast cosinus and sinus

inline float fastcos(float n) {
   float f = n * HALF_MAX_CIRCLE_ANGLE / M_PI;
   int i;
   i = (int)f;
   if (i < 0) {
      return fast_cossin_table[((-i) + QUARTER_MAX_CIRCLE_ANGLE)&MASK_MAX_CIRCLE_ANGLE];
   } else {
      return fast_cossin_table[(i + QUARTER_MAX_CIRCLE_ANGLE)&MASK_MAX_CIRCLE_ANGLE];
   }
   assert(0);
}

inline float fastsin(float n) {
   float f = n * HALF_MAX_CIRCLE_ANGLE / M_PI;
   int i;
   i = (int)f;
   if (i < 0) {
      return fast_cossin_table[(-((-i)&MASK_MAX_CIRCLE_ANGLE)) + MAX_CIRCLE_ANGLE];
   } else {
      return fast_cossin_table[i&MASK_MAX_CIRCLE_ANGLE];
   }
   assert(0);
}

const long iMaxTests = 10000000;

int main(int argc,char *argv[])
{
   long i;
   float s, c;
   //float e[iMaxTests];
   float sc, scr = 0;
   unsigned long dwTickStart, dwTickEnd, dwDuration;

   // Build cossin table
   for (i = 0 ; i < MAX_CIRCLE_ANGLE ; i++)
   {
      fast_cossin_table[i] = (float)sin((double)i * M_PI / HALF_MAX_CIRCLE_ANGLE);
   }

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
   printf("%d sin and cos computed in %d ticks with fast[cos/sin] lookup-table\n", iMaxTests, dwDuration);

   float h;
   dwTickStart = clock();
   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
   {
      f = (float)i;
      h = f;
      s = atan2(f,h);

      // This exist only to force optimiser to not delete code
      sc = s * c;
      if (sc > scr)
      {
         scr = sc;
      }
   }
   dwTickEnd = clock();
   dwDuration = dwTickEnd - dwTickStart;
   printf("%d atan2 computed in %d ticks with normal[atan2]\n", iMaxTests, dwDuration);

   dwTickStart = clock();
   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
   {
      f = (float)i;
      h = f;
      s = fastatan2(f,h);

      // This exist only to force optimiser to not delete code
      sc = s * c;
      if (sc > scr)
      {
         scr = sc;
      }
   }
   dwTickEnd = clock();
   dwDuration = dwTickEnd - dwTickStart;
   printf("%d atan2 computed in %d ticks with fast[atan2]\n", iMaxTests, dwDuration);

   dwTickStart = clock();
   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
   {
      f = (float)i;
      s = fastatan(f);

      // This exist only to force optimiser to not delete code
      sc = s * c;
      if (sc > scr)
      {
         scr = sc;
      }
   }
   dwTickEnd = clock();
   dwDuration = dwTickEnd - dwTickStart;
   printf("%d atan computed in %d ticks with fast[atan]\n", iMaxTests, dwDuration);
   dwTickStart = clock();
   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
   {
      f = (float)i;
      s = atan(f);

      // This exist only to force optimiser to not delete code
      sc = s * c;
      if (sc > scr)
      {
         scr = sc;
      }
   }
   dwTickEnd = clock();
   dwDuration = dwTickEnd - dwTickStart;
   printf("%d atan computed in %d ticks with normal[atan]\n", iMaxTests, dwDuration);

   dwTickStart = clock();
   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
   {
      f = (float)i;
      s = pow(f, f);

      // This exist only to force optimiser to not delete code
      sc = s * c;
      if (sc > scr)
      {
         scr = sc;
      }
   }
   dwTickEnd = clock();
   dwDuration = dwTickEnd - dwTickStart;
   printf("%d pow computed in %d ticks with normal[pow]\n", iMaxTests, dwDuration);
   dwTickStart = clock();
   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
   {
      f = (float)i;
      s = fastpow(f, f);

      // This exist only to force optimiser to not delete code
      sc = s * c;
      if (sc > scr)
      {
         scr = sc;
      }
   }
   dwTickEnd = clock();
   dwDuration = dwTickEnd - dwTickStart;
   printf("%d pow computed in %d ticks with fast[pow]\n", iMaxTests, dwDuration);

   build_sqrt_table();
   dwTickStart = clock();
   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
   {
      f = (float)i;
      s = sqrt(f);
      //e[i] = s;

      // This exist only to force optimiser to not delete code
      sc = s * c;
      if (sc > scr)
      {
         scr = sc;
      }
   }
   dwTickEnd = clock();
   dwDuration = dwTickEnd - dwTickStart;
   printf("%d sqrt computed in %d ticks with normal[sqrt]\n", iMaxTests, dwDuration);
   dwTickStart = clock();
   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
   {
      f = (float)i;
      s = fastsqrt(f);
      //fprintf(stderr,"%f %f\n",s,e[i]);
      // This exist only to force optimiser to not delete code
      sc = s * c;
      if (sc > scr)
      {
         scr = sc;
      }
   }
   dwTickEnd = clock();
   dwDuration = dwTickEnd - dwTickStart;
   printf("%d sqrt computed in %d ticks with fast[sqrt]\n", iMaxTests, dwDuration);

   dwTickStart = clock();
   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
   {
      f = (float)i;
      s = fabs(f);
      // This exist only to force optimiser to not delete code
      sc = s * c;
      if (sc > scr)
      {
         scr = sc;
      }
   }
   dwTickEnd = clock();
   dwDuration = dwTickEnd - dwTickStart;
   printf("%d fabs computed in %d ticks with normal[fabs]\n", iMaxTests, dwDuration);
   dwTickStart = clock();
   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
   {
      f = (float)i;
      s = fastfabs(f);
      // This exist only to force optimiser to not delete code
      sc = s * c;
      if (sc > scr)
      {
         scr = sc;
      }
   }
   dwTickEnd = clock();
   dwDuration = dwTickEnd - dwTickStart;
   printf("%d fabs computed in %d ticks with fast[fabs]\n", iMaxTests, dwDuration);
   printf("Too slow, not implemented.... think a little bit about a new fabs......\n");

   dwTickStart = clock();
   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
   {
      f = (float)i;
      s = exp(f);
      //e[i] = s;
      // This exist only to force optimiser to not delete code
      sc = s * c;
      if (sc > scr)
      {
         scr = sc;
      }
   }
   dwTickEnd = clock();
   dwDuration = dwTickEnd - dwTickStart;
   printf("%d exp computed in %d ticks with normal[exp]\n", iMaxTests, dwDuration);
   dwTickStart = clock();
   for (i = - (iMaxTests/2) ; i < iMaxTests/2 ; i++)
   {
      f = (float)i;
      s = fastexp(f);
      //fprintf(stderr,"%f %f\n",s,e[i]);
      // This exist only to force optimiser to not delete code
      sc = s * c;
      if (sc > scr)
      {
         scr = sc;
      }
   }
   dwTickEnd = clock();
   dwDuration = dwTickEnd - dwTickStart;
   printf("%d exp computed in %d ticks with fast[exp]\n", iMaxTests, dwDuration);

}
