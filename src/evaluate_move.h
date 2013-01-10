/* evaluate_move.c
**
**    evaluate moves for dfferent gametypes
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

#ifndef EVALUATE_MOVE_H
#define EVALUATE_MOVE_H

#include "billard.h"
#include "billmove.h"
#include "player.h"
#include "menu.h"
#include "vmath.h"

extern void (* evaluate_last_move)( struct Player * player, int * actual_player, BallsType * pballs, int * pqueue_view);
void evaluate_last_move_8ball    ( struct Player * player, int * actual_player, BallsType * pballs, int * pqueue_view);
void evaluate_last_move_9ball    ( struct Player * player, int * actual_player, BallsType * pballs, int * pqueue_view);
void evaluate_last_move_carambol ( struct Player * player, int * actual_player, BallsType * pballs, int * pqueue_view);
void evaluate_last_move_snooker  ( struct Player * player, int * actual_player, BallsType * pballs, int * pqueue_view);
void setfunc_evaluate_last_move(void (* eval_func)( struct Player * player, int * actual_player, BallsType * pballs, int * pqueue_view));
void drawstatustext(int win_width, int win_height);  // draw the statusline
void drawstatuswocheck(int win_width, int win_height); // draw the statusline without any checks
void initstatustext();  // initialize the statusline
void concatworkstring (char * concattext); // concatenate the workstring for the next statusline
void emptyworkstring (); // empty the workstrings and the corresponding checkvariables
void setst_text(); // set the statusline for output
void displaystring (char *statusstr); //display an entire string
void snooker_color(char *, int pscore, int player_no, int loop); // set the lower line for the next to play color in snooker
#endif
