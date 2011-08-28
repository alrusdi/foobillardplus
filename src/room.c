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
 *           Create equal rects for the walls                          *
 ***********************************************************************/

void my_rect_wall(void)
{
  glBegin(GL_QUADS);
  glTexCoord2s(0,0);
  glVertex3f(-5.0, 5.0, 2.5);   // Top Left
  glTexCoord2s(4,0);
  glVertex3f( 5.0, 5.0, 2.5);   // Top Right
  glTexCoord2s(4,2);
  glVertex3s( 5, 5, 0);    // Bottom Right
  glTexCoord2s(0,2);
  glVertex3s(-5, 5, 0);    // Bottom Left
  glEnd();
}

/***********************************************************************
 *           Create the room itself (and the gllist)                   *
 ***********************************************************************/

void create_room(int *floor_obj, int *wall1_obj,int *wall2_obj,int *wall3_obj,int *wall4_obj,int *ceiling_obj)
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

  if( graffity1texbind > 0 ) glDeleteTextures( 1, &graffity1texbind );
  create_png_texbind("graffity1.png", &graffity1texbind, 3, GL_RGBA);

  if( graffity2texbind > 0 ) glDeleteTextures( 1, &graffity2texbind );
  create_png_texbind("graffity2.png", &graffity2texbind, 3, GL_RGBA);

  if( closewindowtexbind > 0 ) glDeleteTextures( 1, &closewindowtexbind );
  create_png_texbind("closewindow.png", &closewindowtexbind, 4, GL_RGBA);

  if( doortexbind > 0 ) glDeleteTextures( 1, &doortexbind );
  create_png_texbind("door.png", &doortexbind, 4, GL_RGBA);

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

    glBegin(GL_QUADS);
    glNormal3s( 0, 0, 1 );
    glTexCoord2s(0,0);
    glVertex2s(0,0);
    glTexCoord2s(0,1);
    glVertex2s(5,0);
    glTexCoord2s(1,1);
    glVertex2s(5,-5);
    glTexCoord2s(1,0);
    glVertex2s(0,-5);

    glTexCoord2s(0,0);
    glVertex2s(0,5);
    glTexCoord2s(0,1);
    glVertex2s(5,5);
    glTexCoord2s(1,1);
    glVertex2s(5,0);
    glTexCoord2s(1,0);
    glVertex2s(0,0);

    glTexCoord2s(0,1);
    glVertex2s(-5,0);
    glTexCoord2s(0,0);
    glVertex2s(0,0);
    glTexCoord2s(1,0);
    glVertex2s(0,-5);
    glTexCoord2s(1,1);
    glVertex2s(-5,-5);

    glTexCoord2s(0,1);
    glVertex2s(-5,5);
    glTexCoord2s(0,0);
    glVertex2s(0,5);
    glTexCoord2s(1,0);
    glVertex2s(0,0);
    glTexCoord2s(1,1);
    glVertex2s(-5,0);
    glEnd();
  glEndList();

  if(*wall1_obj != -1 ) glDeleteLists( *wall1_obj, 1 );
  *wall1_obj = glGenLists(1);
  glNewList(*wall1_obj, GL_COMPILE);
    glBlendFunc(GL_ONE,GL_ONE);
    glBindTexture(GL_TEXTURE_2D,corr1texbind);
    my_rect_wall();

    glBindTexture(GL_TEXTURE_2D,graffity2texbind);
    glEnable(GL_BLEND);
    glBegin(GL_QUADS);
    glTexCoord2s(0,0);
    glVertex3f(0.5, 4.999, 2.3);   // Top Left
    glTexCoord2s(1,0);
    glVertex3f( 4.0, 4.999, 2.3);   // Top Right
    glTexCoord2s(1,1);
    glVertex3f( 4.0, 4.999, 0.5);    // Bottom Right
    glTexCoord2s(0,1);
    glVertex3f(0.5, 4.999, 0.5);    // Bottom Left
    glEnd();
    glDisable(GL_BLEND);
  glEndList();

  if(*wall2_obj != -1 ) glDeleteLists( *wall2_obj, 1 );
  *wall2_obj = glGenLists(1);
  glNewList(*wall2_obj, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D,corr1texbind);
    my_rect_wall();

    glBindTexture(GL_TEXTURE_2D,graffity1texbind);
    glEnable(GL_BLEND);
    glBegin(GL_QUADS);
    glTexCoord2s(0,0);
    glVertex3f(-4.0, 4.999, 2.5);   // Top Left
    glTexCoord2s(1,0);
    glVertex3f( 4.0, 4.999, 2.5);   // Top Right
    glTexCoord2s(1,1);
    glVertex3f( 4.0, 4.999, 0.0);    // Bottom Right
    glTexCoord2s(0,1);
    glVertex3f(-4.0, 4.999, 0.0);    // Bottom Left
    glEnd();
    glDisable(GL_BLEND);
  glEndList();

  if(*wall3_obj != -1 ) glDeleteLists( *wall3_obj, 1 );
  *wall3_obj = glGenLists(1);
  glNewList(*wall3_obj, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D,corr2texbind);
    glBegin(GL_QUADS);
    glTexCoord2s(0,0);
    glVertex3f(-5.0, 5.0, 2.5);   // Top Left
    glTexCoord2s(12,0);
    glVertex3f( 5.0, 5.0, 2.5);   // Top Right
    glTexCoord2s(12,6);
    glVertex3s( 5, 5, 0);    // Bottom Right
    glTexCoord2s(0,6);
    glVertex3s(-5, 5, 0);    // Bottom Left
    glEnd();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D,boardtexbind);
    glBegin(GL_QUADS);
    glTexCoord2s(0,0);
    glVertex3f(-2.0, 4.999, 2.0);   // Top Left
    glTexCoord2s(1,0);
    glVertex3f( 0.0, 4.999, 2.0);   // Top Right
    glTexCoord2s(1,1);
    glVertex3f( 0.0, 4.999, 0.7);    // Bottom Right
    glTexCoord2s(0,1);
    glVertex3f(-2.0, 4.999, 0.7);    // Bottom Left
    glEnd();
    glDisable(GL_BLEND);
   glEndList();

   if(*wall4_obj != -1 ) glDeleteLists( *wall4_obj, 1 );
   *wall4_obj = glGenLists(1);
   glNewList(*wall4_obj, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D,corr1texbind);
    my_rect_wall();

    glBindTexture(GL_TEXTURE_2D,pricetexbind);
    glEnable(GL_BLEND);
    glBegin(GL_QUADS);
    glTexCoord2s(0,0);
    glVertex3f(3.0, 4.999, 2.3);   // Top Left
    glTexCoord2s(1,0);
    glVertex3f( 4.0, 4.999, 2.3);   // Top Right
    glTexCoord2s(1,1);
    glVertex3f( 4.0, 4.999, 0.7);    // Bottom Right
    glTexCoord2s(0,1);
    glVertex3f(3.0, 4.999, 0.7);    // Bottom Left
    glEnd();

    glBindTexture(GL_TEXTURE_2D,closewindowtexbind);
    glBegin(GL_QUADS);
    glTexCoord2s(0,0);
    glVertex3f(0.0, 4.999, 2.3);   // Top Left
    glTexCoord2s(1,0);
    glVertex3f( 3.0, 4.999, 2.3);   // Top Right
    glTexCoord2s(1,1);
    glVertex3f( 3.0, 4.999, 0.7);    // Bottom Right
    glTexCoord2s(0,1);
    glVertex3f(0.0, 4.999, 0.7);    // Bottom Left
    glEnd();

    glBindTexture(GL_TEXTURE_2D,doortexbind);
    glBegin(GL_QUADS);
    glTexCoord2s(0,0);
    glVertex3f(-2.0, 4.999, 2.3);   // Top Left
    glTexCoord2s(1,0);
    glVertex3f( -1.0, 4.999, 2.3);   // Top Right
    glTexCoord2s(1,1);
    glVertex3f( -1.0, 4.999, 0.0);    // Bottom Right
    glTexCoord2s(0,1);
    glVertex3f(-2.0, 4.999, 0.0);    // Bottom Left
    glEnd();
    glDisable(GL_BLEND);
   glEndList();

   if(*ceiling_obj != -1 ) glDeleteLists( *ceiling_obj, 1 );
   *ceiling_obj = glGenLists(1);
   glNewList(*ceiling_obj, GL_COMPILE);
    glPolygonMode(GL_FRONT,GL_LINE);  // fill the front of the polygons
    glPolygonMode(GL_BACK,GL_FILL);   // only lines for back (better seeing on zooming)
    glBindTexture(GL_TEXTURE_2D,ceilingtexbind);

    glBegin(GL_QUADS);
    glTexCoord2s(0,0);
    glVertex3f(-5,5,2.5);
    glTexCoord2s(0,8);
    glVertex3f(5,5,2.5);
    glTexCoord2s(8,8);
    glVertex3f(5,-5,2.5);
    glTexCoord2s(8,0);
    glVertex3f(-5,-5,2.5);
    glEnd();
    glPolygonMode(GL_FRONT,GL_FILL);  // fill the front of the polygons
    glPolygonMode(GL_BACK,GL_LINE);   // only lines for back (better seeing on zooming)
   glEndList();
}
