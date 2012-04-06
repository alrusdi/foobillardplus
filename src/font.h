/* font.c
**
**    create pixmaps from text using freetype
**    Copyright (C) 2001  Florian Berger
**    Email: harpin_floh@yahoo.de, florian.berger@jk.uni-linz.ac.at
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

#ifndef FONT_H
#define FONT_H

#ifdef __APPLE__
 #include <OpenGL/gl.h>
#else
 #include <GL/gl.h>
#endif
#include "vmath.h"

void getStringPixmapFT   (char *str, char * fontname, int font_height, char ** data, int * dwidth, int * dheight, int * width, int * height);
GLuint getStringGLListFT (char *str, char * fontname, VMfloat font_height, float depth, VMfloat * width, VMfloat * height);
size_t utf8count( const char *string );  // Count the "real" chars in an utf8string

#endif  /* FONT_H */
