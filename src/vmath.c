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
#include "vmath.h"
#if defined( __SSE__ )
 #include <xmmintrin.h> // the header that defines SSE intrinsics
#endif

/***********************************************************************/

void shape_trans( struct Shape *s, struct Vect t )
{
   int i;
   for(i=0;i<s->pnr;i++){
      s->p[i].x+=t.x;
      s->p[i].y+=t.y;
      s->p[i].z+=t.z;
   }
}

/***********************************************************************/

struct Vect vec_cross( struct Vect v1, struct Vect v2 )
{
   struct Vect vr;
   vr.x = v1.y * v2.z - v2.y * v1.z;
   vr.y = v1.z * v2.x - v2.z * v1.x;
   vr.z = v1.x * v2.y - v2.x * v1.y;
   return vr;
}

/***********************************************************************/

VMfloat vec_mul( struct Vect v1, struct Vect v2 )
{
 return( v1.x*v2.x + v1.y*v2.y + v1.z*v2.z );
}

/***********************************************************************/

struct Vect vec_diff( struct Vect v1, struct Vect v2 )
{
   struct Vect vr;
   vr.x = v1.x - v2.x;
   vr.y = v1.y - v2.y;
   vr.z = v1.z - v2.z;
   return vr;
}

/***********************************************************************/

struct Vect vec_add( struct Vect v1, struct Vect v2 )
{
   struct Vect vr;
   vr.x = v1.x + v2.x;
   vr.y = v1.y + v2.y;
   vr.z = v1.z + v2.z;
   return vr;
}

/***********************************************************************/

struct Vect vec_scale( struct Vect v1, VMfloat scale )
{
   struct Vect vr;
   vr.x = v1.x * scale;
   vr.y = v1.y * scale;
   vr.z = v1.z * scale;
   return vr;
}

/***********************************************************************/

struct Vect vec_rotate( struct Vect v1, struct Vect ang )
{
    struct Vect vr;
    vr=v1;
    rot_ax( vec_unit(ang), &vr, 1, vec_abs(ang) );
    return(vr);
}

/***********************************************************************/

VMfloat vec_abs( struct Vect v )
{
 return( sqrt( v.x*v.x + v.y*v.y + v.z*v.z ));
}

/***********************************************************************/

VMfloat vec_abssq( struct Vect v )
{
 return( v.x*v.x + v.y*v.y + v.z*v.z );
}

/***********************************************************************/

struct Vect vec_unit( struct Vect v )
{
 struct Vect vr;
 VMfloat l;
   l=vec_abs(v);
   if(fabs(l)>1.0E-50){
       vr.x=v.x/l;
       vr.y=v.y/l;
       vr.z=v.z/l;
   } else {
       vr.x=0.0;
       vr.y=0.0;
       vr.z=0.0;
   }
   return vr;
}

/***********************************************************************/

int vec_nearly_equal( struct Vect v1, struct Vect v2, VMfloat tolerance )
{
    return ( fabs(v1.x-v2.x)<tolerance &&
             fabs(v1.y-v2.y)<tolerance &&
             fabs(v1.z-v2.z)<tolerance );
}

/***********************************************************************/

struct Vect vec_xyz( VMfloat x, VMfloat y, VMfloat z )
{
   struct Vect vr;
   vr.x=x; vr.y=y; vr.z=z;
   return vr;
}

/***********************************************************************/

struct Vect  vec_ez(void)
{
 return(vec_xyz(0.0,0.0,1.0));
}

/***********************************************************************/

struct Vect  vec_null(void)
{
 return(vec_xyz(0.0,0.0,0.0));
 }

/***********************************************************************/

void rot_ax( struct Vect ax, struct Vect *v, int nr, VMfloat phi )
{
  struct Vect bx,by,bz; // base
  struct Vect dp,dp2, nax;
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
//         v[i] = vec_diff( v[i], m );
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

    v2ls = v2.x*v2.x + v2.y*v2.y + v2.z*v2.z;
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
    struct Vect rvec;
    rvec.x=v.x*m.m[0]+v.y*m.m[4]+v.z*m.m[ 8]+m.m[12];
    rvec.y=v.x*m.m[1]+v.y*m.m[5]+v.z*m.m[ 9]+m.m[13];
    rvec.z=v.x*m.m[2]+v.y*m.m[6]+v.z*m.m[10]+m.m[14];
    return rvec;
}

/***********************************************************************/

struct Vect tri_center( struct Vect v1, struct Vect v2, struct Vect v3 )
{
    return( vec_xyz( (v1.x+v2.x+v3.x)/3.0,
                     (v1.y+v2.y+v3.y)/3.0,
                     (v1.z+v2.z+v3.z)/3.0 ) );
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
