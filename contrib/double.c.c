/*
 * swap.c
 *
 *  Created on: 17.07.2011
 *      Author: holger
 */

#include "stdio.h"
#include "math.h"

// swap using char pointers
unsigned long long  swap(double d)
{
    unsigned long long a;
    unsigned char *dst = (unsigned char *)&a;
    unsigned char *src = (unsigned char *)&d;

    dst[0] = src[7];
    dst[1] = src[6];
    dst[2] = src[5];
    dst[3] = src[4];
    dst[4] = src[3];
    dst[5] = src[2];
    dst[6] = src[1];
    dst[7] = src[0];

    return a;
}

// unswap using char pointers
double unswap(unsigned long long a)
{

    double d;
    unsigned char *src = (unsigned char *)&a;
    unsigned char *dst = (unsigned char *)&d;

    dst[0] = src[7];
    dst[1] = src[6];
    dst[2] = src[5];
    dst[3] = src[4];
    dst[4] = src[3];
    dst[5] = src[2];
    dst[6] = src[1];
    dst[7] = src[0];

    return d;
}

int main(int argc,char *argv[])
{
    double a;
    unsigned long long b;
    double c;
    for(a=0.0;a<100.0;a+=0.01) {
        // swap to network byte order
        b = swap(a);
        // swap back
        c = unswap(b);
        // now a and C should be EXACTLY the same. but if not, print something
        if (a == c) {
            printf("*****\n%f\n%f\n%f\n%f\n",
            a,b,c,fabs(a-c));
        }


    }
    return 0;
}
