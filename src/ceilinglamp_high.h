/* ceilinglamp_high.h
**
**    code for high quality ceiling lamp
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

#ifndef CEILINGLAMP_HIGH_H
#define CEILINGLAMP_HIGH_H

#ifdef __APPLE__
 #include <OpenGL/gl.h>
#else
 #include <GL/gl.h>
#endif

void ceilinglamp_high(void);
void init_ceilinglamp_high(void);
void display_ceilinglamp_high(void);

#endif
