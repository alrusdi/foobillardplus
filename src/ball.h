/* ball.h
**
**    includefile: code for creating the GL-ball display-lists
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

#ifndef BALL_H
#define BALL_H

#include "vmath.h"

#define MAX_PRIM_NUM 100000  /* this should do it for now - FIXME: make me dynamic */

typedef struct {
    int vnr;
    GLfloat *vert;
    GLfloat *norm;
    GLfloat *tex;
    GLfloat *reftex;
    int num_prim;  /* !=0 if STRIPS are used */
    int prim_size[MAX_PRIM_NUM];  /* sizes of primitives */
    int indnr;
    int *index;
} ElemArray;

void create_texbinds( BallsType *balls );
void draw_balls( BallsType balls, myvec cam_pos, GLfloat cam_FOV, int win_width, int spheretexbind, VMvect * lightpos, int lightnr, unsigned int * cuberef_binds );
void draw_ballpath( BallType * pball );

#endif  /* BALL_H */
