/* vmath.c
**
**    some vector mathematics and structures (float or double)
**    Copyright (C) 2001  Florian Berger
**    Email:  harpin_floh@yahoo.de,  florian.berger@jk.uni-linz.ac.at
**
**    Updated Version foobillard++ started at 12/2010
**    Copyright (C) 2010/2011 Holger Schaekel (foobillardplus@go4more.de)
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License Version 2 as
**    published by the Free Software Foundation;
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program; if not, write to the Free Software
**    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
*/

// gcc sse, 3dnow and so on functions for later programing are declared here:
// http://gcc.gnu.org/onlinedocs/gcc-4.0.2/gcc/X86-Built_002din-Functions.html

#include <math.h>
#include <stdint.h>
#include "vmath.h"

const float M_PI2 = M_PI*2;

/***********************************************************************
 *              fast exp implementation (very imprecise)               *
 *               only to use for the lensflare function                *
 ***********************************************************************/

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


/***********************************************************************
 *            fast sqrt with table lookup (from Nvidia)                *
 ***********************************************************************/
//typedef unsigned char      BYTE;       // better: uint8_t out <stdint.h>
//typedef unsigned short     WORD;       // better: uint16_t out <stdint.h>
//typedef unsigned long      DWORD;      // better: uint32_t out <stdint.h>
//typedef unsigned long      QWORD;      // better: uint64_t out <stdint.h>

#ifdef DWORD
  #define FP_BITS(fp) (*(DWORD *)&(fp))
#else
  #define FP_BITS(fp) (*(uint32_t *)&(fp))
#endif
#define FP_ABS_BITS(fp) (FP_BITS(fp)&0x7FFFFFFF)
#define FP_SIGN_BIT(fp) (FP_BITS(fp)&0x80000000)
#define FP_ONE_BITS 0x3F800000

static unsigned int fast_sqrt_table[0x10000];  // declare table of square roots

typedef union FastSqrtUnion
{
  float f;
  unsigned int i;
} FastSqrtUnion;

/***********************************************************************
 *        The init table sqrt with table lookup (from Nvidia)          *
 ***********************************************************************/

void initlookup_sqrt_table(void) {
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

/***********************************************************************
 *        The function sqrt with table lookup (from Nvidia)            *
 ***********************************************************************/

inline float fastsqrt(float n) {

  if (FP_BITS(n) == 0)
    return 0.0;                 // check for square root of 0

  FP_BITS(n) = fast_sqrt_table[(FP_BITS(n) >> 8) & 0xFFFF] | ((((FP_BITS(n) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);

  return n;
}


/***********************************************************************
 *            fast sinus/cosinus lookup build table                    *
 ***********************************************************************/

#define MAX_CIRCLE_ANGLE 8192
#define HALF_MAX_CIRCLE_ANGLE (MAX_CIRCLE_ANGLE/2)
#define QUARTER_MAX_CIRCLE_ANGLE (MAX_CIRCLE_ANGLE/4)
#define MASK_MAX_CIRCLE_ANGLE (MAX_CIRCLE_ANGLE - 1)

static float fast_cossin_table[MAX_CIRCLE_ANGLE];           // Declare table of fast cosinus and sinus

void initlookup_cossin_table(void) {
  long i;
  // Build cossin table
  for (i = 0; i < MAX_CIRCLE_ANGLE ; i++) {
     fast_cossin_table[i] = (float)sin((double)i * M_PI / HALF_MAX_CIRCLE_ANGLE);
  }
}

/***********************************************************************
 *            fast pow implementation with double                      *
 ***********************************************************************/

// ### TODO ### work not really clean at the moment

inline double fastpow(double a, double b) {
    int tmp = (*(1 + (int *)&a));
    int tmp2 = (int)(b * (tmp - 1072632447) + 1072632447);
    double p = 0.0;
    //*(1 + (int * )&p) = tmp2;
    //return p;
    union { double d; int x[2]; } u = { p }; u.x[1] = tmp2;
    return u.d;
}


/***********************************************************************
 *            fast sinus implementation lookup table                   *
 ***********************************************************************/
inline float fastsin(float n) {
   float f = n * HALF_MAX_CIRCLE_ANGLE / M_PI;
   int i;
   i = (int)f;
   if (i < 0) {
      return fast_cossin_table[(-((-i)&MASK_MAX_CIRCLE_ANGLE)) + MAX_CIRCLE_ANGLE];
   } else {
      return fast_cossin_table[i&MASK_MAX_CIRCLE_ANGLE];
   }
}

/***********************************************************************
 *            fast cosinus implementation lookup table                 *
 ***********************************************************************/

inline float fastcos(float n) {
   float f = n * HALF_MAX_CIRCLE_ANGLE / M_PI;
   int i;
   i = (int)f;
   if (i < 0) {
      return fast_cossin_table[((-i) + QUARTER_MAX_CIRCLE_ANGLE)&MASK_MAX_CIRCLE_ANGLE];
   } else {
      return fast_cossin_table[(i + QUARTER_MAX_CIRCLE_ANGLE)&MASK_MAX_CIRCLE_ANGLE];
   }
}

/***********************************************************************
 *                     fast atan implementation                       *
 ***********************************************************************/

inline float fastatan(float x)
{
    return M_PI_4*x - x*(fabs(x) - 1)*(0.2447 + 0.0663*fabs(x));
}


/***********************************************************************
 *                     fast atan2 implementation                       *
 ***********************************************************************/

inline float fastatan2(float y, float x) {
	  float coeff_1 = M_PI / 4.0f;
	  float coeff_2 = 3.0f * coeff_1;
	  float abs_y = fabs(y);
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

/***********************************************************************/

struct Vect vec_cross( struct Vect v1, struct Vect v2 )
{
   static struct Vect vr;
   vr.x = v1.y * v2.z - v2.y * v1.z;
   vr.y = v1.z * v2.x - v2.z * v1.x;
   vr.z = v1.x * v2.y - v2.x * v1.y;
   return (vr);
}

/***********************************************************************/

VMfloat vec_mul( struct Vect v1, struct Vect v2 )
{
#ifndef USE_SSE
   return( v1.x*v2.x + v1.y*v2.y + v1.z*v2.z );
#else
	  static struct Vect vr;
	  vr.v = _mm_mul_ps(v1.v, v2.v);
	  return (vr.x+vr.y+vr.z);
#endif

}

/***********************************************************************/

struct Vect vec_diff( struct Vect v1, struct Vect v2 )
{
   static struct Vect vr;
#ifndef USE_SSE
   vr.x = v1.x - v2.x;
   vr.y = v1.y - v2.y;
   vr.z = v1.z - v2.z;
#else
   vr.v = _mm_sub_ps(v1.v, v2.v);
#endif
   return (vr);
}

/***********************************************************************/

struct Vect vec_add(struct Vect v1, struct Vect v2 )
{
    static struct Vect vr;

#ifndef USE_SSE
   vr.x = v1.x + v2.x;
   vr.y = v1.y + v2.y;
   vr.z = v1.z + v2.z;
#else
    vr.v = _mm_add_ps(v1.v, v2.v);
#endif
	return (vr);
}

/***********************************************************************/

struct Vect vec_scale( struct Vect v1, VMfloat scale )
{
   static struct Vect vr;

#ifndef USE_SSE
   vr.x = v1.x * scale;
   vr.y = v1.y * scale;
   vr.z = v1.z * scale;
#else
   static struct Vect vr1;
   vr1.v = _mm_set1_ps (scale);
   vr.v = _mm_mul_ps(v1.v, vr1.v);
#endif
   return vr;
}

/***********************************************************************/

struct Vect vec_rotate( struct Vect v1, struct Vect ang )
{
    static struct Vect vr;
    vr=v1;
    rot_ax( vec_unit(ang), &vr, 1, vec_abs(ang) );
    return(vr);
}

/***********************************************************************/

VMfloat vec_abs( struct Vect v )
{

#ifndef USE_SSE
    return( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ));
#else
    static struct Vect vr;
    vr.v = _mm_mul_ps(v.v, v.v);
    return( sqrt( vr.x + vr.y + vr.z ));
#endif
}

/***********************************************************************/

VMfloat vec_abssq( struct Vect v )
{
#ifndef USE_SSE
    return( v.x*v.x + v.y*v.y + v.z*v.z );
#else
  	 static struct Vect vr;
	   vr.v = _mm_mul_ps(v.v, v.v);
    return(vr.x + vr.y + vr.z);
#endif
}

/***********************************************************************/

struct Vect vec_unit( struct Vect v )
{
    static struct Vect vr;
    VMfloat l;
    l=vec_abs(v);
    if(fabs(l)>1.0E-50){
#ifndef USE_SSE
       vr.x=v.x/l;
       vr.y=v.y/l;
       vr.z=v.z/l;
    } else {
       vr.x=0.0;
       vr.y=0.0;
       vr.z=0.0;
    }
#else
     static struct Vect vr1;
   	   vr1.v = _mm_set1_ps(l);
	      vr.v = _mm_div_ps(v.v,vr1.v);
    } else {
       vr.v = _mm_setzero_ps ();
    }
#endif
    return vr;
}

/***********************************************************************/

struct Vect vec_xyz( VMfloat x, VMfloat y, VMfloat z )
{
   static struct Vect vr;
#ifndef USE_SSE
   vr.x=x; vr.y=y; vr.z=z;
#else
   vr.v = _mm_setr_ps (1.0f,x,y,z); // first w to 1.0f, so no null division error can occur
#endif
   return vr;
}

/***********************************************************************/

struct Vect vec_ez(void)
{
#ifndef USE_SSE
   return(vec_xyz(0.0,0.0,1.0));
#else
   static struct Vect vr;
   vr.v = _mm_setr_ps(1.0f,0.0f,0.0f,1.0f);
   return(vr);
#endif
}

/***********************************************************************/

struct Vect vec_null(void)
{
#ifndef USE_SSE
   return(vec_xyz(0.0,0.0,0.0));
#else
   static struct Vect vr;
   vr.v = _mm_setzero_ps ();
   return(vr);
#endif

}

/***********************************************************************/

void rot_ax( struct Vect ax, struct Vect *v, int nr, VMfloat phi )
{
  static struct Vect bx,by,bz; // base
  static struct Vect dp,dp2, nax;
  VMfloat sinphi,cosphi;
  int i;

  if ( phi !=0.0 && vec_abs(ax)>0.0 ){

      bz = vec_unit( ax );
      if( fabs(bz.x) <= fabs(bz.y) && fabs(bz.x) <= fabs(bz.z) ) nax=vec_xyz(1.0,0.0,0.0);
      if( fabs(bz.y) <= fabs(bz.z) && fabs(bz.y) <= fabs(bz.x) ) nax=vec_xyz(0.0,1.0,0.0);
      if( fabs(bz.z) <= fabs(bz.x) && fabs(bz.z) <= fabs(bz.y) ) nax=vec_xyz(0.0,0.0,1.0);
      bx = vec_unit( vec_diff( nax, vec_scale(bz,vec_mul(nax,bz)) ) );
      by = vec_cross(bz,bx);

      sinphi=sin(phi);
      cosphi=cos(phi);

      for( i=0; i<nr; i++ ){
         // transform into axis-system
         dp.x = vec_mul( v[i], bx );
         dp.y = vec_mul( v[i], by );
         dp.z = vec_mul( v[i], bz );
   
         dp2.x = dp.x*cosphi - dp.y*sinphi;
         dp2.y = dp.y*cosphi + dp.x*sinphi;
         dp2.z = dp.z;
      
         // retransform back
         v[i].x = dp2.x * bx.x + dp2.y * by.x + dp2.z * bz.x;
         v[i].y = dp2.x * bx.y + dp2.y * by.y + dp2.z * bz.y;
         v[i].z = dp2.x * bx.z + dp2.y * by.z + dp2.z * bz.z;
      }
 
   }

}

/***********************************************************************/

VMfloat vec_angle( struct Vect v1, struct Vect v2 )
/* returns positive angle between 0 and M_PI */
{
    return( acos(vec_mul( vec_unit(v1), vec_unit(v2) )) );
}

/***********************************************************************/

struct Vect vec_proj( struct Vect v1, struct Vect v2 )
{
    static VMfloat v2ls;
#ifndef USE_SSE
    v2ls = v2.x*v2.x + v2.y*v2.y + v2.z*v2.z;
#else
    static struct Vect vr;
    vr.v = _mm_mul_ps(v2.v, v2.v);
   	v2ls = vr.x + vr.y + vr.z;
#endif
    if( v2ls > 0.0 ){
        return( vec_scale( v2, vec_mul(v1,v2)/v2ls ) );
    } else {
        return( v1 );
    }
}

/***********************************************************************/

struct Vect vec_ncomp( struct Vect v1, struct Vect v2 )
{
    return( vec_diff(v1,vec_proj(v1,v2)) );
}

/***********************************************************************/

VMfloat vec_ndist ( struct Vect v, struct Vect v1, struct Vect v2 )
/* normal distance of v to line(v1,v2) */
{
    return( vec_abs(vec_ncomp(vec_diff(v,v1),vec_diff(v2,v1))) );
}

/***********************************************************************/

struct Vect vec_surf_proj( struct Vect center, struct Vect point, struct Vect n, struct Vect npos )
{
    VMfloat ndist_c, ndist_p;
    npos = vec_proj(npos,n);
    ndist_c=vec_mul(center,n)-vec_mul(npos,n);
    ndist_p=vec_mul(point,n)-vec_mul(npos,n);
    center = vec_ncomp(center,n);
    point = vec_ncomp(point,n);
    return vec_add( npos, vec_add( center, vec_scale(vec_diff(point,center),ndist_c/(ndist_c-ndist_p))));
}

/***********************************************************************/

struct Vect matr4_rdot( struct Matrix4 m, struct Vect v )
{
    static struct Vect rvec;
    rvec.x=v.x*m.m[0]+v.y*m.m[4]+v.z*m.m[ 8]+m.m[12];
    rvec.y=v.x*m.m[1]+v.y*m.m[5]+v.z*m.m[ 9]+m.m[13];
    rvec.z=v.x*m.m[2]+v.y*m.m[6]+v.z*m.m[10]+m.m[14];
    return rvec;
}

/***********************************************************************/

struct Vect tri_center( struct Vect v1, struct Vect v2, struct Vect v3 )
{

#ifndef USE_SSE
    return(vec_xyz((v1.x+v2.x+v3.x)/3.0,
                   (v1.y+v2.y+v3.y)/3.0,
                   (v1.z+v2.z+v3.z)/3.0));
#else
	   static struct Vect vr,vr1,vr2;
    vr.v = _mm_add_ps(v1.v, v2.v);
    vr1.v = _mm_add_ps(vr.v, v3.v);
    vr2.v = _mm_set_ps1(3.0f);
    vr.v = _mm_div_ps(vr1.v,vr2.v);
    return vr;
#endif

}

/***********************************************************************/

VMfloat tri_area_xy( struct Vect v1, struct Vect v2, struct Vect v3 )
/* area of triangle ( +/- for ccw/cw ) */
{
    return( ( (v1.x-v2.x)*(v1.y+v2.y) +
              (v2.x-v3.x)*(v2.y+v3.y) +
              (v3.x-v1.x)*(v3.y+v1.y) ) / 2.0 );
}

/***********************************************************************/

VMfloat tri_vol_xy( struct Vect v1, struct Vect v2, struct Vect v3 )
/* calc the volume of the space under (-z) the triangle (v1,v2,v3) */
{
    return( tri_center(v1,v2,v3).z * tri_area_xy(v1,v2,v3) );
}
