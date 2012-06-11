/* options.c
**
**    global variables for options
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

/*
 gold
 options_diamond_color 0xFFD566
 chrome
 options_diamond_color 0xBBBBFF
 copper
 options_diamond_color 0xFF8866
 green
 options_table_color   0x0D6621
 blue
 options_table_color   0x346070
 red
 options_table_color   0x802020
 ocker
 options_table_color   0x706034
*/

#include "vmath.h"
#include "options.h"

#if defined(WETAB)
enum archType arch = ARCH_WETAB;
#elif defined(WIN32)
enum archType arch = ARCH_WIN32;
#elif defined(WIN64)
enum archType arch = ARCH_WIN64;
#else
enum archType arch = ARCH_LINUX;
#endif

int    options_positional_light=1;
int    options_table_color   = 0x0D6621;
int    options_diamond_color = 0xFFD566;
int    options_frame_color   = 0x401405;
VMfloat options_table_size    = 2.1336;  /* 7 ft (smallest normed) */
int    options_lensflare     = 0;
#ifndef WETAB
    int options_max_ball_detail = 7;
    VMfloat options_ball_detail_nearmax = 0.4;
    VMfloat options_ball_detail_farmin  = 7.0;
#else
    int options_max_ball_detail = 5;
    VMfloat options_ball_detail_nearmax = 0.6;
    VMfloat options_ball_detail_farmin  = 6.5;
#endif
int    options_rgstereo_on         = 0;
int    options_free_view_on        = 0;
int    options_birdview_on         = 0;
int    options_col_ball_pool[]={
                            0xFFFFFF,  /* white */

                            0xEEDD00,  /*  1 yellow */
                            0x0000FF,  /*  2 blue   */
                            0xFF0000,  /*  3 red    */
                            0x7700CC,  /*  4 violet */
                            0xFF8800,  /*  5 orange */
                            0x009900,  /*  6 green  */
                            0x883311,  /*  7 brown  */
                            0x000000,  /*  8 black  */

                            0xEEDD00,  /*  9 yellow */
                            0x0000FF,  /* 10 blue   */
                            0xFF0000,  /* 11 red    */
                            0x7700CC,  /* 12 violet */
                            0xFF8800,  /* 13 orange */
                            0x009900,  /* 14 green  */
                            0x883311   /* 15 brown  */
                          };

int    options_col_ball_carambol[]={
                            0xFFFFFF,  /* cueball1 */
                            0xFFFF00,  /* cueball2 */
                            0xFF0000   /* red ball */
                          };

int    options_col_ball_snooker[]={
                            0xFFFFFF,   /* white cueball */
                            0xFF0000,   /* red    balls */
                            0xFFBB00,   /* orange ball */
                            0x00AA33,   /* green  ball */
                            0x883311,   /* brown  ball */
                            0x0000FF,   /* blue   ball */
                            0xFF66AA,   /* pink   ball */
                            0x000000,   /* black  ball */
                            0xFF0000,   /* red    balls */
                            0xFF0000,   /* red    balls */
                            0xFF0000,   /* red    balls */
                            0xFF0000,   /* red    balls */
                            0xFF0000,   /* red    balls */
                            0xFF0000,   /* red    balls */
                            0xFF0000,   /* red    balls */
                            0xFF0000,   /* red    balls */
                            0xFF0000,   /* red    balls */
                            0xFF0000,   /* red    balls */
                            0xFF0000,   /* red    balls */
                            0xFF0000,   /* red    balls */
                            0xFF0000,   /* red    balls */
                            0xFF0000,   /* red    balls */
                          };

int    *options_col_ball /*= options_col_ball_pool*/;
char   options_net_hostname [1024];  // initialized in billard3d.c
char   options_language [3] = "00";         // language, 00 = uninitialized
int    options_net_portnum = 56341;
int    options_net_speed = 1;        // Speed for network gameplay (*5 = calls per second)
int    options_net_speed_bak = 1;    // Speed for network for restoring after a network play
int    options_net_compatible = 0;   // network very slow compatibility mode
int    options_net_compatible_bak = 0;   // network very slow compatibility mode restoring after network play
int    options_ball_sphere = 0;      // standard sphere (0) or special (1) sphere blending on balls
int    options_rgaim = 0;            /* 0=middle 1=right 2=left */
int    options_bumpref   = 0;  /* bump reflection of chrome edges */
int    options_bumpwood  = 0;  /* bump maps on wood frame */
int    options_balltrace = 0;
int    options_use_sound = 1;
int    options_use_music = 1;
int    options_gamemode  = 0;  /* 0=options_gamemode_match */
int    options_avatar_on = 0;
int    options_vsync = 1;          // vsync on = 1, vsync off = 0 or SDL-Version does not understand vsync
VMfloat options_tourfast = 30.0;

#ifdef TOUCH
  int   options_control_kind = 1;  // behaviour of some Controlkeys 0 = toggle, 1 = set if pressed and unset if unpressed
  int   options_cuberef = 0;          // reflections on balls with cuberef reflections
#else
  int   options_control_kind = 0;  // behaviour of some Controlkeys 0 = toggle, 1 = set if pressed and unset if unpressed
  int   options_cuberef = 1;          // reflections on balls with cuberef reflections
#endif
int    options_cuberef_res = 128;    // how much resolution
int    options_oldmove = 0;        // old mouse movement style from original foobillard, default off
int    options_ai_birdview = 0;    // changes to birdview if player ist AI, Standard 0=off, 1=on
int    options_status_text = 1;    // Text of the statusline, 0 = off, !=0 is on
int    options_maxp_carambol = 10; // Points to arrive for winning a carambol Game
int    options_jump_shots = 1;     // Balls can (1) jump out of the table or not (0)
int    options_jump_shots_net_bak = 1;   // Balls can (1) jump out of the table or not (0) (network play restore int)
int    options_antialiasing = 1;   // Antialiasing functions, 0 = off, >0 on
int    options_fsaa = 0;           // FSAA Test 0 = nothing an 1 is present
int    options_fsaa_value = 0;     // if options_fsaa !=0, the value for fsaa 2 or 4
int    options_maxfsaa = 2;        // max. value for fsaa testing (1,2,4,8)
int    options_show_buttons = 1;   // show the control buttons on the screen = 1, or not = 0
int    options_mouseshoot = 1;     // mouseshoot with angle = 1, without only up/down = 0
int    options_auto_freemove = 1;  // after shoot switch back to freeview (Standard =1), otherwise let the old one = 0
int    options_anisotrop = 0;      // Anisotropic Filter 0 = no, 1 = yes
int    options_tronmode = 0;       // the special tron game mode
int    options_glassballs = 0;     // play with glass balls or other to implement ball types
int    options_deco = 1;           // special for showing deco on walls and so on (1 = on, 0 = off)
int    options_mus_volume = 10;
int    options_furniture = 1;      // special for showing furniture, chairs and so on (2 = high, 1 = default, 0 = off)

#ifdef WETAB
  int    options_snd_volume = 100;
#else
  int    options_snd_volume = 90;
#endif
GLfloat  options_maxanisotrop = 0.0;   // max. of anistropic filters
VMfloat  options_value_anisotrop = 0.0;    // the vaule for anisotropic filtering if supported and < maxanisotrop (default 0.0)

char   options_browser[20] = "firefox";  // the variable to hold the standard Browser for the Manual

/* queries for OpenGL extensions */
int extension_cubemap      = 0;
int extension_multitexture = 0;
int extension_combine      = 0;
int extension_dot3         = 0;
int extension_vp_ARB       = 0;

int extension_rc_NV        = 0;   /* NVIDIA register combiners */
int extension_ts_NV        = 0;   /* NVIDIA texture shader */
int extension_vp_NV        = 0;   /* NVIDIA vertex program */

