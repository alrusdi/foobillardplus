/* table.c
**
**    create the billard-table display lists
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
 #include <OpenGL/glu.h>
 #include <OpenGL/gl.h>
 #include <OpenGL/glext.h>
#else
 #include <GL/glu.h>
 #include <GL/gl.h>
 #include <GL/glext.h>
#endif
#include "billard.h"
#include "table.h"
#include "options.h"
#include "png_loader.h"
#include "bumpref.h"
#include "vmath.h"

#ifdef __APPLE__
	#define glActiveTexture glActiveTextureARB
#endif

#ifdef __MINGW32__
	extern void ( APIENTRY * glActiveTextureARB)( GLenum );
#endif

/***********************************************************************/

// The following function is for debugging only
// the remarks for gllines in the code too

/* void my_glBox( VMfloat x1, VMfloat y1, VMfloat z1, VMfloat x2, VMfloat y2, VMfloat z2 ) {

    VMfloat dummy;

    if( x1 > x2 ){ dummy=x1; x1=x2; x2=dummy; }
    if( y1 > y2 ){ dummy=y1; y1=y2; y2=dummy; }
    if( z1 > z2 ){ dummy=z1; z1=z2; z2=dummy; }
    
    glBegin(GL_QUADS);
    
       glNormal3f( 0.0, -1.0, 0.0 );
       glVertex3f(x1,y1,z1); glVertex3f(x1,y1,z2);
       glVertex3f(x2,y1,z2); glVertex3f(x2,y1,z1);

       glNormal3f( 1.0, 0.0, 0.0 );
       glVertex3f(x2,y1,z1); glVertex3f(x2,y1,z2);
       glVertex3f(x2,y2,z2); glVertex3f(x2,y2,z1);
       
       glNormal3f( 0.0, 1.0, 0.0 );
       glVertex3f(x2,y2,z1); glVertex3f(x2,y2,z2);
       glVertex3f(x1,y2,z2); glVertex3f(x1,y2,z1);

       glNormal3f( -1.0, 0.0, 0.0 );
       glVertex3f(x1,y2,z1); glVertex3f(x1,y2,z2);
       glVertex3f(x1,y1,z2); glVertex3f(x1,y1,z1);

       glNormal3f( 0.0, 0.0, -1.0 );
       glVertex3f(x1,y1,z1); glVertex3f(x2,y1,z1);
       glVertex3f(x2,y2,z1); glVertex3f(x1,y2,z1);
       
       glNormal3f( 0.0, 0.0, 1.0 );
       glVertex3f(x1,y1,z2); glVertex3f(x1,y2,z2);
       glVertex3f(x2,y2,z2); glVertex3f(x2,y1,z2);
       
    glEnd();
} */

/***********************************************************************/

void autonormalize_quad( VMvect v1, VMvect v2, VMvect v3, VMvect v4, int order, GLfloat *VertexData, GLfloat *NormalData )
{
    VMvect n;
    n=vec_unit(vec_cross(vec_diff(v2,v1),vec_diff(v3,v1)));
    NormalData[0] = n.x; NormalData[1] = n.y; NormalData[2] = n.z;
    NormalData[3] = n.x; NormalData[4] = n.y; NormalData[5] = n.z;
    NormalData[6] = n.x; NormalData[7] = n.y; NormalData[8] = n.z;
    NormalData[9] = n.x; NormalData[10] = n.y; NormalData[11] = n.z;
    if( !order ) {
        VertexData[0] = v4.x; VertexData[1] = v4.y; VertexData[2] = v4.z;
        VertexData[3] = v3.x; VertexData[4] = v3.y; VertexData[5] = v3.z;
        VertexData[6] = v2.x; VertexData[7] = v2.y; VertexData[8] = v2.z;
        VertexData[9] = v1.x; VertexData[10] = v1.y; VertexData[11] = v1.z;
    } else {
        VertexData[0] = v1.x; VertexData[1] = v1.y; VertexData[2] = v1.z;
        VertexData[3] = v2.x; VertexData[4] = v2.y; VertexData[5] = v2.z;
        VertexData[6] = v3.x; VertexData[7] = v3.y; VertexData[8] = v3.z;
        VertexData[9] = v4.x; VertexData[10] = v4.y; VertexData[11] = v4.z;
    }
}

/***********************************************************************/

void autonormalize_triangle( VMvect v1, VMvect v2, VMvect v3, int order, GLfloat *VertexData, GLfloat *NormalData )
{
    VMvect n;
    n=vec_unit(vec_cross(vec_diff(v2,v1),vec_diff(v3,v1)));
    NormalData[0] = n.x; NormalData[1] = n.y; NormalData[2] = n.z;
    NormalData[3] = n.x; NormalData[4] = n.y; NormalData[5] = n.z;
    NormalData[6] = n.x; NormalData[7] = n.y; NormalData[8] = n.z;
    if( !order ) {
        VertexData[0] = v3.x; VertexData[1] = v3.y; VertexData[2] = v3.z;
        VertexData[3] = v2.x; VertexData[4] = v2.y; VertexData[5] = v2.z;
        VertexData[6] = v1.x; VertexData[7] = v1.y; VertexData[8] = v1.z;
    } else {
        VertexData[0] = v1.x; VertexData[1] = v1.y; VertexData[2] = v1.z;
        VertexData[3] = v2.x; VertexData[4] = v2.y; VertexData[5] = v2.z;
        VertexData[6] = v3.x; VertexData[7] = v3.y; VertexData[8] = v3.z;
    }
}

/***********************************************************************/

void autonormalize_triangle_round( VMvect v1, VMvect v2, VMvect v3, int order, VMfloat round, GLfloat *VertexData, GLfloat *NormalData )
{
    VMvect n0,c,n;
    n0=vec_cross(vec_diff(v2,v1),vec_diff(v3,v1));
    c=tri_center(v1,v2,v3);
    if( !order ) {
        n=vec_unit(vec_add(n0,vec_scale(vec_diff(v3,c),round)));
        NormalData[0] = n.x; NormalData[1] = n.y; NormalData[2] = n.z;
        VertexData[0] = v3.x; VertexData[1] = v3.y; VertexData[2] = v3.z;
        n=vec_unit(vec_add(n0,vec_scale(vec_diff(v2,c),round)));
        NormalData[3] = n.x; NormalData[4] = n.y; NormalData[5] = n.z;
        VertexData[3] = v2.x; VertexData[4] = v2.y; VertexData[5] = v2.z;
        n=vec_unit(vec_add(n0,vec_scale(vec_diff(v1,c),round)));
        NormalData[6] = n.x; NormalData[7] = n.y; NormalData[8] = n.z;
        VertexData[6] = v1.x; VertexData[7] = v1.y; VertexData[8] = v1.z;
    } else {
        n=vec_unit(vec_add(n0,vec_scale(vec_diff(v1,c),round)));
        NormalData[0] = n.x; NormalData[1] = n.y; NormalData[2] = n.z;
        VertexData[0] = v1.x; VertexData[1] = v1.y; VertexData[2] = v1.z;
        n=vec_unit(vec_add(n0,vec_scale(vec_diff(v2,c),round)));
        NormalData[3] = n.x; NormalData[4] = n.y; NormalData[5] = n.z;
        VertexData[3] = v2.x; VertexData[4] = v2.y; VertexData[5] = v2.z;
        n=vec_unit(vec_add(n0,vec_scale(vec_diff(v3,c),round)));
        NormalData[6] = n.x; NormalData[7] = n.y; NormalData[8] = n.z;
        VertexData[6] = v3.x; VertexData[7] = v3.y; VertexData[8] = v3.z;
    }
}

/***********************************************************************/

void my_Diamondxy( VMfloat wx, VMfloat wy, VMfloat h, int order )
{
    VMvect p[10];
    p[3] = vec_xyz( -wx, 0.0, 0.0 );
    p[2] = vec_xyz( 0.0,  wy, 0.0 );
    p[1] = vec_xyz(  wx, 0.0, 0.0 );
    p[0] = vec_xyz( 0.0, -wy, 0.0 );
    p[4] = vec_xyz( 0.0, 0.0,   h );
    static GLfloat NormalData[3*3*4];
    static GLfloat VertexData[3*3*4];
    autonormalize_triangle_round(p[0],p[1],p[4],order,-0.002,&VertexData[0],&NormalData[0]);
    autonormalize_triangle_round(p[1],p[2],p[4],order,-0.002,&VertexData[9],&NormalData[9]);
    autonormalize_triangle_round(p[2],p[3],p[4],order,-0.002,&VertexData[18],&NormalData[18]);
    autonormalize_triangle_round(p[3],p[0],p[4],order,-0.002,&VertexData[27],&NormalData[27]);
    glNormalPointer(GL_FLOAT, 0, NormalData);
    glVertexPointer(3, GL_FLOAT, 0, VertexData);
    glPushMatrix();
    glDrawArrays(GL_TRIANGLES,0,12);
    glPopMatrix();
}

/***********************************************************************
 *                            For Debugging                            *
 **********************************************************************/

//void my_Rectxy( VMfloat x1, VMfloat y1, VMfloat z1,    /* inside up */
//                VMfloat x2, VMfloat y2, VMfloat z2,
//                int fall_x, int order )   /* outside down */
/*{
    VMvect p[10];

    if(fall_x){
        p[0]=vec_xyz(x1,y1,z1);
        p[1]=vec_xyz(x1,y2,z1);
        p[2]=vec_xyz(x2,y2,z2);
        p[3]=vec_xyz(x2,y1,z2);
        glBegin(GL_QUADS);
          autonormalize_quad(p[0],p[1],p[2],p[3],order);
        glEnd();
    } else {
        p[0]=vec_xyz(x1,y1,z1);
        p[1]=vec_xyz(x1,y2,z2);
        p[2]=vec_xyz(x2,y2,z2);
        p[3]=vec_xyz(x2,y1,z1);
        glBegin(GL_QUADS);
          autonormalize_quad(p[3],p[2],p[1],p[0],!order);
        glEnd();
    }
} */

/***********************************************************************/

VMfloat r1func( VMfloat phi )
{
    VMfloat phi1, rval;

    phi1=atan(HOLE1_XYOFFS/(HOLE1_R*SQR2+HOLE1_XYOFFS));
    if(phi<phi1){
        rval=HOLE1_R/cos(phi+M_PI/4.0);
    } else if (phi>M_PI/2.0-phi1){
        rval=HOLE1_R/cos(M_PI/2.0-phi+M_PI/4.0);
    } else {
        rval=HOLE1_R/cos(phi1+M_PI/4.0)+0.7*HOLE1_R*sin((phi-phi1)/(M_PI/2-phi1-phi1)*M_PI);
    }
    return(rval);
}

/***********************************************************************/

VMfloat r2func( VMfloat phi )
{
    VMfloat phi1, rval;

    phi1=M_PI/8.0;
    if(phi<phi1){
        rval=(HOLE1_R*SQR2-BANDE_D+FRAME_D)/cos(phi);
    } else if (phi>M_PI/2.0-phi1){
        rval=(HOLE1_R*SQR2-BANDE_D+FRAME_D)/cos(M_PI/2.0-phi);
    } else {
        rval=(HOLE1_R*SQR2-BANDE_D+FRAME_D)/cos(phi1);

    }
    return(rval);
}

/***********************************************************************/

void my_Edge( int segnr, VMfloat  (*r1)(VMfloat), VMfloat (*r2)(VMfloat), int order )
/* edge shows to positive x,y direction*/
{
    int i;
    VMfloat phi1,phi2,dphi1,dphi2;
    VMvect n10,n20,n30,n40,n1,n2,n3,n4,n5,n6,n7,n8,v1,v2,v3,v4,v5,v6,v7,v8, v8_2,v8_3, v4_2,v4_3;
    GLfloat VertexData[12*3];
    GLfloat TexData[12*2];
    GLfloat NormalData[12*3];

    n10 = vec_xyz(0,0,1);
    n20 = vec_unit(vec_xyz(FRAME_DH,0,FRAME_D-BANDE_D-FRAME_PHASE));
    n30 = vec_unit(vec_xyz(FRAME_H-FRAME_PHASE,0,-FRAME_PHASE));
    n40 = vec_unit(vec_xyz(FRAME_H-FRAME_PHASE,0,-FRAME_PHASE));
    for(i=0;i<segnr;i++){
        phi1 = M_PI/2.0*(VMfloat)i/(VMfloat)segnr;
        phi2 = M_PI/2.0*(VMfloat)(i+1)/(VMfloat)segnr;
        dphi1 = atan((r2(phi1-0.01)-r2(phi1+0.01))/r2(phi1)/0.02);
        dphi2 = atan((r2(phi2-0.01)-r2(phi2+0.01))/r2(phi2)/0.02);
        n1 = vec_rotate(n10,vec_xyz(0,0,phi1+dphi1));
        v1 = vec_xyz(r1(phi1)*cos(phi1),r1(phi1)*sin(phi1),FRAME_DH);
        n2 = vec_rotate(n20,vec_xyz(0,0,phi1+dphi1));
        v2 = vec_xyz((r2(phi1)-FRAME_PHASE)*cos(phi1),(r2(phi1)-FRAME_PHASE)*sin(phi1),0);
        n3 = vec_rotate(n30,vec_xyz(0,0,phi1+dphi1));
        v3 = vec_xyz(r2(phi1)*cos(phi1),r2(phi1)*sin(phi1),-FRAME_PHASE);
        n4 = vec_rotate(n40,vec_xyz(0,0,phi1+dphi1));
        v4 = vec_xyz((r2(phi1)-FRAME_PHASE)*cos(phi1),(r2(phi1)-FRAME_PHASE)*sin(phi1),-FRAME_H);
        v4_2 = vec_xyz((r2(phi1)-FRAME_PHASE-FRAME_DW)*cos(phi1),(r2(phi1)-FRAME_PHASE-FRAME_DW)*sin(phi1),-FRAME_H);
        v4_3 = vec_xyz((r2(phi1)-FRAME_PHASE-2.0*FRAME_DW)*cos(phi1),(r2(phi1)-FRAME_PHASE-2.0*FRAME_DW)*sin(phi1),-FRAME_H2);
        n5 = vec_rotate(n10,vec_xyz(0,0,phi2+dphi2));
        v5 = vec_xyz(r1(phi2)*cos(phi2),r1(phi2)*sin(phi2),FRAME_DH);
        n6 = vec_rotate(n20,vec_xyz(0,0,phi2+dphi2));
        v6 = vec_xyz((r2(phi2)-FRAME_PHASE)*cos(phi2),(r2(phi2)-FRAME_PHASE)*sin(phi2),0);
        n7 = vec_rotate(n30,vec_xyz(0,0,phi2+dphi2));
        v7 = vec_xyz(r2(phi2)*cos(phi2),r2(phi2)*sin(phi2),-FRAME_PHASE);
        n8 = vec_rotate(n40,vec_xyz(0,0,phi2+dphi2));
        v8 = vec_xyz((r2(phi2)-FRAME_PHASE)*cos(phi2),(r2(phi2)-FRAME_PHASE)*sin(phi2),-FRAME_H);
        v8_2 = vec_xyz((r2(phi2)-FRAME_PHASE-FRAME_DW)*cos(phi2),(r2(phi2)-FRAME_PHASE-FRAME_DW)*sin(phi2),-FRAME_H);
        v8_3 = vec_xyz((r2(phi2)-FRAME_PHASE-2.0*FRAME_DW)*cos(phi2),(r2(phi2)-FRAME_PHASE-2.0*FRAME_DW)*sin(phi2),-FRAME_H2);
        if(!order){
#define FACT 8.0
            TexData[0] = phi1/M_PI*2.0; TexData[1] = 0.0*0.5;
            NormalData[0] = n1.x; NormalData[1] = n1.y; NormalData[2] = n1.z;
            VertexData[0] = v1.x; VertexData[1] = v1.y; VertexData[2] = v1.z;
            TexData[2] = phi2/M_PI*2.0; TexData[3] = 0.0*0.5;
            NormalData[3] = n5.x; NormalData[4] = n5.y; NormalData[5] = n5.z;
            VertexData[3] = v5.x; VertexData[4] = v5.y; VertexData[5] = v5.z;
            TexData[4] = phi1/M_PI*2.0; TexData[5] = 0.9*0.5;
            NormalData[6] = n2.x; NormalData[7] = n2.y; NormalData[8] = n2.z;
            VertexData[6] = v2.x; VertexData[7] = v2.y; VertexData[8] = v2.z;
            TexData[6] = phi2/M_PI*2.0; TexData[7] = 0.9*0.5;
            NormalData[9] = n6.x; NormalData[10] = n6.y; NormalData[11] = n6.z;
            VertexData[9] = v6.x; VertexData[10] = v6.y; VertexData[11] = v6.z;
            TexData[8] = phi1/M_PI*2.0; TexData[9] = 1.0*0.5;
            NormalData[12] = n3.x; NormalData[13] = n3.y; NormalData[14] = n3.z;
            VertexData[12] = v3.x; VertexData[13] = v3.y; VertexData[14] = v3.z;
            TexData[10] = phi2/M_PI*2.0; TexData[11] = 1.0*0.5;
            NormalData[15] = n7.x; NormalData[16] = n7.y; NormalData[17] = n7.z;
            VertexData[15] = v7.x; VertexData[16] = v7.y; VertexData[17] = v7.z;
            TexData[12] = phi1/M_PI*2.0; TexData[13] = 2.0*0.5;
            NormalData[18] = n4.x; NormalData[19] = n4.y; NormalData[20] = n4.z;
            VertexData[18] = v4.x; VertexData[19] = v4.y; VertexData[20] = v4.z;
            TexData[14] = phi2/M_PI*2.0; TexData[15] = 2.0*0.5;
            NormalData[21] = n8.x; NormalData[22] = n8.y; NormalData[23] = n8.z;
            VertexData[21] = v8.x; VertexData[22] = v8.y; VertexData[23] = v8.z;
            TexData[16] = phi1/M_PI*2.0; TexData[17] = 2.0*0.5;
            NormalData[24] = n4.x; NormalData[25] = n4.y; NormalData[26] = n4.z;
            VertexData[24] = v4_2.x; VertexData[25] = v4_2.y; VertexData[26] = v4_2.z;
            TexData[18] = phi2/M_PI*2.0; TexData[19] = 2.0*0.5;
            NormalData[27] = n8.x; NormalData[28] = n8.y; NormalData[29] = n8.z;
            VertexData[27] = v8_2.x; VertexData[28] = v8_2.y; VertexData[29] = v8_2.z;
            TexData[20] = phi1/M_PI*2.0; TexData[21] = 3.0*0.5;
            NormalData[30] = n4.x; NormalData[31] = n4.y; NormalData[32] = n4.z;
            VertexData[30] = v4_3.x; VertexData[31] = v4_3.y; VertexData[32] = v4_3.z;
            TexData[22] = phi2/M_PI*2.0; TexData[23] = 3.0*0.5;
            NormalData[33] = n8.x; NormalData[34] = n8.y; NormalData[35] = n8.z;
            VertexData[33] = v8_3.x; VertexData[34] = v8_3.y; VertexData[35] = v8_3.z;
        } else {
            TexData[0] = 1.0-phi2/M_PI*2.0; TexData[1] = 0.0*0.5;
            NormalData[0] = n5.x; NormalData[1] = n5.y; NormalData[2] = n5.z;
            VertexData[0] = v5.x; VertexData[1] = v5.y; VertexData[2] = v5.z;
            TexData[2] = 1.0-phi1/M_PI*2.0; TexData[3] = 0.0*0.5;
            NormalData[3] = n1.x; NormalData[4] = n1.y; NormalData[5] = n1.z;
            VertexData[3] = v1.x; VertexData[4] = v1.y; VertexData[5] = v1.z;
            TexData[4] = 1.0-phi2/M_PI*2.0; TexData[5] = 0.9*0.5;
            NormalData[6] = n6.x; NormalData[7] = n6.y; NormalData[8] = n6.z;
            VertexData[6] = v6.x; VertexData[7] = v6.y; VertexData[8] = v6.z;
            TexData[6] = 1.0-phi1/M_PI*2.0; TexData[7] = 0.9*0.5;
            NormalData[9] = n2.x; NormalData[10] = n2.y; NormalData[11] = n2.z;
            VertexData[9] = v2.x; VertexData[10] = v2.y; VertexData[11] = v2.z;
            TexData[8] = 1.0-phi2/M_PI*2.0; TexData[9] = 1.0*0.5;
            NormalData[12] = n7.x; NormalData[13] = n7.y; NormalData[14] = n7.z;
            VertexData[12] = v7.x; VertexData[13] = v7.y; VertexData[14] = v7.z;
            TexData[10] = 1.0-phi1/M_PI*2.0; TexData[11] = 1.0*0.5;
            NormalData[15] = n3.x; NormalData[16] = n3.y; NormalData[17] = n3.z;
            VertexData[15] = v3.x; VertexData[16] = v3.y; VertexData[17] = v3.z;
            TexData[12] = 1.0-phi2/M_PI*2.0; TexData[13] = 2.0*0.5;
            NormalData[18] = n8.x; NormalData[19] = n8.y; NormalData[20] = n8.z;
            VertexData[18] = v8.x; VertexData[19] = v8.y; VertexData[20] = v8.z;
            TexData[14] = 1.0-phi1/M_PI*2.0; TexData[15] = 2.0*0.5;
            NormalData[21] = n4.x; NormalData[22] = n4.y; NormalData[23] = n4.z;
            VertexData[21] = v4.x; VertexData[22] = v4.y; VertexData[23] = v4.z;
            TexData[16] = 1.0-phi2/M_PI*2.0; TexData[17] = 2.0*0.5;
            NormalData[24] = n8.x; NormalData[25] = n8.y; NormalData[26] = n8.z;
            VertexData[24] = v8_2.x; VertexData[25] = v8_2.y; VertexData[26] = v8_2.z;
            TexData[18] = 1.0-phi1/M_PI*2.0; TexData[19] = 2.0*0.5;
            NormalData[27] = n4.x; NormalData[28] = n4.y; NormalData[29] = n4.z;
            VertexData[27] = v4_2.x; VertexData[28] = v4_2.y; VertexData[29] = v4_2.z;
            TexData[20] = 1.0-phi2/M_PI*2.0; TexData[21] = 3.0*0.5;
            NormalData[30] = n8.x; NormalData[31] = n8.y; NormalData[32] = n8.z;
            VertexData[30] = v8_3.x; VertexData[31] = v8_3.y; VertexData[32] = v8_3.z;
            TexData[22] = 1.0-phi1/M_PI*2.0; TexData[23] = 3.0*0.5;
            NormalData[33] = n4.x; NormalData[34] = n4.y; NormalData[35] = n4.z;
            VertexData[33] = v4_3.x; VertexData[34] = v4_3.y; VertexData[35] = v4_3.z;
        }
        glTexCoordPointer(2,GL_FLOAT, 0, TexData);
        glVertexPointer(3, GL_FLOAT, 0, VertexData);
        glNormalPointer(GL_FLOAT, 0, NormalData);
        glPushMatrix();
        glDrawArrays(GL_QUAD_STRIP,0,12);
        glPopMatrix();
        /* for debugging
        glBegin(GL_LINES);
            glVertex3f(v1.x,v1.y,v1.z); v1=vec_add(v1,vec_scale(n1,0.1)); glVertex3f(v1.x,v1.y,v1.z);
            glVertex3f(v2.x,v2.y,v2.z); v2=vec_add(v2,vec_scale(n2,0.1)); glVertex3f(v2.x,v2.y,v2.z);
            glVertex3f(v3.x,v3.y,v3.z); v3=vec_add(v3,vec_scale(n3,0.1)); glVertex3f(v3.x,v3.y,v3.z);
            glVertex3f(v4.x,v4.y,v4.z); v4=vec_add(v4,vec_scale(n4,0.1)); glVertex3f(v4.x,v4.y,v4.z);
        glEnd(); */
    }
}

/***********************************************************************/

VMfloat scalefunc( VMfloat phi )
{
    return(0.005*(0.2+0.8*sin(2.0*phi)));
}

/***********************************************************************/

void my_EdgeBumper( int segnr, VMfloat  (*r1)(VMfloat), VMfloat (*sc)(VMfloat), int order )
/* edge shows to positive x,y direction*/
{
    int i;
    VMfloat phi1,phi2,dphi1,dphi2;
    VMvect n10,n20,n30,n40,v10,v20,v30,v40,n1,n2,n3,n4,n5,n6,n7,n8,v1,v2,v3,v4,v5,v6,v7,v8;
    GLfloat VertexData[8*3];
    GLfloat NormalData[8*3];

    n10 = vec_unit(vec_xyz(-1,0,-1));
    n20 = vec_unit(vec_xyz(-1,0,0));
    n30 = vec_unit(vec_xyz(0,0,1));
    n40 = vec_unit(vec_xyz(1,0,3));
    v10 = vec_xyz(0,0,-1);
    v20 = vec_xyz(-1,0,0);
    v30 = vec_xyz(0,0,1);
    v40 = vec_xyz(3,0,0);
    for(i=0;i<segnr;i++){
        phi1 = M_PI/2.0*(VMfloat)i/(VMfloat)segnr;
        phi2 = M_PI/2.0*(VMfloat)(i+1)/(VMfloat)segnr;
        dphi1 = atan((r1(phi1-0.01)-r1(phi1+0.01))/r1(phi1)/0.02);
        dphi2 = atan((r1(phi2-0.01)-r1(phi2+0.01))/r1(phi2)/0.02);
        n1 = vec_rotate(n10,vec_xyz(0,0,phi1+dphi1));
        n2 = vec_rotate(n20,vec_xyz(0,0,phi1+dphi1));
        n3 = vec_rotate(n30,vec_xyz(0,0,phi1+dphi1));
        n4 = vec_rotate(n40,vec_xyz(0,0,phi1+dphi1));
        n5 = vec_rotate(n10,vec_xyz(0,0,phi2+dphi2));
        n6 = vec_rotate(n20,vec_xyz(0,0,phi2+dphi2));
        n7 = vec_rotate(n30,vec_xyz(0,0,phi2+dphi2));
        n8 = vec_rotate(n40,vec_xyz(0,0,phi2+dphi2));
        v1 = vec_xyz(r1(phi1)*cos(phi1),r1(phi1)*sin(phi1),FRAME_DH);
        v2 = vec_xyz(r1(phi1)*cos(phi1),r1(phi1)*sin(phi1),FRAME_DH);
        v3 = vec_xyz(r1(phi1)*cos(phi1),r1(phi1)*sin(phi1),FRAME_DH);
        v4 = vec_xyz(r1(phi1)*cos(phi1),r1(phi1)*sin(phi1),FRAME_DH);
        v5 = vec_xyz(r1(phi2)*cos(phi2),r1(phi2)*sin(phi2),FRAME_DH);
        v6 = vec_xyz(r1(phi2)*cos(phi2),r1(phi2)*sin(phi2),FRAME_DH);
        v7 = vec_xyz(r1(phi2)*cos(phi2),r1(phi2)*sin(phi2),FRAME_DH);
        v8 = vec_xyz(r1(phi2)*cos(phi2),r1(phi2)*sin(phi2),FRAME_DH);
        v1 = vec_add(v1,vec_scale(vec_rotate(v10,vec_xyz(0,0,phi1)),sc(phi1)));
        v2 = vec_add(v2,vec_scale(vec_rotate(v20,vec_xyz(0,0,phi1)),sc(phi1)));
        v3 = vec_add(v3,vec_scale(vec_rotate(v30,vec_xyz(0,0,phi1)),sc(phi1)));
        v4 = vec_add(v4,vec_scale(vec_rotate(v40,vec_xyz(0,0,phi1)),sc(phi1)));
        v5 = vec_add(v5,vec_scale(vec_rotate(v10,vec_xyz(0,0,phi2)),sc(phi2)));
        v6 = vec_add(v6,vec_scale(vec_rotate(v20,vec_xyz(0,0,phi2)),sc(phi2)));
        v7 = vec_add(v7,vec_scale(vec_rotate(v30,vec_xyz(0,0,phi2)),sc(phi2)));
        v8 = vec_add(v8,vec_scale(vec_rotate(v40,vec_xyz(0,0,phi2)),sc(phi2)));
        if(!order){
            NormalData[0] = n1.x; NormalData[1] = n1.y; NormalData[2] = n1.z;
            VertexData[0] = v1.x; VertexData[1] = v1.y; VertexData[2] = v1.z;
            NormalData[3] = n5.x; NormalData[4] = n5.y; NormalData[5] = n5.z;
            VertexData[3] = v5.x; VertexData[4] = v5.y; VertexData[5] = v5.z;
            NormalData[6] = n2.x; NormalData[7] = n2.y; NormalData[8] = n2.z;
            VertexData[6] = v2.x; VertexData[7] = v2.y; VertexData[8] = v2.z;
            NormalData[9] = n6.x; NormalData[10] = n6.y; NormalData[11] = n6.z;
            VertexData[9] = v6.x; VertexData[10] = v6.y; VertexData[11] = v6.z;
            NormalData[12] = n3.x; NormalData[13] = n3.y; NormalData[14] = n3.z;
            VertexData[12] = v3.x; VertexData[13] = v3.y; VertexData[14] = v3.z;
            NormalData[15] = n7.x; NormalData[16] = n7.y; NormalData[17] = n7.z;
            VertexData[15] = v7.x; VertexData[16] = v7.y; VertexData[17] = v7.z;
            NormalData[18] = n4.x; NormalData[19] = n4.y; NormalData[20] = n4.z;
            VertexData[18] = v4.x; VertexData[19] = v4.y; VertexData[20] = v4.z;
            NormalData[21] = n8.x; NormalData[22] = n8.y; NormalData[23] = n8.z;
            VertexData[21] = v8.x; VertexData[22] = v8.y; VertexData[23] = v8.z;
        } else {
            NormalData[0] = n5.x; NormalData[1] = n5.y; NormalData[2] = n5.z;
            VertexData[0] = v5.x; VertexData[1] = v5.y; VertexData[2] = v5.z;
            NormalData[3] = n1.x; NormalData[4] = n1.y; NormalData[5] = n1.z;
            VertexData[3] = v1.x; VertexData[4] = v1.y; VertexData[5] = v1.z;
            NormalData[6] = n6.x; NormalData[7] = n6.y; NormalData[8] = n6.z;
            VertexData[6] = v6.x; VertexData[7] = v6.y; VertexData[8] = v6.z;
            NormalData[9] = n2.x; NormalData[10] = n2.y; NormalData[11] = n2.z;
            VertexData[9] = v2.x; VertexData[10] = v2.y; VertexData[11] = v2.z;
            NormalData[12] = n7.x; NormalData[13] = n7.y; NormalData[14] = n7.z;
            VertexData[12] = v7.x; VertexData[13] = v7.y; VertexData[14] = v7.z;
            NormalData[15] = n3.x; NormalData[16] = n3.y; NormalData[17] = n3.z;
            VertexData[15] = v3.x; VertexData[16] = v3.y; VertexData[17] = v3.z;
            NormalData[18] = n8.x;  NormalData[19] = n8.y; NormalData[20] = n8.z;
            VertexData[18] = v8.x; VertexData[19] = v8.y; VertexData[20] = v8.z;
            NormalData[21] = n4.x; NormalData[22] = n4.y; NormalData[23] = n4.z;
            VertexData[21] = v4.x; VertexData[22] = v4.y; VertexData[23] = v4.z;
        }
        glVertexPointer(3, GL_FLOAT, 0, VertexData);
        glNormalPointer(GL_FLOAT, 0, NormalData);
        glPushMatrix();
        glDrawArrays(GL_QUAD_STRIP,0,8);
        glPopMatrix();
        /* for debugging
        glBegin(GL_LINES);
            glVertex3f(v1.x,v1.y,v1.z); v1=vec_add(v1,vec_scale(n1,0.01)); glVertex3f(v1.x,v1.y,v1.z);
            glVertex3f(v2.x,v2.y,v2.z); v2=vec_add(v2,vec_scale(n2,0.01)); glVertex3f(v2.x,v2.y,v2.z);
            glVertex3f(v3.x,v3.y,v3.z); v3=vec_add(v3,vec_scale(n3,0.01)); glVertex3f(v3.x,v3.y,v3.z);
            glVertex3f(v4.x,v4.y,v4.z); v4=vec_add(v4,vec_scale(n4,0.01)); glVertex3f(v4.x,v4.y,v4.z);
        glEnd(); */
    }
}

/***********************************************************************/

VMfloat r1coverfunc_zero( VMfloat y )
// y not used here, but in use for r1coverfunc
// function is used sometimes as a pointer to it.
// my_cover or my_cover2func use it
// Don't optimize this
{
    return( 0.0 );
}

/***********************************************************************/

VMfloat r1coverfunc( VMfloat y )
{
    if( y<=-1.0 || y>=1.0 ){
        return 0.0;
    } else {
        return( HOLE2_XYOFFS-BANDE_D+sqrt(1.0-y*y)*HOLE2_R );
    }
}

/***********************************************************************/

void my_Cover( int segnr, VMfloat (*r1)(VMfloat), VMfloat l, int order, VMfloat endtan, VMfloat tex_x, VMfloat tex_y )
/* edge shows to pos x,y direction*/
{
    int i;
    VMfloat y1,y2;
    VMfloat tan1, tan2;
    VMvect n1,n2,n3,n4,v1,v2,v3,v4,v5,v6,v7,v8, v4_2,v4_3, v8_2,v8_3;
    VMfloat r2;
    GLfloat VertexData[12*3];
    GLfloat NormalData[12*3];
    GLfloat TexData[12*2];
    r2 = FRAME_D-BANDE_D;
    n1 = vec_unit(vec_xyz(0,0,1));
    n2 = vec_unit(vec_xyz(FRAME_DH/(FRAME_D-BANDE_D-FRAME_PHASE),0,1));
    n3 = vec_unit(vec_xyz(FRAME_H-FRAME_PHASE,0,-FRAME_PHASE));
    n4 = vec_unit(vec_xyz(FRAME_H-FRAME_PHASE,0,-FRAME_PHASE));
    for(i=0;i<segnr;i++){
        y1 = -l/2.0*cos(M_PI*(VMfloat)i/(VMfloat)segnr);
        y2 = -l/2.0*cos(M_PI*(VMfloat)(i+1)/(VMfloat)segnr);
        tan1 = endtan*y1/(l/2.0);
        tan2 = endtan*y2/(l/2.0);
        v1 = vec_xyz(r1(y1/l*2.0),y1,FRAME_DH);       v1.y += tan1 * v1.x;
        v2 = vec_xyz(r2-FRAME_PHASE,y1,0);            v2.y += tan1 * v2.x;
        v3 = vec_xyz(r2,y1,-FRAME_PHASE);             v3.y += tan1 * v3.x;
        v4 = vec_xyz(r2-FRAME_PHASE,y1,-FRAME_H);     v4.y += tan1 * v4.x;
        v4_2 = vec_xyz(r2-FRAME_PHASE-1.0*FRAME_DW,y1,-FRAME_H);     v4_2.y += tan1 * v4_2.x;
        v4_3 = vec_xyz(r2-FRAME_PHASE-2.0*FRAME_DW,y1,-FRAME_H2);    v4_3.y += tan1 * v4_3.x;
        v5 = vec_xyz(r1(y2/l*2.0),y2,FRAME_DH);       v5.y += tan2 * v5.x;
        v6 = vec_xyz(r2-FRAME_PHASE,y2,0);            v6.y += tan2 * v6.x;
        v7 = vec_xyz(r2,y2,-FRAME_PHASE);             v7.y += tan2 * v7.x;
        v8 = vec_xyz(r2-FRAME_PHASE,y2,-FRAME_H);     v8.y += tan2 * v8.x;
        v8_2 = vec_xyz(r2-FRAME_PHASE-1.0*FRAME_DW,y2,-FRAME_H);     v8_2.y += tan2 * v8_2.x;
        v8_3 = vec_xyz(r2-FRAME_PHASE-2.0*FRAME_DW,y2,-FRAME_H2);    v8_3.y += tan2 * v8_3.x;
        if(!order){
            TexData[0] = (y1/l+0.5)*0.4*tex_x; TexData[1] = 0.0*0.5*tex_y;
            NormalData[0] = n1.x; NormalData[1] = n1.y; NormalData[2] = n1.z;
            VertexData[0] = v1.x; VertexData[1] = v1.y; VertexData[2] = v1.z;
            TexData[2] = (y2/l+0.5)*0.4*tex_x; TexData[3] = 0.0*0.5*tex_y;
            NormalData[3] = n1.x; NormalData[4] = n1.y; NormalData[5] = n1.z;
            VertexData[3] = v5.x; VertexData[4] = v5.y; VertexData[5] = v5.z;
            TexData[4] = (y1/l+0.5)*0.4*tex_x; TexData[5] = 0.9*0.5*tex_y;
            NormalData[6] = n2.x; NormalData[7] = n2.y; NormalData[8] = n2.z;
            VertexData[6] = v2.x; VertexData[7] = v2.y; VertexData[8] = v2.z;
            TexData[6] = (y2/l+0.5)*0.4*tex_x; TexData[7] = 0.9*0.5*tex_y;
            NormalData[9] = n2.x; NormalData[10] = n2.y; NormalData[11] = n2.z;
            VertexData[9] = v6.x; VertexData[10] = v6.y; VertexData[11] = v6.z;
            TexData[8] = (y1/l+0.5)*0.4*tex_x; TexData[9] = 1.0*0.5*tex_y;
            NormalData[12] = n3.x; NormalData[13] = n3.y; NormalData[14] = n3.z;
            VertexData[12] = v3.x; VertexData[13] = v3.y; VertexData[14] = v3.z;
            TexData[10] = (y2/l+0.5)*0.4*tex_x; TexData[11] = 1.0*0.5*tex_y;
            NormalData[15] = n3.x; NormalData[16] = n3.y; NormalData[17] = n3.z;
            VertexData[15] = v7.x; VertexData[16] = v7.y; VertexData[17] = v7.z;
            TexData[12] = (y1/l+0.5)*0.4*tex_x; TexData[13] = 2.0*0.5*tex_y;
            NormalData[18] = n4.x; NormalData[19] = n4.y; NormalData[20] = n4.z;
            VertexData[18] = v4.x; VertexData[19] = v4.y; VertexData[20] = v4.z;
            TexData[14] = (y2/l+0.5)*0.4*tex_x; TexData[15] = 2.0*0.5*tex_y;
            NormalData[21] = n4.x; NormalData[22] = n4.y; NormalData[23] = n4.z;
            VertexData[21] = v8.x; VertexData[22] = v8.y; VertexData[23] = v8.z;
            TexData[16] = (y1/l+0.5)*0.4*tex_x; TexData[17] = 3.0*0.5*tex_y;
            NormalData[24] = n4.x; NormalData[25] = n4.y; NormalData[26] = n4.z;
            VertexData[24] = v4_2.x; VertexData[25] = v4_2.y; VertexData[26] = v4_2.z;
            TexData[18] = (y2/l+0.5)*0.4*tex_x; TexData[19] = 3.0*0.5*tex_y;
            NormalData[27] = n4.x; NormalData[28] = n4.y; NormalData[29] = n4.z;
            VertexData[27] = v8_2.x; VertexData[28] = v8_2.y; VertexData[29] = v8_2.z;
            TexData[20] = (y1/l+0.5)*0.4*tex_x; TexData[21] = 3.0*0.5*tex_y;
            NormalData[30] = n4.x; NormalData[31] = n4.y; NormalData[32] = n4.z;
            VertexData[30] = v4_3.x; VertexData[31] = v4_3.y; VertexData[32] = v4_3.z;
            TexData[22] = (y2/l+0.5)*0.4*tex_x; TexData[23] = 3.0*0.5*tex_y;
            NormalData[33] = n4.x; NormalData[34] = n4.y; NormalData[35] = n4.z;
            VertexData[33] = v8_3.x; VertexData[34] = v8_3.y; VertexData[35] = v8_3.z;
        } else {
            TexData[0] = (y2/l+0.5)*0.4*tex_x; TexData[1] = 0.0*0.5*tex_y;
            NormalData[0] = n1.x; NormalData[1] = n1.y; NormalData[2] = n1.z;
            VertexData[0] = v5.x; VertexData[1] = v5.y; VertexData[2] = v5.z;
            TexData[2] = (y1/l+0.5)*0.4*tex_x; TexData[3] = 0.0*0.5*tex_y;
            NormalData[3] = n1.x; NormalData[4] = n1.y; NormalData[5] = n1.z;
            VertexData[3] = v1.x; VertexData[4] = v1.y; VertexData[5] = v1.z;
            TexData[4] = (y2/l+0.5)*0.4*tex_x; TexData[5] = 0.9*0.5*tex_y;
            NormalData[6] = n2.x; NormalData[7] = n2.y; NormalData[8] = n2.z;
            VertexData[6] = v6.x; VertexData[7] = v6.y; VertexData[8] = v6.z;
            TexData[6] = (y1/l+0.5)*0.4*tex_x; TexData[7] = 0.9*0.5*tex_y;
            VertexData[9] = v2.x; VertexData[10] = v2.y; VertexData[11] = v2.z;
            NormalData[9] = n2.x; NormalData[10] = n2.y; NormalData[11] = n2.z;
            TexData[8] = (y2/l+0.5)*0.4*tex_x; TexData[9] = 1.0*0.5*tex_y;
            NormalData[12] = n3.x; NormalData[13] = n3.y; NormalData[14] = n3.z;
            VertexData[12] = v7.x; VertexData[13] = v7.y; VertexData[14] = v7.z;
            NormalData[15] = n3.x; NormalData[16] = n3.y; NormalData[17] = n3.z;
            TexData[10] = (y1/l+0.5)*0.4*tex_x; TexData[11] = 1.0*0.5*tex_y;
            VertexData[15] = v3.x; VertexData[16] = v3.y; VertexData[17] = v3.z;
            TexData[12] = (y2/l+0.5)*0.4*tex_x; TexData[13] = 2.0*0.5*tex_y;
            NormalData[18] = n4.x; NormalData[19] = n4.y; NormalData[20] = n4.z;
            VertexData[18] = v8.x; VertexData[19] = v8.y; VertexData[20] = v8.z;
            TexData[14] = (y1/l+0.5)*0.4*tex_x; TexData[15] = 2.0*0.5*tex_y;
            NormalData[21] = n4.x; NormalData[22] = n4.y; NormalData[23] = n4.z;
            VertexData[21] = v4.x; VertexData[22] = v4.y; VertexData[23] = v4.z;
            TexData[16] = (y2/l+0.5)*0.4*tex_x; TexData[17] = 3.0*0.5*tex_y;
            NormalData[24] = n4.x; NormalData[25] = n4.y; NormalData[26] = n4.z;
            VertexData[24] = v8_2.x; VertexData[25] = v8_2.y; VertexData[26] = v8_2.z;
            TexData[18] = (y1/l+0.5)*0.4*tex_x; TexData[19] = 3.0*0.5*tex_y;
            NormalData[27] = n4.x; NormalData[28] = n4.y; NormalData[29] = n4.z;
            VertexData[27] = v4_2.x; VertexData[28] = v4_2.y; VertexData[29] = v4_2.z;
            TexData[20] = (y2/l+0.5)*0.4*tex_x; TexData[21] = 3.0*0.5*tex_y;
            NormalData[30] = n4.x; NormalData[31] = n4.y; NormalData[32] = n4.z;
            VertexData[30] = v8_3.x; VertexData[31] = v8_3.y; VertexData[32] = v8_3.z;
            TexData[22] = (y1/l+0.5)*0.4*tex_x; TexData[23] = 3.0*0.5*tex_y;
            NormalData[33] = n4.x; NormalData[34] = n4.y; NormalData[35] = n4.z;
            VertexData[33] = v4_3.x; VertexData[34] = v4_3.y; VertexData[35] = v4_3.z;
        }
        glVertexPointer(3, GL_FLOAT, 0, VertexData);
        glNormalPointer(GL_FLOAT, 0, NormalData);
        glTexCoordPointer(2,GL_FLOAT, 0, TexData);
        glPushMatrix();
        glDrawArrays(GL_QUAD_STRIP,0,12);
        glPopMatrix();
    }
}

/***********************************************************************/

VMfloat r2cover_caram( VMfloat y )
{
    VMfloat ampl;
    y=fabs(y);
    ampl=0.03;
    if(y<0.2){
        return( ampl*sin(2.0*M_PI*y/0.2*0.25) );
    }else if(y>=0.15 && y<0.5){
        return( ampl*(0.5+0.5*cos(4.0*M_PI*(y-0.15)/0.35*0.25)) );
    }else if(y>=0.5 && y<0.75){
        return( 0.7*ampl-0.7*ampl*cos(2.0*M_PI*(y-0.5)) );
    }else if(y>=0.75 && y<=1.0){
        return( 0.7*ampl-0.7*ampl*sin(3.7*M_PI*(y-0.75)) );
    }

    return( 0.0 );
}

/***********************************************************************/

void my_Cover2func( int segnr, VMfloat (*r1)(VMfloat), VMfloat (*r2)(VMfloat), VMfloat l, int order, VMfloat endtan )
/* edge shows to pos x,y direction*/
{
    int i;
    VMfloat y1,y2;
    VMfloat tan1, tan2;
    VMvect n1,n2,n3,n4,v1,v2,v3,v4,v5,v6,v7,v8, v4_2,v4_3, v8_2,v8_3;
    VMfloat r2_1,r2_2;
    VMfloat txfact=0.75;
    VMfloat tyfact=6.0;
    GLfloat VertexData[12*3];
    GLfloat NormalData[12*3];
    GLfloat TexData[12*2];
    n1 = vec_unit(vec_xyz(0,0,1));
    n2 = vec_unit(vec_xyz(FRAME_DH/(FRAME_D-BANDE_D-FRAME_PHASE),0,1));
    n3 = vec_unit(vec_xyz(FRAME_H-FRAME_PHASE,0,-FRAME_PHASE));
    n4 = vec_unit(vec_xyz(FRAME_H-FRAME_PHASE,0,-FRAME_PHASE));
    for(i=0;i<segnr;i++){
        if(0){
            y1 = -l/2.0*cos(M_PI*(VMfloat)i/(VMfloat)segnr);
            y2 = -l/2.0*cos(M_PI*(VMfloat)(i+1)/(VMfloat)segnr);
        } else {
            y1 = -l/2.0+l*(VMfloat)i/(VMfloat)segnr;
            y2 = -l/2.0+l*(VMfloat)(i+1)/(VMfloat)segnr;
        }
        tan1 = endtan*y1/(l/2.0);
        tan2 = endtan*y2/(l/2.0);
        r2_1 = FRAME_D-BANDE_D+r2(y1/l*2.0);
        r2_2 = FRAME_D-BANDE_D+r2(y2/l*2.0);
        v1 = vec_xyz(r1(y1/l*2.0),y1,FRAME_DH);
        v1.y += tan1 * v1.x;
        v2 = vec_xyz(r2_1-FRAME_PHASE,y1,0);
        v2.y += tan1 * v2.x;
        v3 = vec_xyz(r2_1,y1,-FRAME_PHASE);
        v3.y += tan1 * v3.x;
        v4 = vec_xyz(r2_1-FRAME_PHASE,y1,-FRAME_H);
        v4.y += tan1 * v4.x;
        v4_2 = vec_xyz(r2_1-FRAME_PHASE-1.0*FRAME_DW,y1,-FRAME_H);
        v4_2.y += tan1 * v4_2.x;
        v4_3 = vec_xyz(r2_1-FRAME_PHASE-2.0*FRAME_DW,y1,-FRAME_H2);
        v4_3.y += tan1 * v4_3.x;
        v5 = vec_xyz(r1(y2/l*2.0),y2,FRAME_DH);
        v5.y += tan2 * v5.x;
        v6 = vec_xyz(r2_2-FRAME_PHASE,y2,0);
        v6.y += tan2 * v6.x;
        v7 = vec_xyz(r2_2,y2,-FRAME_PHASE);
        v7.y += tan2 * v7.x;
        v8 = vec_xyz(r2_2-FRAME_PHASE,y2,-FRAME_H);
        v8.y += tan2 * v8.x;
        v8_2 = vec_xyz(r2_2-FRAME_PHASE-1.0*FRAME_DW,y2,-FRAME_H);
        v8_2.y += tan2 * v8_2.x;
        v8_3 = vec_xyz(r2_2-FRAME_PHASE-2.0*FRAME_DW,y2,-FRAME_H2);
        v8_3.y += tan2 * v8_3.x;
        if(!order){
            TexData[0] = v1.y*txfact; TexData[1] = v1.x*tyfact;
            NormalData[0] = n1.x; NormalData[1] = n1.y; NormalData[2] = n1.z;
            VertexData[0] = v1.x; VertexData[1] = v1.y; VertexData[2] = v1.z;
            TexData[2] = v5.y*txfact; TexData[3] = v5.x*tyfact;
            NormalData[3] = n1.x; NormalData[4] = n1.y; NormalData[5] = n1.z;
            VertexData[3] = v5.x; VertexData[4] = v5.y; VertexData[5] = v5.z;
            TexData[4] = v2.y*txfact; TexData[5] = v2.x*tyfact;
            NormalData[6] = n2.x; NormalData[7] = n2.y; NormalData[8] = n2.z;
            VertexData[6] = v2.x; VertexData[7] = v2.y; VertexData[8] = v2.z;
            TexData[6] = v6.y*txfact; TexData[7] = v6.x*tyfact;
            NormalData[9] = n2.x; NormalData[10] = n2.y; NormalData[11] = n2.z;
            VertexData[9] = v6.x; VertexData[10] = v6.y; VertexData[11] = v6.z;
            TexData[8] = v3.y*txfact; TexData[9] = v3.x*tyfact;
            NormalData[12] = n3.x; NormalData[13] = n3.y; NormalData[14] = n3.z;
            VertexData[12] = v3.x; VertexData[13] = v3.y; VertexData[14] = v3.z;
            TexData[10] = v7.y*txfact; TexData[11] = v7.x*tyfact;
            NormalData[15] = n3.x; NormalData[16] = n3.y; NormalData[17] = n3.z;
            VertexData[15] = v7.x; VertexData[16] = v7.y; VertexData[17] = v7.z;
            TexData[12] = v4.y*txfact; TexData[13] = v4.x*tyfact;
            NormalData[18] = n4.x; NormalData[19] = n4.y; NormalData[20] = n4.z;
            VertexData[18] = v4.x; VertexData[19] = v4.y; VertexData[20] = v4.z;
            TexData[14] = v8.y*txfact; TexData[15] = v8.x*tyfact;
            NormalData[21] = n4.x; NormalData[22] = n4.y; NormalData[23] = n4.z;
            VertexData[21] = v8.x; VertexData[22] = v8.y; VertexData[23] = v8.z;
            TexData[16] = v4_2.y*txfact; TexData[17] = v4_2.x*tyfact;
            NormalData[24] = n4.x; NormalData[25] = n4.y; NormalData[26] = n4.z;
            VertexData[24] = v4_2.x; VertexData[25] = v4_2.y; VertexData[26] = v4_2.z;
            TexData[18] = v8_2.y*txfact; TexData[19] = v8_2.x*tyfact;
            NormalData[27] = n4.x; NormalData[28] = n4.y; NormalData[29] = n4.z;
            VertexData[27] = v8_2.x; VertexData[28] = v8_2.y; VertexData[29] = v8_2.z;
            TexData[20] = v4_3.y*txfact; TexData[21] = v4_3.x*tyfact;
            NormalData[30] = n4.x; NormalData[31] = n4.y; NormalData[32] = n4.z;
            VertexData[30] = v4_3.x; VertexData[31] = v4_3.y; VertexData[32] = v4_3.z;
            TexData[22] = v8_3.y*txfact; TexData[23] = v8_3.x*tyfact;
            NormalData[33] = n4.x; NormalData[34] = n4.y; NormalData[35] = n4.z;
            VertexData[33] = v8_3.x; VertexData[34] = v8_3.y; VertexData[35] = v8_3.z;
        } else {
            TexData[0] = v5.y*txfact; TexData[1] = v5.x*tyfact;
            NormalData[0] = n1.x; NormalData[1] = n1.y; NormalData[2] = n1.z;
            VertexData[0] = v5.x; VertexData[1] = v5.y; VertexData[2] = v5.z;
            TexData[2] = v1.y*txfact; TexData[3] = v1.x*tyfact;
            NormalData[3] = n1.x; NormalData[4] = n1.y; NormalData[5] = n1.z;
            VertexData[3] = v1.x; VertexData[4] = v1.y; VertexData[5] = v1.z;
            TexData[4] = v6.y*txfact; TexData[5] = v6.x*tyfact;
            NormalData[6] = n2.x; NormalData[7] = n2.y; NormalData[8] = n2.z;
            VertexData[6] = v6.x; VertexData[7] = v6.y; VertexData[8] = v6.z;
            TexData[6] = v2.y*txfact; TexData[7] = v2.x*tyfact;
            NormalData[9] = n2.x; NormalData[10] = n2.y; NormalData[11] = n2.z;
            VertexData[9] = v2.x; VertexData[10] = v2.y; VertexData[11] = v2.z;
            TexData[8] = v7.y*txfact; TexData[9] = v7.x*tyfact;
            NormalData[12] = n3.x; NormalData[13] = n3.y; NormalData[14] = n3.z;
            VertexData[12] = v7.x; VertexData[13] = v7.y; VertexData[14] = v7.z;
            TexData[10] = v3.y*txfact; TexData[11] = v3.x*tyfact;
            NormalData[15] = v3.x; NormalData[16] = v3.y; NormalData[17] = v3.z;
            VertexData[15] = v3.x; VertexData[16] = v3.y; VertexData[17] = v3.z;
            TexData[12] = v8.y*txfact; TexData[13] = v8.x*tyfact;
            NormalData[18] = n4.x; NormalData[19] = n4.y; NormalData[20] = n4.z;
            VertexData[18] = v8.x; VertexData[19] = v8.y; VertexData[20] = v8.z;
            TexData[14] = v4.y*txfact; TexData[15] = v4.x*tyfact;
            NormalData[21] = n4.x; NormalData[22] = n4.y; NormalData[23] = n4.z;
            VertexData[21] = v4.x; VertexData[22] = v4.y; VertexData[23] = v4.z;
            TexData[16] = v8_2.y*txfact; TexData[17] = v8_2.x*tyfact;
            NormalData[24] = n4.x; NormalData[25] = n4.y; NormalData[26] = n4.z;
            VertexData[24] = v8_2.x; VertexData[25] = v8_2.y; VertexData[26] = v8_2.z;
            TexData[18] = v4_2.y*txfact; TexData[19] = v4_2.x*tyfact;
            NormalData[27] = n4.x; NormalData[28] = n4.y; NormalData[29] = n4.z;
            VertexData[27] = v4_2.x; VertexData[28] = v4_2.y; VertexData[29] = v4_2.z;
            TexData[20] = v8_3.y*txfact; TexData[21] = v8_3.x*tyfact;
            NormalData[30] = n4.x; NormalData[31] = n4.y; NormalData[32] = n4.z;
            VertexData[30] = v8_3.x; VertexData[31] = v8_3.y; VertexData[32] = v8_3.z;
            TexData[22] = v4_3.y*txfact; TexData[23] = v4_3.x*tyfact;
            NormalData[33] = n4.x; NormalData[34] = n4.y; NormalData[35] = n4.z;
            VertexData[33] = v4_3.x; VertexData[34] = v4_3.y; VertexData[35] = v4_3.z;
        }
        glVertexPointer(3, GL_FLOAT, 0, VertexData);
        glNormalPointer(GL_FLOAT, 0, NormalData);
        glTexCoordPointer(2,GL_FLOAT, 0, TexData);
        glDrawArrays(GL_QUAD_STRIP,0,12);
    }
}

/***********************************************************************/

VMfloat scalefunc01( VMfloat x )
{
    return(scalefunc((x+1.0)*M_PI/4.0));
}

/***********************************************************************/

void my_CoverBumper( int segnr, VMfloat (*r1)(VMfloat), VMfloat (*sc)(VMfloat), VMfloat l, int order )
/* edge shows to pos x,y direction*/
{
    int i;
    VMfloat y1,y2, dphi1, dphi2;
    VMvect v10,v20,v30,v40,n1,n2,n3,n4,n5,n6,n7,n8,n10,n20,n30,n40,v1,v2,v3,v4,v5,v6,v7,v8;
    GLfloat VertexData[8*3];
    GLfloat NormalData[8*3];

    n10 = vec_unit(vec_xyz(-1,0,-1));
    n20 = vec_unit(vec_xyz(-1,0,0));
    n30 = vec_unit(vec_xyz(0,0,1));
    n40 = vec_unit(vec_xyz(1,0,3));
    v10 = vec_xyz(0,0,-1);
    v20 = vec_xyz(-1,0,0);
    v30 = vec_xyz(0,0,1);
    v40 = vec_xyz(3,0,0);
    for(i=0;i<segnr;i++){
        y1 = -l*0.5*cos(M_PI*(VMfloat)i/(VMfloat)segnr);
        y2 = -l*0.5*cos(M_PI*(VMfloat)(i+1)/(VMfloat)segnr);
        dphi1 = atan( (r1((y1-0.001)/l*2.0)-r1((y1+0.001)/l*2.0))/0.002 );
        dphi2 = atan( (r1((y2-0.001)/l*2.0)-r1((y2+0.001)/l*2.0))/0.002 );
        n1 = vec_rotate(n10,vec_xyz(0,0,dphi1));
        n2 = vec_rotate(n20,vec_xyz(0,0,dphi1));
        n3 = vec_rotate(n30,vec_xyz(0,0,dphi1));
        n4 = vec_rotate(n40,vec_xyz(0,0,dphi1));
        n5 = vec_rotate(n10,vec_xyz(0,0,dphi2));
        n6 = vec_rotate(n20,vec_xyz(0,0,dphi2));
        n7 = vec_rotate(n30,vec_xyz(0,0,dphi2));
        n8 = vec_rotate(n40,vec_xyz(0,0,dphi2));
        v1 = vec_add(vec_scale(v10,sc(y1/l*2.0)),vec_xyz(r1(y1/l*2.0),y1,FRAME_DH));
        v2 = vec_add(vec_scale(v20,sc(y1/l*2.0)),vec_xyz(r1(y1/l*2.0),y1,FRAME_DH));
        v3 = vec_add(vec_scale(v30,sc(y1/l*2.0)),vec_xyz(r1(y1/l*2.0),y1,FRAME_DH));
        v4 = vec_add(vec_scale(v40,sc(y1/l*2.0)),vec_xyz(r1(y1/l*2.0),y1,FRAME_DH));
        v5 = vec_add(vec_scale(v10,sc(y2/l*2.0)),vec_xyz(r1(y2/l*2.0),y2,FRAME_DH));
        v6 = vec_add(vec_scale(v20,sc(y2/l*2.0)),vec_xyz(r1(y2/l*2.0),y2,FRAME_DH));
        v7 = vec_add(vec_scale(v30,sc(y2/l*2.0)),vec_xyz(r1(y2/l*2.0),y2,FRAME_DH));
        v8 = vec_add(vec_scale(v40,sc(y2/l*2.0)),vec_xyz(r1(y2/l*2.0),y2,FRAME_DH));
        if(!order){
            NormalData[0] = n1.x; NormalData[1] = n1.y; NormalData[2] = n1.z;
            VertexData[0] = v1.x; VertexData[1] = v1.y; VertexData[2] = v1.z;
            NormalData[3] = n5.x; NormalData[4] = n5.y; NormalData[5] = n5.z;
            VertexData[3] = v5.x; VertexData[4] = v5.y; VertexData[5] = v5.z;
            NormalData[6] = n2.x; NormalData[7] = n2.y; NormalData[8] = n2.z;
            VertexData[6] = v2.x; VertexData[7] = v2.y; VertexData[8] = v2.z;
            NormalData[9] = n6.x; NormalData[10] = n6.y; NormalData[11] = n6.z;
            VertexData[9] = v6.x; VertexData[10] = v6.y; VertexData[11] = v6.z;
            NormalData[12] = n3.x; NormalData[13] = n3.y; NormalData[14] = n3.z;
            VertexData[12] = v3.x; VertexData[13] = v3.y; VertexData[14] = v3.z;
            NormalData[15] = n7.x; NormalData[16] = n7.y; NormalData[17] = n7.z;
            VertexData[15] = v7.x; VertexData[16] = v7.y; VertexData[17] = v7.z;
            NormalData[18] = n4.x; NormalData[19] = n4.y; NormalData[20] = n4.z;
            VertexData[18] = v4.x; VertexData[19] = v4.y; VertexData[20] = v4.z;
            NormalData[21] = n8.x; NormalData[22] = n8.y; NormalData[23] = n8.z;
            VertexData[21] = v8.x; VertexData[22] = v8.y; VertexData[23] = v8.z;
        } else {
            NormalData[0] = n5.x; NormalData[1] = n5.y; NormalData[2] = n5.z;
            VertexData[0] = v5.x; VertexData[1] = v5.y; VertexData[2] = v5.z;
            NormalData[3] = n1.x; NormalData[4] = n1.y; NormalData[5] = n1.z;
            VertexData[3] = v1.x; VertexData[4] = v1.y; VertexData[5] = v1.z;
            NormalData[6] = n6.x; NormalData[7] = n6.y; NormalData[8] = n6.z;
            VertexData[6] = v6.x; VertexData[7] = v6.y; VertexData[8] = v6.z;
            NormalData[9] = n2.x; NormalData[10] = n2.y; NormalData[11] = n2.z;
            VertexData[9] = v2.x; VertexData[10] = v2.y; VertexData[11] = v2.z;
            NormalData[12] = n7.x; NormalData[13] = n7.y; NormalData[14] = n7.z;
            VertexData[12] = v7.x; VertexData[13] = v7.y; VertexData[14] = v7.z;
            NormalData[15] = n3.x; NormalData[16] = n3.y; NormalData[17] = n3.z;
            VertexData[15] = v3.x; VertexData[16] = v3.y; VertexData[17] = v3.z;
            NormalData[18] = n8.x; NormalData[19] = n8.y; NormalData[20] = n8.z;
            VertexData[18] = v8.x; VertexData[19] = v8.y; VertexData[20] = v8.z;
            NormalData[21] = n4.x; NormalData[22] = n4.y; NormalData[23] = n4.z;
            VertexData[21] = v4.x; VertexData[22] = v4.y; VertexData[23] = v4.z;
        }
        glVertexPointer(3, GL_FLOAT, 0, VertexData);
        glNormalPointer(GL_FLOAT, 0, NormalData);
        glDrawArrays(GL_QUAD_STRIP,0,8);
     /* for debugging
        glBegin(GL_LINES);
            glVertex3f(v1.x,v1.y,v1.z); v1=vec_add(v1,vec_scale(n1,0.01)); glVertex3f(v1.x,v1.y,v1.z);
            glVertex3f(v2.x,v2.y,v2.z); v2=vec_add(v2,vec_scale(n2,0.01)); glVertex3f(v2.x,v2.y,v2.z);
            glVertex3f(v3.x,v3.y,v3.z); v3=vec_add(v3,vec_scale(n3,0.01)); glVertex3f(v3.x,v3.y,v3.z);
            glVertex3f(v4.x,v4.y,v4.z); v4=vec_add(v4,vec_scale(n4,0.01)); glVertex3f(v4.x,v4.y,v4.z);
        glEnd(); */
    }
}

/***********************************************************************/

void my_Bande( VMfloat x1, VMfloat y1, VMfloat z1,    /* inside up */
               VMfloat x2, VMfloat y2, VMfloat z2,    /* outside down */
               VMfloat tan1, VMfloat tan2, int order )
{/*   ^ y                        *
  *   |                          *
  *   .z -> x                    *
  *                              *
  *   4  8               9  7    *
  * 0 \                     / 3  *
  *     \5_______________6/      *
  *     1                 2      */
    VMfloat dummy,sin1,sin2,cos1,cos2,r1,r2;
    VMvect p[12];
    GLfloat VertexData[75];
    GLfloat NormalData[75];

#define BANDE_WULST 0.006
    fprintf(stderr,"Call to build a table border\n");
    p[0]=vec_xyz(x1-(y2-y1)*tan1,y2,z1+FRAME_DH);
    p[1]=vec_xyz(x1,y1,z1-BANDE_WULST/2.0);
    p[2]=vec_xyz(x2,y1,z1-BANDE_WULST/2.0);
    p[3]=vec_xyz(x2+(y2-y1)*tan2,y2,z1+FRAME_DH);
    p[4]=vec_xyz(x1-(y2-y1)*tan1,y2,z2);
    p[5]=vec_xyz(x1-(y2-y1)*BANDE_D2RATIO*tan1,y1+(y2-y1)*BANDE_D2RATIO,z2);
    p[6]=vec_xyz(x2+(y2-y1)*BANDE_D2RATIO*tan2,y1+(y2-y1)*BANDE_D2RATIO,z2);
    p[7]=vec_xyz(x2+(y2-y1)*tan2,y2,z2);
    p[8]=vec_xyz(x1,y2,z1+FRAME_DH);
    p[9]=vec_xyz(x2,y2,z1+FRAME_DH);
    p[10]=vec_xyz(x1,y1,z1+BANDE_WULST/2.0);
    p[11]=vec_xyz(x2,y1,z1+BANDE_WULST/2.0);

    if( x1 > x2 ){ dummy=x1; x1=x2; x2=dummy; }
    if( y1 > y2 ){ dummy=y1; y1=y2; y2=dummy; }
    if( z1 > z2 ){ dummy=z1; z1=z2; z2=dummy; }

    r1=sqrt((FRAME_DH-BANDE_WULST)*(FRAME_DH-BANDE_WULST)+(y2-y1)*(y2-y1));
    sin1=fabs((FRAME_DH-BANDE_WULST)/r1);
    cos1=fabs((y2-y1)/r1);

    r2=sqrt((y2-y1)*BANDE_D2RATIO*(y2-y1)*BANDE_D2RATIO+(z1-z2)*(z1-z2));
    sin2=fabs((y2-y1)*BANDE_D2RATIO/r2);
    cos2=fabs((z1-z2)/r2);
       autonormalize_quad(p[10],p[11],p[9],p[8],order,&VertexData[0],&NormalData[0]);
       autonormalize_quad(p[5],p[6],p[2],p[1],order,&VertexData[12],&NormalData[12]);
       if(!order){
           NormalData[24] = 0.0; NormalData[25] = -sin1; NormalData[26] = cos1;
           VertexData[24] = p[10].x; VertexData[25] = p[10].y; VertexData[26] = p[10].z;
           NormalData[27] = 0.0; NormalData[28] = -sin1; NormalData[29] = cos1;
           VertexData[27] = p[11].x; VertexData[28] = p[11].y; VertexData[29] = p[11].z;
           NormalData[30] = 0.0; NormalData[31] = -cos2; NormalData[32] = -sin2;
           VertexData[30] = p[2].x; VertexData[31] = p[2].y; VertexData[32] = p[2].z;
           NormalData[33] = 0.0; NormalData[34] = -cos2; NormalData[35] = -sin2;
           VertexData[33] = p[1].x; VertexData[34] = p[1].y; VertexData[35] = p[1].z;
       }else{
           NormalData[24] = 0.0; NormalData[25] = -cos2; NormalData[26] = -sin2;
           VertexData[24] = p[1].x; VertexData[25] = p[1].y; VertexData[26] = p[1].z;
           NormalData[27] = 0.0; NormalData[28] = -cos2; NormalData[29] = -sin2;
           VertexData[27] = p[2].x; VertexData[28] = p[2].y; VertexData[29] = p[2].z;
           NormalData[30] = 0.0; NormalData[31] = -sin1; NormalData[32] = cos1;
           VertexData[30] = p[11].x; VertexData[31] = p[11].y; VertexData[32] = p[11].z;
           NormalData[33] = 0.0; NormalData[34] = -sin1; NormalData[35] = cos1;
           VertexData[33] = p[10].x; VertexData[34] = p[10].y; VertexData[35] = p[10].z;
       }
    glVertexPointer(3, GL_FLOAT, 0, VertexData);
    glNormalPointer(GL_FLOAT, 0, NormalData);
    glPushMatrix();
    glDrawArrays(GL_QUADS,0,12);

    autonormalize_triangle(p[1],p[10],p[0],order,&VertexData[0],&NormalData[0]);
    autonormalize_triangle(p[2],p[3],p[11],order,&VertexData[9],&NormalData[9]);
    autonormalize_triangle(p[0],p[10],p[8],order,&VertexData[18],&NormalData[18]);
    autonormalize_triangle(p[0],p[5],p[1],order,&VertexData[27],&NormalData[27]);
    autonormalize_triangle(p[0],p[4],p[5],order,&VertexData[36],&NormalData[36]);
    autonormalize_triangle(p[3],p[9],p[11],order,&VertexData[45],&NormalData[45]);
    autonormalize_triangle(p[3],p[2],p[6],order,&VertexData[54],&NormalData[54]);
    autonormalize_triangle(p[3],p[6],p[7],order,&VertexData[63],&NormalData[63]);
    glDrawArrays(GL_TRIANGLES,0,8*3);
    glPopMatrix();
}

/***********************************************************************/

#define TABLETEXCOORD_X(x,y) (-0.7+(y+TABLE_L/2.0)/TABLE_L*2.4-0.2+(x+TABLE_W/2.0)/TABLE_W*0.4)
#define TABLETEXCOORD_Y(x,y) (-0.2+(x+TABLE_W/2.0)/TABLE_W*1.4)

   /* holes-tuch */
void my_HoleTuch( int xfact, int yfact )
{
#define  HOLE1_SEGNR_4 8

       int i,j,k,l;
       VMfloat x,y,phi;
       VMfloat edge_xyoffs;
       edge_xyoffs = HOLE1_R*SQR2-BANDE_D;
       GLfloat VertexData[(HOLE1_SEGNR_4+3)*3*2*2];
       GLfloat NormalData[(HOLE1_SEGNR_4+3)*3*2*2];
       GLfloat TexData[(HOLE1_SEGNR_4+3)*2*2*2];

       //glEnableClientState are defined from the calling function

       NormalData[0] = 0.0; NormalData[1] = 0.0; NormalData[2] = 1.0;
       NormalData[3] = 0.0; NormalData[4] = 0.0; NormalData[5] = 1.0;
       NormalData[6] = 0.0; NormalData[7] = 0.0; NormalData[8] = 1.0;
       NormalData[9] = 0.0; NormalData[10] = 0.0; NormalData[11] = 1.0;
       NormalData[12] = 0.0; NormalData[13] = 0.0; NormalData[14] = 1.0;
       NormalData[15] = 0.0; NormalData[16] = 0.0; NormalData[17] = 1.0;
       x=-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2; y=-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2;
       x*=xfact; y*=yfact;
       TexData[0] = TABLETEXCOORD_X(x,y); TexData[1] = TABLETEXCOORD_Y(x,y);
       VertexData[0] = x; VertexData[1] = y; VertexData[2] = -BALL_D/2.0;
       x=-TABLE_W/2.0-BANDE_D; y=-TABLE_L/2.0+edge_xyoffs;
       x*=xfact; y*=yfact;
       TexData[2] = TABLETEXCOORD_X(x,y); TexData[3] = TABLETEXCOORD_Y(x,y);
       VertexData[3] = x; VertexData[4] = y; VertexData[5] = -BALL_D/2.0;
       x=-TABLE_W/2.0-BANDE_D*BANDE_D2RATIO; y=-TABLE_L/2.0+edge_xyoffs+BANDE_D*HOLE1_TAN;
       x*=xfact; y*=yfact;
       TexData[4] = TABLETEXCOORD_X(x,y); TexData[5] = TABLETEXCOORD_Y(x,y);
       VertexData[6] = x; VertexData[7] = y; VertexData[8] = -BALL_D/2.0;
       x=-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2; y=-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2;
       x*=xfact; y*=yfact;
       TexData[6] = TABLETEXCOORD_X(x,y); TexData[7] = TABLETEXCOORD_Y(x,y);
       VertexData[9] = x; VertexData[10] = y; VertexData[11] = -BALL_D/2.0;
       x=-TABLE_W/2.0+edge_xyoffs+BANDE_D*HOLE1_TAN; y=-TABLE_L/2.0-BANDE_D*BANDE_D2RATIO;
       x*=xfact; y*=yfact;
       TexData[8] = TABLETEXCOORD_X(x,y); TexData[9] = TABLETEXCOORD_Y(x,y);
       VertexData[12] = x; VertexData[13] = y; VertexData[14] = -BALL_D/2.0;
       x=-TABLE_W/2.0+edge_xyoffs; y=-TABLE_L/2.0-BANDE_D;
       x*=xfact; y*=yfact;
       TexData[10] = TABLETEXCOORD_X(x,y); TexData[11] = TABLETEXCOORD_Y(x,y);
       VertexData[15] = x; VertexData[16] = y; VertexData[17] = -BALL_D/2.0;
       glTexCoordPointer(2,GL_FLOAT, 0, TexData);
       glVertexPointer(3, GL_FLOAT, 0, VertexData);
       glNormalPointer(GL_FLOAT, 0, NormalData);
       glPushMatrix();
       glDrawArrays(GL_TRIANGLES,0,6);
       glPopMatrix();

       x=-TABLE_W/2.0-BANDE_D; y=-TABLE_L/2.0+edge_xyoffs;
       x*=xfact; y*=yfact;
       j = 0;
       k = 0;
       TexData[j++] = TABLETEXCOORD_X(x,y); TexData[j++] = TABLETEXCOORD_Y(x,y);
       VertexData[k++] = x; VertexData[k++] = y; VertexData[k++] = -BALL_D/2.0;
       for(i=0;i<HOLE1_SEGNR_4+1;i++){
           phi=M_PI/4.0+(VMfloat)i*M_PI/HOLE1_SEGNR_4/2.0;
           x=-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R*cos(phi);
           y=-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R*sin(phi);
           x*=xfact; y*=yfact;
           TexData[j++] = TABLETEXCOORD_X(x,y); TexData[j++] = TABLETEXCOORD_Y(x,y);
           VertexData[k++] = x; VertexData[k++] = y; VertexData[k++] = -BALL_D/2.0;
       }
       // glnormal like before
       glPushMatrix();
       glDrawArrays(GL_TRIANGLE_FAN,0,HOLE1_SEGNR_4+1);
       glPopMatrix();

       j = 0;
       k = 0;
       x=-TABLE_W/2.0+edge_xyoffs; y=-TABLE_L/2.0-BANDE_D;
       x*=xfact; y*=yfact;
       TexData[j++] = TABLETEXCOORD_X(x,y); TexData[j++] = TABLETEXCOORD_Y(x,y);
       VertexData[k++] = x; VertexData[k++] = y; VertexData[k++] = -BALL_D/2.0;
       for(i=HOLE1_SEGNR_4;i>=0;i--){
           phi=M_PI/4.0+(VMfloat)i*M_PI/HOLE1_SEGNR_4/2.0;
           x=-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R*sin(phi);
           y=-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R*cos(phi);
           x*=xfact; y*=yfact;
           TexData[j++] = TABLETEXCOORD_X(x,y); TexData[j++] = TABLETEXCOORD_Y(x,y);
           VertexData[k++] = x; VertexData[k++] = y; VertexData[k++] = -BALL_D/2.0;
       }
       // glnormal like before
       glPushMatrix();
       glDrawArrays(GL_TRIANGLE_FAN,0,HOLE1_SEGNR_4+1);
       glPopMatrix();

       j = 0;
       k = 0;
       l = 0;
       for(i=-HOLE1_SEGNR_4;i<HOLE1_SEGNR_4+1;i++){
           phi=M_PI/4.0+(VMfloat)i*M_PI/HOLE1_SEGNR_4/2.0;
           x=-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R*cos(phi);
           y=-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R*sin(phi);
           x*=xfact; y*=yfact;
           TexData[j++] = TABLETEXCOORD_X(x,y); TexData[j++] = TABLETEXCOORD_Y(x,y);
           NormalData[k++] = 0.0; NormalData[k++] = 0.0; NormalData[k++] = 1.0;
           VertexData[l++] = x; VertexData[l++] = y; VertexData[l++] = -BALL_D/2.0;
           x=-TABLE_W/2.0-HOLE1_XYOFFS+(HOLE1_R-0.005)*cos(phi);
           y=-TABLE_L/2.0-HOLE1_XYOFFS+(HOLE1_R-0.005)*sin(phi);
           x*=xfact; y*=yfact;
           TexData[j++] = TABLETEXCOORD_X(x,y); TexData[j++] = TABLETEXCOORD_Y(x,y);
           NormalData[k++] = -cos(phi)*xfact; NormalData[k++] = -sin(phi)*yfact; NormalData[k++] = 0.0;
           VertexData[l++] = x; VertexData[l++] = y; VertexData[l++] = -BALL_D/2.0-0.005;
       }
       glPushMatrix();
       glDrawArrays(GL_QUAD_STRIP,0,1+HOLE1_SEGNR_4*2*2);
       glPopMatrix();
       //glDisableClientState are defined inside the calling function
}

/***********************************************************************
 *            grays out a color - for use with stereo view             *
 ***********************************************************************/

void grayen_color( GLfloat * col )
{
    GLfloat c;
    c=(col[0]+col[1]+col[2])/3.0;
    col[0]=c;
    col[1]=c;
    col[2]=c;
}

/***********************************************************************/

int create_table( int reflect_bind, BordersType *borders, int carambol ) {
	   // parameter borders only used for debugging at the end of the function
	   // no error and not much time to work with it. Don't optimize this

	   //VMfloat cm  =  0.01;  // cm (for debugging in function my_glBox)

    static int bumpref_init = 0;
    static int bump_init = 0;
    static BumpRefType bumpref;
    static BumpRefType bumponly;
    static int table_obj=-1;
    static GLuint frametexbind=-1;
    static GLuint tabletexbind=-1;
    static GLuint clothtexbind=-1;
    int flip;
    VMfloat balld  = BALL_D;
    VMfloat tablew = TABLE_W;
    VMfloat tablel = TABLE_L;
    VMfloat tableh = TABLE_H;
    VMfloat bande_d = BANDE_D;
    VMfloat hole_r1,hole_r2, edge_xyoffs;
    VMfloat x,y,fx,fy,xf,yf,phi;
    VMfloat lwood, woodpos;
    GLfloat tab_col_spec[4] = {0.0, 0.0, 0.0, 0.0};
    GLfloat tab_col_diff[4] = {0.05, 0.4, 0.13, 1.0};
    GLfloat tab_col_amb [4];
    GLfloat wood_col_spec[4] = {0.7, 0.7, 0.7, 1.0};
    GLfloat wood_col_spec_null[4] = {0.0, 0.0, 0.0, 1.0};
    GLfloat wood_col_diff[4] = {0.25, 0.08, 0.02, 1.0};
    GLfloat wood_col_diff2[4] = {0.7, 0.7, 0.7, 1.0};
    GLfloat wood_col_diff3[4] = {0.07, 0.07, 0.07, 1.0};
    GLfloat wood_col_amb  [4];
    GLfloat wood_col_amb2 [4];
    GLfloat wood_col_amb3 [4];
    GLfloat wood_col_shin = 100.0;
    GLfloat dia_col_spec[4] = {0.5, 0.5, 0.5, 1.0};
    GLfloat dia_col_diff[4] = {0.8, 0.7, 0.1, 1.0};
    GLfloat dia_col_amb [4];
    GLfloat dia_col_shin = 100.0;
    GLfloat bumpers_col_spec[4] = {1.0, 1.0, 1.0, 1.0};
    GLfloat bumpers_col_diff[4] = {0.3, 0.3, 0.3, 1.0};
    GLfloat bumpers_col_amb [4];
    GLfloat bumpers_col_shin = 100.0;
    GLfloat hole_col_spec[4] = {0.6, 0.6, 0.6, 0.6};
    GLfloat hole_col_diff[4] = {0.2, 0.2, 0.2, 1.0};
    GLfloat hole_col_amb [4];
    GLfloat hole_col_shin = 1000.0;
    GLfloat vx[4]={0,50,60,0};
    GLfloat vy[4]={50,0,60,0};

    int imax,jmax,i,j,k,l,m,n,o;
    VMfloat area_w,area_l;  /* the main area */

    edge_xyoffs = HOLE1_R*SQR2-BANDE_D;

    tab_col_diff[0]=(VMfloat)((options_table_color>>16) & 0xFF)/255.0;
    tab_col_diff[1]=(VMfloat)((options_table_color>> 8) & 0xFF)/255.0;
    tab_col_diff[2]=(VMfloat)((options_table_color>> 0) & 0xFF)/255.0;
    tab_col_diff[3]=1.0-(VMfloat)((options_table_color>>24) & 0xFF)/255.0;

    dia_col_diff[0]=(VMfloat)((options_diamond_color>>16) & 0xFF)/255.0;
    dia_col_diff[1]=(VMfloat)((options_diamond_color>> 8) & 0xFF)/255.0;
    dia_col_diff[2]=(VMfloat)((options_diamond_color>> 0) & 0xFF)/255.0;
    dia_col_diff[3]=1.0-(VMfloat)((options_diamond_color>>24) & 0xFF)/255.0;

    wood_col_diff[0]=(VMfloat)((options_frame_color>>16) & 0xFF)/255.0;
    wood_col_diff[1]=(VMfloat)((options_frame_color>> 8) & 0xFF)/255.0;
    wood_col_diff[2]=(VMfloat)((options_frame_color>> 0) & 0xFF)/255.0;
    wood_col_diff[3]=1.0-(VMfloat)((options_frame_color>>24) & 0xFF)/255.0;

    if(options_rgstereo_on){
        tab_col_diff[0]=0.533;
        tab_col_diff[1]=0.533;
        tab_col_diff[2]=0.533;
        grayen_color(dia_col_diff);
    }

    fprintf(stderr,"Initialize billiard table-frame\n");
    if( frametexbind > 0 ) glDeleteTextures( 1, &frametexbind );
    create_png_texbind("table-frame.png", &frametexbind, 3, GL_RGB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    fprintf(stderr,"Initialize table-texture\n");
    if( tabletexbind > 0 ) glDeleteTextures( 1, &tabletexbind );
#ifdef WETAB
    create_png_texbind("tabletex_wetab_256x256.png", &tabletexbind, 1, GL_LUMINANCE);
#else
    create_png_texbind("tabletex_fB_256x256.png", &tabletexbind, 1, GL_LUMINANCE);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    fprintf(stderr,"Initialize clothing table texture\n");
    if( clothtexbind > 0 ) glDeleteTextures( 1, &clothtexbind );
    create_png_texbind("cloth.png", &clothtexbind, 1, GL_LUMINANCE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    for(i=0;i<3;i++) {
       tab_col_amb[i]=tab_col_diff[i]*0.5;
       dia_col_amb[i]=dia_col_diff[i]*0.5;
       bumpers_col_amb[i]=bumpers_col_diff[i]*0.5;
       hole_col_amb[i]=hole_col_diff[i]*0.5;
       wood_col_amb[i]=wood_col_diff[i]*0.5;
       wood_col_amb2[i]=wood_col_diff2[i]*0.43;
       wood_col_amb3[i]=0.0;
       //dia_col_spec[i]=dia_col_diff[i]*1.0;
       }

   /* initialize bumpref setup */
   if(!carambol){
   if( options_bumpref        &&
       extension_multitexture &&
       extension_cubemap      &&
       !bumpref_init )
   {
       bumpref = bumpref_setup_vp_ts_rc(
                              "bumpref.png", 0.008,  /* bump map */
                              "posx.png", "posy.png", "posz.png", "negx.png", "negy.png", "negz.png", /* cube map */
                              0.00001f  /* z-shift */
                             );
       bumpref_init = 1;
   }
   }

   if(options_bumpwood       &&
      extension_multitexture &&
      extension_rc_NV        &&
      extension_vp_NV        &&
      !bump_init ){
       fprintf(stderr,"setting up wood frame bumpmaps\n");
       bumponly = bump_setup_vp_rc("cloth-col.png",0.007,0);
       //bumponly = bump_setup_vp_rc("table-frame.png",0.007,0);
       bump_init = 1;
   }

   if( table_obj != -1 ) glDeleteLists( table_obj, 1 );
   fprintf(stderr,"Initialize new table GL object\n");
   table_obj = glGenLists(1);
   glNewList(table_obj, GL_COMPILE);
   glShadeModel(GL_SMOOTH);
   glMaterialfv(GL_FRONT, GL_DIFFUSE,   tab_col_diff);
   glMaterialfv(GL_FRONT, GL_AMBIENT,   tab_col_amb);
   glMaterialfv(GL_FRONT, GL_SPECULAR,  tab_col_spec);
   glMaterialf (GL_FRONT, GL_SHININESS, 0.0 );

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,tabletexbind);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   if( extension_multitexture ){
       glActiveTextureARB(GL_TEXTURE1_ARB);
       glEnable(GL_TEXTURE_2D);
       glEnable(GL_TEXTURE_GEN_S);
       glEnable(GL_TEXTURE_GEN_T);
       glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
       glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
       glTexGenfv(GL_S, GL_OBJECT_PLANE, vx);
       glTexGenfv(GL_T, GL_OBJECT_PLANE, vy);
       glBindTexture(GL_TEXTURE_2D,clothtexbind);
       glActiveTextureARB(GL_TEXTURE0_ARB);
   }
   //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   if(carambol){
       //tablew=TABLE_W+2.0*BANDE_D;
       //tablel=TABLE_L+2.0*BANDE_D;
       // see some lines above. Is this right here?
       tablew=TABLE_W;
       tablel=TABLE_L;
       area_w = TABLE_W+2.0*BANDE_D;
       area_l = TABLE_L+2.0*BANDE_D;
   } else {
       area_w = TABLE_W-0.07;
       area_l = TABLE_L-0.07;
   }

/* AREA_SUBDIV_Y and AREA_SUBDIV_X have to be even */
#define AREA_SUBDIV_Y 6
#define AREA_SUBDIV_X 4
   GLfloat TexData[(AREA_SUBDIV_Y*AREA_SUBDIV_X+3)*2*4];
   GLfloat VertexData[(AREA_SUBDIV_Y*AREA_SUBDIV_X+3)*3*4];
   GLfloat NormalData[(AREA_SUBDIV_Y*AREA_SUBDIV_X+3)*3*4];

   jmax=AREA_SUBDIV_Y; imax=AREA_SUBDIV_X;
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2,GL_FLOAT, 0, TexData);
   glVertexPointer(3, GL_FLOAT, 0, VertexData);
   glNormalPointer(GL_FLOAT, 0, NormalData);

   m = 0;
   n = 0;
   o = 0;
   glPushMatrix();
   for(j=0;j<jmax;j++){
       for(i=0;i<imax;i++){
           VMfloat x,y;
           x=-area_w/2.0+i*area_w/imax; y=-area_l/2.0+j*area_l/jmax;
           TexData[m++] = TABLETEXCOORD_X(x,y); TexData[m++] = TABLETEXCOORD_Y(x,y);
           NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = 1.0;
           NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = 1.0;
           NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = 1.0;
           NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = 1.0;
           VertexData[o++] = x; VertexData[o++] = y; VertexData[o++] = -balld/2.0;
           x=-area_w/2.0+i*area_w/imax; y=-area_l/2.0+(j+1)*area_l/jmax;
           TexData[m++] = TABLETEXCOORD_X(x,y); TexData[m++] = TABLETEXCOORD_Y(x,y);
           VertexData[o++] = x; VertexData[o++] = y; VertexData[o++] = -balld/2.0;
           x=-area_w/2.0+(i+1)*area_w/imax; y=-area_l/2.0+(j+1)*area_l/jmax;
           TexData[m++] = TABLETEXCOORD_X(x,y); TexData[m++] = TABLETEXCOORD_Y(x,y);
           VertexData[o++] = x; VertexData[o++] = y; VertexData[o++] = -balld/2.0;
           x=-area_w/2.0+(i+1)*area_w/imax; y=-area_l/2.0+j*area_l/jmax;
           TexData[m++] = TABLETEXCOORD_X(x,y); TexData[m++] = TABLETEXCOORD_Y(x,y);
           VertexData[o++] = x; VertexData[o++] = y; VertexData[o++] = -balld/2.0;
       }
   }
   glDrawArrays(GL_QUADS,0,4*AREA_SUBDIV_Y*AREA_SUBDIV_X);
   glPopMatrix();
   // the following lines are wrong here? set this a few lines before, because can't find a reason that it's here
   //if(carambol){
   //    tablew=TABLE_W;
   //    tablel=TABLE_L;
   //}

   if(!carambol){
       /* holes */
       glFrontFace(GL_CW);
       my_HoleTuch(1.0,1.0);

       glFrontFace(GL_CCW);
       my_HoleTuch(1.0,-1.0);

       glFrontFace(GL_CCW);
       my_HoleTuch(-1.0,1.0);

       glFrontFace(GL_CW);
       my_HoleTuch(-1.0,-1.0);
   }

   /* furchen */
	  fprintf(stderr,"Generate furrow object\n");
   if(!carambol){
   /*lower, upper*/
       fprintf(stderr,"Generate lower upper furrow object\n");
       //glnormal array are well defined before
       glTexCoordPointer(2,GL_FLOAT, 0, TexData);
       glVertexPointer(3, GL_FLOAT, 0, VertexData);
       glNormalPointer(GL_FLOAT, 0, NormalData);
       for(i=0;i<2;i++){
           m = 0;
           o = 0;
           glFrontFace(i==0?GL_CW:GL_CCW);
           TexData[m++] = TABLETEXCOORD_X((area_w/2.0)*(1.0),(-area_l/2.0)*(i==0?1.0:-1.0));
           TexData[m++] = TABLETEXCOORD_Y((area_w/2.0)*(1.0),(-area_l/2.0)*(i==0?1.0:-1.0));
           VertexData[o++] = (area_w/2.0)*(1.0); VertexData[o++] = (-area_l/2.0)*(i==0?1.0:-1.0); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((TABLE_W/2.0-edge_xyoffs-BANDE_D*HOLE1_TAN)*(1.0),(-TABLE_L/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0));
           TexData[m++] = TABLETEXCOORD_Y((TABLE_W/2.0-edge_xyoffs-BANDE_D*HOLE1_TAN)*(1.0),(-TABLE_L/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0));
           VertexData[o++] = (TABLE_W/2.0-edge_xyoffs-BANDE_D*HOLE1_TAN)*(1.0); VertexData[o++] = (-TABLE_L/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0); VertexData[o++] = -BALL_D/2.0;

           for(j=0;j<AREA_SUBDIV_X-1;j++){
               TexData[m++] = TABLETEXCOORD_X((area_w/2.0-area_w*(j+1)/AREA_SUBDIV_X)*(1.0),(-area_l/2.0)*(i==0?1.0:-1.0));
               TexData[m++] = TABLETEXCOORD_Y((area_w/2.0-area_w*(j+1)/AREA_SUBDIV_X)*(1.0),(-area_l/2.0)*(i==0?1.0:-1.0));
               VertexData[o++] = (area_w/2.0-area_w*(j+1)/AREA_SUBDIV_X)*(1.0); VertexData[o++] = (-area_l/2.0)*(i==0?1.0:-1.0); VertexData[o++] = -BALL_D/2.0;
               TexData[m++] = TABLETEXCOORD_X((area_w/2.0-area_w*(j+1)/AREA_SUBDIV_X)*(1.0),(-TABLE_L/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0));
               TexData[m++] = TABLETEXCOORD_Y((area_w/2.0-area_w*(j+1)/AREA_SUBDIV_X)*(1.0),(-TABLE_L/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0));
               VertexData[o++] = (area_w/2.0-area_w*(j+1)/AREA_SUBDIV_X)*(1.0); VertexData[o++] = (-TABLE_L/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0); VertexData[o++] = -BALL_D/2.0;
           }
           TexData[m++] = TABLETEXCOORD_X((area_w/2.0)*(-1.0),(-area_l/2.0)*(i==0?1.0:-1.0));
           TexData[m++] = TABLETEXCOORD_Y((area_w/2.0)*(-1.0),(-area_l/2.0)*(i==0?1.0:-1.0));
           VertexData[o++] = (area_w/2.0)*(-1.0); VertexData[o++] = (-area_l/2.0)*(i==0?1.0:-1.0); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((TABLE_W/2.0-edge_xyoffs-BANDE_D*HOLE1_TAN)*(-1.0),(-TABLE_L/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0));
           TexData[m++] = TABLETEXCOORD_Y((TABLE_W/2.0-edge_xyoffs-BANDE_D*HOLE1_TAN)*(-1.0),(-TABLE_L/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0));
           VertexData[o++] = (TABLE_W/2.0-edge_xyoffs-BANDE_D*HOLE1_TAN)*(-1.0); VertexData[o++] = (-TABLE_L/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0); VertexData[o++] = -BALL_D/2.0;
           glPushMatrix();
           glDrawArrays(GL_QUAD_STRIP,0,(4+(AREA_SUBDIV_X*2-1)));
           glPopMatrix();
       }
       /* some middle pocket triangles */
       fprintf(stderr,"Generate some middle pocket triangles object\n");
       for(i=0;i<2;i++){
           m = 0;
           o = 0;
           glFrontFace(i==0?GL_CW:GL_CCW);
           //glnormal array are well defined before
           TexData[m++] = TABLETEXCOORD_X((-area_w/2.0)*(i==0?1.0:-1.0),(0.0)*(1.0));
           TexData[m++] = TABLETEXCOORD_Y((-area_w/2.0)*(i==0?1.0:-1.0),(0.0)*(1.0));
           VertexData[o++] = (-area_w/2.0)*(i==0?1.0:-1.0); VertexData[o++] = (0.0)*(1.0); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((-TABLE_W/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0),(-HOLE2_R-HOLE2_TAN*BANDE_D)*(1.0));
           TexData[m++] = TABLETEXCOORD_Y((-TABLE_W/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0),(-HOLE2_R-HOLE2_TAN*BANDE_D)*(1.0));
           VertexData[o++] = (-TABLE_W/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0); VertexData[o++] = (-HOLE2_R-HOLE2_TAN*BANDE_D)*(1.0); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R)*(i==0?1.0:-1.0),(0.0)*(1.0));
           TexData[m++] = TABLETEXCOORD_Y((-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R)*(i==0?1.0:-1.0),(0.0)*(1.0));
           VertexData[o++] = (-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R)*(i==0?1.0:-1.0); VertexData[o++] = (0.0)*(1.0); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((-area_w/2.0)*(i==0?1.0:-1.0),(0.0)*(1.0));
           TexData[m++] = TABLETEXCOORD_Y((-area_w/2.0)*(i==0?1.0:-1.0),(0.0)*(1.0));
           VertexData[o++] = (-area_w/2.0)*(i==0?1.0:-1.0); VertexData[o++] = (0.0)*(1.0); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R)*(i==0?1.0:-1.0),(0.0)*(1.0));
           TexData[m++] = TABLETEXCOORD_Y((-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R)*(i==0?1.0:-1.0),(0.0)*(1.0));
           VertexData[o++] = (-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R)*(i==0?1.0:-1.0); VertexData[o++] = (0.0)*(1.0); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((-TABLE_W/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0),(+HOLE2_R+HOLE2_TAN*BANDE_D)*(1.0));
           TexData[m++] = TABLETEXCOORD_Y((-TABLE_W/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0),(+HOLE2_R+HOLE2_TAN*BANDE_D)*(1.0));
           VertexData[o++] = (-TABLE_W/2.0-BANDE_D*BANDE_D2RATIO)*(i==0?1.0:-1.0); VertexData[o++] = (+HOLE2_R+HOLE2_TAN*BANDE_D)*(1.0); VertexData[o++] = -BALL_D/2.0;
           glPushMatrix();
           glDrawArrays(GL_TRIANGLES,0,6);
           glPopMatrix();
       }

       /* lower left, lower right, upper right, upper left */
       fprintf(stderr,"Generate lower left, lower right, upper right, upper left pocket triangles object\n");
       for(i=0;i<4;i++){
           fx=0.0;
           fy=0.0;
           m = 0;
           o = 0;
           switch(i){
           case 0: fx=+1.0; fy=+1.0; glFrontFace(GL_CW);  break;  /* lower left  */
           case 1: fx=-1.0; fy=+1.0; glFrontFace(GL_CCW); break;  /* lower right */
           case 2: fx=-1.0; fy=-1.0; glFrontFace(GL_CW);  break;  /* upper right */
           case 3: fx=+1.0; fy=-1.0; glFrontFace(GL_CCW); break;  /* upper left  */
           }

           /* furchen */
           //glnormal array are well defined before
           TexData[m++] = TABLETEXCOORD_X((-TABLE_W/2.0-BANDE_D2RATIO*BANDE_D)*(fx),(-HOLE1_R-HOLE2_TAN*BANDE_D)*(fy));
           TexData[m++] = TABLETEXCOORD_Y((-TABLE_W/2.0-BANDE_D2RATIO*BANDE_D)*(fx),(-HOLE1_R-HOLE2_TAN*BANDE_D)*(fy));
           VertexData[o++] = (-TABLE_W/2.0-BANDE_D2RATIO*BANDE_D)*(fx); VertexData[o++] = (-HOLE1_R-HOLE2_TAN*BANDE_D)*(fy); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((-area_w/2.0)*(fx),(0.0)*(fy));
           TexData[m++] = TABLETEXCOORD_Y((-area_w/2.0)*(fx),(0.0)*(fy));
           VertexData[o++] = (-area_w/2.0)*(fx); VertexData[o++] = (0.0)*(fy); VertexData[o++] = -BALL_D/2.0;

           for(j=0;j<AREA_SUBDIV_Y/2-1;j++){
               TexData[m++] = TABLETEXCOORD_X((-TABLE_W/2.0-BANDE_D2RATIO*BANDE_D)*(fx),(-area_l*(j+1)/AREA_SUBDIV_Y)*(fy));
               TexData[m++] = TABLETEXCOORD_Y((-TABLE_W/2.0-BANDE_D2RATIO*BANDE_D)*(fx),(-area_l*(j+1)/AREA_SUBDIV_Y)*(fy));
               VertexData[o++] = (-TABLE_W/2.0-BANDE_D2RATIO*BANDE_D)*(fx); VertexData[o++] = (-area_l*(j+1)/AREA_SUBDIV_Y)*(fy); VertexData[o++] = -BALL_D/2.0;
               TexData[m++] = TABLETEXCOORD_X((-area_w/2.0)*(fx),(-area_l*(j+1)/AREA_SUBDIV_Y)*(fy));
               TexData[m++] = TABLETEXCOORD_Y((-area_w/2.0)*(fx),(-area_l*(j+1)/AREA_SUBDIV_Y)*(fy));
               VertexData[o++] = (-area_w/2.0)*(fx); VertexData[o++] = (-area_l*(j+1)/AREA_SUBDIV_Y)*(fy); VertexData[o++] = -BALL_D/2.0;
           }
           TexData[m++] = TABLETEXCOORD_X((-TABLE_W/2.0-BANDE_D2RATIO*BANDE_D)*(fx),(-TABLE_L/2.0+edge_xyoffs+HOLE1_TAN*BANDE_D)*(fy));
           TexData[m++] = TABLETEXCOORD_Y((-TABLE_W/2.0-BANDE_D2RATIO*BANDE_D)*(fx),(-TABLE_L/2.0+edge_xyoffs+HOLE1_TAN*BANDE_D)*(fy));
           VertexData[o++] = (-TABLE_W/2.0-BANDE_D2RATIO*BANDE_D)*(fx); VertexData[o++] = (-TABLE_L/2.0+edge_xyoffs+HOLE1_TAN*BANDE_D)*(fy); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((-area_w/2.0)*(fx),(-area_l/2.0)*(fy));
           TexData[m++] = TABLETEXCOORD_Y((-area_w/2.0)*(fx),(-area_l/2.0)*(fy));
           VertexData[o++] = (-area_w/2.0)*(fx); VertexData[o++] = (-area_l/2.0)*(fy); VertexData[o++] = -BALL_D/2.0;
           glPushMatrix();
           glDrawArrays(GL_QUAD_STRIP,0,4+AREA_SUBDIV_Y-1);
           glPopMatrix();
           m = 0;
           o = 0;
           /* quad between area-edge and hole */
           TexData[m++] = TABLETEXCOORD_X((-area_w/2.0)*(fx),(-area_l/2.0)*(fy));
           TexData[m++] = TABLETEXCOORD_Y((-area_w/2.0)*(fx),(-area_l/2.0)*(fy));
           VertexData[o++] = (-area_w/2.0)*(fx); VertexData[o++] = (-area_l/2.0)*(fy); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2)*(fx),(-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2)*(fy));
           TexData[m++] = TABLETEXCOORD_Y((-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2)*(fx),(-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2)*(fy));
           VertexData[o++] = (-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2)*(fx); VertexData[o++] = (-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2)*(fy); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((-TABLE_W/2.0-BANDE_D2RATIO*BANDE_D)*(fx),(-TABLE_L/2.0+edge_xyoffs+HOLE1_TAN*BANDE_D)*(fy));
           TexData[m++] = TABLETEXCOORD_Y((-TABLE_W/2.0-BANDE_D2RATIO*BANDE_D)*(fx),(-TABLE_L/2.0+edge_xyoffs+HOLE1_TAN*BANDE_D)*(fy));
           VertexData[o++] = (-TABLE_W/2.0-BANDE_D2RATIO*BANDE_D)*(fx); VertexData[o++] = (-TABLE_L/2.0+edge_xyoffs+HOLE1_TAN*BANDE_D)*(fy); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((-area_w/2.0)*(fx),(-area_l/2.0)*(fy));
           TexData[m++] = TABLETEXCOORD_Y((-area_w/2.0)*(fx),(-area_l/2.0)*(fy));
           VertexData[o++] = (-area_w/2.0)*(fx); VertexData[o++] = (-area_l/2.0)*(fy); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((-TABLE_W/2.0+edge_xyoffs+HOLE1_TAN*BANDE_D)*(fx),(-TABLE_L/2.0-BANDE_D2RATIO*BANDE_D)*(fy));
           TexData[m++] = TABLETEXCOORD_Y((-TABLE_W/2.0+edge_xyoffs+HOLE1_TAN*BANDE_D)*(fx),(-TABLE_L/2.0-BANDE_D2RATIO*BANDE_D)*(fy));
           VertexData[o++] = (-TABLE_W/2.0+edge_xyoffs+HOLE1_TAN*BANDE_D)*(fx); VertexData[o++] = (-TABLE_L/2.0-BANDE_D2RATIO*BANDE_D)*(fy); VertexData[o++] = -BALL_D/2.0;
           TexData[m++] = TABLETEXCOORD_X((-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2)*(fx),(-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2)*(fy));
           TexData[m++] = TABLETEXCOORD_Y((-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2)*(fx),(-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2)*(fy));
           VertexData[o++] = (-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2)*(fx); VertexData[o++] = (-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2)*(fy); VertexData[o++] = -BALL_D/2.0;
           glPushMatrix();
           glDrawArrays(GL_TRIANGLES,0,6);
           glPopMatrix();
       }

#define HOLE2_SEGNR_2 12
       fprintf(stderr,"Generate left, right pocket triangles object\n");
       for(k=0;k<2;k++){  /* left, right hole */
           if(k==0) glFrontFace(GL_CCW); else glFrontFace(GL_CW);
           m = 0;
           n = 0;
           o = 0;
           for(i=0;i<HOLE2_SEGNR_2+1;i++){
               phi=(VMfloat)i*M_PI/HOLE2_SEGNR_2;
               x=-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R*sin(phi);
               y=HOLE2_R*cos(phi);
               if(k!=0) x=-x;
               TexData[m++] = TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y);
               NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = 1.0;
               VertexData[o++] = x; VertexData[o++] = y; VertexData[o++] = -BALL_D/2.0 ;
               x=-TABLE_W/2.0-HOLE2_XYOFFS+(HOLE2_R-HOLE2_PHASE)*sin(phi);
               y=(HOLE2_R-HOLE2_PHASE)*cos(phi);
               if(k!=0) x=-x;
               TexData[m++] = TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y);
               NormalData[n++] = -sin(phi); NormalData[n++] = (k==0)?-cos(phi):cos(phi); NormalData[n++] = 0.0;
               VertexData[o++] = x; VertexData[o++] = y; VertexData[o++] = -BALL_D/2.0-HOLE2_PHASE ;
           }
           glPushMatrix();
           glDrawArrays(GL_QUAD_STRIP,0,HOLE2_SEGNR_2*2+1);
           glPopMatrix();
       }

       fprintf(stderr,"Generate left, right pocket fans triangles object\n");
       for(k=0;k<2;k++){  /* left, right hole fans */
           for(j=0;j<2;j++){ /* fan 1, 2 */
               m = 0;
               n = 0;
               o = 0;
               if(j^k) glFrontFace(GL_CCW); else glFrontFace(GL_CW);
               NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = 1.0;
               x=-TABLE_W/2.0-BANDE_D*BANDE_D2RATIO; y=-HOLE2_R-BANDE_D*HOLE2_TAN;
               if(j!=0) y=-y;
               if(k!=0) x=-x;
               TexData[m++] = TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y);
               VertexData[o++] = x; VertexData[o++] = y; VertexData[o++] = -BALL_D/2.0 ;
               for(i=0;i<HOLE2_SEGNR_2/2+1;i++){
                   phi=(VMfloat)i*M_PI/HOLE2_SEGNR_2;
                   x=-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R*sin(phi);
                   y=-HOLE2_R*cos(phi);
                   if(j!=0) y=-y;
                   if(k!=0) x=-x;
                   TexData[m++] = TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y);
                   VertexData[o++] = x; VertexData[o++] = y; VertexData[o++] = -BALL_D/2.0 ;
                   NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = 1.0;
               }
               glPushMatrix();
               glDrawArrays(GL_TRIANGLE_FAN,0,1+HOLE2_SEGNR_2/2+1);
               glPopMatrix();
           }
       }
       glFrontFace(GL_CW);
   } /* if(!carambol) */

/*   my_glBox( -tablew/2.0, -tablel/2.0, -balld/2.0,
              tablew/2.0,  tablel/2.0, -balld/2.0-6.0*cm );*/

   hole_r1 = edge_xyoffs+HOLE1_TAN*BANDE_D /**100.0*/;
   hole_r2 = HOLE2_R+HOLE2_TAN*BANDE_D  /**100.0*/;

/*   my_glBox( -tablew/2.0-5.0*cm, -hole_r2, -balld/2.0-5.0*cm,
             -tablew/2.0,    -tablel/2.0+hole_r1, 0.0 );
   my_glBox( -tablew/2.0-5.0*cm, +hole_r2, -balld/2.0-5.0*cm,
             -tablew/2.0,   +tablel/2.0-hole_r1, 0.0 );
   my_glBox( +tablew/2.0+5.0*cm, -hole_r2, -balld/2.0-5.0*cm,
             +tablew/2.0,   -tablel/2.0+hole_r1, 0.0 );
   my_glBox( +tablew/2.0+5.0*cm, +hole_r2, -balld/2.0-5.0*cm,
             +tablew/2.0,   +tablel/2.0-hole_r1, 0.0 );
   my_glBox( -tablew/2.0+hole_r1, -tablel/2.0-5.0*cm, -balld/2.0-5.0*cm,
             +tablew/2.0-hole_r1, -tablel/2.0,   0.0 );
   my_glBox( -tablew/2.0+hole_r1, +tablel/2.0+5.0*cm, -balld/2.0-5.0*cm,
             +tablew/2.0-hole_r1, +tablel/2.0,   0.0 );
*/

   fprintf(stderr,"Generate pocket objects: ");
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   if(! carambol){
       /* upper */
   	   fprintf(stderr,"Not carom: ");
       my_Bande( -tablew/2.0+hole_r1, tablel/2.0, 0.0, +tablew/2.0-hole_r1, tablel/2.0+bande_d, -balld/2.0, HOLE1_TAN, HOLE1_TAN, 0 );

       fprintf(stderr,"lower "); /* lower */
       glPushMatrix();
       glScalef(1.0,-1.0,1.0);
       my_Bande( -tablew/2.0+hole_r1, tablel/2.0, 0.0, +tablew/2.0-hole_r1, tablel/2.0+bande_d, -balld/2.0, HOLE1_TAN, HOLE1_TAN, 1 );
       glPopMatrix();

       glPushMatrix();
       glRotatef( 90.0, 0.0,0.0,1.0 );
       fprintf(stderr,"upper left ");/* upper left */
       my_Bande( hole_r2, tablew/2.0, 0.0, +tablew-hole_r1, tablew/2.0+bande_d, -balld/2.0, HOLE2_TAN, HOLE1_TAN, 0 );
       glScalef(1.0,-1.0,1.0);
       fprintf(stderr,"upper right ");/* upper right */
       my_Bande( hole_r2, tablew/2.0, 0.0, +tablew-hole_r1, tablew/2.0+bande_d, -balld/2.0, HOLE2_TAN, HOLE1_TAN, 1 );
       glPopMatrix();

       glPushMatrix();
       glRotatef( 90.0, 0.0,0.0,1.0 );
       fprintf(stderr,"lower left "); /* lower left */
       my_Bande( -tablew+hole_r1, tablew/2.0, 0.0, -hole_r2, tablew/2.0+bande_d, -balld/2.0, HOLE1_TAN, HOLE2_TAN, 0 );
       fprintf(stderr,"lower right "); /* lower right */
       glScalef(1.0,-1.0,1.0);
       my_Bande( -tablew+hole_r1, tablew/2.0, 0.0, -hole_r2, tablew/2.0+bande_d, -balld/2.0, HOLE1_TAN, HOLE2_TAN, 1 );
       glPopMatrix();
       fprintf(stderr,"\n");
   } else {
      fprintf(stderr,"carom: ");
      fprintf(stderr,"upper "); /* upper */
       my_Bande( -tablew/2.0, tablel/2.0, 0.0, +tablew/2.0, tablel/2.0+bande_d, -balld/2.0, 1.0, 1.0, 0 );

       fprintf(stderr,"lower ");/* lower */
       glPushMatrix();
       glScalef(1.0,-1.0,1.0);
       my_Bande( -tablew/2.0, tablel/2.0, 0.0, +tablew/2.0, tablel/2.0+bande_d, -balld/2.0, 1.0, 1.0, 1 );
       glPopMatrix();

       glPushMatrix();
       glRotatef( 90.0, 0.0,0.0,1.0 );
       fprintf(stderr,"upper left "); /* upper left */
       my_Bande( 0.0, tablew/2.0, 0.0, +tablew,  tablew/2.0+bande_d, -balld/2.0, 0.0, 1.0, 0 );
       fprintf(stderr,"upper right "); /* upper right */
       glScalef(1.0,-1.0,1.0);
       my_Bande( 0.0, tablew/2.0, 0.0, +tablew,  tablew/2.0+bande_d, -balld/2.0, 0.0, 1.0, 1 );

       glPopMatrix();

       glPushMatrix();
       glRotatef( 90.0, 0.0,0.0,1.0 );
       fprintf(stderr,"lower left "); /* lower left */
       my_Bande( -tablew, tablew/2.0, 0.0, 0.0, tablew/2.0+bande_d, -balld/2.0, 1.0, 0.0, 0 );
       fprintf(stderr,"lower right "); /* lower right */
       glScalef(1.0,-1.0,1.0);
       my_Bande( -tablew, tablew/2.0, 0.0, 0.0, tablew/2.0+bande_d, -balld/2.0, 1.0, 0.0, 1 );
       glPopMatrix();
       fprintf(stderr,"\n");
   }

   /* disable 2nd tex unit for cloth texture */
       glActiveTextureARB(GL_TEXTURE1_ARB);
       glDisable(GL_TEXTURE_2D);
       glActiveTextureARB(GL_TEXTURE0_ARB);

   /* diamonds */
   fprintf(stderr,"Generate diamonds border objects\n");
   glMaterialfv(GL_FRONT, GL_DIFFUSE,   dia_col_diff);
   glMaterialfv(GL_FRONT, GL_AMBIENT,   dia_col_amb);
   glMaterialfv(GL_FRONT, GL_SPECULAR,  dia_col_spec);
   glMaterialf (GL_FRONT, GL_SHININESS, dia_col_shin);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,reflect_bind);
   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T);
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   /*left right*/
   for(i=0;i<4;i++){
       flip=0;
       glPushMatrix();
       switch(i){
       case 0: flip=0; break;
       case 1: glScalef( -1.0,  1.0, 1.0 ); flip=1; break;
       case 2: glScalef(  1.0, -1.0, 1.0 ); flip=1; break;
       case 3: glScalef( -1.0, -1.0, 1.0 ); flip=0; break;
       }
       glPushMatrix();
       glTranslatef( tablew/2.0+FRAME_D/2.0-FRAME_PHASE/2.0+bande_d/2.0, 2.0*tablel/8.0, FRAME_DH/2.0 );
       glRotatef( atan(FRAME_DH/(FRAME_D-bande_d-FRAME_PHASE))*180.0/M_PI, 0.0, 1.0, 0.0 );
       my_Diamondxy( 0.02, 0.014, 0.006, flip );
       glPopMatrix();
       glPushMatrix();
       glTranslatef( tablew/2.0+FRAME_D/2.0-FRAME_PHASE/2.0+bande_d/2.0, 3.0*tablel/8.0, FRAME_DH/2.0 );
       glRotatef( atan(FRAME_DH/(FRAME_D-bande_d-FRAME_PHASE))*180.0/M_PI, 0.0, 1.0, 0.0 );
       my_Diamondxy( 0.02, 0.014, 0.006, flip );
       glPopMatrix();
       glTranslatef( tablew/2.0+FRAME_D/2.0-FRAME_PHASE/2.0+bande_d/2.0, 1.0*tablel/8.0, FRAME_DH/2.0 );
       glRotatef( atan(FRAME_DH/(FRAME_D-bande_d-FRAME_PHASE))*180.0/M_PI, 0.0, 1.0, 0.0 );
       my_Diamondxy( 0.02, 0.014, 0.006, flip );
       glPopMatrix();
   }
   /*upper lower*/
   for(i=0;i<2;i++){
       flip=0;
       glPushMatrix();
       switch(i){
       case 0: flip=0; break;
       case 1: glScalef( 1.0, -1.0, 1.0 ); flip=1; break;
       }
       glPushMatrix();
       glTranslatef( -tablew/2.0+1.0*tablew/4.0, tablel/2.0+FRAME_D/2.0-FRAME_PHASE/2.0+bande_d/2.0, FRAME_DH/2.0 );
       glRotatef( -atan(FRAME_DH/(FRAME_D-bande_d-FRAME_PHASE))*180.0/M_PI, 1.0, 0.0, 0.0 );
       my_Diamondxy( 0.014, 0.02, 0.006, flip );
       glPopMatrix();
       glPushMatrix();
       glTranslatef( -tablew/2.0+2.0*tablew/4.0, tablel/2.0+FRAME_D/2.0-FRAME_PHASE/2.0+bande_d/2.0, FRAME_DH/2.0 );
       glRotatef( -atan(FRAME_DH/(FRAME_D-bande_d-FRAME_PHASE))*180.0/M_PI, 1.0, 0.0, 0.0 );
       my_Diamondxy( 0.014, 0.02, 0.006, flip );
       glPopMatrix();
       glTranslatef( -tablew/2.0+3.0*tablew/4.0, tablel/2.0+FRAME_D/2.0-FRAME_PHASE/2.0+bande_d/2.0, FRAME_DH/2.0 );
       glRotatef( -atan(FRAME_DH/(FRAME_D-bande_d-FRAME_PHASE))*180.0/M_PI, 1.0, 0.0, 0.0 );
       my_Diamondxy( 0.014, 0.02, 0.006, flip );
       glPopMatrix();
   }

   /* gold edges and covers */
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   if(!carambol){
 	     fprintf(stderr,"Generate gold/silver edges and cover objects\n");
       if( options_bumpref        &&
           extension_multitexture &&
           extension_cubemap
           )
       {
           //fprintf(stderr,"%f %f %f \n",dia_col_diff[0]*0.8, dia_col_diff[1]*0.8, dia_col_diff[2]*0.8);
           glColor4f(dia_col_diff[0]*0.8, dia_col_diff[1]*0.8, dia_col_diff[2]*0.8,1.0);
           bumpref_use(&bumpref);
       }

       glPushMatrix();
       glTranslatef( tablew/2.0-edge_xyoffs, tablel/2.0-edge_xyoffs, 0.0 );
        my_Edge( 10, r1func, r2func, 0 );
       glPopMatrix();
       glPushMatrix();
         glScalef( -1.0, +1.0, +1.0 );
         glTranslatef( tablew/2.0-edge_xyoffs, tablel/2.0-edge_xyoffs, 0.0 );
         my_Edge( 10, r1func, r2func, 1 );
       glPopMatrix();
       glPushMatrix();
         glScalef( -1.0, -1.0, +1.0 );
         glTranslatef( tablew/2.0-edge_xyoffs, tablel/2.0-edge_xyoffs, 0.0 );
         my_Edge( 10, r1func, r2func, 0 );
       glPopMatrix();
       glPushMatrix();
         glScalef( +1.0, -1.0, +1.0 );
         glTranslatef( tablew/2.0-edge_xyoffs, tablel/2.0-edge_xyoffs, 0.0 );
         my_Edge( 10, r1func, r2func, 1 );
       glPopMatrix();

       /* covers */
       glPushMatrix();
         glTranslatef( tablew/2.0+bande_d, 0.0, 0.0 );
         my_Cover( 9, r1coverfunc, 2.0*HOLE2_R, 0, 0.0, 1.0, 1.0 );
       glPopMatrix();
       glPushMatrix();
         glScalef( -1.0, +1.0, +1.0 );
         glTranslatef( tablew/2.0+bande_d, 0.0, 0.0 );
         my_Cover( 9, r1coverfunc, 2.0*HOLE2_R, 1, 0.0, 1.0, 1.0 );
       glPopMatrix();
       if( options_bumpref        &&
           extension_multitexture &&
           extension_cubemap
           )
       {
           bumpref_restore();
       }

       /* bumpers for gold edges and covers */
       fprintf(stderr,"Generate bumpers for gold/silver edges and covers\n");
       glMaterialfv(GL_FRONT, GL_DIFFUSE,   bumpers_col_diff);
       glMaterialfv(GL_FRONT, GL_AMBIENT,   bumpers_col_amb);
       glMaterialfv(GL_FRONT, GL_SPECULAR,  bumpers_col_spec);
       glMaterialf (GL_FRONT, GL_SHININESS, bumpers_col_shin );
       glDisable(GL_TEXTURE_2D);
       glPushMatrix();
         glTranslatef( tablew/2.0-edge_xyoffs, tablel/2.0-edge_xyoffs, 0.0 );
         my_EdgeBumper( 10, r1func, scalefunc, 0 );
       glPopMatrix();
       glPushMatrix();
         glScalef( -1.0, +1.0, +1.0 );
         glTranslatef( tablew/2.0-edge_xyoffs, tablel/2.0-edge_xyoffs, 0.0 );
         my_EdgeBumper( 10, r1func, scalefunc, 1 );
       glPopMatrix();
       glPushMatrix();
         glScalef( -1.0, -1.0, +1.0 );
         glTranslatef( tablew/2.0-edge_xyoffs, tablel/2.0-edge_xyoffs, 0.0 );
         my_EdgeBumper( 10, r1func, scalefunc, 0 );
       glPopMatrix();
       glPushMatrix();
         glScalef( +1.0, -1.0, +1.0 );
         glTranslatef( tablew/2.0-edge_xyoffs, tablel/2.0-edge_xyoffs, 0.0 );
         my_EdgeBumper( 10, r1func, scalefunc, 1 );
       glPopMatrix();
       /* bumpers for covers */
       glPushMatrix();
       glDisableClientState(GL_TEXTURE_COORD_ARRAY);
         glTranslatef( tablew/2.0+bande_d, 0.0, 0.0 );
         my_CoverBumper( 9, r1coverfunc, scalefunc01, HOLE2_R*2.0, 0 );
       glPopMatrix();
       glPushMatrix();
         glScalef( -1.0, +1.0, +1.0 );
         glTranslatef( tablew/2.0+bande_d, 0.0, 0.0 );
         my_CoverBumper( 9, r1coverfunc, scalefunc01, HOLE2_R*2.0, 1 );
       glPopMatrix();

       /* hole backfaces */
       glMaterialfv(GL_FRONT, GL_DIFFUSE,   hole_col_diff);
       glMaterialfv(GL_FRONT, GL_AMBIENT,   hole_col_amb);
       glMaterialfv(GL_FRONT, GL_SPECULAR,  hole_col_spec);
       glMaterialf (GL_FRONT, GL_SHININESS, hole_col_shin );
#define HOLE1_BACKSEGNR 8
       glVertexPointer(3, GL_FLOAT, 0, VertexData);
       glNormalPointer(GL_FLOAT, 0, NormalData);
       for(j=0;j<4;j++){ /* 4 edge holes */
           xf=0.0;
           yf=0.0;
           switch(j){
           case 0: glFrontFace(GL_CCW); xf=+1.0; yf=+1.0; break;
           case 1: glFrontFace(GL_CW);  xf=-1.0; yf=+1.0; break;
           case 2: glFrontFace(GL_CW);  xf=+1.0; yf=-1.0; break;
           case 3: glFrontFace(GL_CCW); xf=-1.0; yf=-1.0; break;
           }
           n = 0;
           o = 0;
           NormalData[n++] = (-1.0/SQR2)*xf; NormalData[n++] = (1.0/SQR2)*yf; NormalData[n++] = 0.0;
           VertexData[o++] = (-TABLE_W/2.0+edge_xyoffs)*xf; VertexData[o++] = (-TABLE_L/2.0-BANDE_D)*yf; VertexData[o++] = FRAME_DH;
           NormalData[n++] = (0)*xf; NormalData[n++] = (0)*yf; NormalData[n++] = -1.0;
           VertexData[o++] = (-TABLE_W/2.0+edge_xyoffs)*xf; VertexData[o++] = (-TABLE_L/2.0-BANDE_D)*yf; VertexData[o++] = -0.1;
           for( i=0 ; i<HOLE1_BACKSEGNR+1 ; i++ ){
               VMfloat phi;
               phi = -M_PI/4.0-M_PI*(VMfloat)i/(VMfloat)HOLE1_BACKSEGNR;
               NormalData[n++] = (-cos(phi))*xf; NormalData[n++] = (-sin(phi))*yf; NormalData[n++] = 0.0;
               VertexData[o++] = (-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R*cos(phi))*xf; VertexData[o++] = (-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R*sin(phi))*yf; VertexData[o++] = FRAME_DH;
               NormalData[n++] = (0)*xf; NormalData[n++] = (0)*yf; NormalData[n++] = -1.0;
               VertexData[o++] = (-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R*cos(phi))*xf; VertexData[o++] = (-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R*sin(phi))*yf; VertexData[o++] = -0.1;
           }
           NormalData[n++] = (1.0/SQR2)*xf; NormalData[n++] = (-1.0/SQR2)*yf; NormalData[n++] = 0.0;
           VertexData[o++] = (-TABLE_W/2.0-BANDE_D)*xf; VertexData[o++] = (-TABLE_L/2.0+edge_xyoffs)*yf; VertexData[o++] = FRAME_DH;
           NormalData[n++] = (0)*xf; NormalData[n++] = (0)*yf; NormalData[n++] = -1.0;
           VertexData[o++] = (-TABLE_W/2.0-BANDE_D)*xf; VertexData[o++] = (-TABLE_L/2.0+edge_xyoffs)*yf; VertexData[o++] = -0.1;
           glPushMatrix();
           glDrawArrays(GL_QUAD_STRIP,0,4+((HOLE1_BACKSEGNR+1)*2));
           glPopMatrix();
           /* black ground of hole */
           o = 0;
           n = 0;
           NormalData[n++] = (0)*xf; NormalData[n++] = (0)*yf; NormalData[n++] = -1.0;
           VertexData[o++] = (-TABLE_W/2.0-HOLE1_XYOFFS-HOLE1_R*sqrt(2.0))*xf; VertexData[o++] = (-TABLE_L/2.0-HOLE1_XYOFFS)*yf; VertexData[o++] = -0.1;
           NormalData[n++] = (0)*xf; NormalData[n++] = (0)*yf; NormalData[n++] = -1.0;
           VertexData[o++] = (-TABLE_W/2.0-HOLE1_XYOFFS)*xf; VertexData[o++] = (-TABLE_L/2.0-HOLE1_XYOFFS-HOLE1_R*sqrt(2.0))*yf; VertexData[o++] = -0.1;
           NormalData[n++] = (0)*xf; NormalData[n++] = (0)*yf; NormalData[n++] = -1.0;
           VertexData[o++] = (-TABLE_W/2.0-HOLE1_XYOFFS+0.5)*xf; VertexData[o++] = (-TABLE_L/2.0-HOLE1_XYOFFS-HOLE1_R*sqrt(2.0))*yf; VertexData[o++] = -0.1;
           NormalData[n++] = (0)*xf; NormalData[n++] = (0)*yf; NormalData[n++] = -1.0;
           VertexData[o++] = (-TABLE_W/2.0-HOLE1_XYOFFS-HOLE1_R*sqrt(2.0))*xf; VertexData[o++] = (-TABLE_L/2.0-HOLE1_XYOFFS+0.5)*yf; VertexData[o++] = -0.1;
           glPushMatrix();
           glDrawArrays(GL_QUADS,0,4);
           glPopMatrix();
       }
#define HOLE2_BACKSEGNR 8

       fprintf(stderr,"Generate side holes\n");
       for(j=0;j<2;j++){ /* 2 side holes */
           xf=0.0;
           switch(j){
           case 0: glFrontFace(GL_CCW); xf=+1.0; break;
           case 1: glFrontFace(GL_CW);  xf=-1.0; break;
           }
           n = 0;
           o = 0;
           NormalData[n++] = 0.0; NormalData[n++] = 1.0; NormalData[n++] = 0.0;
           VertexData[o++] = (-TABLE_W/2.0-BANDE_D)*xf; VertexData[o++] = -HOLE2_R; VertexData[o++] = FRAME_DH;
           NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = -1.0;
           VertexData[o++] = (-TABLE_W/2.0-BANDE_D)*xf; VertexData[o++] = -HOLE2_R; VertexData[o++] = -0.1;
           for( i=0 ; i<HOLE2_BACKSEGNR+1 ; i++ ){
               VMfloat phi;
               phi = -M_PI/2.0-M_PI*(VMfloat)i/(VMfloat)HOLE2_BACKSEGNR;
               NormalData[n++] = (-cos(phi))*xf; NormalData[n++] = -sin(phi); NormalData[n++] = 0.0;
               VertexData[o++] = (-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R*cos(phi))*xf; VertexData[o++] = HOLE2_R*sin(phi); VertexData[o++] = FRAME_DH ;
               NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = -1.0;
               VertexData[o++] = (-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R*cos(phi))*xf; VertexData[o++] = HOLE2_R*sin(phi); VertexData[o++] = -0.1;
           }
           NormalData[n++] = 0.0; NormalData[n++] = -1.0; NormalData[n++] = 0.0;
           VertexData[o++] = (-TABLE_W/2.0-BANDE_D)*xf; VertexData[o++] = +HOLE2_R; VertexData[o++] = FRAME_DH;
           NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = -1.0;
           VertexData[o++] = (-TABLE_W/2.0-BANDE_D)*xf; VertexData[o++] = +HOLE2_R; VertexData[o++] = -0.1;
           glPushMatrix();
           glDrawArrays(GL_QUAD_STRIP,0,4+((HOLE2_BACKSEGNR+1)*2));
           glPopMatrix();
           /* black ground of hole */
           o = 0;
           n = 0;
           NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = -1.0;
           VertexData[o++] = (-TABLE_W/2.0-HOLE2_XYOFFS-HOLE2_R)*xf; VertexData[o++] = +HOLE2_R; VertexData[o++] = -0.1;
           NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = -1.0;
           VertexData[o++] = (-TABLE_W/2.0-HOLE2_XYOFFS-HOLE2_R)*xf; VertexData[o++] = -HOLE2_R; VertexData[o++] = -0.1;
           NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = -1.0;
           VertexData[o++] = (-TABLE_W/2.0-HOLE2_XYOFFS+2.5*HOLE2_R)*xf; VertexData[o++] = -4*HOLE2_R; VertexData[o++] = -0.1;
           NormalData[n++] = 0.0; NormalData[n++] = 0.0; NormalData[n++] = -1.0;
           VertexData[o++] = (-TABLE_W/2.0-HOLE2_XYOFFS+2.5*HOLE2_R)*xf; VertexData[o++] = +4*HOLE2_R; VertexData[o++] = -0.1;
           glPushMatrix();
           glDrawArrays(GL_QUADS,0,4);
           glPopMatrix();
       }
       glFrontFace(GL_CW);
   }
   /* wood-frame */
   fprintf(stderr,"Generate wood frame\n");
   glEnable(GL_TEXTURE_2D);
   glMaterialfv(GL_FRONT, GL_DIFFUSE,   wood_col_diff2);
   glMaterialfv(GL_FRONT, GL_AMBIENT,   wood_col_amb2);
   glMaterialfv(GL_FRONT, GL_SPECULAR,  wood_col_spec_null);
   glMaterialf (GL_FRONT, GL_SHININESS, wood_col_shin);
   glBindTexture(GL_TEXTURE_2D,frametexbind);
   glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glMaterialfv(GL_FRONT, GL_DIFFUSE,   wood_col_diff2);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   for(i=0;i<2;i++){
       if(i==1){
           glDisable(GL_TEXTURE_2D);
           glMaterialfv(GL_FRONT, GL_DIFFUSE,   wood_col_diff3);
           glMaterialfv(GL_FRONT, GL_AMBIENT,   wood_col_amb3);
           glMaterialfv(GL_FRONT, GL_SPECULAR,  wood_col_spec);
           glEnable(GL_BLEND);
           glBlendFunc(GL_ONE,GL_ONE);
           if(options_bumpwood       &&
              extension_multitexture &&
              extension_rc_NV        &&
              extension_vp_NV        &&
              bump_init )
           {
               fprintf(stderr,"applying wood frame bumpmaps\n");
               bump_set_diff(&bumponly,0.0,0.0,0.0);
               bump_set_spec(&bumponly,0.5,0.5,0.5);
               bump_use(&bumponly);
           }
       }
       if(!carambol){
           lwood   = TABLE_L/2.0-HOLE1_R*sqrt(2.0)+BANDE_D-HOLE2_R;
           woodpos = HOLE2_R+lwood/2;

           if(options_bumpwood       &&
              extension_multitexture &&
              extension_rc_NV        &&
              extension_vp_NV        &&
              bump_init )
           {
               bump_set_light( &bumponly, -(tablew/2.0+bande_d), -woodpos, 0.7 );
           }
           /* upper right */
           glPushMatrix();
           glTranslatef( tablew/2.0+bande_d, woodpos, 0.0 );
           my_Cover( 1, r1coverfunc_zero, lwood, 0, 0.0, 2.0, 2.0 );
           glPopMatrix();
           /* lower right */
           glPushMatrix();
           glScalef( +1.0, -1.0, +1.0 );
           glTranslatef( tablew/2.0+bande_d, woodpos, 0.0 );
           my_Cover( 1, r1coverfunc_zero, lwood, 1, 0.0, 2.0, 2.0 );
           glPopMatrix();
           /* upper left */
           glPushMatrix();
           glScalef( -1.0, +1.0, +1.0 );
           glTranslatef( tablew/2.0+bande_d, woodpos, 0.0 );
           my_Cover( 1, r1coverfunc_zero, lwood, 1, 0.0, 2.0, 2.0 );
           glPopMatrix();
           /* lower left */
           glPushMatrix();
           glScalef( -1.0, -1.0, +1.0 );
           glTranslatef( tablew/2.0+bande_d, woodpos, 0.0 );
           my_Cover( 1, r1coverfunc_zero, lwood, 0, 0.0, 2.0, 2.0 );
           glPopMatrix();

           if(options_bumpwood       &&
              extension_multitexture &&
              extension_rc_NV        &&
              extension_vp_NV        &&
              bump_init )
           {
               bump_set_light( &bumponly, -(tablel/2.0+bande_d), 0.0, 0.7 );
           }
           /* upper */
           glPushMatrix();
           glTranslatef( 0.0, tablel/2.0+bande_d, 0.0 );
           glRotatef( 90.0, 0.0,0.0,1.0 );
           my_Cover( 1, r1coverfunc_zero, tablew-2.0*hole_r1+2.0*bande_d*HOLE1_TAN, 0, 0.0, 2.0, 2.0 );
           glPopMatrix();
           /* lower */
           glPushMatrix();
           glTranslatef( 0.0, -tablel/2.0-bande_d, 0.0 );
           glRotatef( -90.0, 0.0,0.0,1.0 );
           my_Cover( 1, r1coverfunc_zero, tablew-2.0*hole_r1+2.0*bande_d*HOLE1_TAN, 0, 0.0, 2.0, 2.0 );
           glPopMatrix();
       } else {
           if(options_bumpwood       &&
              extension_multitexture &&
              extension_rc_NV        &&
              extension_vp_NV        &&
              bump_init )
           {
               bump_set_light( &bumponly, -(tablel/2.0+bande_d), 0.0, 0.7 );
           }
           /* upper */
           glPushMatrix();
           glTranslatef( 0.0, tablel/2.0+bande_d, 0.0 );
           glRotatef( 90.0, 0.0,0.0,1.0 );
           my_Cover2func( 32, r1coverfunc_zero, r2cover_caram, tablew+2.0*bande_d, 0, 1.0 );
           glPopMatrix();
           /* lower */
           glPushMatrix();
           glTranslatef( 0.0, -tablel/2.0-bande_d, 0.0 );
           glRotatef( -90.0, 0.0,0.0,1.0 );
           my_Cover2func( 32, r1coverfunc_zero, r2cover_caram, tablew+2.0*bande_d, 0, 1.0 );
           glPopMatrix();
           if(options_bumpwood       &&
              extension_multitexture &&
              extension_rc_NV        &&
              extension_vp_NV        &&
              bump_init )
           {
               bump_set_light( &bumponly, -(tablew/2.0+bande_d), 0.0, 0.7 );
           }
           /* left */
           glPushMatrix();
           glScalef( -1.0, +1.0, +1.0 );
           glTranslatef( tablew/2.0+bande_d, 0.0, 0.0 );
           my_Cover2func( 32, r1coverfunc_zero, r2cover_caram, tablel+2.0*bande_d, 1, 1.0 );
           glPopMatrix();
           /* right */
           glPushMatrix();
           glTranslatef( tablew/2.0+bande_d, 0.0, 0.0 );
           my_Cover2func( 32, r1coverfunc_zero, r2cover_caram, tablel+2.0*bande_d, 0, 1.0 );
           glPopMatrix();
       }
       if(i==1){
           if(options_bumpwood       &&
              extension_multitexture &&
              extension_rc_NV        &&
              extension_vp_NV        &&
              bump_init )
           {
               bump_restore();
           }
       }
   } /* 2 turns (textured and highlight) */

   glDisable(GL_BLEND);
/*********************
 *   For Debugging   *
 *********************/
       /*left right*/
/*  for(i=0;i<4;i++){
       int flip=0;
       glPushMatrix();
       switch(i){
       case 0: flip=0; break; 
       case 1: glScalef( -1.0,  1.0, 1.0 ); flip=1; break;
       case 2: glScalef(  1.0, -1.0, 1.0 ); flip=1; break;
       case 3: glScalef( -1.0, -1.0, 1.0 ); flip=0; break;
       }
       my_Rectxy( tablew/2.0+bande_d, tablel/2.0-hole_r1+bande_d*HOLE1_TAN, FRAME_DH,
                  tablew/2.0+FRAME_D-FRAME_PHASE, HOLE2_W/2.0-bande_d*HOLE2_TAN,        0.0,
                  1, flip );
       my_Rectxy( tablew/2.0+FRAME_D-FRAME_PHASE, tablel/2.0-hole_r1+bande_d*HOLE1_TAN, 0.0,
                  tablew/2.0+FRAME_D, HOLE2_W/2.0-bande_d*HOLE2_TAN,        -FRAME_PHASE,
                  1, flip );
       my_Rectxy( tablew/2.0+FRAME_D, tablel/2.0-hole_r1+bande_d*HOLE1_TAN, -FRAME_PHASE,
                  tablew/2.0+FRAME_D-FRAME_PHASE, HOLE2_W/2.0-bande_d*HOLE2_TAN, -FRAME_H,
                  1, flip );
       glPopMatrix();
   }*/
   /*upper lower*/
/*   glTexGenfv(GL_S, GL_OBJECT_PLANE, s_gen_params2);
   glTexGenfv(GL_T, GL_OBJECT_PLANE, t_gen_params2);
   for(i=0;i<2;i++){
       int flip=0;
       glPushMatrix();
       switch(i){
       case 0: flip=1; break;
       case 1: glScalef( 1.0, -1.0, 1.0 ); flip=0; break;
       }
       my_Rectxy( -tablew/2.0+hole_r1-bande_d*HOLE1_TAN, tablel/2.0+bande_d, FRAME_DH,
                  +tablew/2.0-hole_r1+bande_d*HOLE1_TAN, tablel/2.0+FRAME_D-FRAME_PHASE, 0.0,
                  0, flip );
       my_Rectxy( -tablew/2.0+hole_r1-bande_d*HOLE1_TAN, tablel/2.0+FRAME_D-FRAME_PHASE, 0.0,
                  +tablew/2.0-hole_r1+bande_d*HOLE1_TAN, tablel/2.0+FRAME_D, -FRAME_PHASE,
                  0, flip );
       my_Rectxy( -tablew/2.0+hole_r1-bande_d*HOLE1_TAN, tablel/2.0+FRAME_D, -FRAME_PHASE,
                  +tablew/2.0-hole_r1+bande_d*HOLE1_TAN, tablel/2.0+FRAME_D-FRAME_PHASE, -FRAME_H,
                  0, flip );
       glPopMatrix();
   }*/

   /* table feet */

   glEnable(GL_TEXTURE_2D);
   glMaterialfv(GL_FRONT, GL_AMBIENT,   wood_col_amb);
   glMaterialfv(GL_FRONT, GL_SPECULAR,  wood_col_spec_null);
   glMaterialf (GL_FRONT, GL_SHININESS, wood_col_shin);
   glBindTexture(GL_TEXTURE_2D,reflect_bind);
   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T);
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

//   my_glBox( -tablew/2.0*0.8-3.0*cm, -tablel/2.0*0.8-3.0*cm, -balld/2.0-6.0*cm,
//            +tablew/2.0*0.8+3.0*cm, +tablel/2.0*0.8+3.0*cm, -tableh-balld/2.0 );

#define FEET_EDGE_STEPS 6
#define FEET_X_STEPS 5
#define FEET_Y_STEPS 5
#define FEET_R  0.2
#define FEET_R2 0.1
#define FEET_W  (tablew+0.23)
#define FEET_W2 (FEET_W-2.0*FEET_R+2.0*FEET_R2)
#define FEET_L  (tablel+0.23)
#define FEET_L2 (FEET_L-2.0*FEET_R+2.0*FEET_R2)
#define FEET_X_FUNC(x) 0.1*cos(M_PI/2.0*(x))
#define FEET_Y_FUNC(x) 0.16*sin(M_PI/2.0*(x))
   for(k=0;k<2;k++){
       for(l=0;l<2;l++){
           glPushMatrix();
           if (k^l) glFrontFace(GL_CCW); else glFrontFace(GL_CW);
           if (k) glScalef(-1.0,1.0,1.0);
           if (l) glScalef(1.0,-1.0,1.0);
           n = 0;
           o = 0;
           glVertexPointer(3, GL_FLOAT, 0, VertexData);
           glNormalPointer(GL_FLOAT, 0, NormalData);
           for(i=0;i<FEET_X_STEPS;i++){
               NormalData[n++] = 0.0; NormalData[n++] = -1.0; NormalData[n++] = 0.0;
               NormalData[n++] = 0.0; NormalData[n++] = -1.0; NormalData[n++] = 0.0;
               VertexData[o++] = -(FEET_W/2.0 -FEET_R )/(VMfloat)FEET_X_STEPS*i; VertexData[o++] = -(FEET_L/2.0); VertexData[o++] = -0.1;
               VertexData[o++] = -(FEET_W2/2.0-FEET_R2)/(VMfloat)FEET_X_STEPS*i; VertexData[o++] = -(FEET_L2/2.0)-FEET_X_FUNC((VMfloat)i/(VMfloat)FEET_X_STEPS)*(FEET_L-FEET_L2)/2.0/(tableh+balld/2.0-0.1); VertexData[o++] = -tableh-balld/2.0+FEET_X_FUNC((VMfloat)i/(VMfloat)FEET_X_STEPS);
           }
           for(i=0;i<FEET_EDGE_STEPS;i++){
               NormalData[n++] = -1.0/sqrt(2.0); NormalData[n++] = -1.0/sqrt(2.0); NormalData[n++] = 0.0;
               NormalData[n++] = -1.0/sqrt(2.0); NormalData[n++] = -1.0/sqrt(2.0); NormalData[n++] = 0.0;
               VertexData[o++] = -(FEET_W /2.0-FEET_R )-FEET_R *sin(M_PI/2.0*(VMfloat)i/(VMfloat)FEET_X_STEPS); VertexData[o++] = -(FEET_L/2.0- FEET_R )-FEET_R *cos(M_PI/2.0*(VMfloat)i/(VMfloat)FEET_X_STEPS); VertexData[o++] = -0.1;
               VertexData[o++] = -(FEET_W2/2.0-FEET_R2)-FEET_R2*sin(M_PI/2.0*(VMfloat)i/(VMfloat)FEET_X_STEPS); VertexData[o++] = -(FEET_L2/2.0-FEET_R2)-FEET_R2*cos(M_PI/2.0*(VMfloat)i/(VMfloat)FEET_X_STEPS); VertexData[o++] = -tableh-balld/2.0;
           }
           for(i=1;i<FEET_Y_STEPS+1;i++){
               NormalData[n++] = -1.0; NormalData[n++] = 0.0; NormalData[n++] = 0.0;
               NormalData[n++] = -1.0; NormalData[n++] = 0.0; NormalData[n++] = 0.0;
               VertexData[o++] = -(FEET_W/2.0); VertexData[o++] = -(FEET_L/2.0 -FEET_R )/(VMfloat)FEET_Y_STEPS*(FEET_Y_STEPS-i); VertexData[o++] = -0.1;
               VertexData[o++] = -(FEET_W2/2.0)-FEET_Y_FUNC((VMfloat)i/(VMfloat)FEET_Y_STEPS)*(FEET_W-FEET_W2)/2.0/(tableh+balld/2.0-0.1); VertexData[o++] = -(FEET_L2/2.0-FEET_R2)/(VMfloat)FEET_Y_STEPS*(FEET_Y_STEPS-i); VertexData[o++] = -tableh-balld/2.0+FEET_Y_FUNC((VMfloat)i/(VMfloat)FEET_Y_STEPS);
           }
           glPushMatrix();
           glDrawArrays(GL_QUAD_STRIP,0,FEET_X_STEPS*2+FEET_EDGE_STEPS*2+FEET_Y_STEPS*2+1);
           glPopMatrix();
           glPopMatrix();
       }
   }
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
  // for debugging only
  /* for(i=0;i<borders->holenr;i++){
       GLUquadric * qd;
       qd=gluNewQuadric();
       glPushMatrix();
       glTranslatef( borders->hole[i].pos.x, borders->hole[i].pos.y, borders->hole[i].pos.z );
       gluSphere( qd, borders->hole[i].r, 8, 8 );
       glPopMatrix();
   } */
   glEndList();
   fprintf(stderr,"Return the new table object\n");
   return table_obj;
}
