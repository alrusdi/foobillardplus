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
#include "options.h"
#include "fire.h"

static int fire_id[MAX_FIRE_TEXTURES];          // fire displaylist glcompile-id
static GLuint firetexbind[MAX_FIRE_TEXTURES];   // texture bindings

/***********************************************************************
 *                  Initialize the fire textures                       *
 ***********************************************************************/


void init_fire(void) {
    int i;
    char firename[10];
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
         glDisable(GL_BLEND);
         glEndList();
    }

}

/***********************************************************************
 *                  Display fire textures id displaylist               *
 ***********************************************************************/


void display_fire(int id) {
    glCallList(fire_id[id]);
}
