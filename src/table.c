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
#include <GL/glu.h>
#include <GL/gl.h>
#include "billard.h"
#include "table.h"
#include "options.h"
#include "png_loader.h"
#include "bumpref.h"

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

void autonormalize_quad( VMvect v1, VMvect v2, VMvect v3, VMvect v4, int order )
{
    VMvect n;
    n=vec_unit(vec_cross(vec_diff(v2,v1),vec_diff(v3,v1)));
    glNormal3f( n.x, n.y, n.z );
    if( !order ) {
        glVertex3f(v4.x,v4.y,v4.z);
        glVertex3f(v3.x,v3.y,v3.z);
        glVertex3f(v2.x,v2.y,v2.z);
        glVertex3f(v1.x,v1.y,v1.z);
    } else {
        glVertex3f(v1.x,v1.y,v1.z);
        glVertex3f(v2.x,v2.y,v2.z);
        glVertex3f(v3.x,v3.y,v3.z);
        glVertex3f(v4.x,v4.y,v4.z);
    }
}

/***********************************************************************/

void autonormalize_triangle( VMvect v1, VMvect v2, VMvect v3, int order )
{
    VMvect n;
    n=vec_unit(vec_cross(vec_diff(v2,v1),vec_diff(v3,v1)));
    if( !order ) {
        glNormal3f( n.x, n.y, n.z );
        glVertex3f(v3.x,v3.y,v3.z);
        glNormal3f( n.x, n.y, n.z );
        glVertex3f(v2.x,v2.y,v2.z);
        glNormal3f( n.x, n.y, n.z );
        glVertex3f(v1.x,v1.y,v1.z);
    } else {
        glNormal3f( n.x, n.y, n.z );
        glVertex3f(v1.x,v1.y,v1.z);
        glNormal3f( n.x, n.y, n.z );
        glVertex3f(v2.x,v2.y,v2.z);
        glNormal3f( n.x, n.y, n.z );
        glVertex3f(v3.x,v3.y,v3.z);
    }
}

/***********************************************************************/

void autonormalize_triangle_round( VMvect v1, VMvect v2, VMvect v3, int order, VMfloat round )
{
    VMvect n0,c,n;
    n0=vec_cross(vec_diff(v2,v1),vec_diff(v3,v1));
    c=tri_center(v1,v2,v3);
    if( !order ) {
        n=vec_unit(vec_add(n0,vec_scale(vec_diff(v3,c),round))); glNormal3f( n.x, n.y, n.z );
        glVertex3f(v3.x,v3.y,v3.z);
        n=vec_unit(vec_add(n0,vec_scale(vec_diff(v2,c),round))); glNormal3f( n.x, n.y, n.z );
        glVertex3f(v2.x,v2.y,v2.z);
        n=vec_unit(vec_add(n0,vec_scale(vec_diff(v1,c),round))); glNormal3f( n.x, n.y, n.z );
        glVertex3f(v1.x,v1.y,v1.z);
    } else {
        n=vec_unit(vec_add(n0,vec_scale(vec_diff(v1,c),round))); glNormal3f( n.x, n.y, n.z );
        glVertex3f(v1.x,v1.y,v1.z);
        n=vec_unit(vec_add(n0,vec_scale(vec_diff(v2,c),round))); glNormal3f( n.x, n.y, n.z );
        glVertex3f(v2.x,v2.y,v2.z);
        n=vec_unit(vec_add(n0,vec_scale(vec_diff(v3,c),round))); glNormal3f( n.x, n.y, n.z );
        glVertex3f(v3.x,v3.y,v3.z);
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

    glBegin(GL_TRIANGLES);
       autonormalize_triangle_round(p[0],p[1],p[4],order,-0.002);
       autonormalize_triangle_round(p[1],p[2],p[4],order,-0.002);
       autonormalize_triangle_round(p[2],p[3],p[4],order,-0.002);
       autonormalize_triangle_round(p[3],p[0],p[4],order,-0.002);
    glEnd();
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
    VMvect n00,n10,n20,n30,n40,n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,v0,v1,v2,v3,v4,v5,v6,v7,v8,v9, v8_2,v8_3, v4_2,v4_3;
    n00 = vec_xyz(-1,0,0);
    n10 = vec_xyz(0,0,1);
    n20 = vec_unit(vec_xyz(FRAME_DH,0,FRAME_D-BANDE_D-FRAME_PHASE));
    n30 = vec_unit(vec_xyz(FRAME_H-FRAME_PHASE,0,-FRAME_PHASE));
    n40 = vec_unit(vec_xyz(FRAME_H-FRAME_PHASE,0,-FRAME_PHASE));
    for(i=0;i<segnr;i++){
        phi1 = M_PI/2.0*(VMfloat)i/(VMfloat)segnr;
        phi2 = M_PI/2.0*(VMfloat)(i+1)/(VMfloat)segnr;
        dphi1 = atan((r2(phi1-0.01)-r2(phi1+0.01))/r2(phi1)/0.02);
        dphi2 = atan((r2(phi2-0.01)-r2(phi2+0.01))/r2(phi2)/0.02);
        n0 = vec_rotate(n00,vec_xyz(0,0,phi1+dphi1));
        v0 = vec_xyz(r1(phi1)*cos(phi1),r1(phi1)*sin(phi1),FRAME_DH-0.006);
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
        n9 = vec_rotate(n00,vec_xyz(0,0,phi2+dphi2));
        v9 = vec_xyz(r1(phi2)*cos(phi2),r1(phi2)*sin(phi2),FRAME_DH-0.006);
        if(!order){
#define FACT 8.0
            glBegin(GL_QUAD_STRIP);
            glTexCoord2f(phi1/M_PI*2.0, 0.0*0.5);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v1.x,v1.y,v1.z);
            glTexCoord2f(phi2/M_PI*2.0, 0.0*0.5);
            glNormal3f(n5.x,n5.y,n5.z);
            glVertex3f(v5.x,v5.y,v5.z);
            glTexCoord2f(phi1/M_PI*2.0, 0.9*0.5);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v2.x,v2.y,v2.z);
            glTexCoord2f(phi2/M_PI*2.0, 0.9*0.5);
            glNormal3f(n6.x,n6.y,n6.z);
            glVertex3f(v6.x,v6.y,v6.z);
            glTexCoord2f(phi1/M_PI*2.0, 1.0*0.5);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v3.x,v3.y,v3.z);
            glTexCoord2f(phi2/M_PI*2.0, 1.0*0.5);
            glNormal3f(n7.x,n7.y,n7.z);
            glVertex3f(v7.x,v7.y,v7.z);
            glTexCoord2f(phi1/M_PI*2.0, 2.0*0.5);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4.x,v4.y,v4.z);
            glTexCoord2f(phi2/M_PI*2.0, 2.0*0.5);
            glNormal3f(n8.x,n8.y,n8.z);
            glVertex3f(v8.x,v8.y,v8.z);
            glTexCoord2f(phi1/M_PI*2.0, 2.0*0.5);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4_2.x,v4_2.y,v4_2.z);
            glTexCoord2f(phi2/M_PI*2.0, 2.0*0.5);
            glNormal3f(n8.x,n8.y,n8.z);
            glVertex3f(v8_2.x,v8_2.y,v8_2.z);
            glTexCoord2f(phi1/M_PI*2.0, 3.0*0.5);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4_3.x,v4_3.y,v4_3.z);
            glTexCoord2f(phi2/M_PI*2.0, 3.0*0.5);
            glNormal3f(n8.x,n8.y,n8.z);
            glVertex3f(v8_3.x,v8_3.y,v8_3.z);
            glEnd();
        } else {
            glBegin(GL_QUAD_STRIP);
            glTexCoord2f(1.0-phi2/M_PI*2.0, 0.0*0.5);
            glNormal3f(n5.x,n5.y,n5.z);
            glVertex3f(v5.x,v5.y,v5.z);
            glTexCoord2f(1.0-phi1/M_PI*2.0, 0.0*0.5);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v1.x,v1.y,v1.z);
            glTexCoord2f(1.0-phi2/M_PI*2.0, 0.9*0.5);
            glNormal3f(n6.x,n6.y,n6.z);
            glVertex3f(v6.x,v6.y,v6.z);
            glTexCoord2f(1.0-phi1/M_PI*2.0, 0.9*0.5);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v2.x,v2.y,v2.z);
            glTexCoord2f(1.0-phi2/M_PI*2.0, 1.0*0.5);
            glNormal3f(n7.x,n7.y,n7.z);
            glVertex3f(v7.x,v7.y,v7.z);
            glTexCoord2f(1.0-phi1/M_PI*2.0, 1.0*0.5);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v3.x,v3.y,v3.z);
            glTexCoord2f(1.0-phi2/M_PI*2.0, 2.0*0.5);
            glNormal3f(n8.x,n8.y,n8.z);
            glVertex3f(v8.x,v8.y,v8.z);
            glTexCoord2f(1.0-phi1/M_PI*2.0, 2.0*0.5);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4.x,v4.y,v4.z);
            glTexCoord2f(1.0-phi2/M_PI*2.0, 2.0*0.5);
            glNormal3f(n8.x,n8.y,n8.z);
            glVertex3f(v8_2.x,v8_2.y,v8_2.z);
            glTexCoord2f(1.0-phi1/M_PI*2.0, 2.0*0.5);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4_2.x,v4_2.y,v4_2.z);
            glTexCoord2f(1.0-phi2/M_PI*2.0, 3.0*0.5);
            glNormal3f(n8.x,n8.y,n8.z);
            glVertex3f(v8_3.x,v8_3.y,v8_3.z);
            glTexCoord2f(1.0-phi1/M_PI*2.0, 3.0*0.5);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4_3.x,v4_3.y,v4_3.z);
            glEnd();
        }
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
            glBegin(GL_QUAD_STRIP);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v1.x,v1.y,v1.z);
            glNormal3f(n5.x,n5.y,n5.z);
            glVertex3f(v5.x,v5.y,v5.z);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v2.x,v2.y,v2.z);
            glNormal3f(n6.x,n6.y,n6.z);
            glVertex3f(v6.x,v6.y,v6.z);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v3.x,v3.y,v3.z);
            glNormal3f(n7.x,n7.y,n7.z);
            glVertex3f(v7.x,v7.y,v7.z);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4.x,v4.y,v4.z);
            glNormal3f(n8.x,n8.y,n8.z);
            glVertex3f(v8.x,v8.y,v8.z);
            glEnd();
        } else {
            glBegin(GL_QUAD_STRIP);
            glNormal3f(n5.x,n5.y,n5.z);
            glVertex3f(v5.x,v5.y,v5.z);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v1.x,v1.y,v1.z);
            glNormal3f(n6.x,n6.y,n6.z);
            glVertex3f(v6.x,v6.y,v6.z);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v2.x,v2.y,v2.z);
            glNormal3f(n7.x,n7.y,n7.z);
            glVertex3f(v7.x,v7.y,v7.z);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v3.x,v3.y,v3.z);
            glNormal3f(n8.x,n8.y,n8.z);
            glVertex3f(v8.x,v8.y,v8.z);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4.x,v4.y,v4.z);
            glEnd();
        }
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
            glBegin(GL_QUAD_STRIP);
            glTexCoord2f((y1/l+0.5)*0.4*tex_x, 0.0*0.5*tex_y);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v1.x,v1.y,v1.z);
            glTexCoord2f((y2/l+0.5)*0.4*tex_x, 0.0*0.5*tex_y);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v5.x,v5.y,v5.z);
            glTexCoord2f((y1/l+0.5)*0.4*tex_x, 0.9*0.5*tex_y);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v2.x,v2.y,v2.z);
            glTexCoord2f((y2/l+0.5)*0.4*tex_x, 0.9*0.5*tex_y);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v6.x,v6.y,v6.z);
            glTexCoord2f((y1/l+0.5)*0.4*tex_x, 1.0*0.5*tex_y);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v3.x,v3.y,v3.z);
            glTexCoord2f((y2/l+0.5)*0.4*tex_x, 1.0*0.5*tex_y);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v7.x,v7.y,v7.z);
            glTexCoord2f((y1/l+0.5)*0.4*tex_x, 2.0*0.5*tex_y);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4.x,v4.y,v4.z);
            glTexCoord2f((y2/l+0.5)*0.4*tex_x, 2.0*0.5*tex_y);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v8.x,v8.y,v8.z);
            glTexCoord2f((y1/l+0.5)*0.4*tex_x, 3.0*0.5*tex_y);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4_2.x,v4_2.y,v4_2.z);
            glTexCoord2f((y2/l+0.5)*0.4*tex_x, 3.0*0.5*tex_y);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v8_2.x,v8_2.y,v8_2.z);
            glTexCoord2f((y1/l+0.5)*0.4*tex_x, 3.0*0.5*tex_y);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4_3.x,v4_3.y,v4_3.z);
            glTexCoord2f((y2/l+0.5)*0.4*tex_x, 3.0*0.5*tex_y);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v8_3.x,v8_3.y,v8_3.z);
            glEnd();
        } else {
            glBegin(GL_QUAD_STRIP);
            glTexCoord2f((y2/l+0.5)*0.4*tex_x, 0.0*0.5*tex_y);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v5.x,v5.y,v5.z);
            glTexCoord2f((y1/l+0.5)*0.4*tex_x, 0.0*0.5*tex_y);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v1.x,v1.y,v1.z);
            glTexCoord2f((y2/l+0.5)*0.4*tex_x, 0.9*0.5*tex_y);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v6.x,v6.y,v6.z);
            glTexCoord2f((y1/l+0.5)*0.4*tex_x, 0.9*0.5*tex_y);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v2.x,v2.y,v2.z);
            glTexCoord2f((y2/l+0.5)*0.4*tex_x, 1.0*0.5*tex_y);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v7.x,v7.y,v7.z);
            glTexCoord2f((y1/l+0.5)*0.4*tex_x, 1.0*0.5*tex_y);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v3.x,v3.y,v3.z);
            glTexCoord2f((y2/l+0.5)*0.4*tex_x, 2.0*0.5*tex_y);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v8.x,v8.y,v8.z);
            glTexCoord2f((y1/l+0.5)*0.4*tex_x, 2.0*0.5*tex_y);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4.x,v4.y,v4.z);
            glTexCoord2f((y2/l+0.5)*0.4*tex_x, 3.0*0.5*tex_y);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v8_2.x,v8_2.y,v8_2.z);
            glTexCoord2f((y1/l+0.5)*0.4*tex_x, 3.0*0.5*tex_y);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4_2.x,v4_2.y,v4_2.z);
            glTexCoord2f((y2/l+0.5)*0.4*tex_x, 3.0*0.5*tex_y);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v8_3.x,v8_3.y,v8_3.z);
            glTexCoord2f((y1/l+0.5)*0.4*tex_x, 3.0*0.5*tex_y);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4_3.x,v4_3.y,v4_3.z);
            glEnd();
        }
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
            glBegin(GL_QUAD_STRIP);
            glTexCoord2f(v1.y*txfact,v1.x*tyfact);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v1.x,v1.y,v1.z);
            glTexCoord2f(v5.y*txfact,v5.x*tyfact);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v5.x,v5.y,v5.z);
            glTexCoord2f(v2.y*txfact,v2.x*tyfact);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v2.x,v2.y,v2.z);
            glTexCoord2f(v6.y*txfact,v6.x*tyfact);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v6.x,v6.y,v6.z);
            glTexCoord2f(v3.y*txfact,v3.x*tyfact);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v3.x,v3.y,v3.z);
            glTexCoord2f(v7.y*txfact,v7.x*tyfact);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v7.x,v7.y,v7.z);
            glTexCoord2f(v4.y*txfact,v4.x*tyfact);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4.x,v4.y,v4.z);
            glTexCoord2f(v8.y*txfact,v8.x*tyfact);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v8.x,v8.y,v8.z);
            glTexCoord2f(v4_2.y*txfact,v4_2.x*tyfact);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4_2.x,v4_2.y,v4_2.z);
            glTexCoord2f(v8_2.y*txfact,v8_2.x*tyfact);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v8_2.x,v8_2.y,v8_2.z);
            glTexCoord2f(v4_3.y*txfact,v4_3.x*tyfact);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4_3.x,v4_3.y,v4_3.z);
            glTexCoord2f(v8_3.y*txfact,v8_3.x*tyfact);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v8_3.x,v8_3.y,v8_3.z);
            glEnd();
        } else {
            glBegin(GL_QUAD_STRIP);
            glTexCoord2f(v5.y*txfact,v5.x*tyfact);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v5.x,v5.y,v5.z);
            glTexCoord2f(v1.y*txfact,v1.x*tyfact);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v1.x,v1.y,v1.z);
            glTexCoord2f(v6.y*txfact,v6.x*tyfact);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v6.x,v6.y,v6.z);
            glTexCoord2f(v2.y*txfact,v2.x*tyfact);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v2.x,v2.y,v2.z);
            glTexCoord2f(v7.y*txfact,v7.x*tyfact);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v7.x,v7.y,v7.z);
            glTexCoord2f(v3.y*txfact,v3.x*tyfact);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v3.x,v3.y,v3.z);
            glTexCoord2f(v8.y*txfact,v8.x*tyfact);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v8.x,v8.y,v8.z);
            glTexCoord2f(v4.y*txfact,v4.x*tyfact);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4.x,v4.y,v4.z);
            glTexCoord2f(v8_2.y*txfact,v8_2.x*tyfact);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v8_2.x,v8_2.y,v8_2.z);
            glTexCoord2f(v4_2.y*txfact,v4_2.x*tyfact);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4_2.x,v4_2.y,v4_2.z);
            glTexCoord2f(v8_3.y*txfact,v8_3.x*tyfact);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v8_3.x,v8_3.y,v8_3.z);
            glTexCoord2f(v4_3.y*txfact,v4_3.x*tyfact);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4_3.x,v4_3.y,v4_3.z);
            glEnd();
        }
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
            glBegin(GL_QUAD_STRIP);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v1.x,v1.y,v1.z);
            glNormal3f(n5.x,n5.y,n5.z);
            glVertex3f(v5.x,v5.y,v5.z);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v2.x,v2.y,v2.z);
            glNormal3f(n6.x,n6.y,n6.z);
            glVertex3f(v6.x,v6.y,v6.z);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v3.x,v3.y,v3.z);
            glNormal3f(n7.x,n7.y,n7.z);
            glVertex3f(v7.x,v7.y,v7.z);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4.x,v4.y,v4.z);
            glNormal3f(n8.x,n8.y,n8.z);
            glVertex3f(v8.x,v8.y,v8.z);
            glEnd();
        } else {
            glBegin(GL_QUAD_STRIP);
            glNormal3f(n5.x,n5.y,n5.z);
            glVertex3f(v5.x,v5.y,v5.z);
            glNormal3f(n1.x,n1.y,n1.z);
            glVertex3f(v1.x,v1.y,v1.z);
            glNormal3f(n6.x,n6.y,n6.z);
            glVertex3f(v6.x,v6.y,v6.z);
            glNormal3f(n2.x,n2.y,n2.z);
            glVertex3f(v2.x,v2.y,v2.z);
            glNormal3f(n7.x,n7.y,n7.z);
            glVertex3f(v7.x,v7.y,v7.z);
            glNormal3f(n3.x,n3.y,n3.z);
            glVertex3f(v3.x,v3.y,v3.z);
            glNormal3f(n8.x,n8.y,n8.z);
            glVertex3f(v8.x,v8.y,v8.z);
            glNormal3f(n4.x,n4.y,n4.z);
            glVertex3f(v4.x,v4.y,v4.z);
            glEnd();
        }
/*      for debugging
        glBegin(GL_LINES);
            glVertex3f(v1.x,v1.y,v1.z); v1=vec_add(v1,vec_scale(n1,0.01)); glVertex3f(v1.x,v1.y,v1.z);
            glVertex3f(v2.x,v2.y,v2.z); v2=vec_add(v2,vec_scale(n2,0.01)); glVertex3f(v2.x,v2.y,v2.z);
            glVertex3f(v3.x,v3.y,v3.z); v3=vec_add(v3,vec_scale(n3,0.01)); glVertex3f(v3.x,v3.y,v3.z);
            glVertex3f(v4.x,v4.y,v4.z); v4=vec_add(v4,vec_scale(n4,0.01)); glVertex3f(v4.x,v4.y,v4.z);
        glEnd();*/
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

#define BANDE_WULST 0.006

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

    glBegin(GL_QUADS);
       autonormalize_quad(p[10],p[11],p[9],p[8],order);
       autonormalize_quad(p[5],p[6],p[2],p[1],order);
       if(!order){
           glNormal3f(0,-sin1,cos1);
           glVertex3f(p[10].x,p[10].y,p[10].z);
           glNormal3f(0,-sin1,cos1);
           glVertex3f(p[11].x,p[11].y,p[11].z);
           glNormal3f(0,-cos2,-sin2);
           glVertex3f(p[2].x,p[2].y,p[2].z);
           glNormal3f(0,-cos2,-sin2);
           glVertex3f(p[1].x,p[1].y,p[1].z);
       }else{
           glNormal3f(0,-cos2,-sin2);
           glVertex3f(p[1].x,p[1].y,p[1].z);
           glNormal3f(0,-cos2,-sin2);
           glVertex3f(p[2].x,p[2].y,p[2].z);
           glNormal3f(0,-sin1,cos1);
           glVertex3f(p[11].x,p[11].y,p[11].z);
           glNormal3f(0,-sin1,cos1);
           glVertex3f(p[10].x,p[10].y,p[10].z);
       }
    glEnd();

    glBegin(GL_TRIANGLES);
       autonormalize_triangle(p[1],p[10],p[0],order);
       autonormalize_triangle(p[2],p[3],p[11],order);

       autonormalize_triangle(p[0],p[10],p[8],order);
       autonormalize_triangle(p[0],p[5],p[1],order);
       autonormalize_triangle(p[0],p[4],p[5],order);
       autonormalize_triangle(p[3],p[9],p[11],order);
       autonormalize_triangle(p[3],p[2],p[6],order);
       autonormalize_triangle(p[3],p[6],p[7],order);
    glEnd();
}

/***********************************************************************/

#define TABLETEXCOORD_X(x,y) (-0.7+(y+TABLE_L/2.0)/TABLE_L*2.4-0.2+(x+TABLE_W/2.0)/TABLE_W*0.4)
#define TABLETEXCOORD_Y(x,y) (-0.2+(x+TABLE_W/2.0)/TABLE_W*1.4)

   /* holes-tuch */
void my_HoleTuch( int xfact, int yfact )
{
       int i;
       VMfloat x,y,phi;
       VMfloat edge_xyoffs;
       edge_xyoffs = HOLE1_R*SQR2-BANDE_D;

       glBegin(GL_TRIANGLES);

       x=-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2; y=-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2;
       x*=xfact; y*=yfact;
       glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
       glNormal3f( 0.0, 0.0, 1.0 );
       glVertex3f(x,y,-BALL_D/2.0);
       x=-TABLE_W/2.0-BANDE_D; y=-TABLE_L/2.0+edge_xyoffs;
       x*=xfact; y*=yfact;
       glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
       glNormal3f( 0.0, 0.0, 1.0 );
       glVertex3f(x,y,-BALL_D/2.0);
       x=-TABLE_W/2.0-BANDE_D*BANDE_D2RATIO; y=-TABLE_L/2.0+edge_xyoffs+BANDE_D*HOLE1_TAN;
       x*=xfact; y*=yfact;
       glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
       glNormal3f( 0.0, 0.0, 1.0 );
       glVertex3f(x,y,-BALL_D/2.0);

       x=-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2; y=-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2;
       x*=xfact; y*=yfact;
       glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
       glNormal3f( 0.0, 0.0, 1.0 );
       glVertex3f(x,y,-BALL_D/2.0);
       x=-TABLE_W/2.0+edge_xyoffs+BANDE_D*HOLE1_TAN; y=-TABLE_L/2.0-BANDE_D*BANDE_D2RATIO;
       x*=xfact; y*=yfact;
       glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
       glNormal3f( 0.0, 0.0, 1.0 );
       glVertex3f(x,y,-BALL_D/2.0);
       x=-TABLE_W/2.0+edge_xyoffs; y=-TABLE_L/2.0-BANDE_D;
       x*=xfact; y*=yfact;
       glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
       glNormal3f( 0.0, 0.0, 1.0 );
       glVertex3f(x,y,-BALL_D/2.0);

       glEnd();

#define  HOLE1_SEGNR_4 8
       glBegin(GL_TRIANGLE_FAN);
       x=-TABLE_W/2.0-BANDE_D; y=-TABLE_L/2.0+edge_xyoffs;
       x*=xfact; y*=yfact;
       glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
       glNormal3f( 0.0, 0.0, 1.0 );
       glVertex3f(x,y,-BALL_D/2.0);
       for(i=0;i<HOLE1_SEGNR_4+1;i++){
           phi=M_PI/4.0+(VMfloat)i*M_PI/HOLE1_SEGNR_4/2.0;
           x=-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R*cos(phi);
           y=-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R*sin(phi);
           x*=xfact; y*=yfact;
           glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
           glNormal3f( 0.0, 0.0, 1.0 );
           glVertex3f(x,y,-BALL_D/2.0);
       }
       glEnd();

       glBegin(GL_TRIANGLE_FAN);
       x=-TABLE_W/2.0+edge_xyoffs; y=-TABLE_L/2.0-BANDE_D;
       x*=xfact; y*=yfact;
       glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
       glNormal3f( 0.0, 0.0, 1.0 );
       glVertex3f(x,y,-BALL_D/2.0);
       for(i=HOLE1_SEGNR_4;i>=0;i--){
           phi=M_PI/4.0+(VMfloat)i*M_PI/HOLE1_SEGNR_4/2.0;
           x=-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R*sin(phi);
           y=-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R*cos(phi);
           x*=xfact; y*=yfact;
           glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
           glNormal3f( 0.0, 0.0, 1.0 );
           glVertex3f(x,y,-BALL_D/2.0);
       }
       glEnd();

       glBegin(GL_QUAD_STRIP);
       for(i=-HOLE1_SEGNR_4;i<HOLE1_SEGNR_4+1;i++){
           phi=M_PI/4.0+(VMfloat)i*M_PI/HOLE1_SEGNR_4/2.0;
           x=-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R*cos(phi);
           y=-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R*sin(phi);
           x*=xfact; y*=yfact;
           glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
           glNormal3f( 0.0, 0.0, 1.0 );
           glVertex3f(x,y,-BALL_D/2.0);
           x=-TABLE_W/2.0-HOLE1_XYOFFS+(HOLE1_R-0.005)*cos(phi);
           y=-TABLE_L/2.0-HOLE1_XYOFFS+(HOLE1_R-0.005)*sin(phi);
           x*=xfact; y*=yfact;
           glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
           glNormal3f( -cos(phi)*xfact, -sin(phi)*yfact, 0.0 );
           glVertex3f(x,y,-BALL_D/2.0-0.005);
       }
       glEnd();
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

int create_table( int reflect_bind, BordersType *borders, int carambol )
{
    static int init = 0;
    static int bumpref_init = 0;
    static int bump_init = 0;
    static BumpRefType bumpref;
    static BumpRefType bumponly;
    static int table_obj=-1;
    static GLuint frametexbind=-1;
    static GLuint floortexbind=-1;
    static GLuint tabletexbind=-1;
    static GLuint clothtexbind=-1;
    int frametexw,frametexh;
    int tabletexw,tabletexh;
    int clothtexw,clothtexh;
    int depth;
    int flip;
    char * frametexdata;
    char * tabletexdata;
    char * clothtexdata;
    VMfloat balld  = BALL_D;
    VMfloat tablew = TABLE_W;
    VMfloat tablel = TABLE_L;
    VMfloat tableh = TABLE_H;
    VMfloat bande_d = BANDE_D;
//    VMfloat cm  =  0.01;  // cm (for debugging in function my_glBox)
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

    int imax,jmax,i,j,k,l;
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
    if( frametexbind > 0 ) glDeleteTextures( 1, &frametexbind );
    glGenTextures(1,&frametexbind);
    load_png("table-frame.png",&frametexw,&frametexh,&depth,&frametexdata);
    glBindTexture(GL_TEXTURE_2D,frametexbind);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, frametexw, frametexh, GL_RGB, GL_UNSIGNED_BYTE, frametexdata);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options_tex_min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options_tex_mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if(options_anisotrop && options_value_anisotrop > 0.0) {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, options_value_anisotrop);
    }
    free( frametexdata );

    if( floortexbind > 0 ) glDeleteTextures( 1, &floortexbind );
    glGenTextures(1,&floortexbind);
    load_png("floor.png",&frametexw,&frametexh,&depth,&frametexdata);
    glBindTexture(GL_TEXTURE_2D,floortexbind);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, frametexw, frametexh, GL_RGB, GL_UNSIGNED_BYTE, frametexdata);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options_tex_min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options_tex_mag_filter);
    if(options_anisotrop && options_value_anisotrop > 0.0) {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, options_value_anisotrop);
    }
    free( frametexdata );

    if( tabletexbind > 0 ) glDeleteTextures( 1, &tabletexbind );
    glGenTextures(1,&tabletexbind);
#ifdef WETAB
    load_png("tabletex_wetab_256x256.png",&tabletexw,&tabletexh,&depth,&tabletexdata);
#else
    load_png("tabletex_fB_256x256.png",&tabletexw,&tabletexh,&depth,&tabletexdata);
#endif
    glBindTexture(GL_TEXTURE_2D,tabletexbind);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 1, tabletexw, tabletexh, GL_LUMINANCE, GL_UNSIGNED_BYTE, tabletexdata);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options_tex_min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options_tex_mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    if(options_anisotrop && options_value_anisotrop > 0.0) {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, options_value_anisotrop);
    }
    free( tabletexdata );

    if( clothtexbind > 0 ) glDeleteTextures( 1, &clothtexbind );
        glGenTextures(1,&clothtexbind);
        load_png("cloth.png",&clothtexw,&clothtexh,&depth,&clothtexdata);
        glBindTexture(GL_TEXTURE_2D,clothtexbind);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 1, clothtexw, clothtexh, GL_LUMINANCE,
                          GL_UNSIGNED_BYTE, clothtexdata);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options_tex_min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options_tex_mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if(options_anisotrop && options_value_anisotrop > 0.0) {
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, options_value_anisotrop);
        }
        free( clothtexdata );


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
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   if(carambol){
       tablew=TABLE_W+2.0*BANDE_D;
       tablel=TABLE_L+2.0*BANDE_D;
       area_w = TABLE_W+2.0*BANDE_D;
       area_l = TABLE_L+2.0*BANDE_D;
   } else {
       area_w = TABLE_W-0.07;
       area_l = TABLE_L-0.07;
   }

/* AREA_SUBDIV_Y and AREA_SUBDIV_X have to be even */
#define AREA_SUBDIV_Y 6
#define AREA_SUBDIV_X 4
   jmax=AREA_SUBDIV_Y; imax=AREA_SUBDIV_X;
   for(j=0;j<jmax;j++){
       for(i=0;i<imax;i++){
           VMfloat x,y;
           glBegin(GL_QUADS);
           x=-area_w/2.0+i*area_w/imax; y=-area_l/2.0+j*area_l/jmax;
           glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
           glNormal3f( 0.0, 0.0, 1.0 );
           glVertex3f( x,y, -balld/2.0);
           x=-area_w/2.0+i*area_w/imax; y=-area_l/2.0+(j+1)*area_l/jmax;
           glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
           glNormal3f( 0.0, 0.0, 1.0 );
           glVertex3f( x,y, -balld/2.0);
           x=-area_w/2.0+(i+1)*area_w/imax; y=-area_l/2.0+(j+1)*area_l/jmax;
           glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
           glNormal3f( 0.0, 0.0, 1.0 );
           glVertex3f( x,y, -balld/2.0);
           x=-area_w/2.0+(i+1)*area_w/imax; y=-area_l/2.0+j*area_l/jmax;
           glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
           glNormal3f( 0.0, 0.0, 1.0 );
           glVertex3f( x,y, -balld/2.0);

           glEnd();
       }
   }
   if(carambol){
       tablew=TABLE_W;
       tablel=TABLE_L;
   }

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
   if(!carambol){
#define TABLEVERTEX(x,y,z,f1,f2) \
             glNormal3f(0.0,0.0,1.0); \
             glTexCoord2f( TABLETEXCOORD_X((x)*(f1),(y)*(f2)), TABLETEXCOORD_Y((x)*(f1),(y)*(f2)) ); \
             glVertex3f((x)*(f1),(y)*(f2),z);
   /*lower, upper*/
       for(i=0;i<2;i++){
           glFrontFace(i==0?GL_CW:GL_CCW);
           glBegin(GL_QUAD_STRIP);
           TABLEVERTEX( area_w/2.0,                                -area_l/2.0,                        -BALL_D/2.0, 1.0, i==0?1.0:-1.0 );
           TABLEVERTEX( TABLE_W/2.0-edge_xyoffs-BANDE_D*HOLE1_TAN, -TABLE_L/2.0-BANDE_D*BANDE_D2RATIO, -BALL_D/2.0, 1.0, i==0?1.0:-1.0 );
           for(j=0;j<AREA_SUBDIV_X-1;j++){
               TABLEVERTEX( area_w/2.0-area_w*(j+1)/AREA_SUBDIV_X, -area_l/2.0,                        -BALL_D/2.0, 1.0, i==0?1.0:-1.0 );
               TABLEVERTEX( area_w/2.0-area_w*(j+1)/AREA_SUBDIV_X, -TABLE_L/2.0-BANDE_D*BANDE_D2RATIO, -BALL_D/2.0, 1.0, i==0?1.0:-1.0 );
           }
           TABLEVERTEX( area_w/2.0,                                -area_l/2.0,                        -BALL_D/2.0, -1.0, i==0?1.0:-1.0 );
           TABLEVERTEX( TABLE_W/2.0-edge_xyoffs-BANDE_D*HOLE1_TAN, -TABLE_L/2.0-BANDE_D*BANDE_D2RATIO, -BALL_D/2.0, -1.0, i==0?1.0:-1.0 );
           glEnd();
       }

       /* some middle pocket triangles */
       for(i=0;i<2;i++){
           glFrontFace(i==0?GL_CW:GL_CCW);
           glBegin(GL_TRIANGLES);
           TABLEVERTEX( -area_w/2.0,                                  0.0,                              -BALL_D/2.0, i==0?1.0:-1.0, 1.0 );
           TABLEVERTEX( -TABLE_W/2.0-BANDE_D*BANDE_D2RATIO,          -HOLE2_R-HOLE2_TAN*BANDE_D,        -BALL_D/2.0, i==0?1.0:-1.0, 1.0 );
           TABLEVERTEX( -TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R,            0.0,                              -BALL_D/2.0, i==0?1.0:-1.0, 1.0 );

           TABLEVERTEX( -area_w/2.0,                                  0.0,                              -BALL_D/2.0, i==0?1.0:-1.0, 1.0 );
           TABLEVERTEX( -TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R,            0.0,                              -BALL_D/2.0, i==0?1.0:-1.0, 1.0 );
           TABLEVERTEX( -TABLE_W/2.0-BANDE_D*BANDE_D2RATIO,          +HOLE2_R+HOLE2_TAN*BANDE_D,        -BALL_D/2.0, i==0?1.0:-1.0, 1.0 );

           glEnd();
       }

       /* lower left, lower right, upper right, upper left */
       for(i=0;i<4;i++){
           fx=0.0;
           fy=0.0;
           switch(i){
           case 0: fx=+1.0; fy=+1.0; glFrontFace(GL_CW);  break;  /* lower left  */
           case 1: fx=-1.0; fy=+1.0; glFrontFace(GL_CCW); break;  /* lower right */
           case 2: fx=-1.0; fy=-1.0; glFrontFace(GL_CW);  break;  /* upper right */
           case 3: fx=+1.0; fy=-1.0; glFrontFace(GL_CCW); break;  /* upper left  */
           }

           glBegin(GL_QUAD_STRIP);
           /* furchen */
           TABLEVERTEX( -TABLE_W/2.0-BANDE_D2RATIO*BANDE_D, -HOLE1_R-HOLE2_TAN*BANDE_D,                 -BALL_D/2.0, fx,fy );
           TABLEVERTEX( -area_w/2.0,                        0.0,                                        -BALL_D/2.0, fx,fy );
           for(j=0;j<AREA_SUBDIV_Y/2-1;j++){
               TABLEVERTEX( -TABLE_W/2.0-BANDE_D2RATIO*BANDE_D, -area_l*(j+1)/AREA_SUBDIV_Y,            -BALL_D/2.0, fx,fy );
               TABLEVERTEX( -area_w/2.0,                        -area_l*(j+1)/AREA_SUBDIV_Y,            -BALL_D/2.0, fx,fy );
           }
           TABLEVERTEX( -TABLE_W/2.0-BANDE_D2RATIO*BANDE_D, -TABLE_L/2.0+edge_xyoffs+HOLE1_TAN*BANDE_D, -BALL_D/2.0, fx,fy );
           TABLEVERTEX( -area_w/2.0,                        -area_l/2.0,                                -BALL_D/2.0, fx,fy );
           glEnd();

           /* quad between area-edge and hole */
           glBegin(GL_TRIANGLES);
           TABLEVERTEX( -area_w/2.0,                            -area_l/2.0,                                -BALL_D/2.0, fx,fy );
           TABLEVERTEX( -TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2, -TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2,     -BALL_D/2.0, fx,fy );
           TABLEVERTEX( -TABLE_W/2.0-BANDE_D2RATIO*BANDE_D,     -TABLE_L/2.0+edge_xyoffs+HOLE1_TAN*BANDE_D, -BALL_D/2.0, fx,fy );

           TABLEVERTEX( -area_w/2.0,                            -area_l/2.0,                                -BALL_D/2.0, fx,fy );
           TABLEVERTEX( -TABLE_W/2.0+edge_xyoffs+HOLE1_TAN*BANDE_D, -TABLE_L/2.0-BANDE_D2RATIO*BANDE_D,     -BALL_D/2.0, fx,fy );
           TABLEVERTEX( -TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2, -TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R/SQR2,     -BALL_D/2.0, fx,fy );

           glEnd();
       }

#define HOLE2_SEGNR_2 12
       for(k=0;k<2;k++){  /* left, right hole */
           if(k==0) glFrontFace(GL_CCW); else glFrontFace(GL_CW);
           glBegin(GL_QUAD_STRIP);
           for(i=0;i<HOLE2_SEGNR_2+1;i++){
               phi=(VMfloat)i*M_PI/HOLE2_SEGNR_2;
               x=-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R*sin(phi);
               y=HOLE2_R*cos(phi);
               if(k!=0) x=-x;
               glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
               glNormal3f( 0.0, 0.0, 1.0 );
               glVertex3f( x,y, -BALL_D/2.0 );
               x=-TABLE_W/2.0-HOLE2_XYOFFS+(HOLE2_R-HOLE2_PHASE)*sin(phi);
               y=(HOLE2_R-HOLE2_PHASE)*cos(phi);
               if(k!=0) x=-x;
               glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
               glNormal3f( -sin(phi), (k==0)?-cos(phi):cos(phi), 0.0 );
               glVertex3f( x,y, -BALL_D/2.0-HOLE2_PHASE );
           }
           glEnd();
       }

       for(k=0;k<2;k++){  /* left, right hole fans */
           for(j=0;j<2;j++){ /* fan 1, 2 */
               if(j^k) glFrontFace(GL_CCW); else glFrontFace(GL_CW);
               glBegin(GL_TRIANGLE_FAN);
               glNormal3f( 0.0, 0.0, 1.0 );
               x=-TABLE_W/2.0-BANDE_D*BANDE_D2RATIO; y=-HOLE2_R-BANDE_D*HOLE2_TAN;
               if(j!=0) y=-y;
               if(k!=0) x=-x;
               glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
               glVertex3f( x, y, -BALL_D/2.0 );
               for(i=0;i<HOLE2_SEGNR_2/2+1;i++){
                   phi=(VMfloat)i*M_PI/HOLE2_SEGNR_2;
                   x=-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R*sin(phi);
                   y=-HOLE2_R*cos(phi);
                   if(j!=0) y=-y;
                   if(k!=0) x=-x;
                   glTexCoord2f( TABLETEXCOORD_X(x,y), TABLETEXCOORD_Y(x,y) );
                   glNormal3f( 0.0, 0.0, 1.0 );
                   glVertex3f( x,y, -BALL_D/2.0 );
               }
               glEnd();
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

   if(! carambol){
       /* upper */
       my_Bande( -tablew/2.0+hole_r1, tablel/2.0,        0.0,
                 +tablew/2.0-hole_r1, tablel/2.0+bande_d, -balld/2.0,
                 HOLE1_TAN, HOLE1_TAN, 0 );

       /* lower */
       glPushMatrix();
       glScalef(1.0,-1.0,1.0);
       my_Bande( -tablew/2.0+hole_r1, tablel/2.0,        0.0,
                 +tablew/2.0-hole_r1, tablel/2.0+bande_d, -balld/2.0,
                 HOLE1_TAN, HOLE1_TAN, 1 );
       glPopMatrix();

       glPushMatrix();
       glRotatef( 90.0, 0.0,0.0,1.0 );
       /* upper left */
       my_Bande( hole_r2,         tablew/2.0,        0.0,
                 +tablew-hole_r1, tablew/2.0+bande_d, -balld/2.0,
                 HOLE2_TAN, HOLE1_TAN, 0 );
       glScalef(1.0,-1.0,1.0);
       /* upper right */
       my_Bande( hole_r2,         tablew/2.0,        0.0,
                 +tablew-hole_r1, tablew/2.0+bande_d, -balld/2.0,
                 HOLE2_TAN, HOLE1_TAN, 1 );
       glPopMatrix();

       glPushMatrix();
       glRotatef( 90.0, 0.0,0.0,1.0 );

       /* lower left */
       my_Bande( -tablew+hole_r1,         tablew/2.0,        0.0,
                 -hole_r2, tablew/2.0+bande_d, -balld/2.0,
                 HOLE1_TAN, HOLE2_TAN, 0 );
       /* lower right */
       glScalef(1.0,-1.0,1.0);
       my_Bande( -tablew+hole_r1,         tablew/2.0,        0.0,
                 -hole_r2, tablew/2.0+bande_d, -balld/2.0,
                 HOLE1_TAN, HOLE2_TAN, 1 );
       glPopMatrix();
   } else {
       /* upper */
       my_Bande( -tablew/2.0, tablel/2.0,         0.0,
                 +tablew/2.0, tablel/2.0+bande_d, -balld/2.0,
                 1.0, 1.0, 0 );

       /* lower */
       glPushMatrix();
       glScalef(1.0,-1.0,1.0);
       my_Bande( -tablew/2.0, tablel/2.0,        0.0,
                 +tablew/2.0, tablel/2.0+bande_d, -balld/2.0,
                 1.0, 1.0, 1 );
       glPopMatrix();

       glPushMatrix();
       glRotatef( 90.0, 0.0,0.0,1.0 );

       /* upper left */
       my_Bande( 0.0,      tablew/2.0,         0.0,
                 +tablew,  tablew/2.0+bande_d, -balld/2.0,
                 0.0, 1.0, 0 );
       /* upper right */
       glScalef(1.0,-1.0,1.0);
       my_Bande( 0.0,      tablew/2.0,         0.0,
                 +tablew,  tablew/2.0+bande_d, -balld/2.0,
                 0.0, 1.0, 1 );

       glPopMatrix();

       glPushMatrix();
       glRotatef( 90.0, 0.0,0.0,1.0 );

       /* lower left */
       my_Bande( -tablew,  tablew/2.0,         0.0,
                 0.0,      tablew/2.0+bande_d, -balld/2.0,
                 1.0, 0.0, 0 );
       /* lower right */
       glScalef(1.0,-1.0,1.0);
       my_Bande( -tablew,  tablew/2.0,         0.0,
                 0.0,      tablew/2.0+bande_d, -balld/2.0,
                 1.0, 0.0, 1 );
       glPopMatrix();
   }

   /* disable 2nd tex unit for cloth texture */
       glActiveTextureARB(GL_TEXTURE1_ARB);
       glDisable(GL_TEXTURE_2D);
       glActiveTextureARB(GL_TEXTURE0_ARB);

   /* diamonds */
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
       glPushMatrix();
       glTranslatef( tablew/2.0+FRAME_D/2.0-FRAME_PHASE/2.0+bande_d/2.0, 1.0*tablel/8.0, FRAME_DH/2.0 );
       glRotatef( atan(FRAME_DH/(FRAME_D-bande_d-FRAME_PHASE))*180.0/M_PI, 0.0, 1.0, 0.0 );
       my_Diamondxy( 0.02, 0.014, 0.006, flip );
       glPopMatrix();
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
       glPushMatrix();
       glTranslatef( -tablew/2.0+3.0*tablew/4.0, tablel/2.0+FRAME_D/2.0-FRAME_PHASE/2.0+bande_d/2.0, FRAME_DH/2.0 );
       glRotatef( -atan(FRAME_DH/(FRAME_D-bande_d-FRAME_PHASE))*180.0/M_PI, 1.0, 0.0, 0.0 );
       my_Diamondxy( 0.014, 0.02, 0.006, flip );
       glPopMatrix();
       glPopMatrix();
   }

   /* gold edges and covers */
   if(!carambol){
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
       for(j=0;j<4;j++){ /* 4 edge holes */
           xf=0.0;
           yf=0.0;
           switch(j){
           case 0: glFrontFace(GL_CCW); xf=+1.0; yf=+1.0; break;
           case 1: glFrontFace(GL_CW);  xf=-1.0; yf=+1.0; break;
           case 2: glFrontFace(GL_CW);  xf=+1.0; yf=-1.0; break;
           case 3: glFrontFace(GL_CCW); xf=-1.0; yf=-1.0; break;
           }
           glBegin(GL_QUAD_STRIP);
           glNormal3f( (-1.0/SQR2)*xf,(1.0/SQR2)*yf,0.0 );
           glVertex3f( (-TABLE_W/2.0+edge_xyoffs)*xf, (-TABLE_L/2.0-BANDE_D)*yf,  FRAME_DH );
           glNormal3f( (0)*xf, (0)*yf,-1.0 );
           glVertex3f( (-TABLE_W/2.0+edge_xyoffs)*xf, (-TABLE_L/2.0-BANDE_D)*yf, -0.1 );
           for( i=0 ; i<HOLE1_BACKSEGNR+1 ; i++ ){
               VMfloat phi;
               phi = -M_PI/4.0-M_PI*(VMfloat)i/(VMfloat)HOLE1_BACKSEGNR;
               glNormal3f( (-cos(phi))*xf,(-sin(phi))*yf,0.0 );
               glVertex3f( (-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R*cos(phi))*xf, (-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R*sin(phi))*yf, FRAME_DH );
               glNormal3f( (0)*xf, (0)*yf,-1.0 );
               glVertex3f( (-TABLE_W/2.0-HOLE1_XYOFFS+HOLE1_R*cos(phi))*xf, (-TABLE_L/2.0-HOLE1_XYOFFS+HOLE1_R*sin(phi))*yf, -0.1 );
           }
           glNormal3f( (1.0/SQR2)*xf,(-1.0/SQR2)*yf,0.0 );
           glVertex3f( (-TABLE_W/2.0-BANDE_D)*xf, (-TABLE_L/2.0+edge_xyoffs)*yf,  FRAME_DH );
           glNormal3f( (0)*xf,(0)*yf,-1.0 );
           glVertex3f( (-TABLE_W/2.0-BANDE_D)*xf, (-TABLE_L/2.0+edge_xyoffs)*yf,  -0.1 );
           glEnd();
           /* black ground of hole */
           glBegin(GL_QUADS);
           glVertex3f( (-TABLE_W/2.0-HOLE1_XYOFFS-HOLE1_R*sqrt(2.0))*xf, (-TABLE_L/2.0-HOLE1_XYOFFS)*yf,  -0.1 );
           glVertex3f( (-TABLE_W/2.0-HOLE1_XYOFFS)*xf, (-TABLE_L/2.0-HOLE1_XYOFFS-HOLE1_R*sqrt(2.0))*yf,  -0.1 );
           glVertex3f( (-TABLE_W/2.0-HOLE1_XYOFFS+0.5)*xf, (-TABLE_L/2.0-HOLE1_XYOFFS-HOLE1_R*sqrt(2.0))*yf,  -0.1 );
           glVertex3f( (-TABLE_W/2.0-HOLE1_XYOFFS-HOLE1_R*sqrt(2.0))*xf, (-TABLE_L/2.0-HOLE1_XYOFFS+0.5)*yf,  -0.1 );
           glEnd();
       }

#define HOLE2_BACKSEGNR 8

       for(j=0;j<2;j++){ /* 2 side holes */
           xf=0.0;
           switch(j){
           case 0: glFrontFace(GL_CCW); xf=+1.0; break;
           case 1: glFrontFace(GL_CW);  xf=-1.0; break;
           }
           glBegin(GL_QUAD_STRIP);
           glNormal3f( 0.0, 1.0, 0.0 );
           glVertex3f( (-TABLE_W/2.0-BANDE_D)*xf, -HOLE2_R,  FRAME_DH );
           glNormal3f( 0, 0,-1.0 );
           glVertex3f( (-TABLE_W/2.0-BANDE_D)*xf, -HOLE2_R,  -0.1 );
           for( i=0 ; i<HOLE2_BACKSEGNR+1 ; i++ ){
               VMfloat phi;
               phi = -M_PI/2.0-M_PI*(VMfloat)i/(VMfloat)HOLE2_BACKSEGNR;
               glNormal3f( (-cos(phi))*xf,-sin(phi),0.0 );
               glVertex3f( (-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R*cos(phi))*xf, HOLE2_R*sin(phi), FRAME_DH );
               glNormal3f( 0, 0,-1.0 );
               glVertex3f( (-TABLE_W/2.0-HOLE2_XYOFFS+HOLE2_R*cos(phi))*xf, HOLE2_R*sin(phi), -0.1 );

           }
           glNormal3f( 0.0, -1.0, 0.0 );
           glVertex3f( (-TABLE_W/2.0-BANDE_D)*xf, +HOLE2_R,  FRAME_DH );
           glNormal3f( 0, 0,-1.0 );
           glVertex3f( (-TABLE_W/2.0-BANDE_D)*xf, +HOLE2_R,  -0.1 );
           glEnd();
           /* black ground of hole */
           glBegin(GL_QUADS);
           glVertex3f( (-TABLE_W/2.0-HOLE2_XYOFFS-HOLE2_R)*xf, +HOLE2_R,  -0.1 );
           glVertex3f( (-TABLE_W/2.0-HOLE2_XYOFFS-HOLE2_R)*xf, -HOLE2_R,  -0.1 );
           glVertex3f( (-TABLE_W/2.0-HOLE2_XYOFFS+2.5*HOLE2_R)*xf, -4*HOLE2_R,  -0.1 );
           glVertex3f( (-TABLE_W/2.0-HOLE2_XYOFFS+2.5*HOLE2_R)*xf, +4*HOLE2_R,  -0.1 );
           glEnd();
       }
       glFrontFace(GL_CW);
   }

   /* wood-frame */
   glMaterialfv(GL_FRONT, GL_AMBIENT,   wood_col_amb);
   glMaterialfv(GL_FRONT, GL_SPECULAR,  wood_col_spec_null);
   glMaterialf (GL_FRONT, GL_SHININESS, wood_col_shin);

   /* ### FIXME ### what is the function calls here - for OLD NV bumpmapping ??? */
   /*glActiveTextureARB(GL_TEXTURE1_ARB);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glActiveTextureARB(GL_TEXTURE0_ARB);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); */

   glEnable(GL_TEXTURE_2D);
   glMaterialfv(GL_FRONT, GL_DIFFUSE,   wood_col_diff2);
   glMaterialfv(GL_FRONT, GL_AMBIENT,   wood_col_amb2);
   glBindTexture(GL_TEXTURE_2D,frametexbind);
   glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glMaterialfv(GL_FRONT, GL_DIFFUSE,   wood_col_diff2);

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
   glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T);
   glDisable(GL_TEXTURE_2D);
   glMaterialfv(GL_FRONT, GL_AMBIENT,   wood_col_amb);
   glMaterialfv(GL_FRONT, GL_SPECULAR,  wood_col_spec_null);
   glMaterialf (GL_FRONT, GL_SHININESS, wood_col_shin);
   glEnable(GL_TEXTURE_2D);
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
           glBegin(GL_QUAD_STRIP);
           for(i=0;i<FEET_X_STEPS;i++){
               glNormal3f( 0.0, -1.0, 0.0 );
               glVertex3f( -(FEET_W/2.0 -FEET_R )/(VMfloat)FEET_X_STEPS*i, -(FEET_L/2.0),  -0.1 );
               glNormal3f( 0.0, -1.0, 0.0 );
               glVertex3f( -(FEET_W2/2.0-FEET_R2)/(VMfloat)FEET_X_STEPS*i, -(FEET_L2/2.0)-FEET_X_FUNC((VMfloat)i/(VMfloat)FEET_X_STEPS)*(FEET_L-FEET_L2)/2.0/(tableh+balld/2.0-0.1), -tableh-balld/2.0+FEET_X_FUNC((VMfloat)i/(VMfloat)FEET_X_STEPS) );
           }
           for(i=0;i<FEET_EDGE_STEPS;i++){
               glNormal3f( -1.0/sqrt(2.0), -1.0/sqrt(2.0), 0.0 );
               glVertex3f( -(FEET_W /2.0-FEET_R )-FEET_R *sin(M_PI/2.0*(VMfloat)i/(VMfloat)FEET_X_STEPS), -(FEET_L/2.0- FEET_R )-FEET_R *cos(M_PI/2.0*(VMfloat)i/(VMfloat)FEET_X_STEPS), -0.1 );
               glNormal3f( -1.0/sqrt(2.0), -1.0/sqrt(2.0), 0.0 );
               glVertex3f( -(FEET_W2/2.0-FEET_R2)-FEET_R2*sin(M_PI/2.0*(VMfloat)i/(VMfloat)FEET_X_STEPS), -(FEET_L2/2.0-FEET_R2)-FEET_R2*cos(M_PI/2.0*(VMfloat)i/(VMfloat)FEET_X_STEPS), -tableh-balld/2.0 );
           }
           for(i=1;i<FEET_Y_STEPS+1;i++){
               glNormal3f( -1.0, 0.0, 0.0 );
               glVertex3f( -(FEET_W/2.0),  -(FEET_L/2.0 -FEET_R )/(VMfloat)FEET_Y_STEPS*(FEET_Y_STEPS-i), -0.1 );
               glNormal3f( -1.0, 0.0, 0.0 );
               glVertex3f( -(FEET_W2/2.0)-FEET_Y_FUNC((VMfloat)i/(VMfloat)FEET_Y_STEPS)*(FEET_W-FEET_W2)/2.0/(tableh+balld/2.0-0.1), -(FEET_L2/2.0-FEET_R2)/(VMfloat)FEET_Y_STEPS*(FEET_Y_STEPS-i), -tableh-balld/2.0+FEET_Y_FUNC((VMfloat)i/(VMfloat)FEET_Y_STEPS) );
           }
           glEnd();
           glPopMatrix();
       }
   }

   /* Now the floor itself */

   glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T);
   glPushMatrix();
   glTranslatef(0,0,-tableh-balld/2.0);

   glBindTexture(GL_TEXTURE_2D,floortexbind);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   glBegin(GL_QUADS);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(0,0);
   glVertex2f(0,0);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(0,1);
   glVertex2f(5,0);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(1,1);
   glVertex2f(5,-5);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(1,0);
   glVertex2f(0,-5);

   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(0,0);
   glVertex2f(0,5);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(0,1);
   glVertex2f(5,5);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(1,1);
   glVertex2f(5,0);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(1,0);
   glVertex2f(0,0);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,floortexbind);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   glBegin(GL_QUADS);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(0,1);
   glVertex2f(-5,0);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(0,0);
   glVertex2f(0,0);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(1,0);
   glVertex2f(0,-5);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(1,1);
   glVertex2f(-5,-5);

   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(0,1);
   glVertex2f(-5,5);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(0,0);
   glVertex2f(0,5);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(1,0);
   glVertex2f(0,0);
   glNormal3f( 0.0, 0.0, 1.0 );
   glTexCoord2f(1,1);
   glVertex2f(-5,0);
   glEnd();
   glPopMatrix();
   glEnable(GL_TEXTURE_2D);

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

   if(!init) init = 1;
   return table_obj;
}
