/* billard3d.h
**
**    code for game history
**
**    Version foobillard++ started at 12/2010
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

// don't use functions of history unless the language is initialized!

#ifndef BILLARD3D_H
#define BILLARD3D_H

#include "player.h"

//#define TIME_INTERPOLATE

#define CUE_BALL_IND (player[act_player].cue_ball)
#define CUE_BALL_POS (balls.ball[CUE_BALL_IND].r)
#define CUE_BALL_XYPOS (vec_xyz(CUE_BALL_POS.x,CUE_BALL_POS.y,0.0))
#define CUEBALL_MAXSPEED 7.0

#define CUESTEP 0.02               // step for Cue up down and cursor keys
#define CUESTEPMAX 1.0             // max. step cue
#define STEP1 0.01                 // step for one keypress circle
#define FREEVIEW_STEP1 0.01        // step for one keypress up/down
#define STEPMAX  2.0               // max. step keypress for left/right key
#define FREEVIEW_STEPMAX 1.0       // max. step keypress for up/down key

#define TOURNAMENT_ROUND_NUM 4     // Rounds for Tournament
#define ROSTER_MAX_NUM 16          // max. Player no. for all players incl. Tournament

#define FREE_VIEW  (!queue_view && options_free_view_on)

#define strcpy_uscore_2_whtspace(d,s) {int i; for(i=0;(d[i]=(s[i]!='_'?s[i]:' '))!=0;i++);}
#define strcpy_whtspace_2_uscore(d,s) {int i; for(i=0;(d[i]=(s[i]!=' '?s[i]:'_'))!=0;i++);}

#define queue_point_x  (player[act_player].cue_x)
#define queue_point_y  (player[act_player].cue_y)
#define queue_strength (player[act_player].strength)

struct PlayerRoster {
    int nr;       /* number of players */
    struct Player player[ROSTER_MAX_NUM];   /* max. players incl. Tournament*/
};

struct TournamentState_ {
    int round_num;
    int game_ind;
    int round_ind;
    int wait_for_next_match; /* show status meanwhile */
    int overall_winner;
    int tournament_over;
    VMfloat ai_fast_motion;
    struct {
        int roster_player1;
        int roster_player2;
        int winner;
        int hits;
        int rounds;
    } game[TOURNAMENT_ROUND_NUM/*rounds*/][1<<(TOURNAMENT_ROUND_NUM-1)/*games*/];

    struct PlayerRoster roster;

};

void Key( int key, int modifiers );
void KeyUp( int key );
void MouseEvent(MouseButtonEnum button,MouseButtonState  state, int x, int y);
void MouseMotion(int x, int y);
void DisplayFunc(void);
void ResizeWindow(int w,int h);
void save_config(void);

#endif
