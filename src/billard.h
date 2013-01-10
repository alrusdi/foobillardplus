/* billard.h
**
**    includefile for code for positioning the balls
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

#ifndef BILLARD_H
#define BILLARD_H

#include <stdlib.h>   /* size_t */
#include "options.h"
#include "billmove.h"
#include "vmath.h"
#include <math.h>

#define TABLE_W  (options_table_size/2.0)
#define TABLE_L  (options_table_size)
#define TABLE_H  0.84      /* 84 cm */
#define BALL_M      0.17   /* 170 g */
#define BALL_D   57.15e-3  /* 57.15 mm */
#define HOLE1_R   (0.110/2.0)  /* d=110 mm */
#define HOLE2_R   (0.115/2.0)  /* d=115 mm */
#define QUEUE_L    1.4     /* 1.4 m */
#define QUEUE_D1   0.035   /* 3.5cm */
#define QUEUE_D2   0.010   /* 1.0cm */
#define BANDE_D   0.035  /* 3.5cm to be conform with normed opening of middle pockets */
#define BANDE_D2RATIO  0.5  /* (1-0.3)*BANDE_D */
#define BANDE_D2 (BANDE_D*(1.0-0.3))  /* (1-0.3)*BANDE_D */
#define HOLE1_W  (2.0*HOLE1_R-SQR2*BANDE_D*(1.0-HOLE1_TAN))  /* */
#define HOLE2_W  (HOLE2_R*2.0+HOLE2_TAN*BANDE_D*2.0)  /* */
#define HOLE1_WH  HOLE1_W/2.0  /* */
#define HOLE2_WH  HOLE2_W/2.0  /* */
#define FRAME_D  (2.0*HOLE2_R+0.05)   /* d_hole+5cm */
#define FRAME_H   0.09   /*  9cm */
#define FRAME_H2  0.16   /* 16cm */
#define FRAME_DH 0.010           /* +7mm */
#define FRAME_PHASE 0.01        /* 1cm */
#define FRAME_DW    0.017        /* 1.7cm */
#define WOOD_D   (FRAME_D-BANDE_D)  /* */
#define HOLE1_TAN  1.42815 /* cotan(35�) */
#define HOLE2_TAN  0.36397 /* tan(20�) */
#define HOLE1_XYOFFS (0.04/SQR2)  /* */
#define HOLE2_XYOFFS (HOLE2_R+0.005)  /* */
#define HOLE1_AIMOFFS 0.02  /* */
#define HOLE2_AIMOFFS 0.02  /* */
#define HOLE1_PHASE   0.005  /* */
#define HOLE2_PHASE   0.005  /* */
#define SQR2     sqrt(2.0)
#define BALL_FULL 1
#define BALL_HALF 2
#define BALL_ANY  0

#ifndef BILLARD_C
extern void (*create_scene)( BallsType * balls );
extern void (*create_walls)( BordersType * walls );
#endif

void create_0hole_walls( BordersType * walls );   /* carambol */
void create_6hole_walls( BordersType * walls );   /* pool */

void create_8ball_scene   ( BallsType * balls );   /* 8-pool */
void create_9ball_scene   ( BallsType * balls );   /* 9-pool */
void create_carambol_scene( BallsType * balls );   /* carambol */
void create_snooker_scene ( BallsType * balls );   /* snooker */

int balls_in_game( BallsType * balls, int full_half );

void setfunc_create_scene( void (*func)( BallsType * balls ) );
void setfunc_create_walls( void (*func)( BordersType * walls ) );
void setfunc_malloc_free( void * (*func_malloc)( size_t size ), void (*func_free)( void * ptr ) );

#endif /* BILLARD_H */
