/* player.h
**
**    player structure
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

#ifndef PLAYER_H
#define PLAYER_H
#include "textobj.h"
#include "vmath.h"

struct Player{
    int is_AI;
    int is_net;
    int half_full;
    int queue_view;
    int place_cue_ball;
    int winner;
    VMfloat Zque;
    VMfloat Xque;
    VMfloat cue_x;
    VMfloat cue_y;
    VMfloat strength;
    VMfloat err;  /* err-ability of ai player */
    char name[256];
    textObj * text;
    textObj * score_text;
    int snooker_on_red;  /* 1: player is on red,   0: player is on coloured */
    int snooker_next_color;  /* if all reds are gone : 0/1: any color, other : color to play*/
    int score;
    int cue_ball;  /* index of cue ball for this player */
    int next_9ball;  /* next ball to play in 9ball */
};

#endif
