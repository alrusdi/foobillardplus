/* queue.h
**
**    includefile: create the billard-queue display lists
**    Copyright (C) 2001  Florian Berger
**    Email:  harpin_floh@yahoo.de,  florian.berger@jk.uni-linz.ac.at
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

#ifndef BILLARD_QUEUE_H
#define BILLARD_QUEUE_H

#include "vmath.h"

void delete_queue_texbind( void );
void create_queue_texbind( void );
int  create_queue(VMfloat (*)(VMfloat));
void draw_queue( VMvect pos, GLfloat Xrot, GLfloat Zrot, GLfloat zoffs, GLfloat xoffs, GLfloat yoffs, int spheretexbind, VMvect * lightpos, int lightnr );

#endif  /* BILLARD_QUEUE_H */
