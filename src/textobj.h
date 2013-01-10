/* textobj.h
**
**    quad with text as texture using OpenGL
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

#ifndef TEXTOBJ_H
#define TEXTOBJ_H

#include "vmath.h"

#define HBOUND_CENTER  0x0001
#define HBOUND_RIGHT   0x0002
#define HBOUND_LEFT    0x0003
#define VBOUND_CENTER  0x0100
#define VBOUND_TOP     0x0200
#define VBOUND_BOTTOM  0x0300

/***********************************************************************/

struct _textObj{
    int    tex_id;
    int    quad_id;
    VMfloat height;
    VMfloat quad_w;
    VMfloat quad_h;
    char   str[256];
    char   fontname[256];

    int    is_3D;
    int    obj3D_id;
    VMfloat obj3D_w;
    VMfloat obj3D_h;
    VMfloat depth3D;
    int    ppspline3D;
};

typedef struct _textObj textObj;

/***********************************************************************/

textObj * textObj_new(char * str, char * fontname, int height);
textObj * textObj3D_new(char * str, char * fontname, VMfloat height, VMfloat depth);
void      textObj_setText( textObj * obj, char * str );
void      textObj_delete_last( textObj * obj );
void      textObj_append_char( textObj * obj, int c );
void      textObj_setFont( textObj * obj, char * fontname );
void      textObj_draw( textObj * obj );
void      textObj_draw_centered( textObj * obj );
void      textObj_draw_bound( textObj * obj, int hbound, int vbound );
void      textObj_delete( textObj * obj );

#endif
