/* aiplayer.h
**
**    code for positioning artifitial intelligence player
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
#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "billard.h"
#include "player.h"
#include "vmath.h"

#ifndef AIPLAYER_C
extern VMvect (*ai_get_stroke_dir)( BallsType * balls, BordersType * walls, struct Player * pplayer );
#endif

VMfloat stroke_angle( BallType * bcue, BallType * bhit, HoleType * hole );
int ball_in_way( int ballnr, VMvect aim, BallsType * balls );
void ai_set_skill( VMfloat skill );
void ai_set_err( VMfloat err );
VMvect ai_get_stroke_dir_8ball   ( BallsType * balls, BordersType * walls, struct Player * pplayer );
VMvect ai_get_stroke_dir_9ball   ( BallsType * balls, BordersType * walls, struct Player * pplayer );
VMvect ai_get_stroke_dir_carambol( BallsType * balls, BordersType * walls, struct Player * pplayer );
VMvect ai_get_stroke_dir_snooker ( BallsType * balls, BordersType * walls, struct Player * pplayer );

void setfunc_ai_get_stroke_dir(VMvect (*func)( BallsType * balls, BordersType * walls, struct Player * pplayer ));

#endif /* AIPLAYER_H */
