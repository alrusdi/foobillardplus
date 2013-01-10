/* pngloader.h
**
**    includefile: load png-textures
**    Copyright (C) 2001  Florian Berger
**    Email: harpin_floh@yahoo.de, florian.berger@jk.uni-linz.ac.at
**
**    Updated Version foobillard++ started at 12/2010
**    Copyright (C) 2010 - 2013 Holger Schaekel (foobillardplus@go4more.de)
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

#ifndef PNG_LOADER_H
#define PNG_LOADER_H

#include "vmath.h"

int load_png(char * file_name, int * w, int * h, int * depth, char ** data);
void create_png_texbind(char *file_name, GLuint *texbind, GLuint component, GLuint format);
void Snapshot(int width, int height);

#endif  /* PNG_LOADER_H */
