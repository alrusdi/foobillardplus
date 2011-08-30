/* options.h
**
**    global defines for options
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

#ifndef OPTIONS_H
  #define OPTIONS_H

#include <GL/gl.h>
#include "vmath.h"

#define options_tex_min_filter GL_LINEAR_MIPMAP_LINEAR
#define options_tex_mag_filter GL_LINEAR

/* Compatibility with the Intel GMA3150 in the WeTab
   very old behaviour of OpenGL 1.0 for some things
   don't use with FSAA or MSAA, Dating: 01/2011
   */

#define WETAB_ALIASING

/* End of compatibility section WeTab */

#define options_diamond_color_gold    0xFFD566
#define options_diamond_color_chrome  0xFFFFFF
#define options_diamond_color_copper  0xFF66D5
#define options_diamond_color_black   0x888888
#define options_table_color_red       0x802020
#define options_table_color_green     0x0D6621
#define options_table_color_blue      0x346070
#define options_table_color_black     0x383838
#define options_table_color_beige     0x807060
#define options_frame_color_white     0xCCCCCC
#define options_frame_color_default   0x401405

/* ball detail settings - very slow machines */
#define options_max_ball_detail_LOW 3
#define options_ball_detail_nearmax_LOW 0.7
#define options_ball_detail_farmin_LOW 5.0

/* ball detail settings - slow machines */
#define options_max_ball_detail_MED 4
#define options_ball_detail_nearmax_MED 0.7
#define options_ball_detail_farmin_MED 5.5

/* ball detail settings - fast machines */
#define options_max_ball_detail_HIGH 5
#define options_ball_detail_nearmax_HIGH 0.6
#define options_ball_detail_farmin_HIGH 6.5

/* ball detail settings - very fast machines */
#define options_max_ball_detail_VERYHIGH 7
#define options_ball_detail_nearmax_VERYHIGH 0.4
#define options_ball_detail_farmin_VERYHIGH  7.0

#define options_player_fontname    "DejaVuSans.ttf"
#define options_help_fontname      "DejaVuSans-Bold.ttf"
#define options_menu_fontname      "DejaVuSans-Bold.ttf"
#define options_winner_fontname    "DejaVuSans-Bold.ttf"
#define options_ball_fontname      "DejaVuSans-Bold.ttf"
#define options_score_fontname     "DejaVuSans-Bold.ttf"
#define options_roster_fontname    "DejaVuSans.ttf"
#define options_status_fontname    "DejaVuSans.ttf"

#ifdef WETAB
  #define options_snd_volume 2.0
#else
  #define options_snd_volume 1.0
#endif

#define options_3D_winnertext 1

#define options_gamemode_match      0
#define options_gamemode_training   1
#define options_gamemode_tournament 2

extern int     options_positional_light;
extern int     options_diamond_color;
extern int     options_table_color;
extern int     options_frame_color;
extern VMfloat options_table_size;
extern int     options_lensflare;
extern int     options_max_ball_detail;
extern VMfloat options_ball_detail_nearmax;
extern VMfloat options_ball_detail_farmin;
extern int     options_rgstereo_on;
extern int *   options_col_ball;
extern int     options_col_ball_pool[];
extern int     options_col_ball_carambol[];
extern int     options_col_ball_snooker[];
extern char    options_net_hostname[];
extern int     options_net_portnum;
extern int     options_net_compatible;           // network very slow compatibility mode
extern int     options_net_compatible_bak;       // network very slow compatibility mode restoring after network play
extern int     options_net_speed;        // Speed for network gameplay (calls per second)
extern int     options_net_speed_bak;    // Speed for network for restoring after a network play
extern int     options_free_view_on;
extern int     options_birdview_on;
extern int     options_cuberef;
extern int     options_cuberef_res;
extern int     options_rgaim;
extern int     options_bumpref;  /* bump reflection of chrome edges */
extern int     options_bumpwood; /* bumpmaps on wood frame */
extern int     options_balltrace;
extern int     options_use_sound;   /* for taking care of non-sound systems */
extern int     options_gamemode;
extern int     options_avatar_on;
extern VMfloat  options_tourfast;
extern int     options_control_kind;    // behaviour of some Controlkeys 0 = toggle, 1 = set if pressed and unset if unpressed
extern int     options_ai_birdview;     // changes to birdview if player ist AI, Standard 0=off, 1=on
extern int     options_status_text;     // Show the statusline ? != 0
extern int     options_maxp_carambol;   // Points to arrive for winning a carambol Game
extern int     options_jump_shots;      // Balls can (1) jump out of the table or not (0)
extern int     options_jump_shots_net_bak;   // Balls can (1) jump out of the table or not (0) (network play restore int)
extern int     options_antialiasing;    // Antialiasing functions, 0 = off, >0 on
extern int     options_fsaa;            // FSAA Test 0 = nothing an 1 is present
extern int     options_fsaa_value;      // if options_fsaa !=0, the value for fsaa 2 or 4
extern int     options_show_buttons;    // show the control buttons on the screen = 1, or not = 0
extern int     options_mouseshoot;      // mouseshoot with angle = 1, without only up/down = 0
extern int     options_auto_freemove;   // after shoot switch back to freeview (Standard =1), otherwise let the old one = 0
extern int     options_anisotrop;       // Anisotropic Filter 0 = no, 1 = yes
extern int     options_tronmode;        // the special tron game mode
extern int     options_glassballs;      // play with glass balls or other to implement ball types
extern int     options_deco;            // special for showing deco on walls, chairs and so on (1 = on, 0 = off)
extern GLfloat   options_maxanisotrop;    // max. of anistropic filters
extern VMfloat   options_value_anisotrop; // the vaule for aisotropic filtering if supported and < maxanisotrop

extern char    options_browser[20];     // the variable to hold the standard Browser for the Manual

/* queries for OpenGL extensions */
extern int extension_cubemap;
extern int extension_multitexture;
extern int extension_combine;
extern int extension_dot3;
extern int extension_vp_ARB;

extern int extension_rc_NV;   /* NVIDIA register combiners */
extern int extension_ts_NV;   /* NVIDIA texture shader */
extern int extension_vp_NV;   /* NVIDIA vertex program */

/* compatibility defines */

#ifndef GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB
#ifdef GL_TEXTURE_CUBE_MAP_POSITIVE_X
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB GL_TEXTURE_CUBE_MAP_POSITIVE_X
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB GL_TEXTURE_CUBE_MAP_POSITIVE_Y
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB GL_TEXTURE_CUBE_MAP_POSITIVE_Z
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB GL_TEXTURE_CUBE_MAP_NEGATIVE_X
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
#define GL_TEXTURE_CUBE_MAP_ARB GL_TEXTURE_CUBE_MAP
#define GL_REFLECTION_MAP_ARB GL_REFLECTION_MAP
#endif
#endif

#ifndef GL_TEXTURE0_ARB
#ifdef GL_TEXTURE0
#define GL_TEXTURE0_ARB GL_TEXTURE0
#define GL_TEXTURE1_ARB GL_TEXTURE1
#define GL_TEXTURE2_ARB GL_TEXTURE2

#define GL_TEXTURE3_ARB GL_TEXTURE3
#endif
#endif

enum netSpeed { NET_SPEED_LOW=1, NET_SPEED_NORMAL, NET_SPEED_MIDDLE, NET_SPEED_HIGH }; //network-speed *5 = 5,10,15,20 packets per second

#endif
