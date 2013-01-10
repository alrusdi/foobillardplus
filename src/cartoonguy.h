/* cartoonguy.h
**
**    code for high quality cartoon guy
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

#ifndef CARTOONGUY_H
#define CARTOONGUY_H

#ifdef __APPLE__
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif

void cartoonguy(void);
void init_cartoonguy(void);
void display_cartoonguy(void);

#endif
