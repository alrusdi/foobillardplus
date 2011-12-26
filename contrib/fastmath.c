/*
 * fastmath.c
 * Test for new math-routines
 *
 *  Created on: 24.12.2011
 *      Author: holger
 */

// gcc -o fastmath fastmath.c ../src/vmath.c

#include "stdio.h"
#include "../src/vmath.h"

#include <assert.h>
#include <math.h>
#include <conio.h>
#include <time.h>

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
	   getch();
    return 0;
}
