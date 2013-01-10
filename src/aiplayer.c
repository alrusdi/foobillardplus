/* aiplayer.c
**
**    code for positioning artifitial intelligence player
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

#define AIPLAYER_C
#include "aiplayer.h"
#include "vmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

VMvect (*ai_get_stroke_dir)( BallsType * balls, BordersType * walls, struct Player *pplayer ) = ai_get_stroke_dir_8ball;

static VMfloat ai_skill=1.0; /* not used ...yet */
static VMfloat ai_err=0.0;

/***********************************************************************/

void ai_set_skill( VMfloat skill ) /* not used ...yet */
{
    ai_skill=skill;
}

/***********************************************************************/

void ai_set_err( VMfloat err )
{
    ai_err=err;
}

/***********************************************************************/

static int my_rand(int nr)
{
    return rand()%nr;
}

/***********************************************************************/

static VMfloat my_rand01(void)
{
    return (VMfloat)rand()/(VMfloat)RAND_MAX;
}

/***********************************************************************/

VMfloat stroke_angle( BallType * bcue, BallType * bhit, HoleType * hole )
{
    VMvect r_hit;

    r_hit = vec_scale(vec_unit(vec_diff(bhit->r,hole->aim)),(bcue->d+bhit->d)/2.0);
    r_hit = vec_add(bhit->r,r_hit);
    return(
           vec_angle(vec_diff(r_hit,bcue->r), vec_diff(hole->aim,r_hit))
          );
}

/***********************************************************************/

int ball_in_way( int ballnr, VMvect aim, BallsType * balls )
{
    VMvect way, iball;
    VMfloat par, norm, lway;
    int inway=0;
    int i;

    for(i=0;i<balls->nr;i++) {
      if( balls->ball[i].in_game && i!=ballnr ){
        way   = vec_diff(aim,balls->ball[ballnr].r);
        lway  = vec_abs(way);
        iball = vec_diff(balls->ball[i].r,balls->ball[ballnr].r);
        par   = vec_mul(vec_unit(way),iball);
        norm  = vec_abs(vec_cross(vec_unit(way),iball));
        if( par>0.0 && par<lway && norm<(balls->ball[i].d+balls->ball[ballnr].d)/2.0 ){
            //fprintf(stderr,"ball_in_way:%d (ballnr=%d)\n",i,ballnr);
            inway=1;
            break;
        }
      }
    }
    return( inway );
}

/***********************************************************************/

int ind_ball_nr( int nr, BallsType * balls )
{
    int i;
    for( i=0 ; i<balls->nr ; i++ ){
        if( balls->ball[i].nr == nr ) break;
    }
    return i;
}

/***********************************************************************/

int nth_in_game( int n, BallsType * balls, int full_half )
{
    int i;
    for( i=0; i<balls->nr && n>=0; i++ ){
        if( full_half == BALL_FULL && balls->ball[i].nr<8 && balls->ball[i].nr>0 ){
            n--;
        }
        if( full_half == BALL_HALF && balls->ball[i].nr>8 ){
            n--;
        }
        if( full_half == BALL_ANY && ( balls->ball[i].nr>8 || (balls->ball[i].nr<8 && balls->ball[i].nr>0) )){
            n--;
        }
    }
    return i;
}

/***********************************************************************/

VMvect ai_get_stroke_dir_8ball( BallsType * balls, BordersType * walls, struct Player * pplayer )
{
#define full_half (pplayer->half_full)
    VMvect r_hit;
    VMfloat angle;
    VMfloat minangle = M_PI;
    BallType *bhit, *bcue;
    HoleType *hole;
    int minball=0;
    int minhole=-1;
    int i,j;

    bcue = &balls->ball[0];
    for( i=1; i<balls->nr; i++ ) if ( balls->ball[i].in_game ){
        if( ( full_half==BALL_HALF && balls->ball[i].nr>8 ) ||
            ( full_half==BALL_FULL && balls->ball[i].nr<8 ) ||
            ( full_half==BALL_ANY  && balls->ball[i].nr!=8 ) ||
            ( balls->ball[i].nr==8 && balls_in_game(balls,full_half)==0 ) ){
            bhit = &balls->ball[i];
            for( j=0; j<walls->holenr; j++ ){
                hole = &walls->hole[j];
                r_hit = vec_scale(vec_unit(vec_diff(bhit->r,hole->aim)),(bcue->d+bhit->d)/2.0);
                r_hit = vec_add(bhit->r,r_hit);
                if( !ball_in_way(0,r_hit,balls) && !ball_in_way(i,hole->aim,balls) ){
                    angle = fabs( vec_angle( vec_diff(r_hit,bcue->r), vec_diff(hole->aim,r_hit) ) );
                    if( angle<minangle ){
                        minball = i;
                        minhole = j;
                        minangle = angle;
                        //fprintf(stderr,"aiplayer: ball:%d hole:%d\n",balls->ball[i].nr,minhole);
                    }
                }
            }
        }
    }
    //fprintf(stderr,"aiplayer: 1\n");

    if( minball==0 ){  /* no proper ball found */
        minball=0;
        switch(full_half) {
          case BALL_FULL:
            if( BM_get_balls_out_full()!=7 ){
                minball=1+my_rand(7-BM_get_balls_out_full());
            }
            break;
          case BALL_HALF:
            if( BM_get_balls_out_half()!=7 ){
                minball=1+my_rand(7-BM_get_balls_out_half());
            }
            break;
          case BALL_ANY:
            if( BM_get_balls_out_total()!=15 ){
                minball=1+my_rand(15-BM_get_balls_out_total());
            }
            break;
        }
        if( minball==0 ){
            minball = ind_ball_nr(8,balls);
        } else {
            minball = nth_in_game(minball,balls,full_half);
        }
    }
    //fprintf(stderr,"aiplayer: 2\n");

    bhit = &balls->ball[minball];
    if(minhole!=-1){
        hole = &walls->hole[minhole];
        r_hit = vec_scale(vec_unit(vec_diff(bhit->r,hole->aim)),(bcue->d+bhit->d)/2.0);
        r_hit = vec_diff(vec_add(bhit->r,r_hit),bcue->r);
    } else {  /* no proper ball found */
        //fprintf(stderr,"aiplayer: no proper ball found\n");
        r_hit = vec_diff(bhit->r,bcue->r);
    }

    r_hit=vec_add(r_hit,vec_scale(vec_xyz(my_rand01()-0.5,my_rand01()-0.5,my_rand01()-0.5),0.02*ai_err)); ///vec_abs(r_hit)));

    return vec_unit(r_hit);
#undef full_half
}

/***********************************************************************/

VMvect ai_get_stroke_dir_9ball( BallsType * balls, BordersType * walls, struct Player * pplayer ) {
	   // pplayer not used in function, but needed from other calls to set_ai_get_stroke_dir
	   // Don't optimize this
    VMvect r_hit;
    VMfloat angle;
    VMfloat minangle = M_PI;
    BallType *bhit, *bcue;
    HoleType *hole;
    int minind=0;
    int minnr=15;
    int minhole=-1;
    int i,j;

    bcue = &balls->ball[0];
    for( i=1; i<balls->nr; i++ ){
        if ( balls->ball[i].in_game && balls->ball[i].nr<minnr ){
            minnr = balls->ball[i].nr;
            minind = i;
        }
    }
    bhit = &balls->ball[minind];
    for( j=0; j<walls->holenr; j++ ){
        hole = &walls->hole[j];
        r_hit = vec_scale(vec_unit(vec_diff(bhit->r,hole->aim)),(bcue->d+bhit->d)/2.0);
        r_hit = vec_add(bhit->r,r_hit);
        if( !ball_in_way(0,r_hit,balls) && !ball_in_way(minind,hole->aim,balls) ) {
            angle = fabs( vec_angle( vec_diff(r_hit,bcue->r), vec_diff(hole->aim,r_hit) ) );
            if( angle<minangle ){
                minhole = j;  minangle = angle;
                //fprintf(stderr,"aiplayer: hole:%d\n",minhole);
            }
        } /* else {
            fprintf(stderr,"aiplayer: hole:%d - no! inway1=%d, inway2=%d\n",j,ball_in_way(0,r_hit,balls),ball_in_way(i,hole->aim,balls));
        } */
    }

    if(minhole!=-1){
        hole = &walls->hole[minhole];
        r_hit = vec_scale(vec_unit(vec_diff(bhit->r,hole->aim)),(bcue->d+bhit->d)/2.0);
        r_hit = vec_diff(vec_add(bhit->r,r_hit),bcue->r);
    } else {  /* no proper ball found */
        //fprintf(stderr,"aiplayer: no proper hole found\n");
        r_hit = vec_diff(bhit->r,bcue->r);
    }

    r_hit=vec_add(r_hit,vec_scale(vec_xyz(my_rand01()-0.5,my_rand01()-0.5,my_rand01()-0.5),0.02*ai_err)); //*vec_abs(r_hit)));

    return vec_unit(r_hit);
}

/***********************************************************************/

#define IS_RED(x) ( x==1 || x>=8 )

int snooker_ball_legal(int ball,struct Player *player)
{
    return ((IS_RED(ball) && player->snooker_on_red)
          ||(!IS_RED(ball) && !player->snooker_on_red && player->snooker_next_color<=1)
          ||(player->snooker_on_red==0 && ball==player->snooker_next_color && player->snooker_next_color>1));
}

/***********************************************************************/

#define CUE_OBJ_WEIGHT 0.2
#define OBJ_HOLE_WEIGHT 0.7
#define POINT_WEIGHT 0.3
#define CENTERHOLE_ANGLE_WEIGHT 1
#define CORNERHOLE_ANGLE_WEIGHT 1
#define ANGLE_WEIGHT 1

VMvect ai_get_stroke_dir_snooker( BallsType * balls, BordersType * walls, struct Player * pplayer )
{
    VMvect r_hit = vec_null();
    VMvect hole_aim = vec_null();
    VMvect hole_aim_def = vec_null();
    VMvect min_r_hit = vec_null();
    VMfloat angle, hole_angle,weight,corner_acentrism, d;
    BallType *bhit, *bcue;
    HoleType *hole;
    int minball=0;
    int minhole=-1;
    int i,j;
    int legal_ball=1;
    VMfloat minweight=12;

    //fprintf(stderr,"aiplayer: start player[%s]\n",pplayer->name);

    bcue = &balls->ball[0];
    for( i=balls->nr-1;i>=1; i-- ) if ( balls->ball[i].in_game ){
        if(snooker_ball_legal(i,pplayer)) {
            bhit = &balls->ball[i];
            for( j=0; j<walls->holenr; j++ ){
                hole = &walls->hole[j];
                if(j>1) {
                    hole_aim=hole->aim;
                    if(hole_aim.x>0) hole_aim.x-=0.007;
                    if(hole_aim.x<0) hole_aim.x+=0.007;
                    if(hole_aim.y>0) hole_aim.y-=0.007;
                    if(hole_aim.y<0) hole_aim.y+=0.007;
                } else {
                    hole_aim=vec_scale(hole->aim,1.02);
                }
                r_hit = vec_scale(vec_unit(vec_diff(bhit->r,hole_aim)),(bcue->d+bhit->d)/2.0);
                r_hit = vec_add(bhit->r,r_hit);
                if( !ball_in_way(0,r_hit,balls) && !ball_in_way(i,hole_aim,balls) ){
                    angle = fabs( vec_angle( vec_diff(r_hit,bcue->r), vec_diff(hole_aim,r_hit) ) );
                    hole_angle = vec_angle(vec_diff(hole_aim,bhit->r),vec_diff(vec_xyz(2*hole_aim.x,hole_aim.y,0),hole_aim));
                    corner_acentrism=fabs(hole_angle-M_PI/4);
                    /*fprintf(stderr,"aiplayer: ball:%d hole:%d raw hole_angle %f\n",balls->ball[i].nr,j, hole_angle*180/M_PI);*/
                    /* normalized to give an unweighted maximum of +- 2.5 per item */
                    weight = 0.64 * ANGLE_WEIGHT * pow(angle,3)
                             +(j<=1)*0.64 * CENTERHOLE_ANGLE_WEIGHT * pow(hole_angle,3)
                             +(j>1) * 5.0 * CORNERHOLE_ANGLE_WEIGHT * pow(corner_acentrism,3)
                             +0.04 * OBJ_HOLE_WEIGHT * vec_abssq(vec_diff(bhit->r,hole_aim))
                             +0.15 * CUE_OBJ_WEIGHT * vec_abs(vec_diff(bhit->r,bcue->r))
                             -1.5 * POINT_WEIGHT *(i>=2||i<=7?i-2:0) 
                             ; 
                    if(weight <minweight){ 
                        minball = i; minhole = j;  minweight = weight;hole_aim_def=hole_aim;
                        //fprintf(stderr,"aiplayer: ball:%d hole:%d\n",i,minhole);
                    }
                }
            }
        }
    }
    /*fprintf(stderr,"aiplayer: 1\n");*/

    if( minball==0 ) {
    /* no pottable ball found, hit any legal ball */
        for( i=balls->nr-1;i>=1 && minball==0;i--) if ( balls->ball[i].in_game ){
            if(snooker_ball_legal(i,pplayer)) {
                for(d=-(balls->ball[0].d-0.001);d<(balls->ball[0].d-0.001) && minball==0;d+=0.005) {
                   VMvect offset,new_r_hit;
                   bhit = &balls->ball[i];

                   r_hit = vec_diff(bhit->r,bcue->r);

                   offset=vec_scale(vec_unit(vec_rotate(r_hit,vec_xyz(0,0,M_PI/2))),d);
                   new_r_hit = vec_diff(vec_add(bhit->r,offset),bcue->r);
                   new_r_hit = vec_diff(new_r_hit,vec_scale(vec_unit(new_r_hit),balls->ball[i].d/1.99));
                   
                   if( !ball_in_way(0,vec_add(balls->ball[0].r,new_r_hit),balls)) {
                       //fprintf(stderr,"ball found\n");
                       minball=i;
                       min_r_hit=new_r_hit;

                   }
               }
            }
        }
    }

    if( minball==0 ) {
    /* no legal ball found, just play anything */
        legal_ball=0;
        minball=0;
        if (pplayer->snooker_on_red) {
           minball=2+my_rand(6);
        } else if (pplayer->snooker_on_red==0 && pplayer->snooker_next_color<2) {
           minball=7+my_rand(15);
           if(minball==21) minball=1;
        } else {
           minball=pplayer->snooker_next_color;
        }
    }
    /*fprintf(stderr,"aiplayer: 2\n");*/

    bhit = &balls->ball[minball];
    if(minhole!=-1){
        hole = &walls->hole[minhole];
        r_hit = vec_scale(vec_unit(vec_diff(bhit->r,hole_aim_def)),(bcue->d+bhit->d)/2.0);
        r_hit = vec_diff(vec_add(bhit->r,r_hit),bcue->r);
    } else {  /* no proper ball found */
        if(legal_ball==0) {
           //fprintf(stderr,"aiplayer: no proper ball found\n");
           r_hit = vec_diff(bhit->r,bcue->r);
        } else {
           //fprintf(stderr,"aiplayer: can hit legal ball : %d\n",minball);
           r_hit=min_r_hit;
        }
    }

    r_hit=vec_add(r_hit,vec_scale(vec_xyz(my_rand01()-0.5,my_rand01()-0.5,my_rand01()-0.5),0.02*ai_err)); ///vec_abs(r_hit)));

    //(fprintf(stderr,"aiplayer: done\n");
    return vec_unit(r_hit);
}
#undef IS_RED

/***********************************************************************/

VMvect ai_get_stroke_dir_carambol( BallsType * balls, BordersType * walls, struct Player * pplayer ) {
	   // walls not used, but needed as set as function pointer
	   // Don't optimize this
#define CUE_BALL_IND (pplayer->cue_ball)
    int i,j,k;
    int foundshot=0;
    VMvect wc;    /* way of cueball to objectball 1 */
    VMvect w12;   /* way of cueball from objectball 1 to objectball 2 */
    VMvect d12;   /* vec from objectball 1 to objectball 2 */
    VMvect p1;    /* vec from ball1 to cueball at hit */
    VMvect min_wc;
    VMfloat min_angle=M_PI;
    VMfloat th;

    for(i=0;i<balls->nr;i++) if(i!=CUE_BALL_IND){             /* i = objectball 1 */
        for( j=0 ; j<balls->nr ; j++ ){                       /* j = objectball 2 */
            if(j!=CUE_BALL_IND && j!=i) break;
        }
        //fprintf(stderr,"ai_get_stroke_dir_carambol:i=%d,j=%d\n",i,j);
        d12 = vec_diff( balls->ball[j].r, balls->ball[i].r );
        th = acos(BALL_D/vec_abs(d12));
        for(k=0;k<2;k++){  /* the two possible tangents */
            p1  = vec_scale( vec_unit( vec_rotate(d12,vec_xyz(0,0,(k==0)?th:-th)) ), BALL_D );
            wc  = vec_diff( vec_add(balls->ball[i].r,p1), balls->ball[CUE_BALL_IND].r );
            w12 = vec_diff( balls->ball[j].r, vec_add(balls->ball[i].r,p1) );
            if( vec_mul(wc,w12)>0.0 && vec_mul(wc,p1)<0.0 ){  /* conditions for possible shot */
                foundshot=1;
                if( vec_angle( wc, w12 ) < min_angle ){
                    min_angle = vec_angle( wc, w12 );
                    min_wc = wc;
                }
            }
        }
    }
    if(foundshot){
        return vec_unit(min_wc);
    } else {
        return vec_unit( vec_diff(balls->ball[(CUE_BALL_IND+1+(rand()%2))%3].r, balls->ball[CUE_BALL_IND].r) );
    }
#undef CUE_BALL_IND
}

/***********************************************************************/

void setfunc_ai_get_stroke_dir(VMvect (*func)( BallsType * balls, BordersType * walls, struct Player * pplayer ))
{
    ai_get_stroke_dir=func;
}
