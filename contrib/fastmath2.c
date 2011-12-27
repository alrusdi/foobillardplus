/*
 * fastmath2.c
 * Test for new math-routines in another way
 * uses lookup tables
 *
 *  Created on: 24.12.2011
 *      Author: holger
 */

// gcc -o fastmath2 fastmath2.c -lm -ffast-math
// if this is better in speed as with -ffastmath , then use it

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>


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

}
