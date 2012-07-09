/* queue.c
**
**    create the billard-queue display lists
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

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef __APPLE__
 #include <OpenGL/gl.h>
 #include <OpenGL/glu.h>
 #include <OpenGL/glext.h>
#else
 #include <GL/gl.h>
 #include <GL/glu.h>
 #include <GL/glext.h>
#endif
#include "billard.h"
#include "queue.h"
#include "png_loader.h"
#include "options.h"

/***********************************************************************/

static GLfloat col_shad [4] = {0.1, 0.1, 0.1, 1.0};
static VMfloat cue_shad_w2 = 0.014;
static unsigned int queuetexbind;

/***********************************************************************/

VMfloat rfunc(VMfloat x)
{
    return(cos(x*M_PI));
}

/***********************************************************************/

int create_queue(VMfloat (*rfunc)(VMfloat))
{
   int queue_obj, i,j,l,m,n;
// on touch-devices not so high cue
#ifdef TOUCH
   #define SEGS 16
#else
   #define SEGS 48
#endif
   /* number of segments along the cue, originally foobillard = 5 */
#define SEGNO 1
   int sidenr=SEGS;  // number of sides for triangles around the cue
   GLfloat dl1=0.004;
   GLfloat dl2=0.001;
   VMfloat ph;
   VMvect v1,v2,n1,n2;
   int segnr=SEGNO;
   VMfloat r1,r2,dr1,dr2;
   GLfloat VertexData[SEGS*3*2+6];
   GLfloat TexData[SEGS*2*2+6];
   GLfloat NormalData[SEGS*3*2+6];
   queue_obj = glGenLists(1);
   glNewList(queue_obj, GL_COMPILE);
   glPushMatrix();
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glTexCoordPointer(2,GL_FLOAT, 0, TexData);
   glVertexPointer(3, GL_FLOAT, 0, VertexData);
   glNormalPointer(GL_FLOAT, 0, NormalData);
   for(j=0;j<segnr;j++){
       l = 0;
       m = 0;
       n = 0;

       r1=0.5*(QUEUE_D1-(QUEUE_D1-QUEUE_D2)*(rfunc((VMfloat)j/segnr)-rfunc(0))/(rfunc(1)-rfunc(0)));
       r2=0.5*(QUEUE_D1-(QUEUE_D1-QUEUE_D2)*(rfunc((VMfloat)(j+1)/segnr)-rfunc(0))/(rfunc(1)-rfunc(0)));

       dr1=0.5*(QUEUE_D1-(QUEUE_D1-QUEUE_D2)*(rfunc((VMfloat)j/segnr+0.001)-rfunc(0))/(rfunc(1)-rfunc(0)));
       dr1=(dr1-r1)/0.001;
       dr2=0.5*(QUEUE_D1-(QUEUE_D1-QUEUE_D2)*(rfunc((VMfloat)(j+1)/segnr+0.001)-rfunc(0))/(rfunc(1)-rfunc(0)));
       dr2=(dr2-r2)/0.001;
       for(i=0;i<sidenr+1;i++){
           ph=2.0*M_PI/(VMfloat)sidenr*i;
           v1.x = r1*cos(ph);
           v1.y = r1*sin(ph);
           v1.z = (QUEUE_L-dl1)-(QUEUE_L-dl1-dl2)/segnr*j;
           v2.x = r2*cos(ph);
           v2.y = r2*sin(ph);
           v2.z = (QUEUE_L-dl1)-(QUEUE_L-dl1-dl2)/segnr*(j+1);
           n1.x = cos(ph);
           n1.y = sin(ph);
           n1.z = -dr1;
           n1=vec_unit(n1);
           n2.x = cos(ph);
           n2.y = sin(ph);
           n2.z = -dr2;
           n2=vec_unit(n2);
           NormalData[l++] = n2.x; NormalData[l++] = n2.y; NormalData[l++] = n2.z;
           TexData[m++] = 1.0-v2.z/QUEUE_L; TexData[m++] = (v2.x+QUEUE_D1/2.0)/QUEUE_D1;
           VertexData[n++] = v2.x; VertexData[n++] = v2.y; VertexData[n++] = v2.z;
           NormalData[l++] = n1.x; NormalData[l++] = n1.y; NormalData[l++] = n1.z;
           TexData[m++] = 1.0-v1.z/QUEUE_L; TexData[m++] = (v1.x+QUEUE_D1/2.0)/QUEUE_D1;
           VertexData[n++] = v1.x; VertexData[n++] = v1.y; VertexData[n++] = v1.z;
       }
       glDrawArrays(GL_TRIANGLE_STRIP,0,SEGS*2+2);
   }
   n = 0;
   m = 0;
   l = 0;
   glPopMatrix();
   glPushMatrix();
   VertexData[n++] = 0.0; VertexData[n++] = 0.0; VertexData[n++] = 0.0;
   for(i=0;i<sidenr+2;i++){
       ph=2.0*M_PI/(VMfloat)sidenr*i;
       v2.x = QUEUE_D2/2.0*cos(ph);
       v2.y = QUEUE_D2/2.0*sin(ph);
       v2.z = dl2;
       VertexData[n++] = v2.x; VertexData[n++] = v2.y; VertexData[n++] = v2.z;
       TexData[m++] = 1.0-v2.z/QUEUE_L; TexData[m++] = (v2.x+QUEUE_D1/2.0)/QUEUE_D1;
       NormalData[l++] = 0.0; NormalData[l++] = 0.0; NormalData[l++] = -1.0;
   }
   // draw the top of the cue
   glDrawArrays(GL_TRIANGLE_FAN,0,SEGS+2);
   glPopMatrix();
   glPushMatrix();
   n = 0;
   m = 0;
   l = 0;
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   VertexData[n++] = 0.0; VertexData[n++] = 0.0; VertexData[n++] = QUEUE_L;
   for(i=0;i<sidenr+2;i++){
       ph=2.0*M_PI-2.0*M_PI/(VMfloat)sidenr*i;
       v1.x = QUEUE_D1/2.0*cos(ph);
       v1.y = QUEUE_D1/2.0*sin(ph);
       v1.z = QUEUE_L-dl1;
       VertexData[n++] = v1.x; VertexData[n++] = v1.y; VertexData[n++] = v1.z;
       TexData[m++] = 1.0-v1.z/QUEUE_L; TexData[m++] = (v1.x+QUEUE_D1/2.0)/QUEUE_D1;
       NormalData[l++] = 0.0; NormalData[l++] = 0.0; NormalData[l++] = 1.0;
   }
      // draw the back of the cue
      glDrawArrays(GL_TRIANGLE_FAN,0,SEGS+2);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_NORMAL_ARRAY);
      glPopMatrix();
   glEndList();

   return queue_obj;
}

/***********************************************************************/

void delete_queue_texbind( void )
{
    glDeleteTextures(1,&queuetexbind);
}

/***********************************************************************/

void create_queue_texbind( void )
{
    int queuetexw,queuetexh, depth;
    char * queuetexdata;
    glGenTextures(1,&queuetexbind);
    load_png("queue.png",&queuetexw,&queuetexh,&depth,&queuetexdata);
    glBindTexture(GL_TEXTURE_2D,queuetexbind);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, queuetexw, queuetexh, GL_RGB,
                      GL_UNSIGNED_BYTE, queuetexdata);
    if(options_anisotrop && options_value_anisotrop > 0.0) {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, options_value_anisotrop);
    }
    free(queuetexdata);
}

/***********************************************************************/

void draw_queue( VMvect pos0, GLfloat Xrot, GLfloat Zrot, GLfloat zoffs,
                 GLfloat xoffs, GLfloat yoffs, int spheretexbind, VMvect * lightpos, int lightnr )
{
    static int init=0;
    static int queue_obj,queue_obj_reflection;
    static unsigned int queueshadowbind;
    static int queuetexw,queuetexh;
    static char * queuetexdata;
    GLfloat queue_col_spec[4] = {0.0, 0.0, 0.0, 1.0};
    GLfloat queue_col_diff[4] = {0.8, 0.8, 0.8, 1.0};
    GLfloat queue_col_refl[4] = {1.0, 1.0, 1.0, 0.2};
    GLfloat queue_col_amb [4];
    int i,depth;
    VMvect dir,nx,ny,hitpoint,pos;
    VMvect shoulder1, shoulder2, shoulder_dir, hand1, hand2, elbow1, elbow2, xdir, ydir; //Avatar
    VMfloat x1,x; //Avatar
    //we don't need to optimize sin/cos calculation here, because the cue is only drawn in uncritical program places
    dir = vec_xyz(MATH_SIN(Zrot*M_PI/180.0)*MATH_SIN(Xrot*M_PI/180.0),
                  MATH_COS(Zrot*M_PI/180.0)*MATH_SIN(Xrot*M_PI/180.0),
                  MATH_COS(Xrot*M_PI/180.0));
    nx = vec_unit(vec_cross(vec_ez(),dir));  /* parallel to table */
    ny = vec_unit(vec_cross(nx,dir));        /* orthogonal to dir and nx */
    //fprintf(stderr,"%f %f %f\n",dir.x,dir.y,dir.z);
    hitpoint = vec_add(vec_scale(nx,xoffs),vec_scale(ny,yoffs));
    pos = vec_add(pos0,hitpoint);
    pos = vec_add(pos,vec_scale(dir,zoffs));

    for(i=0;i<3;i++) queue_col_amb[i]=queue_col_diff[i]*0.5;
    glDisable(GL_CULL_FACE);
    if(!init){
        create_queue_texbind();
        glGenTextures(1,&queueshadowbind);
        load_png("queue_shadow.png",&queuetexw,&queuetexh,&depth,&queuetexdata);
        glBindTexture(GL_TEXTURE_2D,queueshadowbind);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 1, queuetexw, queuetexh, GL_LUMINANCE, GL_UNSIGNED_BYTE, queuetexdata);
        free(queuetexdata);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options_tex_min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options_tex_mag_filter);
        queue_obj=create_queue(rfunc);
        queue_obj_reflection = glGenLists(1);
        glNewList(queue_obj_reflection, GL_COMPILE);
        /* draw queue-reflections */
            glCallList( queue_obj );
            glPolygonOffset( 0.0, -1.0 );
            glEnable( GL_POLYGON_OFFSET_FILL );
            glDepthMask (GL_FALSE);
            glEnable(GL_BLEND);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, queue_col_refl);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            glBindTexture(GL_TEXTURE_2D,spheretexbind);
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            glCallList( queue_obj );
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            glDisable(GL_BLEND);
            glDepthMask (GL_TRUE);
            glDisable( GL_POLYGON_OFFSET_FILL );
        glEndList();
        init=1;
    }

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   queue_col_diff);
    glMaterialfv(GL_FRONT, GL_AMBIENT,   queue_col_amb);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  queue_col_spec);
    glMaterialf (GL_FRONT, GL_SHININESS, 0.0 );

    glPushMatrix();

    glTranslatef( pos.x, pos.y, pos.z );
    glRotatef( -Zrot, 0.0, 0.0, 1.0 );
    glRotatef( -Xrot, 1.0, 0.0, 0.0 );
    glBindTexture(GL_TEXTURE_2D,queuetexbind);
    if(options_anisotrop) {
       glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, options_value_anisotrop );
    }
    glCallList(queue_obj_reflection);
    glPopMatrix();

    /* draw queue-shadow */
    VMvect p1,p2,vn;

    GLfloat col_spec [4] = {0.0, 0.0, 0.0, 1.0}; /* dont need any specular because of reflections */
    GLfloat col_amb  [4] = {0.2, 0.2, 0.2, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT,   col_amb );
    glMaterialfv(GL_FRONT, GL_SPECULAR,  col_spec);
    glMaterialf (GL_FRONT, GL_SHININESS, 0.0     );
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDepthMask (GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc (GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    for(i=0;i<lightnr;i++){
        /* cue tip projected on table */
        p1 = vec_surf_proj(lightpos[i],             /* proj point */
                           pos,                     /* point to proj */
                           vec_xyz(0,0,1),          /* surface normal */
                           vec_xyz(0,0,-BALL_D/2.02) /* surface pos */
                           );
        /* cue tail (somewhat) projected on table */
        p2 = vec_surf_proj(lightpos[i],             /* proj point */
                           vec_add(pos,vec_scale(dir,0.3)),                     /* point to proj */
                           vec_xyz(0,0,1),          /* surface normal */
                           vec_xyz(0,0,-BALL_D/2.02) /* surface pos */
                           );
//      fprintf(stderr,"i=%d, p1=<%f,%f,%f>, p2=<%f,%f,%f>\n",i,p1.x,p1.y,p1.z,p2.x,p2.y,p2.z);
        vn = vec_diff(p2,p1);
        vn = vec_unit(vec_xyz(-vn.y,vn.x,0.0));

        col_shad[0]=0.7-0.4*vec_abs(vec_diff(lightpos[i],p1));
        col_shad[1]=col_shad[0];
        col_shad[2]=col_shad[0];
        col_shad[3]=1.0;
        glMaterialfv(GL_FRONT, GL_DIFFUSE, col_shad);

        glPushMatrix();

        glBindTexture(GL_TEXTURE_2D,queueshadowbind);
        GLfloat VertexData1[12];
        static const GLshort TexData1[] = {0,0,1,0,0,1,1,1};
        static const GLfloat NormalData1[] = {0.0,0.0,1.0,0.0,0.0,1.0,0.0,0.0,1.0,0.0,0.0,1.0};
        VertexData1[0] = p1.x-vn.x*cue_shad_w2;
        VertexData1[1] = p1.y-vn.y*cue_shad_w2;
        VertexData1[2] = p1.z;
        VertexData1[3] = p1.x+vn.x*cue_shad_w2;
        VertexData1[4] = p1.y+vn.y*cue_shad_w2;
        VertexData1[5] = p1.z;
        VertexData1[9] = p2.x+vn.x*cue_shad_w2;
        VertexData1[10] = p2.y+vn.y*cue_shad_w2;
        VertexData1[11] = p2.z;
        VertexData1[6] = p2.x-vn.x*cue_shad_w2;
        VertexData1[7] = p2.y-vn.y*cue_shad_w2;
        VertexData1[8] = p2.z;
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glTexCoordPointer(2,GL_SHORT, 0, TexData1);
        glVertexPointer(3, GL_FLOAT, 0, VertexData1);
        glNormalPointer(GL_FLOAT, 0, NormalData1);
        glDrawArrays(GL_TRIANGLE_STRIP,0,4);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glPopMatrix();
        }
    glDisable(GL_BLEND);
    glDepthMask (GL_TRUE);

    if( options_avatar_on ){
      #define upperarm_l 0.30
      #define forearm_l  0.35
      #define shoulder_l 0.35
      #define shoulder1_h 0.50

        hand2 = vec_add(pos0,hitpoint);
        hand2 = vec_add(hand2,vec_scale(dir,0.3));
        shoulder1 = vec_add(pos0,hitpoint);
        shoulder1 = vec_add(shoulder1,vec_scale(dir,1.25));
        shoulder1.z = shoulder1_h;
        shoulder_dir = vec_unit(vec_add(vec_add(vec_scale(nx,-0.4),vec_scale(ny,0.4)),vec_scale(dir,-0.7)));
        shoulder2 = vec_add(shoulder1,vec_scale(shoulder_dir,shoulder_l));
        hand1 = vec_add(pos,vec_scale(dir,1.2));

        xdir = vec_unit(vec_diff(shoulder1,hand1));
        ydir = vec_unit(vec_cross(nx,xdir));
        x  = vec_abs(vec_diff(shoulder1,hand1));
        x1 = x/2.0 + (upperarm_l*upperarm_l - forearm_l*forearm_l)/x;
        elbow1 = vec_add(vec_scale(xdir,-x1),vec_scale(ydir,sqrt(upperarm_l*upperarm_l-x1*x1)));
        elbow1 = vec_add(elbow1,shoulder1);

        xdir = vec_unit(vec_diff(shoulder2,hand2));
        ydir = vec_unit(vec_cross(nx,xdir));
        x  = vec_abs(vec_diff(shoulder2,hand2));
        x1 = x/2.0 + (upperarm_l*upperarm_l - forearm_l*forearm_l)/x;
        elbow2 = vec_add(vec_scale(xdir,-x1),vec_scale(ydir,sqrt(upperarm_l*upperarm_l-x1*x1)));
        elbow2 = vec_add(elbow2,shoulder2);

        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

#ifdef WETAB_ALIASING
        if(options_antialiasing) {
          glLineWidth(1.5);
          glEnable(GL_LINE_SMOOTH);
          glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
        }
#endif
        GLfloat VertexData[15];
        static const GLfloat ColorData[] = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
        VertexData[0] = hand1.x;
        VertexData[1] = hand1.y;
        VertexData[2] = hand1.z;
        VertexData[3] = elbow1.x;
        VertexData[4] = elbow1.y;
        VertexData[5] = elbow1.z;
        VertexData[6] = shoulder1.x;
        VertexData[7] = shoulder1.y;
        VertexData[8] = shoulder1.z;
        VertexData[9] = shoulder2.x;
        VertexData[10] = shoulder2.y;
        VertexData[11] = shoulder2.z;
        VertexData[12] = hand2.x;
        VertexData[13] = hand2.y;
        VertexData[14] = hand2.z;
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, VertexData);
        glColorPointer(3, GL_FLOAT, 0, ColorData);
        glPushMatrix();
        glDrawArrays(GL_LINE_STRIP, 0, 5);
        glPopMatrix();
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
#ifdef WETAB_ALIASING
        if(options_antialiasing) {
          glDisable(GL_LINE_SMOOTH);
          glDisable(GL_BLEND);
        }
#endif
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
    }
    glEnable(GL_CULL_FACE);

}
