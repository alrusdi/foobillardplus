/* evaluate_move.c
**
**    evaluate moves for dfferent gametypes
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "evaluate_move.h"
#include "options.h"
#include "language.h"
#include "font.h"
#include "textobj.h"
#include <SDL.h>
#include "sound_stuff.h"

// functions inside

void spot_snooker_ball(BallsType *balls,int nr);
void (* evaluate_last_move)( struct Player * player, int * pact_player, BallsType * pballs, int * pqueue_view, GLfloat * pXque ) = evaluate_last_move_8ball;

// Macros

#define MAX(x,y) ((x)>(y)?(x):(y));

// Variables etc.

typedef enum
{
   SN_PLAY_RED,
   SN_PLAY_ANY_COLOR,
   SN_PLAY_YELLOW,
   SN_PLAY_GREEN,
   SN_PLAY_BROWN,
   SN_PLAY_BLUE,
   SN_PLAY_PINK,
   SN_PLAY_BLACK,
   SN_DONE
}
SnookerBallToPlay;

typedef struct
{
   SnookerBallToPlay to_play;
}
SnookerState;

// Functions / Procedures

/***********************************************************************/

static int in_strafraum(VMvect pos)
{
    return(pos.y<-TABLE_L/4.0);
}
static SnookerState st={SN_PLAY_RED};    // Hold the realnames as index of the next to play ball in snooker
static textObj *stline1 = (textObj *)0;  // for the statusline on top of the screen
static textObj *stline2 = (textObj *)0;  // the optional second one
static int stline1_act = 0;              // something todo with the textObj
static int stline2_act = 0;

static char emptystring[256] = "\0";   // for initializing a whole empty string
static char workstring1[256] = "\0";   // workstring for the next 1. statusline
static char workstring2[256] = "\0";   // workstring for the 2. optinal statusline

/***********************************************************************/

void evaluate_last_move_8ball( struct Player * player, int * pact_player, BallsType * pballs, int * pqueue_view, GLfloat * pXque )
{
#define act_player (*pact_player)
#define balls      (*pballs)
    int out_half=0;
    int out_full=0;
    int nextplayer=1;
    int foul=0;
    int winner = 0;       // to check for a winner for the statusline, ooh, ugly
    int first_ball_hit;
    int eigene_da = 0;
    int in_own = 0;
    int i;
    int (* get_balls_out_own)(void);
    char statusstr[200];  // make a better statusline

    out_half = BM_get_balls_out_half();
    out_full = BM_get_balls_out_full();

    emptyworkstring();

    /* if not own ball played first -> foul */
    first_ball_hit=BM_get_1st_ball_hit();
    if( player[act_player].half_full == BALL_FULL ){
        if ( first_ball_hit>8 && first_ball_hit<16 ) foul=1;
    }
    if( player[act_player].half_full == BALL_HALF ){
       if ( first_ball_hit>0 && first_ball_hit<8 ) foul=1;
    }
    if (foul) {
      //Foreign ball first alluded
      concatworkstring(localeText[185]);
    }

    /* at second place, because no foul on break */
    if( player[act_player].half_full == BALL_ANY ){
        if(out_half>out_full){
            player[act_player].half_full=BALL_HALF;
            player[!act_player].half_full=BALL_FULL;
            nextplayer=0;
        }
        if(out_half<out_full){
            player[act_player].half_full=BALL_FULL;
            player[!act_player].half_full=BALL_HALF;
            nextplayer=0;
        }
    }

    /* if 8 first played and own balls are there -> foul */
    if ( first_ball_hit==8 ){
        if( player[act_player].half_full==BALL_FULL ){
            for(i=0;i<pballs->nr;i++){
                if( pballs->ball[i].in_game && pballs->ball[i].nr>0 && pballs->ball[i].nr<8 ){
                    eigene_da=1;
                    break;
                }
            }
        } else if( player[act_player].half_full==BALL_HALF ){
            for(i=0;i<pballs->nr;i++){
                if( pballs->ball[i].in_game && pballs->ball[i].nr>8 && pballs->ball[i].nr<16 ){
                    eigene_da=1;
                    break;
                }
            }
        }
        if(eigene_da) {
          foul=1;
          //Played 8 ball first
          concatworkstring(localeText[186]);
          }
    }

    /* if played ball is in penalty area */
    if ( player[act_player].place_cue_ball &&
        in_strafraum(BM_get_1st_ball_hit_pos()) &&
        !BM_get_non_strafraum_wall_hit_before_1st_ball(in_strafraum) ){
        foul=1;
        //First ball played in the penalty
        concatworkstring(localeText[187]);
    }

    /* if no own ball in - next player */
    if( player[act_player].half_full==BALL_HALF && out_half!=0 ){
        nextplayer=0;
        //Half ball pocketed - good.
        concatworkstring(localeText[188]);
    }
    if( player[act_player].half_full==BALL_FULL && out_full!=0 ){
        nextplayer=0;
        //Full ball pocketed - good.
        concatworkstring(localeText[189]);
    }

    if( BM_get_balls_hit()==0 ) {
      foul=1;
      //No ball hit
      concatworkstring(localeText[190]);
    }

    /* if white out */
    if( BM_get_white_out() ){
      nextplayer=1;
      foul=1;
      balls.ball[0].in_game=1;
      balls.ball[0].in_hole=0;
      //game ball lost
      concatworkstring(localeText[191]);
#ifdef USE_SOUND
      balls.ball[0].soundplayed = 0;
      if(options_gamemode==options_gamemode_tournament && player[0].is_AI && player[1].is_AI) {
        //nosound
      } else {
      	PlaySound(wave_oneball,options_snd_volume);
      }
#endif
    }

    /* if foul */
    if( foul ){
//        VMfloat x,y;
        /* this is done now for all balls in billard3d.c after evaluate_last_move  */
        balls.ball[0].v=vec_xyz(0.0,0.0,0.0);
        balls.ball[0].w=vec_xyz(0.0,0.0,0.0);
        balls.ball[0].r=vec_xyz(0.0,-TABLE_L/4.0,0.0);
    }

    /* if 8 out */
    if( BM_get_ball_out(8) ){

        if( player[act_player].half_full==BALL_HALF ){
            get_balls_out_own=BM_get_balls_out_half;
        } else {
            get_balls_out_own=BM_get_balls_out_full;
        }

        /* count own balls in game */
        for(i=0;i<pballs->nr;i++){
            if( player[act_player].half_full==BALL_FULL && pballs->ball[i].nr>0 && pballs->ball[i].nr<8 && pballs->ball[i].in_game ) in_own++;
            if( player[act_player].half_full==BALL_HALF && pballs->ball[i].nr>8 && pballs->ball[i].nr<16 && pballs->ball[i].in_game ) in_own++;
        }

        /* only one and last one */
        if(!foul && get_balls_out_own()==0 && in_own==0 && player[act_player].half_full!=BALL_ANY)   /* potting 8 at break caused a win (this should fix it) */
          {
            player[act_player].winner=1;
            winner = 1;
            // erase statusline because game is over
            emptyworkstring();
            setst_text();
          } else {
            player[(act_player==1)?0:1].winner=1;
            winner = 1;
            // erase statusline dto.
            emptyworkstring();
            setst_text();
          }
    }

    BM_reset_move_info();

    if( player[act_player].place_cue_ball ) player[act_player].place_cue_ball=0;

    if(foul) nextplayer=1;

    if(nextplayer){
        player[act_player].queue_view=*pqueue_view;
        act_player = (act_player==1) ? 0 : 1 ;
        if( foul ) player[act_player].place_cue_ball=1;
        *pXque=player[act_player].Xque;
        *pqueue_view=player[act_player].queue_view;
        if(!winner) {
           if(strlen(player[act_player].name) > 0) {
              //player %s court
              sprintf(statusstr,localeText[192],player[act_player].name);
              concatworkstring(statusstr);
           } else {
              //player %d court
              sprintf(statusstr,localeText[193],act_player+1);
              concatworkstring(statusstr);
           }
        }
    } else {
        if(!winner) {
           //Player remains on course
           concatworkstring(localeText[194]);
        }
    }

    setst_text();

#undef balls
#undef act_player
}

/***********************************************************************/

void evaluate_last_move_9ball( struct Player * player, int * pact_player, BallsType * pballs, int * pqueue_view, GLfloat * pXque )
{
#define act_player (*pact_player)
    int nextplayer=1;
    int i, dummy;
    int foul = 0;
    int minball = 15;
    int winner = 0;       // to check for a winner for the statusline, ooh, ugly
    char statusstr[200];  // make a better statusline

    emptyworkstring();

    for(i=0;i<pballs->nr;i++){
        if( pballs->ball[i].nr!=0 && pballs->ball[i].nr<minball && pballs->ball[i].in_game ) {
            minball=pballs->ball[i].nr;
        }
    }

    dummy=BM_get_min_ball_out();
    if ( dummy<minball && dummy!=0 ) minball=dummy;

    //fprintf(stderr,"eval_move_9ball:      minball: %d\n",minball);
    //fprintf(stderr,"eval_move_9ball: 1st ball hit: %d\n",BM_get_1st_ball_hit());

    if( BM_get_1st_ball_hit()==minball && BM_get_balls_out_all() > 0 && !BM_get_white_out())
      {
        nextplayer=0;
      }

    if( BM_get_balls_hit()==0 ) {
        foul=1;
        //No ball hit
        concatworkstring(localeText[190]);
        }

    if( BM_get_1st_ball_hit()!=minball ) {
        foul=1;
        //Wrong ball is played
        concatworkstring(localeText[195]);
        }

    /* if white out*/
    if( BM_get_white_out() )
      {
        foul=1;
        nextplayer=1;
        pballs->ball[0].in_game=1;
        pballs->ball[0].in_hole=0;
        //Game ball lost
        concatworkstring(localeText[191]);
      }

    /* if foul - place cue ball */
    if( foul ){
        /* this is done now for all balls in billard3d.c after evaluate_last_move  */
        pballs->ball[0].v=vec_xyz(0.0,0.0,0.0);
        pballs->ball[0].w=vec_xyz(0.0,0.0,0.0);
        pballs->ball[0].r=vec_xyz(0,-TABLE_L/4.0,0.0);
#ifdef USE_SOUND
        pballs->ball[0].soundplayed = 0;
        if(options_gamemode==options_gamemode_tournament && player[0].is_AI && player[1].is_AI) {
          //nosound
        } else {
        	PlaySound(wave_oneball,options_snd_volume);
        }
#endif
    }

    //fprintf(stderr,"foul:%d, nextplayer:%d, BM_get_ball_out(9):%d\n",foul,nextplayer,BM_get_ball_out(9));
    if( !foul && !nextplayer && BM_get_ball_out(9) ){
        player[act_player].winner=1;
        // erase statusline because game is over
        emptyworkstring();
        setst_text();
    }

    if( foul && BM_get_ball_out(9) ){
        player[(act_player==1)?0:1].winner=1;
        winner = 1;
        // erase statusline because game is over
        emptyworkstring();
        setst_text();
    }

    BM_reset_move_info();

    if( player[act_player].place_cue_ball ) player[act_player].place_cue_ball=0;

    if(nextplayer){
        player[act_player].queue_view=*pqueue_view;
        act_player = (act_player==1) ? 0 : 1 ;
        if( foul ) player[act_player].place_cue_ball=1;
        *pXque=player[act_player].Xque;
        *pqueue_view=player[act_player].queue_view;
        if(!winner) {
           if(strlen(player[act_player].name)>0) {
              //player %s court
              sprintf(statusstr,localeText[192],player[act_player].name);
              concatworkstring(statusstr);
           } else {
              //player %d court
              sprintf(statusstr,localeText[193],act_player+1);
              concatworkstring(statusstr);
           }
        }
    } else {
        if (!winner) {
           //Player remains on course
           concatworkstring(localeText[196]);
        }
    }
    setst_text();

#undef act_player
}

/***********************************************************************/

void evaluate_last_move_carambol( struct Player * player, int * pact_player, BallsType * pballs, int * pqueue_view, GLfloat * pXque )
{
#define act_player (*pact_player)
#define CUE_BALL_IND (player[act_player].cue_ball)
    int nextplayer=1;
    int bhit1, bhit2, i;
    int foul = 0;
    char statusstr[200];  // make a better statusline

    emptyworkstring();

    bhit1=BM_get_nth_ball_hit_by_ind(CUE_BALL_IND,1);

    for( i=2 ; (bhit2=BM_get_nth_ball_hit_by_ind(CUE_BALL_IND,i))!=-1 && bhit2==bhit1 ; i++ );

    //fprintf(stderr,"cueball=%d,%d\n",CUE_BALL_IND, pballs->ball[CUE_BALL_IND].nr);
    //fprintf(stderr,"bhit1=%d\n",bhit1);
    //fprintf(stderr,"bhit2=%d\n",bhit2);

    if( BM_get_white_out() ){
      nextplayer=1;
      foul = 1;
      pballs->ball[0].in_game=1;
      pballs->ball[0].in_hole=0;
      pballs->ball[0].v=vec_xyz(0.0,0.0,0.0);
      pballs->ball[0].w=vec_xyz(0.0,0.0,0.0);
      pballs->ball[0].r=vec_xyz(TABLE_W/4.0,-TABLE_L/4.0,0.0);
      //White cue ball is lost.
      concatworkstring(localeText[197]);
      player[act_player].score--;
#ifdef USE_SOUND
      if(options_gamemode==options_gamemode_tournament && player[0].is_AI && player[1].is_AI) {
        //nosound
      } else {
      	PlaySound(wave_oneball,options_snd_volume);
      }
#endif
      }
    if( BM_get_ball_out(1) ){
      nextplayer=1;
      foul = 1;
      pballs->ball[1].in_game=1;
      pballs->ball[1].in_hole=0;
      pballs->ball[1].v=vec_xyz(0.0,0.0,0.0);
      pballs->ball[1].w=vec_xyz(0.0,0.0,0.0);
      pballs->ball[1].r=vec_xyz(0.0,-TABLE_L/4.0,0.0);
      //yellow cue ball is lost.
      concatworkstring(localeText[198]);
      player[act_player].score--;
#ifdef USE_SOUND
      if(options_gamemode==options_gamemode_tournament && player[0].is_AI && player[1].is_AI) {
        //nosound
      } else {
      	PlaySound(wave_oneball,options_snd_volume);
      }
#endif
      }
    if( BM_get_ball_out(2) ){
      nextplayer=1;
      foul = 1;
      pballs->ball[2].in_game=1;
      pballs->ball[2].in_hole=0;
      pballs->ball[2].v=vec_xyz(0.0,0.0,0.0);
      pballs->ball[2].w=vec_xyz(0.0,0.0,0.0);
      pballs->ball[2].r=vec_xyz(0.0,+TABLE_L/4.0,0.0);
      player[act_player].score--;
      //red ball lost
      concatworkstring(localeText[199]);
#ifdef USE_SOUND
      if(options_gamemode==options_gamemode_tournament && player[0].is_AI && player[1].is_AI) {
        //nosound
      } else {
      	PlaySound(wave_oneball,options_snd_volume);
      }
#endif
      }

    if(!foul && bhit2!=-1 && bhit1!=-1 && bhit2!=bhit1 ){
      nextplayer=0;
      player[act_player].score++;
      //Good hit - make the next
      concatworkstring(localeText[200]);
    }

    BM_reset_move_info();

    if(nextplayer){
        player[act_player].queue_view=*pqueue_view;
        act_player = (act_player==1) ? 0 : 1 ;
        *pXque=player[act_player].Xque;
        *pqueue_view=player[act_player].queue_view;
        if(strlen(player[act_player].name)>0) {
           // Player %s court
           sprintf(statusstr,localeText[192],player[act_player].name);
           concatworkstring(statusstr);
        } else {
           //Player %d court
           sprintf(statusstr,localeText[193],act_player+1);
           concatworkstring(statusstr);
        }
    }

    if(!nextplayer && player[act_player].score >= options_maxp_carambol) {
        // Carambol winner
        player[act_player].winner=1;
        // erase statusline because game is over
        emptyworkstring();
        setst_text();       
       }

    setst_text();

#undef CUE_BALL_IND
#undef act_player
}

/***********************************************************************/

void evaluate_last_move_snooker( struct Player * player, int * pact_player, BallsType * pballs, int * pqueue_view, GLfloat * pXque )
{
#define act_player (*pact_player)
#define IS_RED(x) ( x==1 || x>=8 )
    int red_balls_are_in_game=0;
    //static SnookerState st={SN_PLAY_RED};
    int color_to_pot;
    int i;
    int act_score=0;
    int act_penalty=0;
    int foul=0;
    int ball_out;
    int other_player;
    int b1hit;
    char statusstr[200];  // make a better statusline
#ifdef USE_SOUND
    int playsound = 0;    // if set, play sound for new ball in only for one time

    for(i=0;i<22;i++) {
        pballs->ball[i].soundplayed = 0;
        }
#endif
    emptyworkstring();
    other_player=(act_player==1)?0:1;
    b1hit = BM_get_1st_ball_hit();
    if(b1hit>=8) b1hit=1;
    if(player[act_player].place_cue_ball ) player[act_player].place_cue_ball=0;
    for(i=0;i<pballs->nr;i++) {
        if( IS_RED(pballs->ball[i].nr) && pballs->ball[i].in_game ) {
            red_balls_are_in_game=1; 
            break;
        }
    }

    /* if white ball in */
    if( BM_get_white_out() ) {
        foul=1;
        act_penalty =MAX(act_penalty,(BM_get_1st_ball_hit()<=7?BM_get_1st_ball_hit():4));
        spot_snooker_ball(pballs,0);
        player[other_player].place_cue_ball=1;
        //Game ball lost
        concatworkstring(localeText[201]);
#ifdef USE_SOUND
        if(options_gamemode==options_gamemode_tournament && player[0].is_AI && player[1].is_AI) {
          //nosound
        } else {
        	PlaySound(wave_oneball,options_snd_volume);
        }
#endif
    }

    switch(st.to_play)
    {
    case SN_PLAY_RED:
        color_to_pot=1;
        if(b1hit!=1) {
            foul=1;
            act_penalty=MAX(act_penalty,b1hit);
          }
        i=1;
        while((ball_out=BM_get_nth_ball_out(i++))>=0) {
            if(IS_RED(ball_out)) {
                act_score+=1;
              } else {
                act_penalty=MAX(act_penalty,ball_out);
                foul=1;
#ifdef USE_SOUND
                playsound++;
#endif
              }
          }
        for(i=2;i<8;i++) {  // red out ?
            if( BM_get_ball_out(i)) {
                spot_snooker_ball(pballs,i);
            }
          }
        st.to_play=SN_PLAY_ANY_COLOR;
        if(foul) {
          //Red ball was required.
          concatworkstring(localeText[202]);
        } else {
          if(act_score>0) {
            //red ball - good
            concatworkstring(localeText[203]);
            }
          }
        break;
    case SN_PLAY_ANY_COLOR:
        if(b1hit==1) {
            foul=1;
            act_penalty=MAX(act_penalty,7);
          }
        color_to_pot=b1hit;
        i=1;
        while((ball_out=BM_get_nth_ball_out(i++))>=0) {
            if(ball_out==color_to_pot) {
                act_score+=ball_out;
              } else {
                foul=1;
                act_penalty=MAX(act_penalty,ball_out==1?7:ball_out);
              }
#ifdef USE_SOUND
                playsound++;
#endif
        }
        if(red_balls_are_in_game) {
            st.to_play=SN_PLAY_RED;
          } else {
            st.to_play=SN_PLAY_YELLOW;
          }
        for(i=2;i<8;i++) {
            if( BM_get_ball_out(i))
                spot_snooker_ball(pballs,i);
          }
        if(foul) {
          //Colored ball was required.
          concatworkstring(localeText[204]);
        } else {
          if(act_score>0) {
            //Colored ball - good
            concatworkstring(localeText[205]);
          }
        }
        break;
    case SN_PLAY_YELLOW:
    case SN_PLAY_GREEN:
    case SN_PLAY_BROWN:
    case SN_PLAY_BLUE:
    case SN_PLAY_PINK:
    case SN_PLAY_BLACK:
        color_to_pot=st.to_play;
        if(b1hit!=color_to_pot) {
            foul=1;
            act_penalty=MAX(act_penalty,b1hit);
            act_penalty=MAX(act_penalty,color_to_pot);
          }
        i=1;
        while((ball_out=BM_get_nth_ball_out(i++))>=0) {
            if(ball_out==color_to_pot) {
              act_score+=ball_out;
            } else {
              foul=1;
              act_penalty=MAX(act_penalty,b1hit);
              act_penalty=MAX(act_penalty,color_to_pot);
#ifdef USE_SOUND
              playsound++;
#endif
            }
        }
        if(!foul && act_score>0) st.to_play++;

        for(i=st.to_play;i<8;i++) {
            if( BM_get_ball_out(i))
                spot_snooker_ball(pballs,i);
        }
        if(foul) {
          //% s was required.
          sprintf(statusstr,localeText[206],localeText[st.to_play+177]);
          concatworkstring(statusstr);
        } else {
          if(act_score>0) {
            //% s holed - well done.
            sprintf(statusstr,localeText[207],localeText[st.to_play+176]);
            concatworkstring(statusstr);
            }
        }
        break;
    case SN_DONE:
        break;
    }

    if(foul) {
        act_penalty =MAX(act_penalty,4);
        player[other_player].score += act_penalty ;
      } else {
        player[act_player].score += act_score ;
      }
    if(act_score==0 || foul) {
        if(red_balls_are_in_game) {
            st.to_play=SN_PLAY_RED;
          } else {
            if(st.to_play<=SN_PLAY_ANY_COLOR) {
                st.to_play=SN_PLAY_YELLOW;
              }
          }
        player[act_player].queue_view=*pqueue_view;
        act_player = other_player;
        *pXque=player[act_player].Xque;
        *pqueue_view=player[act_player].queue_view;
        if(strlen(player[act_player].name)>0) {
           //player %s court
           sprintf(statusstr,localeText[192],player[act_player].name);
           concatworkstring(statusstr);
        } else {
           //player %d court
           sprintf(statusstr,localeText[193],act_player+1);
           concatworkstring(statusstr);
        }
      }

    player[act_player].snooker_on_red=st.to_play==SN_PLAY_RED;
    player[act_player].snooker_next_color=st.to_play;

    if(st.to_play==SN_DONE) {
        other_player = (act_player+1)%2;
        if(player[act_player].score>player[other_player].score) {
            player[act_player].winner=1;
            player[other_player].winner=0;
          }
        if(player[act_player].score<player[other_player].score) {
            player[act_player].winner=0;
            player[other_player].winner=1;
          }
        // game over, clear statusline
        emptyworkstring();
        setst_text();
      }

#ifdef USE_SOUND
    if(playsound) {
     if(options_gamemode==options_gamemode_tournament && player[0].is_AI && player[1].is_AI) {
       //nosound
     } else {
     	PlaySound(wave_oneball,options_snd_volume);
     }
    }
#endif

    BM_reset_move_info();
    setst_text();
}

/***********************************************************************/

void setfunc_evaluate_last_move(void (* eval_func)( struct Player * player, int * actual_player, BallsType * pballs, int * pqueue_view, GLfloat * pXque ) )
{
    evaluate_last_move=eval_func;
}

/***********************************************************************/

void initstatustext() {
   if (!stline1) {
      stline1 = textObj_new(emptystring, options_status_fontname, 20); //with place enough for 255 Characters !!!

      textObj_setText(stline1, emptystring);
      stline1_act = 0;
      
   }
   if (!stline2) {
      stline2 = textObj_new(emptystring, options_status_fontname, 20); // dto.
      textObj_setText(stline1, emptystring);
      stline2_act = 0;
   }
}

/***********************************************************************/

// Set Text of the statusline
// Call this function only with the correct setting of the OpenGL functions
// the call is inside billard3d.c inside the correct context settings
// have a look there

// Draw the statusline incl. the second optional
// the pointers stline1 and stline2 have to be initialized with correct values
// of a textObj or a null-pointer

void drawstatustext(int win_width, int win_height) {

    if(!options_status_text) {
       return;
    }
    drawstatuswocheck(win_width, win_height);
}

/***********************************************************************/

void drawstatuswocheck(int win_width, int win_height) {

    glPushMatrix();
    glTranslatef(-0.96,0.87,-1.0);
    glScalef(2.0/win_width,2.0/win_height,1.0);
    if (stline1) { 
      textObj_draw( stline1);
      }
    if (stline2) {
      glTranslatef(0.0,-22.0,0.0);
      textObj_draw(stline2);
      }
    glPopMatrix();
}

/***********************************************************************/
// concatenate the workstrings for the statusline
// without initializing the statusline stline1 and stline2 textObj

void concatworkstring (char * concattext) {
    if (utf8count(workstring1) < 60) {
      strcat(workstring1, concattext );
      stline1_act = 1;
    } else {
      strcat(workstring2, concattext );
      stline2_act = 1;
    }
}

/***********************************************************************/
// set the statusline for output

void setst_text() {
    if(stline1_act) {
      textObj_setText(stline1, workstring1);
    } else {
      textObj_setText(stline1, emptystring);
    }
    if(stline2_act) {
      textObj_setText(stline2, workstring2);
    } else {
      textObj_setText(stline2, emptystring);
    }
}

/***********************************************************************/
// make the workstrings empty
// and empty the checkvariables too

void emptyworkstring () {
    stline1_act = 0;
    workstring1[0] = '\0';
    stline2_act = 0;
    workstring2[0] = '\0';
}

/***********************************************************************/
// display an entire string

void displaystring (char *statusstr) {
    emptyworkstring();
    concatworkstring(statusstr);
    setst_text();
}

/***********************************************************************/
// set the lower line for the next to play color in snooker

void snooker_color(char *nextcolor, int pscore, int player_no, int loop) {
  if(st.to_play == SN_DONE || player_no!=loop) { //game over or not actual player
    sprintf( nextcolor, "%+04d", pscore);
  } else {
    sprintf( nextcolor, "%+04d  %s", pscore, localeText[177+st.to_play]);
  }
}
