/* sittingboy.h
**
**    code for display sitting boy
**
**    Updated Version foobillard++ started at 12/2010
**    Copyright (C) 2012 Holger Schaekel (foobillardplus@go4more.de)
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

#ifndef SITTINGBOY_H
#define SITTINGBOY_H

#ifdef __APPLE__
 #include <OpenGL/gl.h>
#else
 #include <GL/gl.h>
#endif

void sittingboy(void);
void init_sittingboy(void);
void display_sittingboy(void);

#endif
