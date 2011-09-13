/* room.c
**
**    create the billard-room
**
**    Version foobillard++ started at 12/2010
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

#include <stdio.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include "png_loader.h"
#include "billard.h"

/***********************************************************************
 *           Create equal rects for the floor                          *
 ***********************************************************************/

void my_rect_floor(GLshort a,GLshort b,GLshort c,GLshort d,GLshort e,GLshort f,GLshort g,GLshort h,GLshort i,GLshort j,GLshort k,GLshort l,GLshort m,GLshort n,GLshort o,GLshort p)
{
  glTexCoord2s(a,b);
  glVertex2s(c,d);
  glTexCoord2s(e,f);
  glVertex2s(g,h);
  glTexCoord2s(i,j);
  glVertex2s(k,l);
  glTexCoord2s(m,n);
  glVertex2s(o,p);
}

/***********************************************************************
 *           Create equal rects for the walls                          *
 ***********************************************************************/

void my_rect_wall(void)
{
  glBegin(GL_QUADS);
  glTexCoord2s(0,0);
  glVertex3f(-5.0, 5.0, 2.5);
  glTexCoord2s(4,0);
  glVertex3f( 5.0, 5.0, 2.5);
  glTexCoord2s(4,2);
  glVertex3s( 5, 5, 0);
  glTexCoord2s(0,2);
  glVertex3s(-5, 5, 0);
  glEnd();
}

/***********************************************************************
 *           Create -most- rects (cabinet, graffity                    *
 ***********************************************************************/

void my_rect_most(GLfloat a, GLfloat b, GLfloat c, GLfloat d, GLfloat e, GLfloat f, GLfloat g, GLfloat h, GLfloat i, GLfloat j, GLfloat k, GLfloat l)
{
  glTexCoord2s(0,0);
  glVertex3f(a,b,c);
  glTexCoord2s(1,0);
  glVertex3f(d,e,f);
  glTexCoord2s(1,1);
  glVertex3f(g,h,i);
  glTexCoord2s(0,1);
  glVertex3f(j,k,l);

}

/***********************************************************************
 *           Create the room itself (and the gllist)                   *
 ***********************************************************************/

void create_room(int *floor_obj, int *wall1_2_obj, int *wall3_obj, int *wall4_c_obj)
{

  static GLuint corr1texbind=-1;
  static GLuint corr2texbind=-1;
  static GLuint graffity1texbind=-1;
  static GLuint graffity2texbind=-1;
  static GLuint closewindowtexbind=-1;
  static GLuint doortexbind=-1;
  static GLuint boardtexbind=-1;
  static GLuint ceilingtexbind=-1;
  static GLuint pricetexbind=-1;
  static GLuint floortexbind=-1;
  static GLuint cabinetbacktexbind=-1;
  static GLuint cabinetframetexbind=-1;
  static GLuint cabinetfronttexbind=-1;
  static GLuint stoneframetexbind=-1;
  static GLuint winbig1texbind =-1;
  static GLuint winbacktexbind =-1;
  VMfloat balld  = BALL_D;
  VMfloat tableh = TABLE_H;

  if( floortexbind > 0 ) glDeleteTextures( 1, &floortexbind );
  create_png_texbind("floor.png", &floortexbind, 3, GL_RGB);

  if( corr1texbind > 0 ) glDeleteTextures( 1, &corr1texbind );
  create_png_texbind("corr1.png", &corr1texbind, 3, GL_RGB);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  if( corr2texbind > 0 ) glDeleteTextures( 1, &corr2texbind );
  create_png_texbind("corr2.png", &corr2texbind, 3, GL_RGB);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  if( stoneframetexbind > 0 ) glDeleteTextures( 1, &stoneframetexbind );
  create_png_texbind("stone-frame.png", &stoneframetexbind, 3, GL_RGB);


  if( cabinetbacktexbind > 0 ) glDeleteTextures( 1, &cabinetbacktexbind );
  create_png_texbind("cabinet-back.png", &cabinetbacktexbind, 3, GL_RGBA);
  if( cabinetframetexbind > 0 ) glDeleteTextures( 1, &cabinetframetexbind );
  create_png_texbind("cabinet-frame.png", &cabinetframetexbind, 3, GL_RGB);
  if( cabinetfronttexbind > 0 ) glDeleteTextures( 1, &cabinetfronttexbind );
  create_png_texbind("cabinet-front.png", &cabinetfronttexbind, 4, GL_RGBA);

  if( graffity1texbind > 0 ) glDeleteTextures( 1, &graffity1texbind );
  create_png_texbind("graffity1.png", &graffity1texbind, 3, GL_RGBA);

  if( graffity2texbind > 0 ) glDeleteTextures( 1, &graffity2texbind );
  create_png_texbind("graffity2.png", &graffity2texbind, 3, GL_RGBA);

  if( closewindowtexbind > 0 ) glDeleteTextures( 1, &closewindowtexbind );
  create_png_texbind("closewindow.png", &closewindowtexbind, 4, GL_RGBA);

  if( doortexbind > 0 ) glDeleteTextures( 1, &doortexbind );
  create_png_texbind("door.png", &doortexbind, 4, GL_RGBA);

  if( winbig1texbind > 0 ) glDeleteTextures( 1, &winbig1texbind );
  create_png_texbind("window-big1.png", &winbig1texbind, 4, GL_RGBA);
  if( winbacktexbind > 0 ) glDeleteTextures( 1, &winbacktexbind );
  create_png_texbind("skyline.png", &winbacktexbind, 3, GL_RGB);

  if( boardtexbind > 0 ) glDeleteTextures( 1, &boardtexbind );
  create_png_texbind("board.png", &boardtexbind, 4, GL_RGBA);

  if( ceilingtexbind > 0 ) glDeleteTextures( 1, &ceilingtexbind );
  create_png_texbind("ceiling.png", &ceilingtexbind, 3, GL_RGB);

  if( pricetexbind > 0 ) glDeleteTextures( 1, &pricetexbind );
  create_png_texbind("price.png", &pricetexbind, 4, GL_RGBA);

// room walls and floor

  // Now the floor. it's ever available and must be called first !!!! in the room-building
  // and direct after the table build

  if(*floor_obj != -1 ) glDeleteLists( *floor_obj, 1 );
  *floor_obj = glGenLists(1);
  glNewList(*floor_obj, GL_COMPILE);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glTranslatef(0,0,-tableh-balld/2.0);

    glBindTexture(GL_TEXTURE_2D,floortexbind);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glNormal3s( 0, 0, 1 );
    glBegin(GL_QUADS);
    my_rect_floor(0,0,0,0,0,1,5,0,1,1,5,-5,1,0,0,-5);
    my_rect_floor(0,0,0,5,0,1,5,5,1,1,5,0,1,0,0,0);
    my_rect_floor(0,1,-5,0,0,0,0,0,1,0,0,-5,1,1,-5,-5);
    my_rect_floor(0,1,-5,5,0,0,0,5,1,0,0,0,1,1,-5,0);
    glEnd();
  glEndList();

  if(*wall1_2_obj != -1 ) glDeleteLists( *wall1_2_obj, 1 );
  *wall1_2_obj = glGenLists(1);
  glNewList(*wall1_2_obj, GL_COMPILE);
    glBlendFunc(GL_ONE,GL_ONE);

    // wall 1
    glBindTexture(GL_TEXTURE_2D,corr1texbind);
    my_rect_wall();

    glBindTexture(GL_TEXTURE_2D,graffity2texbind);
    glEnable(GL_BLEND);
    glBegin(GL_QUADS);
    my_rect_most(0.5,4.999,2.3,4.0,4.999,2.3,4.0,4.999,0.5,0.5,4.999,0.5);
    glEnd();
    glDisable(GL_BLEND);
    // billiards cabinet
    glBindTexture(GL_TEXTURE_2D,cabinetbacktexbind);
    glBegin(GL_QUADS);
    my_rect_most(-3.6,4.999,2.3,-2.4,4.999,2.3,-2.4,4.999,0.5,-3.6,4.999,0.5);
    glEnd();

    glBindTexture(GL_TEXTURE_2D,cabinetframetexbind);
    //left frame
    glBegin(GL_QUADS);
    my_rect_most(-3.6,4.8,2.3,-3.6,4.999,2.3,-3.6,4.999,0.5,-3.6,4.8,0.5);
    //bottom frame
    my_rect_most(-3.6,4.8,0.5,-3.6,4.999,0.5,-2.4,4.999,0.5,-2.4,4.8,0.5);
    //right frame
    my_rect_most(-2.4,4.8,2.3,-2.4,4.999,2.3,-2.4,4.999,0.5,-2.4,4.8,0.5);
    //upper frame
    my_rect_most(-3.6,4.8,2.3,-3.6,4.999,2.3,-2.4,4.999,2.3,-2.4,4.8,2.3);
    glEnd();
    //front
    glBindTexture(GL_TEXTURE_2D,cabinetfronttexbind);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBegin(GL_QUADS);
    my_rect_most(-3.6,4.8,2.3,-2.4,4.8,2.3,-2.4,4.8,0.5,-3.6,4.8,0.5);
    glEnd();
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);
    // wall 2
    glRotatef(90.0,0.0,0.0,1.1);
    glBindTexture(GL_TEXTURE_2D,corr1texbind);
    my_rect_wall();

    glBindTexture(GL_TEXTURE_2D,graffity1texbind);
    glEnable(GL_BLEND);
    glBegin(GL_QUADS);
    my_rect_most(-4.0,4.999,2.5,4.0,4.999,2.5,4.0,4.999,0.0,-4.0,4.999,0.0);
    glEnd();
    glDisable(GL_BLEND);
   glEndList();
   if(*wall3_obj != -1 ) glDeleteLists( *wall3_obj, 1 );
   *wall3_obj = glGenLists(1);
   glNewList(*wall3_obj, GL_COMPILE);
    // wall 3
    glBindTexture(GL_TEXTURE_2D,corr2texbind);
    glBegin(GL_QUADS);
    glTexCoord2s(0,0);
    glVertex3f(-5.0, 5.0, 2.5);
    glTexCoord2s(7,0);
    glVertex3f( 1.0, 5.0, 2.5);
    glTexCoord2s(7,6);
    glVertex3s( 1, 5, 0);
    glTexCoord2s(0,6);
    glVertex3s(-5, 5, 0);

    glTexCoord2s(0,0);
    glVertex3f(1.0, 5.0, 2.5);
    glTexCoord2f(4.0,0.0);
    glVertex3f( 4.0, 5.0, 2.5);
    glTexCoord2f(4.0,0.5);
    glVertex3f( 4.0, 5.0, 2.3);
    glTexCoord2f(0.0,0.5);
    glVertex3f(1.0, 5.0, 2.3);

    glTexCoord2s(0,0);
    glVertex3f(1.0, 5.0, 0.5);
    glTexCoord2f(4.0,0.0);
    glVertex3f( 4.0, 5.0, 0.5);
    glTexCoord2f(4.0,1.265);
    glVertex3f( 4.0, 5.0, 0.0);
    glTexCoord2f(0.0,1.265);
    glVertex3f(1.0, 5.0, 0.0);

    glTexCoord2s(0,0);
    glVertex3f(4.0, 5.0, 2.5);
    glTexCoord2s(1,0);
    glVertex3f( 5.0, 5.0, 2.5);
    glTexCoord2s(1,6);
    glVertex3s( 5, 5, 0);
    glTexCoord2s(0,6);
    glVertex3s(4, 5, 0);

    // ### TODO ### WeTab check !!!!
    /*    glTexCoord2s(0,0);
    glVertex3f(-5.0, 5.0, 2.5);
    glTexCoord2s(12,0);
    glVertex3f( 5.0, 5.0, 2.5);
    glTexCoord2s(12,6);
    glVertex3s( 5, 5, 0);
    glTexCoord2s(0,6);
    glVertex3s(-5, 5, 0); */
    glEnd();
    // wood-frame across the stone to the big window
    glBindTexture(GL_TEXTURE_2D,stoneframetexbind);
    //upper frame
    glBegin(GL_QUADS);
    my_rect_most(1.0,5.0,2.3,4.0,5.0,2.3,4.0,5.2,2.3,1.0,5.2,2.3);
    //left frame
    my_rect_most(1.0,5.0,2.3,1.0,5.2,2.3,1.0,5.2,0.5,1.0,5.0,0.5);
    //bottom frame
    glTexCoord2s(0,0);
    my_rect_most(1.0,5.2,0.5,4.0,5.2,0.5,4.0,5.0,0.5,1.0,5.0,0.5);
    // right frame
    my_rect_most(4.0,5.0,0.5,4.0,5.2,0.5,4.0,5.2,2.3,4.0,5.0,2.3);
    glEnd();

    // skyline behind window
    glBindTexture(GL_TEXTURE_2D,winbacktexbind);
    glBegin(GL_QUADS);
    my_rect_most(1.0,5.6,2.3,4.5,5.6,2.3,4.5,5.6,0.5,1.0,5.6,0.5);
    glEnd();
    // chalk board
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D,boardtexbind);
    glBegin(GL_QUADS);
    my_rect_most(-2.0,4.999,2.0,0.0,4.999,2.0,0.0,4.999,0.7,-2.0,4.999,0.7);
    glEnd();
    // big dark wood window
    glBindTexture(GL_TEXTURE_2D,winbig1texbind);
    glBegin(GL_QUADS);
    my_rect_most(1.0,5.2,2.3,4.0,5.2,2.3,4.0,5.2,0.5,1.0,5.2,0.5);
    glEnd();
    glDisable(GL_BLEND);
   glEndList();
   if(*wall4_c_obj != -1 ) glDeleteLists( *wall4_c_obj, 1 );
   *wall4_c_obj = glGenLists(1);
   glNewList(*wall4_c_obj, GL_COMPILE);
    // wall 4
    glRotatef(90.0,0.0,0.0,1.1);
    glBindTexture(GL_TEXTURE_2D,corr1texbind);
    my_rect_wall();
    // price list
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,pricetexbind);
    glEnable(GL_BLEND);
    glBegin(GL_QUADS);
    my_rect_most(3.0,4.999,2.3,4.0,4.999,2.3,4.0,4.999,0.7,3.0,4.999,0.7);
    glEnd();
    // closed window
    glBindTexture(GL_TEXTURE_2D,closewindowtexbind);
    glBegin(GL_QUADS);
    my_rect_most(0.0,4.999,2.3,3.0,4.999,2.3,3.0,4.999,0.7,0.0,4.999,0.7);
    glEnd();
    // door
    glBindTexture(GL_TEXTURE_2D,doortexbind);
    glBegin(GL_QUADS);
    my_rect_most(-2.0,4.999,2.3,-1.0,4.999,2.3,-1.0,4.999,0.0,-2.0,4.999,0.0);
    glEnd();
    glDisable(GL_BLEND);
    // ceiling
    glBindTexture(GL_TEXTURE_2D,ceilingtexbind);
    glBegin(GL_QUADS);
    glTexCoord2s(0,0);
    glVertex3f(-5,-5,2.5);
    glTexCoord2s(8,0);
    glVertex3f(5,-5,2.5);
    glTexCoord2s(8,8);
    glVertex3f(5,5,2.5);
    glTexCoord2s(0,8);
    glVertex3f(-5,5,2.5);
    glEnd();
   glEndList();
}
