/* fire.c
**
**    code for fire in fireplace
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

#include <stdio.h>
#include <GL/gl.h>
#include "png_loader.h"
#include "fire.h"

static int fire_id[MAX_FIRE_TEXTURES];          // fire displaylist glcompile-id
static int fire_id_high[MAX_FIRE_TEXTURES];     // fire displaylist glcompile-id high meshes
static GLuint firetexbind[MAX_FIRE_TEXTURES];   // texture bindings
static GLuint firemeshtexbind;                  // texture binding for the mesh

/***********************************************************************
 *                  Initialize the fire textures                       *
 ***********************************************************************/


void init_fire(void) {
    int i;
    char firename[10];

    fprintf(stderr,"load fireplace fire graphics\n");
    create_png_texbind("firemesh.png", &firemeshtexbind, 3, GL_RGB);
    for (i=0;i<MAX_FIRE_TEXTURES;i++) {
    	  sprintf(firename,"fire%i.png",i);
    	  create_png_texbind(firename, &firetexbind[i], 3, GL_RGB);
       fire_id[i] = glGenLists(1);
       glNewList(fire_id[i], GL_COMPILE);
   	     glPopMatrix();
   	     glPushMatrix();
         glRotatef(270.0,90.0,0.0,0.0);
         glTranslatef(-1.05,-0.05,4.6);
         glEnable(GL_BLEND);
         glBlendFunc (GL_ONE, GL_ONE);
         glEnable(GL_TEXTURE_2D);
         glBindTexture(GL_TEXTURE_2D,firetexbind[i]);
         glBegin(GL_QUADS);
          glTexCoord2s(0,0);
          glVertex3f(0.0,0.0,0.0);
          glTexCoord2s(0,1);
          glVertex3f(0.0,0.75,0.0);
          glTexCoord2s(1,1);
          glVertex3f(0.75,0.75,0.0);
          glTexCoord2s(1,0);
          glVertex3f(0.75,0.0,0.0);
         glEnd();
         glBlendFunc (GL_ONE, GL_SRC_ALPHA);
         glBindTexture(GL_TEXTURE_2D,firemeshtexbind);
         glBegin(GL_QUADS);
          glTexCoord2s(0,0);
          glVertex3f(-0.08,0.62,-0.12);
          glTexCoord2s(0,1);
          glVertex3f(-0.08,0.75,-0.12);
          glTexCoord2s(1,1);
          glVertex3f(0.695,0.75,-0.12);
          glTexCoord2s(1,0);
          glVertex3f(0.695,0.62,-0.12);
         glEnd();
         glDisable(GL_BLEND);
         glEndList();

         fire_id_high[i] = glGenLists(1);
         glNewList(fire_id_high[i], GL_COMPILE);
     	     glPushMatrix();
           glRotatef(180.0,180.0,0.0,0.0);
           glTranslatef(-0.95,-0.25,4.7);
           glEnable(GL_BLEND);
           glBlendFunc (GL_ONE, GL_ONE);
           glEnable(GL_TEXTURE_2D);
           glBindTexture(GL_TEXTURE_2D,firetexbind[i]);
           glBegin(GL_QUADS);
            glTexCoord2s(0,0);
            glVertex3f(0.0,0.0,0.0);
            glTexCoord2s(0,1);
            glVertex3f(0.0,0.75,0.0);
            glTexCoord2s(1,1);
            glVertex3f(0.75,0.75,0.0);
            glTexCoord2s(1,0);
            glVertex3f(0.75,0.0,0.0);
           glEnd();
           glBlendFunc (GL_ONE, GL_SRC_ALPHA);
           glBindTexture(GL_TEXTURE_2D,firemeshtexbind);
           glBegin(GL_QUADS);
            glTexCoord2s(0,0);
            glVertex3f(-0.0,0.52,-0.2);
            glTexCoord2s(0,1);
            glVertex3f(-0.0,0.68,-0.2);
            glTexCoord2s(1,1);
            glVertex3f(0.68,0.68,-0.2);
            glTexCoord2s(1,0);
            glVertex3f(0.68,0.52,-0.2);
           glEnd();
           glDisable(GL_BLEND);
           glDisable(GL_TEXTURE_2D);
           glPopMatrix();
           glEndList();
    }
    fprintf(stderr,"Initialize fire lists completed\n");
}

/***********************************************************************
 *                  Display fire textures id displaylist               *
 ***********************************************************************/

void display_fire(int id) {
    glCallList(fire_id[id]);
}

/***********************************************************************
 *          Display fire textures id displaylist high meshes           *
 ***********************************************************************/

void display_fire_high(int id) {
    glCallList(fire_id_high[id]);
}
