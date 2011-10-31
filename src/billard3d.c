/* billard3d.c
**
**    drawing all with OpenGL
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

/* some gcc compilers don't like to declare or use a function declared pointer inside a function.
 * I don't know why, but the program crashes if this is done. So most variables are declared outside
 * functions to solve the problem. Not a good programming, but I don't know other help
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#ifdef	HAVE_ENDIAN_H
	#include <endian.h>
#endif	// HAVE_ENDIAN_H
#include <getopt.h>

#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include <SDL.h>
#include <SDL_audio.h>
#ifdef NETWORKING
  #include <SDL_net.h>
#endif

#ifdef __MINGW32__ //RB
   void ( APIENTRY * glActiveTextureARB)( GLenum );
#endif
#include "language.h"
#include "billard.h"
#include "ball.h"
#include "table.h"
#include "queue.h"
#include "png_loader.h"
#include "aiplayer.h"
#include "options.h"
#include "player.h"
#include "evaluate_move.h"
#include "helpscreen.h"
#include "font.h"
#include "textobj.h"
#include "sys_stuff.h"
#ifdef NETWORKING
  #include "net_socket.h"
#endif
#include "sound_stuff.h"
#include "menu.h"
#include "room.h"
#include "mesh.h"

#define LIT 1
#define TEXTURED 2
#define REFLECT 3
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

//#define TIME_INTERPOLATE

/* control-flags */
int control__updated=0;  /* just activated */
int control__active=0;   /* one control is active */
/* the controls */
int control__english=0;
int control__cue_butt_updown=0;
int control__mouse_shoot=0;
int control__place_cue_ball=0;
int control__fov = 0;

#ifndef WETAB
   int win_width = 1024; //Standard for "others"
   int win_height = 768;
   static int fullscreen=0;  /* this is not updated during runtime - its only for startup */
#else
   int win_width = 1366; //958 <- game frame spec // specs for fullscreen on WeTab
   int win_height = 768; //750 <- game frame spec
#endif

VMfloat scr_dpi = 80.0; // dots per inch for Stereo viewing

BallsType balls;
BallsType bakballs;
BordersType walls;

static VMfloat step = 0.03;          // step begin keypress for left/right key
static VMfloat freeview_step = 0.03; // step begin keypress for up/down key
static int hudbuttonpressed = 0;   // if mousebutton is pressed on hud-buttons, 0 = no, 1 = yes

static int frametime_ms_max = 200;
static int frametime_ms = 40;

static GLuint table_obj = 0;
static GLfloat Xrot = -70.0, Yrot = 0.0, Zrot = 0.0;
static GLfloat Xque = -83.0, Zque = 0.0;
static GLfloat Xrot_offs=0.0, Yrot_offs=0.0, Zrot_offs=0.0;

int b1_hold = 0;
int start_x, start_y;
int mouse_moved_after_b1_dn = 0;
int b2_hold = 0;
int scaling_start, scaling_start2;
int b2_b1_hold = 0;

GLfloat cam_dist_aim = 2.5;
GLfloat cam_dist;
GLfloat cam_FOV=40.0;

VMvect  free_view_pos_aim;
VMvect  free_view_pos;

static int vline_id= -1;             // helpline glcompile-id
static int cueball_id= -1;           // place-cueball-tex glcompile-id
static int english_id= -1;           // english move glcompile-id
static int mleft_id= -1;             // left menu bar glcompile-id (training)
static int mleftnormal_id= -1;       // left menu bar glcompile-id (normal)
static int mright_id= -1;            // right menu bar glcompile-id
static int english1_id= -1;          // english move glcompile-id
static int shoot_id= -1;             // mouse-shoot glcompile-id
static int cuebutt_id= -1;           // cue butt up/down glcompile-id
static int cueball1_id= -1;          // helpline cross move glcompile-id
static int fov_id= -1;               // front of view glcompile-id
static int freeview_id= -1;          // freeview glcompile-id
static int cubemap_id= -1;           // cubemap glcompile-id
static int cubemap1_id= -1;          // cubemap glcompile-id
static int tourn_id= -1;             // tournament glcompile-id
static int hudstuff_id= -1;          // hud stuff mode changes glcompile-id
static int enddisp_id= -1;           // end changes on end of displayfunc glcompile-id
static int queue_view=1;             // the queue view inside the gameplay
static int old_queue_view=1;         // to restore the old state of the queue view for and after a shoot (freeview)
static int old_birdview_queue;       // the old queue view before toggle to birdview
static int old_options_free_view_on; // the old free_view before toggle to birdview
static int old_birdview_ai=0;        // if options_ai_birdview is on this holds the old state for next move and does not allow change until the hit is done

static int leftmenu = 0;             // left Slider menu is closed on startup
static int rightmenu = 0;            // right slider menu is closed on startup
static int leftcount = 0;            // the left counter for left menu for sliding
static int rightcount = 0;           // the right counter for left menu for sliding
#define MENUCOUNT 177                // how much to count for sliding in/out left/right menu

int vline_on=1;                      // helpline is on on start

/* values for the screen button offsets inside the "whole OpenGL world"
   relevant are m_matrix[0] = x and m_matrix[5] = y
   the offset to this known
 */

static GLdouble b_projection[16] = {0};
static GLdouble b_modelview[16] = {0};
static GLint b_viewport[4];
static GLdouble x_upbutton,y_upbutton,z_dummy;
static GLdouble x_downbutton,y_downbutton;
static GLdouble x_strengthbar,y_strengthbar,x_strengthbar_end,y_strengthbar_end;
static GLdouble x_backbutton,y_backbutton;
static GLdouble x_nextbutton,y_nextbutton;
static GLdouble x_shootbutton,y_shootbutton;

static VMfloat queue_anim=0.0;
static VMfloat button_anim=0.0;
static GLfloat queue_offs=0.06;
static int  balls_moving=0;

/* reflection map */
static GLuint spheretexbind;
static GLuint placecueballtexbind;
static GLuint blendetexbind;
static GLuint lightflaretexbind;
static GLuint halfsymboltexbind;
static GLuint fullsymboltexbind;
static GLuint fullhalfsymboltexbind;
static GLuint fblogotexbind;
static GLuint introtexbind;
static GLuint btexbind;
static GLuint ntexbind;
static GLuint stexbind;
static GLuint utexbind;
static GLuint dtexbind;
static GLuint englishbind;
static GLuint kreuzbind;
static GLuint mleftbind;
static GLuint mleftnormalbind;
static GLuint mrightbind;
static GLuint networkbind;
static GLuint sbind;
static GLuint bbind;
static GLuint ebind;
static GLuint mbind;
static GLuint fbind;
static GLuint freeviewbind;
static GLuint discbind;

static int show_disc = 0;
static int helpscreen_on=0;
static int cuberef_allballs_texbind_nr = 0;
static unsigned int *cuberef_allballs_texbind = 0;

VMvect comp_dir;

static int  act_player=0;   /* 0 or 1 */
static char * player_names[]={localeText[0],localeText[1],localeText[2],localeText[3]};  /* "Human Player","AI Player","Human Player 2","AI Player 2" */
// the static char * are only in use with debug later in source with the following inside the line (search for it)
// sprintf(str,"%s - %s",player[act_player].name,half_full_names
// static char * half_full_names[]={localeText[4],localeText[5],localeText[6]}; /* "any","full","half" */
static int  b1_b2_hold=0;

struct Player player[2];

VMvect lightpos[10];
int    lightnr=3;

enum gameType gametype = GAME_8BALL; //first start are 8ball (without a valid conf-file)

menuType  *g_act_menu;  // start menu entries declared here and used from
menuType  *g_main_menu; // inside this and menu.c

int   g_shot_due=1;        /* a shot to be due at the beginning */
VMfloat g_motion_ratio=1.0;  /* a shot to be due at the beginning */

#ifdef NETWORKING
  static int old_actplayer=0; /* the same for network player change */
  static int init_netclient = 0;     // call a client net-game on 1
  static int wait_key = 0;           // set with every mouse-click and keystroke (for waiting purposes)
  static void (* network_game)(void);// holds the network function if a network game is set
  static int (* net_send_data)(void);// holds the hardcoding or slow compatble network send function
  static int (* net_get_data)(void);  // holds the hardcoding or slow compatble network get function
  static int wait_seconds = 0;       // for network play the longest time to wait for connect
  static int wait_client = 0;        // wait for client connect to server = 1
  static int wait_server = 0;        // wait for server connect from client = 1
  static int init_netserver = 0;     // call a host net-game on 1
  static int active_net_game = 0;    // 1 if network game is active and 0 if not
  static int send_data = 0;          // 0 no data send, != 0 data to send
  static int get_data = 0;           // 0 no data to get != data get
  static int netorder=0;             // the order 0 = nothing, 1 = end
  static int data_len;               // the length of network sended data and for the pointer bug in some gcc
  static char net_data[5000];        // all data send as a string for 32/64 Bit, Win/Linux compatibility
  static char net_workstr[5000];     // network work string for building the next ip-packets
  static TCPsocket server=NULL;      // hold a server TCP connection socket
  static TCPsocket client=NULL;      // hold a client TCP connection socket
  static TCPsocket g_socket=NULL;    // socket for game-play
  static char *ip_adresses;          // hold the 0 terminated IP-Addresses of the Host/Client
  static char *ipptr[9];             // pointer to ip_adresses, max. 9
  static VMfloat queue_offs_sik;     // for quick converting double to float and the pointer bug in some gcc
  SDL_TimerID net_id=NULL;           // hold a SDL-Timer Event in network play in initialization
  SDL_TimerID active_net_timer=NULL; // timer on !=NULL, NULL = off for network game
  static char *dst;                  // to put send or get data into variables and the pointer bug in some gcc
  #ifndef VMATH_SINGLE_PRECISION
    VMfloat xpointer,zpointer;     // to switch between double and float
  #endif
  static char *sub_string;           // for sprintf and sscanf inside network get or send data (slow compatibility mode)
#endif

#ifdef TIME_INTERPOLATE
  int g_frametime_laststep;
  int g_frametime_fromlast;
  BallsType g_lastballs;
  BallsType g_drawballs;
#endif

struct PlayerRoster {
    int nr;       /* number of players */
    struct Player player[ROSTER_MAX_NUM];   /* max. players incl. Tournament*/
} human_player_roster;

static struct TournamentState_ {
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
    } game[TOURNAMENT_ROUND_NUM/*rounds*/][1<<(TOURNAMENT_ROUND_NUM-1)/*games*/];

    struct PlayerRoster roster;

} tournament_state;

static textObj * winner_name_text_obj;  //for the name of the winner
static textObj * winner_text_obj;       // for the winning text
static textObj * tourn_winner_obj;      // for tournament winning text
static textObj * seconds_text_obj;      // for the network countdown
static textObj * ip_text_obj[9];        // for the network ip-adresses
static textObj * ip1_text_obj;          // a little Help Text
static textObj * ip2_text_obj;          // for the network host name
static textObj * esc_stop_obj;          // text for cancel
static textObj * stbar_text_obj;        // for the strength bar %

static char stbar_text[6];              // format the value for output to string

static char * appname_str="foobillardplus";

static struct option long_options[] = {

    {"player1",      required_argument, (int *)localeText[7], OPT_PLAYER1},
    {"player2",      required_argument, (int *)localeText[8], OPT_PLAYER2},
    {"p1",           required_argument, (int *)localeText[9], OPT_PLAYER1},
    {"p2",           required_argument, (int *)localeText[10], OPT_PLAYER2},
    {"name1",        required_argument, (int *)localeText[11], OPT_NAME1},
    {"name2",        required_argument, (int *)localeText[12], OPT_NAME2},
    {"8ball",        no_argument,       (int *)localeText[13], OPT_8BALL},
    {"9ball",        no_argument,       (int *)localeText[14], OPT_9BALL},
    {"carambol",     no_argument,       (int *)localeText[15], OPT_CARAMBOL},
    {"snooker",      no_argument,       (int *)localeText[16], OPT_SNOOKER},
    {"chromeblue",   no_argument,       (int *)localeText[20], OPT_CHROMEBLUE},
    {"goldgreen",    no_argument,       (int *)localeText[21], OPT_GOLDGREEN},
    {"goldred",      no_argument,       (int *)localeText[22], OPT_GOLDRED},
    {"blackbeige",   no_argument,       (int *)localeText[24], OPT_BLACKBEIGE},
    {"tronmode",     required_argument, (int *)localeText[237], OPT_TRONMODE},
    {"tablesize",    required_argument, (int *)localeText[25], OPT_TABLESIZE},
    {"lensflare",    no_argument,       (int *)localeText[26], OPT_LENSFLARE},
    {"nolensflare",  no_argument,       (int *)localeText[27], OPT_NOLENSFLARE},
    {"poslight",     no_argument,       (int *)localeText[28], OPT_POSLIGHT},
    {"dirlight",     no_argument,       (int *)localeText[29], OPT_DIRLIGHT},
    {"ai1err",       required_argument, (int *)localeText[30], OPT_AI1ERR},
    {"ai2err",       required_argument, (int *)localeText[31], OPT_AI2ERR},
    {"balldetail",   required_argument, (int *)localeText[32], OPT_BALLDETAIL},
    {"glassball",    required_argument, (int *)localeText[46], OPT_GLASSBALL},
    {"rgstereo",     no_argument,       (int *)localeText[33], OPT_RGSTEREO},
    {"rgaim",        required_argument, (int *)localeText[34], OPT_RGAIM},
    {"hostaddr",     required_argument, (int *)localeText[35], OPT_HOSTADDR},
    {"portnum",      required_argument, (int *)localeText[36], OPT_PORTNUM},
#ifndef WETAB
    {"geometry",     required_argument, (int *)localeText[37], OPT_GEOMETRY},
    {"fullscreen",   no_argument,       (int *)localeText[38], OPT_FULLSCREEN},
    {"browser",      required_argument, (int *)localeText[49], OPT_BROWSER},
#endif
    {"freemove",     required_argument, (int *)localeText[39], OPT_FREEMOVE},
    {"cuberef",      required_argument, (int *)localeText[40], OPT_CUBEREF},
    {"cuberes",      required_argument, (int *)localeText[41], OPT_CUBERES},
    {"bumpref",      required_argument, (int *)localeText[42], OPT_BUMPREF},
    {"bumpwood",     required_argument, (int *)localeText[43], OPT_BUMPWOOD},
    {"balltraces",   required_argument, (int *)localeText[44], OPT_BALLTRACE},
    {"gamemode",     required_argument, (int *)localeText[45], OPT_GAMEMODE},
    {"avatar",       required_argument, (int *)localeText[47], OPT_AVATAR},
    {"tourfast",     required_argument, (int *)localeText[48], OPT_TOURFAST},
    {"showbuttons",  required_argument, (int *)localeText[17], OPT_SHOW_BUTTONS},
    {"jumpshots",    required_argument, (int *)localeText[17], OPT_JUMP_SHOTS},
    {"aliasing",     required_argument, (int *)localeText[17], OPT_ANTIALIASING},
    {"statustext",   required_argument, (int *)localeText[17], OPT_STATUSTEXT},
    {"usesound",     required_argument, (int *)localeText[17], OPT_USE_SOUND},
    {"pcarambol",    required_argument, (int *)localeText[18], OPT_MAXP_CARAMBOL},
    {"controlkind",  required_argument, (int *)localeText[17], OPT_CONTROL_KIND},
    {"aibirdview",   required_argument, (int *)localeText[17], OPT_AI_BIRDVIEW},
    {"anisotrop",    required_argument, (int *)localeText[17], OPT_VALUE_ANISOTROP},
    {"mouseshoot",   required_argument, (int *)localeText[17], OPT_MSHOOT},
    {"auto_freemove",required_argument, (int *)localeText[17], OPT_FREEMOVE1},
    {"fsaa",         required_argument, (int *)localeText[17], OPT_FSAA},
    {"roomtexture",  required_argument, (int *)localeText[17], OPT_ROOM},
    {"furnituretex", required_argument, (int *)localeText[17], OPT_FURNITURE},
#ifdef NETWORKING
    {"netspeed",     required_argument, (int *)localeText[239], OPT_NET_SPEED},
    {"netcompatible",required_argument, (int *)localeText[17], OPT_NET_COMPATIBLE},
#endif
    {"help",         no_argument,       (int *)localeText[50], OPT_HELP},
    {NULL,           0,                 NULL, 0}
};

#ifdef USE_SOUND
  static int playonce = 0;  // to check for applause only play once
#endif

// for tron mode
static GLfloat ambient_torus[3] = {0.19, 0.19, 0.19};		// Torus
static GLfloat diffuse_torus[3] = {0.51, 0.51, 0.51};
static GLfloat specular_torus[3]= {0.51, 0.51, 0.51};

// some for display-lists and the intro animation
static int introtexture = 0; // show the introtexture until keystroke
static int floor_obj = -1;   // for the room the floor obj
static int carpet_obj = -1;  // for the room the carpet obj
static int wall1_2_obj = -1; // for the room the walls 1 + 2 obj
static int wall3_obj = -1;   // for the wall 3 obj (the problem with big window
static int wall4_c_obj = -1; // for wall 4 and ceiling obj

/***********************************************************************/

// Some Functions inside

void shoot( int ani );           //Shoot
void set_gametype( int gtype );  //Set the game type
void restart_game_common();      //restart the current game (only with common things)
void restart_game(void);         //dto., but restart all
void zoom_in_out(int direction); //zoom in/out the table
VMfloat strength01(VMfloat value); //min/max the strength bar
void control_set(int *control_param);
void control_unset(int *control_param);
void control_toggle(int *control_param);
void setenglish(VMfloat addx, VMfloat addy);
void setcueball(VMvect *cueball, VMfloat x, VMfloat y, int cue_ball);
void setfov(VMfloat fovstep);
void restart_game_training(void);
#ifdef NETWORKING
  int join_network_game(void);
  int host_network_game(void);
  void close_listener(void);          // close all network listener, wait_keys etc.
#endif

// End Functions inside

#ifdef NETWORKING

/***********************************************************************
 *                    Network get data soft coding                     *
 *            get as string!!!!! 32/64 Bit Win/Linux stable            *
 ***********************************************************************/

int net_get_data_soft(void)
{
    int i;
    if(netorder>1) {
     return(1);
    }
    //fprintf(stderr,"net_get_data begin\n");
#ifdef VMATH_SINGLE_PRECISION
     if(socket_read(g_socket,net_data,200) == 200){
        sscanf(net_data,"%i %f %f %f %f %f %f %i",&netorder,&Xque,&Zque,&player[act_player].cue_x,&player[act_player].cue_y,&player[act_player].strength,&queue_offs_sik,&data_len);
#else
     if(socket_read(g_socket,net_data,300) == 300){
        sscanf(net_data,"%i %f %f %lf %lf %lf %lf %i",&netorder,&xpointer,&zpointer,&player[act_player].cue_x,&player[act_player].cue_y,&player[act_player].strength,&queue_offs_sik,&data_len);
        Xque = xpointer;
        Zque = zpointer;
#endif
        queue_offs = queue_offs_sik;
        //fprintf(stderr,"client: ball data read %i\n",data_len);
    } else {
     // Network error - terminating
     displaystring (localeText[246]);
     close_listener();
     return(0);
    }
    if((netorder<0 || netorder >2) || (data_len<0 || data_len>sizeof(net_data))) {
     // *totally* Network error - terminating
     displaystring (localeText[246]);
     close_listener();
     return(0);
    }
    //fprintf(stderr,"Netorder: %i\n",netorder);
    if(netorder) {
      netorder++;
      return(1);
    }
    if(socket_read(g_socket,net_data,data_len)==data_len) {
       //fprintf(stderr,"net_get_data: %i\n",data_len);
       sub_string = strtok(net_data, ":");
       i = 0;
       while (sub_string!= NULL && i<balls.nr) {
#ifdef VMATH_SINGLE_PRECISION
         sscanf(sub_string,"%f %f %f %f %f %f %f %f %f %i %i %i %i",
#else
         sscanf(sub_string,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %i %i %i %i",
#endif
          &balls.ball[i].r.x,&balls.ball[i].r.y,&balls.ball[i].r.z,&balls.ball[i].v.x,&balls.ball[i].v.y,
          &balls.ball[i].v.z,&balls.ball[i].w.x,&balls.ball[i].w.y,&balls.ball[i].w.z,&balls.ball[i].nr,
          &balls.ball[i].in_game,&balls.ball[i].in_fov,&balls.ball[i].in_hole);
         i++;
         sub_string = strtok(NULL, ":");
       }
    }  else {
     // Network error - terminating
     displaystring (localeText[246]);
     close_listener();
     return(0);
  }
    return (1);
}

/***********************************************************************
 *                   Network send data softcoding                      *
 *           send as string!!!!! 32/64 Bit Win/Linux stable            *
 ***********************************************************************/

int net_send_data_soft(void)
{
    int i;
    if(netorder>1) {
     return(1);
    }
    queue_offs_sik = queue_offs;
    net_data[0] = 0;
    data_len = 0;
    for(i=0;i<balls.nr;i++){
#ifdef VMATH_SINGLE_PRECISION
        sprintf(net_workstr,"%f %f %f %f %f %f %f %f %f %i %i %i %i :",
#else
        sprintf(net_workstr,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %i %i %i %i :",
#endif
          balls.ball[i].r.x,balls.ball[i].r.y,balls.ball[i].r.z,balls.ball[i].v.x,balls.ball[i].v.y,
          balls.ball[i].v.z,balls.ball[i].w.x,balls.ball[i].w.y,balls.ball[i].w.z,balls.ball[i].nr,
          balls.ball[i].in_game,balls.ball[i].in_fov,balls.ball[i].in_hole);
        strcat(net_data,net_workstr);
    }
    data_len = strlen(net_data);
#ifdef VMATH_SINGLE_PRECISION
    sprintf(net_workstr,"%i %f %f %f %f %f %f %i",netorder,Xque,Zque,player[act_player].cue_x,player[act_player].cue_y,player[act_player].strength,queue_offs_sik,(int)data_len);
    if(socket_write(g_socket,net_workstr,200) != 200) {
#else
    sprintf(net_workstr,"%i %lf %lf %lf %lf %lf %lf %i",netorder,Xque,Zque,player[act_player].cue_x,player[act_player].cue_y,player[act_player].strength,queue_offs_sik,(int)data_len);
    if(socket_write(g_socket,net_workstr,300) != 300) {
#endif
      // Network error - terminating
      displaystring (localeText[247]);
      close_listener();
      //fprintf(stderr,"Net-write error\n");
    }
    if(netorder) {
     netorder++;
     return(1);
    }
    if(socket_write(g_socket,net_data,data_len)!=data_len) {
     // Network error - terminating
     displaystring (localeText[247]);
     close_listener();
     return(0);
    }
    //fprintf(stderr,"host: ball data write %i\n",(int)data_len);
    return(1);
}

/***********************************************************************
 *                    Network get data hard coding                     *
 ***********************************************************************/

int net_get_data_hard(void)
{
    int i;
    int index = 0;
    if(netorder>1) {
     return(1);
    }
    //fprintf(stderr,"net_get_data begin\n");
#ifdef VMATH_SINGLE_PRECISION
    if(socket_read(g_socket,net_data,28) == 28){
        dst = (char *)&netorder;
        dst[0] = net_data[0];
        dst[1] = net_data[1];
        dst = (char *)&Xque;
        dst[0] = net_data[2];
        dst[1] = net_data[3];
        dst[2] = net_data[4];
        dst[3] = net_data[5];
        dst = (char *)&Zque;
        dst[0] = net_data[6];
        dst[1] = net_data[7];
        dst[2] = net_data[8];
        dst[3] = net_data[9];
        dst = (char *)&player[act_player].cue_x;
        dst[0] = net_data[10];
        dst[1] = net_data[11];
        dst[2] = net_data[12];
        dst[3] = net_data[13];
        dst = (char *)&player[act_player].cue_y;
        dst[0] = net_data[14];
        dst[1] = net_data[15];
        dst[2] = net_data[16];
        dst[3] = net_data[17];
        dst = (char *)&player[act_player].strength;
        dst[0] = net_data[18];
        dst[1] = net_data[19];
        dst[2] = net_data[20];
        dst[3] = net_data[21];
        dst = (char *)&queue_offs_sik;
        dst[0] = net_data[22];
        dst[1] = net_data[23];
        dst[2] = net_data[24];
        dst[3] = net_data[25];
        dst = (char *)&data_len;
        dst[0] = net_data[26];
        dst[1] = net_data[27];
#else
    if(socket_read(g_socket,net_data,44) == 44){
        dst = (char *)&netorder;
        dst[0] = net_data[0];
        dst[1] = net_data[1];
        dst = (char *)&Xque;
        dst[0] = net_data[2];
        dst[1] = net_data[3];
        dst[2] = net_data[4];
        dst[3] = net_data[5];
        dst = (char *)&Zque;
        dst[0] = net_data[6];
        dst[1] = net_data[7];
        dst[2] = net_data[8];
        dst[3] = net_data[9];
        dst = (char *)&player[act_player].cue_x;
        dst[0] = net_data[10];
        dst[1] = net_data[11];
        dst[2] = net_data[12];
        dst[3] = net_data[13];
        dst[4] = net_data[14];
        dst[5] = net_data[15];
        dst[6] = net_data[16];
        dst[7] = net_data[17];
        dst = (char *)&player[act_player].cue_y;
        dst[0] = net_data[18];
        dst[1] = net_data[19];
        dst[2] = net_data[20];
        dst[3] = net_data[21];
        dst[4] = net_data[22];
        dst[5] = net_data[23];
        dst[6] = net_data[24];
        dst[7] = net_data[25];
        dst = (char *)&player[act_player].strength;
        dst[0] = net_data[26];
        dst[1] = net_data[27];
        dst[2] = net_data[28];
        dst[3] = net_data[29];
        dst[4] = net_data[30];
        dst[5] = net_data[31];
        dst[6] = net_data[32];
        dst[7] = net_data[33];
        dst = (char *)&queue_offs_sik;
        dst[0] = net_data[34];
        dst[1] = net_data[35];
        dst[2] = net_data[36];
        dst[3] = net_data[37];
        dst[4] = net_data[38];
        dst[5] = net_data[39];
        dst[6] = net_data[40];
        dst[7] = net_data[41];
        dst = (char *)&data_len;
        dst[0] = net_data[42];
        dst[1] = net_data[43];
#endif
        queue_offs = queue_offs_sik;
        //fprintf(stderr,"client: ball data read %i\n",data_len);
    } else {
     // Network error - terminating
     displaystring (localeText[246]);
     close_listener();
     return(0);
    }
    if((netorder<0 || netorder >2) || (data_len<0 || data_len>sizeof(net_data))) {
     // *totally* Network error - terminating
     displaystring (localeText[246]);
     close_listener();
     return(0);
    }
    //fprintf(stderr,"Netorder: %i\n",netorder);
    if(netorder) {
      netorder++;
      return(1);
    }

#ifdef VMATH_SINGLE_PRECISION
        if(socket_read(g_socket,net_data,data_len) == data_len){
          for(i=0;i<balls.nr && index < data_len;i++) {
            dst = (char *)&balls.ball[i].r.x;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst = (char *)&balls.ball[i].r.y;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst = (char *)&balls.ball[i].r.z;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst = (char *)&balls.ball[i].v.x;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst = (char *)&balls.ball[i].v.y;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst = (char *)&balls.ball[i].v.z;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst = (char *)&balls.ball[i].w.x;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst = (char *)&balls.ball[i].w.y;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst = (char *)&balls.ball[i].w.z;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst = (char *)&balls.ball[i].nr;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst = (char *)&balls.ball[i].in_game;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst = (char *)&balls.ball[i].in_fov;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst = (char *)&balls.ball[i].in_hole;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
          }
#else
        if(socket_read(g_socket,net_data,data_len) == data_len){
          for(i=0;i<balls.nr && index < data_len;i++) {
            dst = (char *)&balls.ball[i].r.x;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst[4] = net_data[index++];
            dst[5] = net_data[index++];
            dst[6] = net_data[index++];
            dst[7] = net_data[index++];
            dst = (char *)&balls.ball[i].r.y;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst[4] = net_data[index++];
            dst[5] = net_data[index++];
            dst[6] = net_data[index++];
            dst[7] = net_data[index++];
            dst = (char *)&balls.ball[i].r.z;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst[4] = net_data[index++];
            dst[5] = net_data[index++];
            dst[6] = net_data[index++];
            dst[7] = net_data[index++];
            dst = (char *)&balls.ball[i].v.x;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst[4] = net_data[index++];
            dst[5] = net_data[index++];
            dst[6] = net_data[index++];
            dst[7] = net_data[index++];
            dst = (char *)&balls.ball[i].v.y;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst[4] = net_data[index++];
            dst[5] = net_data[index++];
            dst[6] = net_data[index++];
            dst[7] = net_data[index++];
            dst = (char *)&balls.ball[i].v.z;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst[4] = net_data[index++];
            dst[5] = net_data[index++];
            dst[6] = net_data[index++];
            dst[7] = net_data[index++];
            dst = (char *)&balls.ball[i].w.x;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst[4] = net_data[index++];
            dst[5] = net_data[index++];
            dst[6] = net_data[index++];
            dst[7] = net_data[index++];
            dst = (char *)&balls.ball[i].w.y;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst[4] = net_data[index++];
            dst[5] = net_data[index++];
            dst[6] = net_data[index++];
            dst[7] = net_data[index++];
            dst = (char *)&balls.ball[i].w.z;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst[2] = net_data[index++];
            dst[3] = net_data[index++];
            dst[4] = net_data[index++];
            dst[5] = net_data[index++];
            dst[6] = net_data[index++];
            dst[7] = net_data[index++];
            dst = (char *)&balls.ball[i].nr;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst = (char *)&balls.ball[i].in_game;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst = (char *)&balls.ball[i].in_fov;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
            dst = (char *)&balls.ball[i].in_hole;
            dst[0] = net_data[index++];
            dst[1] = net_data[index++];
          }
#endif
    }  else {
     // Network error - terminating
     displaystring (localeText[246]);
     close_listener();
     return(0);
  }
    return (1);
}

/***********************************************************************
 *                   Network send data hardcoding                      *
 ***********************************************************************/

int net_send_data_hard(void)
{
    int i;
    if(netorder>1) {
     return(1);
    }
    queue_offs_sik = queue_offs;
    net_data[0] = 0;
    data_len = 0;
    for(i=0;i<balls.nr;i++){
#ifdef VMATH_SINGLE_PRECISION
        dst = (char *)&balls.ball[i].r.x;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        dst = (char *)&balls.ball[i].r.y;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        dst = (char *)&balls.ball[i].r.z;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        dst = (char *)&balls.ball[i].v.x;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        dst = (char *)&balls.ball[i].v.y;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        dst = (char *)&balls.ball[i].v.z;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        dst = (char *)&balls.ball[i].w.x;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        dst = (char *)&balls.ball[i].w.y;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        dst = (char *)&balls.ball[i].w.z;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        dst = (char *)&balls.ball[i].nr;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        dst = (char *)&balls.ball[i].in_game;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        dst = (char *)&balls.ball[i].in_fov;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        dst = (char *)&balls.ball[i].in_hole;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        }
#else
        dst = (char *)&balls.ball[i].r.x;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        net_data[data_len++] = dst[4];
        net_data[data_len++] = dst[5];
        net_data[data_len++] = dst[6];
        net_data[data_len++] = dst[7];
        dst = (char *)&balls.ball[i].r.y;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        net_data[data_len++] = dst[4];
        net_data[data_len++] = dst[5];
        net_data[data_len++] = dst[6];
        net_data[data_len++] = dst[7];
        dst = (char *)&balls.ball[i].r.z;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        net_data[data_len++] = dst[4];
        net_data[data_len++] = dst[5];
        net_data[data_len++] = dst[6];
        net_data[data_len++] = dst[7];
        dst = (char *)&balls.ball[i].v.x;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        net_data[data_len++] = dst[4];
        net_data[data_len++] = dst[5];
        net_data[data_len++] = dst[6];
        net_data[data_len++] = dst[7];
        dst = (char *)&balls.ball[i].v.y;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        net_data[data_len++] = dst[4];
        net_data[data_len++] = dst[5];
        net_data[data_len++] = dst[6];
        net_data[data_len++] = dst[7];
        dst = (char *)&balls.ball[i].v.z;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        net_data[data_len++] = dst[4];
        net_data[data_len++] = dst[5];
        net_data[data_len++] = dst[6];
        net_data[data_len++] = dst[7];
        dst = (char *)&balls.ball[i].w.x;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        net_data[data_len++] = dst[4];
        net_data[data_len++] = dst[5];
        net_data[data_len++] = dst[6];
        net_data[data_len++] = dst[7];
        dst = (char *)&balls.ball[i].w.y;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        net_data[data_len++] = dst[4];
        net_data[data_len++] = dst[5];
        net_data[data_len++] = dst[6];
        net_data[data_len++] = dst[7];
        dst = (char *)&balls.ball[i].w.z;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        net_data[data_len++] = dst[2];
        net_data[data_len++] = dst[3];
        net_data[data_len++] = dst[4];
        net_data[data_len++] = dst[5];
        net_data[data_len++] = dst[6];
        net_data[data_len++] = dst[7];
        dst = (char *)&balls.ball[i].nr;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        dst = (char *)&balls.ball[i].in_game;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        dst = (char *)&balls.ball[i].in_fov;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        dst = (char *)&balls.ball[i].in_hole;
        net_data[data_len++] = dst[0];
        net_data[data_len++] = dst[1];
        }
#endif
#ifdef VMATH_SINGLE_PRECISION
    dst = (char *)&netorder;
    net_workstr[0] = dst[0];
    net_workstr[1] = dst[1];
    dst = (char *)&Xque;
    net_workstr[2] = dst[0];
    net_workstr[3] = dst[1];
    net_workstr[4] = dst[2];
    net_workstr[5] = dst[3];
    dst = (char *)&Zque;
    net_workstr[6] = dst[0];
    net_workstr[7] = dst[1];
    net_workstr[8] = dst[2];
    net_workstr[9] = dst[3];
    dst = (char *)&player[act_player].cue_x;
    net_workstr[10] = dst[0];
    net_workstr[11] = dst[1];
    net_workstr[12] = dst[2];
    net_workstr[13] = dst[3];
    dst = (char *)&player[act_player].cue_y;
    net_workstr[14] = dst[0];
    net_workstr[15] = dst[1];
    net_workstr[16] = dst[2];
    net_workstr[17] = dst[3];
    dst = (char *)&player[act_player].strength;
    net_workstr[18] = dst[0];
    net_workstr[19] = dst[1];
    net_workstr[20] = dst[2];
    net_workstr[21] = dst[3];
    dst = (char *)&queue_offs_sik;
    net_workstr[22] = dst[0];
    net_workstr[23] = dst[1];
    net_workstr[24] = dst[2];
    net_workstr[25] = dst[3];
    dst = (char *)&data_len;
    net_workstr[26] = dst[0];
    net_workstr[27] = dst[1];

    if(socket_write(g_socket,net_workstr,28) != 28) {
#else
    dst = (char *)&netorder;
    net_workstr[0] = dst[0];
    net_workstr[1] = dst[1];
    dst = (char *)&Xque;
    net_workstr[2] = dst[0];
    net_workstr[3] = dst[1];
    net_workstr[4] = dst[2];
    net_workstr[5] = dst[3];
    dst = (char *)&Zque;
    net_workstr[6] = dst[0];
    net_workstr[7] = dst[1];
    net_workstr[8] = dst[2];
    net_workstr[9] = dst[3];
    dst = (char *)&player[act_player].cue_x;
    net_workstr[10] = dst[0];
    net_workstr[11] = dst[1];
    net_workstr[12] = dst[2];
    net_workstr[13] = dst[3];
    net_workstr[14] = dst[4];
    net_workstr[15] = dst[5];
    net_workstr[16] = dst[6];
    net_workstr[17] = dst[7];
    dst = (char *)&player[act_player].cue_y;
    net_workstr[18] = dst[0];
    net_workstr[19] = dst[1];
    net_workstr[20] = dst[2];
    net_workstr[21] = dst[3];
    net_workstr[22] = dst[4];
    net_workstr[23] = dst[5];
    net_workstr[24] = dst[6];
    net_workstr[25] = dst[7];
    dst = (char *)&player[act_player].strength;
    net_workstr[26] = dst[0];
    net_workstr[27] = dst[1];
    net_workstr[28] = dst[2];
    net_workstr[29] = dst[3];
    net_workstr[30] = dst[4];
    net_workstr[31] = dst[5];
    net_workstr[32] = dst[6];
    net_workstr[33] = dst[7];
    dst = (char *)&queue_offs_sik;
    net_workstr[34] = dst[0];
    net_workstr[35] = dst[1];
    net_workstr[36] = dst[2];
    net_workstr[37] = dst[3];
    net_workstr[38] = dst[4];
    net_workstr[39] = dst[5];
    net_workstr[40] = dst[6];
    net_workstr[41] = dst[7];
    dst = (char *)&data_len;
    net_workstr[42] = dst[0];
    net_workstr[43] = dst[1];

    if(socket_write(g_socket,net_workstr,44) != 44) {
#endif
      // Network error - terminating
      displaystring (localeText[247]);
      close_listener();
      //fprintf(stderr,"Net-write error\n");
    }
    if(netorder) {
     netorder++;
     return(1);
    }
    if(socket_write(g_socket,net_data,data_len)!=data_len) {
     // Network error - terminating
     displaystring (localeText[247]);
     close_listener();
     return(0);
    }
    //fprintf(stderr,"host: ball data write %i\n",(int)data_len);
    return(1);
}

/***********************************************************************
 *                      Network get data timer                         *
 ***********************************************************************/

Uint32 net_get_timer(Uint32 intervall, void *param)
{
    get_data++;
    return (intervall);
}

/***********************************************************************
 *                      Network send data timer                        *
 ***********************************************************************/

Uint32 net_send_timer(Uint32 intervall, void *param)
{
    send_data++;
    return(intervall);
}

/***********************************************************************
 *  counter waiting on network Server connect (SDL-Timer function)     *
 ***********************************************************************/

Uint32 wait_for_connect(Uint32 intervall,void *param) {

  wait_server = 1;
  if(--wait_seconds < 0) { //count the seconds down
    wait_seconds = 0;
  }
  return (intervall);
}

/***********************************************************************
 *  counter waiting on network client connect (SDL-Timer function)     *
 ***********************************************************************/

Uint32 wait_for_server_connect(Uint32 intervall,void *param) {

  wait_client = 1;
  if(--wait_seconds < 0) { //count the seconds down
    wait_seconds = 0;
  }
  return (intervall);
}

/***********************************************************************
 *            the whole network function with initialization           *
 ***********************************************************************/

static void play_network(void)
{

 int i;

 //initialize network game
 if(!active_net_game) {
     if(init_netclient) { //client to server connect
       init_netclient = 0;
       if(net_id) {
          SDL_RemoveTimer(net_id);
          net_id=NULL;
       }
     join_network_game();
   }
   if(init_netserver) { //server to client connect
     init_netserver = 0;
     if(net_id) {
        SDL_RemoveTimer(net_id);
        net_id=NULL;
     }
     host_network_game();
   }
   //!active_game end
 } else {
     if(active_net_timer == NULL) {
        if(player[act_player].is_net) {
          netorder = 0;
          active_net_timer = SDL_AddTimer(200/options_net_speed,net_get_timer,NULL);
          /* clear recorded ballpaths */
          for( i=0 ; i<balls.nr ; i++ ){
              BM_clearpath( &balls.ball[i] );
          }
        } else {
          netorder = 0;
          active_net_timer = SDL_AddTimer(200/options_net_speed,net_send_timer,NULL);
        }
     }
     if(get_data) {
        net_get_data();
        if(netorder && active_net_timer !=NULL) {
         SDL_RemoveTimer(active_net_timer);
         active_net_timer = NULL;
        }
        get_data = 0;
     } else if(send_data) {
        net_send_data();
        if(netorder && active_net_timer !=NULL) {
         SDL_RemoveTimer(active_net_timer);
         active_net_timer = NULL;
        }
        send_data = 0;
     }
  } // active_game end

 if(net_id!=NULL && !wait_seconds) {  // timer runs but wait-time over
    //no connect in network game - cancel all
    //fprintf(stderr,"Network closing\n");
    displaystring (localeText[261]);
    close_listener();
 }

 if(wait_seconds) {
   if(wait_server) { // server wait for connection
      wait_server = 0;
      if(server && !g_socket) {  // accept a connection coming in on g_socket
         //fprintf(stderr,"wait for server connect\n");
         if((g_socket=SDLNet_TCP_Accept(server))) {
            wait_seconds = 0;
            wait_server = 0;
            init_netserver = 1;
         }
      }
   } else if(wait_client) { // client want to connect
      wait_client = 0;
      if(client==NULL) {
        //fprintf(stderr,"wait for Client connect\n");
        if((g_socket=host_create_socket(options_net_hostname))!=NULL) { //network game connect
         //connect is ok
         wait_client = 0;
         wait_seconds = 0;
         client = g_socket;
         displaystring (localeText[260]);
         init_netclient = 1;
        }
      }
   }
 } // wait_seconds end
}

/***********************************************************************
 *                 if no network play is in function                   *
 ***********************************************************************/
static void no_network(void) {
 return;
}
#endif // NETWORKING

/***********************************************************************
 *  the time for the disc png is shown after F5 (SDL-Timer function)   *
 ***********************************************************************/

Uint32 notshow_disc(Uint32 intervall,void *param) {
  show_disc = 0;
  return 0;
}

/***********************************************************************
 *              Closes the Helpscreen and the Menu if open             *
 ***********************************************************************/
void close_screen()
{

    // close the help-screen
    helpscreen_on = 0;
    // close Menu if open
    if(g_act_menu != (menuType *)0 ) {
      while(g_act_menu != g_main_menu) {
          menu_exit(&g_act_menu);
      }
    }
}

/***********************************************************************
 *                   Process an option of the rc-file                  *
 ***********************************************************************/

void process_option(enum optionType act_option)
{

       switch(act_option){
       case OPT_PLAYER1:
           human_player_roster.player[0].is_AI=(optarg[0]=='a')?1:0;
           human_player_roster.player[0].queue_view=(optarg[0]=='a')?0:1;
           queue_view=human_player_roster.player[0].queue_view;
       break;
       case OPT_PLAYER2:
           human_player_roster.player[1].is_AI=(optarg[0]=='a')?1:0;
           human_player_roster.player[1].queue_view=(optarg[0]=='a')?0:1;
       break;
       case OPT_NAME1:
           strcpy_uscore_2_whtspace(human_player_roster.player[0].name,optarg);
           strcpy_uscore_2_whtspace(player[0].name,optarg);
           break;
       case OPT_NAME2:
           strcpy_uscore_2_whtspace(human_player_roster.player[1].name,optarg);
           strcpy_uscore_2_whtspace(player[1].name,optarg);
           break;
       case OPT_8BALL:
           set_gametype( GAME_8BALL );
           break;
       case OPT_9BALL:
           set_gametype( GAME_9BALL );
           break;
       case OPT_CARAMBOL:
           set_gametype( GAME_CARAMBOL );
           break;
       case OPT_SNOOKER:
           set_gametype( GAME_SNOOKER );
           break;
       case OPT_CHROMEBLUE:
           options_diamond_color=options_diamond_color_chrome;
           options_table_color=options_table_color_blue;
           break;
       case OPT_GOLDGREEN:
           options_diamond_color=options_diamond_color_gold;
           options_table_color=options_table_color_green;
           break;
       case OPT_GOLDRED:
           options_diamond_color=options_diamond_color_gold;
           options_table_color=options_table_color_red;
           break;
       case OPT_BLACKBEIGE:
           options_diamond_color=options_diamond_color_black;
           options_table_color=options_table_color_beige;
           break;
       case OPT_TRONMODE:
         switch(optarg[1]){
           case 'f': /* off */
             options_tronmode=0;
             break;
           case 'n': /* on  */
             options_tronmode=1;
             break;
         }
       case OPT_TABLESIZE:
#ifdef VMATH_SINGLE_PRECISION
        sscanf(optarg,"%f",&options_table_size);
#else
        sscanf(optarg,"%lf",&options_table_size);
#endif
           options_table_size*=0.3048;
           break;
       case OPT_LENSFLARE:
           options_lensflare=1;
           break;
       case OPT_NOLENSFLARE:
           options_lensflare=0;
           break;
       case OPT_POSLIGHT:
           options_positional_light=1;
           break;
       case OPT_DIRLIGHT:
           options_positional_light=0;
           break;
       case OPT_AI1ERR:
#ifdef VMATH_SINGLE_PRECISION
           sscanf(optarg,"%f",&(human_player_roster.player[0].err));
#else
           sscanf(optarg,"%lf",&(human_player_roster.player[0].err));
#endif
           break;
       case OPT_AI2ERR:
#ifdef VMATH_SINGLE_PRECISION
           sscanf(optarg,"%f",&(human_player_roster.player[1].err));
#else
           sscanf(optarg,"%lf",&(human_player_roster.player[1].err));
#endif
           break;
       case OPT_GLASSBALL:
          switch(optarg[1]){
           case 'f': /* off */
              options_glassballs=0;
              break;
           case 'n': /* on  */
              options_glassballs=1;
              break;
          }
          break;
       case OPT_BALLDETAIL:
           switch(optarg[0]) {
              case 'l':
                options_max_ball_detail     = options_max_ball_detail_LOW;
                options_ball_detail_nearmax = options_ball_detail_nearmax_LOW;
                options_ball_detail_farmin  = options_ball_detail_farmin_LOW;
                break;
              case 'm':
                options_max_ball_detail     = options_max_ball_detail_MED;
                options_ball_detail_nearmax = options_ball_detail_nearmax_MED;
                options_ball_detail_farmin  = options_ball_detail_farmin_MED;
                break;
              case 'h':
#ifdef WETAB
              case 'v':
#endif
                options_max_ball_detail     = options_max_ball_detail_HIGH;
                options_ball_detail_nearmax = options_ball_detail_nearmax_HIGH;
                options_ball_detail_farmin  = options_ball_detail_farmin_HIGH;
                break;
#ifndef WETAB
              case 'v':
                options_max_ball_detail     = options_max_ball_detail_VERYHIGH;
                options_ball_detail_nearmax = options_ball_detail_nearmax_VERYHIGH;
                options_ball_detail_farmin  = options_ball_detail_farmin_VERYHIGH;
                break;
#endif
             }
           break;
       case OPT_RGSTEREO:
           options_rgstereo_on=1;
           break;
       case OPT_RGAIM:
           if(optarg[0]=='l') options_rgaim=1;
           if(optarg[0]=='r') options_rgaim=2;
           if(optarg[0]=='m') options_rgaim=0;
           break;
       case OPT_HOSTADDR:
           strcpy(options_net_hostname,optarg);
           break;
       case OPT_PORTNUM:
           sscanf(optarg,"%d",&options_net_portnum);
           if(options_net_portnum<1024) options_net_portnum = 1024;
           if(options_net_portnum>65535) options_net_portnum = 65535;
           break;
#ifndef WETAB
       case OPT_GEOMETRY:
           sscanf(optarg,"%dx%d",&win_width,&win_height);
           break;
       case OPT_FULLSCREEN:
           fullscreen=1;
           break;
       case OPT_BROWSER:
#ifndef WETAB
           strcpy(options_browser,optarg);
           initLanguage(0);
#endif
           break;
#endif
       case OPT_FREEMOVE:
          switch(optarg[1]){
             case 'f': /* off */
                options_free_view_on=0;
                break;
             case 'n': /* on  */
                options_free_view_on=1;
                break;
            }
          break;
       case OPT_CUBEREF:
          switch(optarg[1]){
             case 'f': /* off */
                options_cuberef=0;
                break;
             case 'n': /* on  */
                options_cuberef=1;
                break;
            }
          break;
       case OPT_CUBERES:
          sscanf(optarg,"%d", &options_cuberef_res);
          break;
       case OPT_BUMPREF:
          switch(optarg[1]){
             case 'f': /* off */
                options_bumpref=0;
                break;
             case 'n': /* on  */
                options_bumpref=1;
                break;
            }
          break;
       case OPT_BUMPWOOD:
          switch(optarg[1]){
             case 'f': /* off */
                options_bumpwood=0;
                break;
             case 'n': /* on  */
                options_bumpwood=1;
                break;
            }
          break;
       case OPT_BALLTRACE:
          switch(optarg[1]){
             case 'f': /* off */
                options_balltrace=0;
                break;
             case 'n': /* on  */
                options_balltrace=1;
                break;
            }
          break;
       case OPT_GAMEMODE:
          if (strncasecmp("match",optarg,5)==0){
             options_gamemode=options_gamemode_match;
          } else if(strncasecmp("train",optarg,5)==0){
             options_gamemode=options_gamemode_training;
          } else if(strncasecmp("tourn",optarg,5)==0){
             options_gamemode=options_gamemode_tournament;
          }
          break;
       case OPT_AVATAR:
          switch(optarg[1]){
             case 'f': /* off */
                options_avatar_on=0;
                break;
             case 'n': /* on  */
                options_avatar_on=1;
                break;
            }
          break;
       case OPT_MSHOOT:
          switch(optarg[1]){
             case 'f': /* off */
                options_mouseshoot=0;
                break;
             case 'n': /* on  */
                options_mouseshoot=1;
                break;
            }
          break;
       case OPT_TOURFAST:
#ifdef VMATH_SINGLE_PRECISION
          sscanf(optarg,"%f",&options_tourfast);
#else
          sscanf(optarg,"%lf",&options_tourfast);
#endif
          break;
       case OPT_HELP:
          exit(1);
          break;
       case OPT_SHOW_BUTTONS:
          switch(optarg[1]){
             case 'f': /* off */
                options_show_buttons=0;
                break;
             case 'n': /* on  */
                options_show_buttons=1;
                break;
          }
          break;
       case OPT_JUMP_SHOTS:
          switch(optarg[1]){
             case 'f': /* off */
                options_jump_shots=0;
                break;
             case 'n': /* on  */
                options_jump_shots=1;
                break;
          }
          break;
       case OPT_ANTIALIASING:
          switch(optarg[1]){
             case 'f': /* off */
                options_antialiasing=0;
                break;
             case 'n': /* on  */
                options_antialiasing=1;
                break;
          }
          break;
       case OPT_STATUSTEXT:
          switch(optarg[1]){
             case 'f': /* off */
                options_status_text=0;
                break;
             case 'n': /* on  */
                options_status_text=1;
                break;
          }
          break;
       case OPT_USE_SOUND:
          switch(optarg[1]){
             case 'f': /* off */
                options_use_sound=0;
                break;
             case 'n': /* on  */
                options_use_sound=1;
                break;
          }
          break;
       case OPT_ROOM:
          switch(optarg[1]){
             case 'f': /* off */
                options_deco=0;
                break;
             case 'n': /* on  */
                options_deco=1;
                break;
          }
          break;
       case OPT_FURNITURE:
             switch(optarg[1]){
                case 'f': /* off */
                   options_furniture=0;
                   break;
                case 'n': /* on  */
                   options_furniture=1;
                   break;
             }
             break;
       case OPT_FREEMOVE1:
          switch(optarg[1]){
             case 'f': /* off */
                options_auto_freemove=0;
                break;

             case 'n': /* on  */
                options_auto_freemove=1;
                break;
          }
          break;
       case OPT_MAXP_CARAMBOL:
           sscanf(optarg,"%d",&options_maxp_carambol);
           if(options_maxp_carambol < 5) options_maxp_carambol = 5;
           if(options_maxp_carambol > 100) options_maxp_carambol = 100;
          break;
       case OPT_CONTROL_KIND:
          switch(optarg[1]){
             case 'f': /* off */
                options_control_kind=0;
                break;
             case 'n': /* on  */
                options_control_kind=1;
                break;
          }
          break;
       case OPT_AI_BIRDVIEW:
          switch(optarg[1]){
             case 'f': /* off */
                options_ai_birdview=0;
                break;
             case 'n': /* on  */
                options_ai_birdview=1;
                break;
          }
          break;
       case OPT_FSAA:
             sscanf(optarg,"%d",&options_fsaa_value);
             switch(options_fsaa_value){
                case 1: /* ok */
                case 2:
                case 4:
                case 8:
                  break;
                default:
                  options_fsaa_value = 0;
                  break;
                }
             break;
#ifdef NETWORKING
       case OPT_NET_SPEED:
             sscanf(optarg,"%d",&options_net_speed);
             if(options_net_speed <NET_SPEED_LOW) { options_net_speed = NET_SPEED_LOW; }
             if(options_net_speed >NET_SPEED_HIGH) { options_net_speed = NET_SPEED_HIGH; }
             break;
       case OPT_NET_COMPATIBLE:
          switch(optarg[1]){
             case 'f': /* off */
              options_net_compatible=0;
              net_send_data = net_send_data_hard;
              net_get_data = net_get_data_hard;
              break;
             case 'n': /* on  */
              options_net_compatible=1;
              net_send_data = net_send_data_soft;
              net_get_data = net_get_data_soft;
              break;
          }
          break;
#endif
       case OPT_VALUE_ANISOTROP:
#ifdef VMATH_SINGLE_PRECISION
          sscanf(optarg,"%f",&options_value_anisotrop);
#else
          sscanf(optarg,"%lf",&options_value_anisotrop);
#endif
          if(options_value_anisotrop < 0.0f) options_value_anisotrop = 0.0f;
          break;
       case OPT_DUMMY:
          break;
       }
}

/***********************************************************************
 *                 print the Helpscreen on console output              *
 ***********************************************************************/

void print_help(struct option * opt, char *appname)
{
    int i;

    fprintf(stderr,localeText[51],appname);
    fprintf(stderr,"%s",localeText[52]);
    for(i=0;opt[i].name!=0;i++){
        fprintf(stderr,"--%s %s\n",opt[i].name,opt[i].has_arg?"<arg>":"");
        fprintf(stderr,"     %s\n",(char *)(opt[i].flag));
        opt[i].flag=NULL;
    }
    //fprintf(stderr,"%s\n",localeText[53]);
}

/***********************************************************************
 *                      Load the config from file                      *
 ***********************************************************************/

int load_config( char *** confv, int * confc, char ** argv, int argc )
{
    FILE * f;
    int c,i;             // loop variables
    char * str;          // pointer to the string
    char allstr[10000];  // place the whole config-file
    char filename[300];  // config-filename with path

    *confc=1;
    str=allstr;

#ifdef __MINGW32__ //HS
    sprintf(filename,"%s\\.foobillardrc",getenv("USERPROFILE"));
#else
    sprintf(filename,"%s/.foobillardrc",getenv("HOME"));
#endif
    fprintf(stderr,"%s\n",filename);
    if( (f=fopen(filename,"rb")) != NULL ){
        do{
            str[0]='-'; str[1]='-';
            for( i=2 ; (c=fgetc(f))!='\n' && c!=EOF ; i++ ){
                if( c!=' ' && c!=0x13 && c!=0x0A ) str[i]=c;
                else {
                    str[i]=0;
                    (*confc)++;
                }
            }
            str[i]=0;
            if( str[2]!=0 ){
                (*confc)++;
/*           fprintf(stderr,"confstring:<%s> confc=%d\n",str,*confc);*/
                str+=i+1;
            }
        } while( c!=EOF );

        *confv = malloc( (argc+*confc)*sizeof(char *) );
        str=allstr;
/*    fprintf(stderr,"allstr:<%s>\n",allstr);*/
        (*confv)[0]=argv[0];
        for(i=1;i<*confc;i++){
            (*confv)[i]=str;
/*        fprintf(stderr,"confstring2:<%s>\n",(*confv)[i]);*/
            if( i!=(*confc)-1 ){ for(;(*str)!=0;str++); str++; }
        }
        for(i=1;i<argc;i++){
            (*confv)[*confc+i-1]=argv[i];
        }
        (*confc)+=argc-1;
    } else {
        (*confv)=argv;
        *confc=argc;
/*      fprintf(stderr,"no rc file found\n");*/
    }
    return (f!=NULL);
}


/***********************************************************************
 *              Write one argument to the config to file               *
 ***********************************************************************/

void write_rc(FILE * f, int opt, char * arg)
{
    int i;
    for( i=0; i<OPT_DUMMY && long_options[i].val!=opt ; i++ );

    if( arg!=NULL ){
        char argstr[256];
        strcpy_whtspace_2_uscore(argstr,arg);
        fprintf(f,"%s=%s\n",long_options[i].name,argstr);
    } else {
        fprintf(f,"%s\n",long_options[i].name);
    }
}

/***********************************************************************
 *                      Write the config file                          *
 ***********************************************************************/

void save_config(void)
{
    int opt;
    FILE * f;
    char filename[512];
    char str[256];

#ifdef __MINGW32__ //HS
    sprintf(filename,"%s\\.foobillardrc",getenv("USERPROFILE"));
#else
    sprintf(filename,"%s/.foobillardrc",getenv("HOME"));
#endif
    if((f=fopen(filename,"wb"))==NULL){
        //can't write to %s - check rights\n
        fprintf(stderr,localeText[54],filename);
        return;
    }

    for(opt=0;opt<OPT_DUMMY;opt++){
        //fprintf(stderr,"save_config: writing option %d\n",opt);
        switch(opt){
        case OPT_PLAYER1:
             write_rc(f,opt,(human_player_roster.player[0].is_AI)?"ai":"human");
             break;
        case OPT_PLAYER2:
             write_rc(f,opt,(human_player_roster.player[1].is_AI)?"ai":"human");
             break;
        case OPT_NAME1:
             write_rc(f,opt,human_player_roster.player[0].name);
             break;
        case OPT_NAME2:
             write_rc(f,opt,human_player_roster.player[1].name);
             break;
        case OPT_8BALL:
             if(gametype==GAME_8BALL) {
                write_rc(f,opt,NULL); 
                }
             break;
        case OPT_9BALL:
             if(gametype==GAME_9BALL) {
                write_rc(f,opt,NULL);
                }
             break;
        case OPT_CARAMBOL:
             if(gametype==GAME_CARAMBOL) {
                write_rc(f,opt,NULL);
                }
             break;
        case OPT_SNOOKER:
             if(gametype==GAME_SNOOKER) {
                write_rc(f,opt,NULL);
                }
             break;
        case OPT_CHROMEBLUE:
             if( options_diamond_color==options_diamond_color_chrome && options_table_color==options_table_color_blue) {
                write_rc(f,opt,NULL);
                }
             break;
        case OPT_GOLDGREEN:
             if( options_diamond_color==options_diamond_color_gold && options_table_color==options_table_color_green) {
                write_rc(f,opt,NULL);
                }
             break;
        case OPT_GOLDRED:
             if( options_diamond_color==options_diamond_color_gold && options_table_color==options_table_color_red) {
                write_rc(f,opt,NULL);
                }
             break;
        case OPT_BLACKBEIGE:
             if( options_diamond_color==options_diamond_color_black && options_table_color==options_table_color_beige) {
                write_rc(f,opt,NULL);
                }
             break;
        case OPT_TRONMODE:
              write_rc(f,opt, options_tronmode?"on":"off");
              break;
        case OPT_TABLESIZE:
             sprintf(str,"%f",options_table_size/0.3048);
             write_rc(f,opt,str);
             break;
        case OPT_LENSFLARE:
             if (options_lensflare) {
                write_rc(f,opt,NULL);
                }
             break;
        case OPT_POSLIGHT:
             if(options_positional_light) {
               write_rc(f,opt,NULL);
             }
             break;
        case OPT_DIRLIGHT:
             if(!options_positional_light) {
               write_rc(f,opt,NULL);
             }
             break;
        case OPT_AI1ERR:
             sprintf(str,"%f",human_player_roster.player[0].err);
             write_rc(f,opt,str);
             break;
        case OPT_AI2ERR:
             sprintf(str,"%f",human_player_roster.player[1].err);
             write_rc(f,opt,str);
             break;
        case OPT_GLASSBALL:
           write_rc(f,opt, options_glassballs?"on":"off");
           break;
        case OPT_BALLDETAIL:
            if (options_max_ball_detail == options_max_ball_detail_LOW ) {
                write_rc(f,opt,"l");
                break;
                }
            if(options_max_ball_detail == options_max_ball_detail_MED ) {
                write_rc(f,opt,"m");
                break;
                }
            if(options_max_ball_detail == options_max_ball_detail_HIGH ) {
                write_rc(f,opt,"h");
                break;
                }
            if(options_max_ball_detail == options_max_ball_detail_VERYHIGH ) {
#ifndef WETAB
                write_rc(f,opt,"v");
#else
                write_rc(f,opt,"h");
#endif
                break;
               }
            break;
        case OPT_RGSTEREO:
             if(options_rgstereo_on) {
                write_rc(f,opt,NULL);
                }
             break;
        case OPT_RGAIM:
             if( options_rgaim==1 ) write_rc(f,opt,"l");
             if( options_rgaim==2 ) write_rc(f,opt,"r");
             if( options_rgaim==0 ) write_rc(f,opt,"m");
             break;
        case OPT_HOSTADDR:
             write_rc(f,opt,options_net_hostname);
             break;
        case OPT_PORTNUM:
             sprintf(str,"%d",options_net_portnum);
             write_rc(f,opt,str);
             break;
#ifndef WETAB
        case OPT_GEOMETRY: 
             sprintf(str,"%dx%d",win_width,win_height);
             write_rc(f,opt,str);
             break;
        case OPT_FULLSCREEN:
             if (sys_get_fullscreen()) {
                write_rc(f,opt,NULL);
                }
             break;
        case OPT_BROWSER:
             write_rc(f,opt,options_browser);
             break;
#endif
        case OPT_FREEMOVE:
             if(options_birdview_on) {
               write_rc(f,opt, old_options_free_view_on?"on":"off");
             } else {
               write_rc(f,opt, options_free_view_on?"on":"off");
             }
             break;
        case OPT_FREEMOVE1:
             write_rc(f,opt, options_auto_freemove?"on":"off");
             break;
        case OPT_CUBEREF:
             write_rc(f,opt, options_cuberef?"on":"off");
             break;
        case OPT_CUBERES: 
             sprintf(str,"%d", options_cuberef_res);
             write_rc(f,opt,str);
             break;
        case OPT_BUMPREF:
             write_rc(f,opt, options_bumpref?"on":"off");
             break;
        case OPT_BUMPWOOD:
             write_rc(f,opt, options_bumpwood?"on":"off");
             break;
        case OPT_BALLTRACE:
             write_rc(f,opt, options_balltrace?"on":"off");
             break;
        case OPT_GAMEMODE:
            switch(options_gamemode){
                case options_gamemode_match:
                    write_rc(f,opt,"match");
                    break;
                case options_gamemode_training:
                    write_rc(f,opt,"training");
                    break;
                case options_gamemode_tournament:
                    write_rc(f,opt,"tournament");
                    break;
                }
             break;
        case OPT_AVATAR:
             write_rc(f,opt, options_avatar_on?"on":"off");
             break;
        case OPT_MSHOOT:
             write_rc(f,opt, options_mouseshoot?"on":"off");
             break;
        case OPT_TOURFAST:
             sprintf(str,"%f",options_tourfast);
             write_rc(f,opt,str);
             break;
        case OPT_SHOW_BUTTONS:
             write_rc(f,opt, options_show_buttons?"on":"off");
             break;
       case OPT_JUMP_SHOTS:
             write_rc(f,opt, options_jump_shots?"on":"off");
             break;
       case OPT_ANTIALIASING:
             write_rc(f,opt, options_antialiasing?"on":"off");
             break;
       case OPT_STATUSTEXT:
             write_rc(f,opt, options_status_text?"on":"off");
             break;
       case OPT_USE_SOUND:
             write_rc(f,opt, options_use_sound?"on":"off");
             break;
       case OPT_MAXP_CARAMBOL:
             sprintf(str,"%d", options_maxp_carambol);
             write_rc(f,opt,str);
             break;
       case OPT_CONTROL_KIND:
             write_rc(f,opt, options_control_kind?"on":"off");
             break;
       case OPT_AI_BIRDVIEW:
             write_rc(f,opt, options_ai_birdview?"on":"off");
             break;
       case OPT_FSAA:
             sprintf(str,"%d",options_fsaa_value);
             write_rc(f,opt,str);
          break;
#ifdef NETWORKING
       case OPT_NET_SPEED:
             sprintf(str,"%d",options_net_speed);
             write_rc(f,opt,str);
          break;
       case OPT_NET_COMPATIBLE:
             write_rc(f,opt,options_net_compatible?"on":"off");
             break;
#endif
       case OPT_VALUE_ANISOTROP:
             sprintf(str,"%f",options_value_anisotrop);
             write_rc(f,opt,str);
          break;
       case OPT_ROOM:
          write_rc(f,opt, options_deco?"on":"off");
          break;
       case OPT_FURNITURE:
          write_rc(f,opt, options_furniture?"on":"off");
          break;
       }
    }
    fclose(f);
}

/***********************************************************************
 *                  select gametype, standard 8Ball                    *
 ***********************************************************************/

void set_gametype( int gtype )
{
    gametype=gtype;
    switch (gametype) {
        case GAME_9BALL:
          setfunc_evaluate_last_move( evaluate_last_move_9ball );
          setfunc_create_scene( create_9ball_scene );
          setfunc_create_walls( create_6hole_walls );
          setfunc_ai_get_stroke_dir( ai_get_stroke_dir_9ball );
          player[0].cue_ball=0;
          player[1].cue_ball=0;
          player[act_player].place_cue_ball=1;
          human_player_roster.player[0].cue_ball=0;
          human_player_roster.player[1].cue_ball=0;
          human_player_roster.player[act_player].place_cue_ball=1;
          break;
        case GAME_CARAMBOL:
          setfunc_evaluate_last_move( evaluate_last_move_carambol );
          setfunc_create_scene( create_carambol_scene );
          setfunc_create_walls( create_0hole_walls );
          setfunc_ai_get_stroke_dir( ai_get_stroke_dir_carambol );
          player[0].cue_ball=0;
          player[1].cue_ball=1;
          player[act_player].place_cue_ball=0;
          human_player_roster.player[0].cue_ball=0;
          human_player_roster.player[1].cue_ball=1;
          human_player_roster.player[act_player].place_cue_ball=0;
          break;
        case GAME_SNOOKER:
          setfunc_evaluate_last_move( evaluate_last_move_snooker );
          setfunc_create_scene( create_snooker_scene );
          setfunc_create_walls( create_6hole_walls );
          setfunc_ai_get_stroke_dir( ai_get_stroke_dir_snooker );
          player[0].cue_ball=0;
          player[1].cue_ball=0;
          player[act_player].place_cue_ball=1;
          human_player_roster.player[0].cue_ball=0;
          human_player_roster.player[1].cue_ball=0;
          human_player_roster.player[act_player].place_cue_ball=1;
          break;
        default:
          // Standard 8Ball
          gametype = GAME_8BALL;
          setfunc_evaluate_last_move( evaluate_last_move_8ball );
          setfunc_create_scene( create_8ball_scene );
          setfunc_create_walls( create_6hole_walls );
          setfunc_ai_get_stroke_dir( ai_get_stroke_dir_8ball );
          player[0].cue_ball=0;
          player[1].cue_ball=0;
          player[act_player].place_cue_ball=1;
          human_player_roster.player[0].cue_ball=0;
          human_player_roster.player[1].cue_ball=0;
          human_player_roster.player[act_player].place_cue_ball=1;        
       }
}

/***********************************************************************
 *                     Angle inside 180 degrees                        *
 ***********************************************************************/

VMfloat angle_pm180(VMfloat ang)
{
    while ( ang >  180.0 ) ang-=360.0;
    while ( ang < -180.0 ) ang+=360.0;
    return ang;
}

/***********************************************************************
 *                     Angle inside 360 degrees                        *
 ***********************************************************************/
VMfloat angle_pm360(VMfloat ang)
{
    while ( ang >  360.0 ) ang-=360.0;
    while ( ang < -0.0 ) ang+=360.0;
    return ang;
}

/***********************************************************************
 *              Toggle the cue view (german: queue)                    *
 ***********************************************************************/

void toggle_queue_view(void)
{
    VMfloat th, ph;
    queue_view = (queue_view==0)?1:0;
    if( queue_view ){
        Xrot_offs=angle_pm180(Xrot-Xque);
        Xrot=Xque;
        Zrot_offs=angle_pm180(Zrot-Zque);
        Zrot=Zque;
    } else {
        th=Xrot/180.0*M_PI;
        ph=Zrot/180.0*M_PI;
        free_view_pos_aim = vec_scale(vec_xyz(sin(th)*sin(ph),sin(th)*cos(ph),cos(th)), cam_dist);
        free_view_pos_aim = vec_add( free_view_pos_aim, CUE_BALL_XYPOS );
        free_view_pos = free_view_pos_aim ;
    }
}

/*********************************************************************
 * Toggle the birdview (in birdview mode only some keys are allowed) *
 *********************************************************************/

void birdview(void)
{
   VMfloat Xoffs=0.0-Xrot;
   VMfloat Zoffs=-90.0-Zrot;

   options_birdview_on = (options_birdview_on==0)?1:0;
   if(options_birdview_on) {
      old_options_free_view_on = options_free_view_on;
      options_free_view_on = 1;
      old_birdview_queue = queue_view;
      if(queue_view) toggle_queue_view();
      free_view_pos_aim = vec_xyz(0,0,3.5*options_table_size/2.1336);
      free_view_pos = free_view_pos_aim ;
      Xrot += Xoffs;
      Zrot += Zoffs;
      Xrot_offs -= Xoffs;
      Zrot_offs -= Zoffs;
      Xrot = 0.0;
      Zrot = -90.0;
      Xrot_offs = 0.0;
      Zrot_offs = 0.0;
      old_queue_view = queue_view;
   } else {
      toggle_queue_view();
      if(queue_view != old_birdview_queue) {
         toggle_queue_view();
         }
      if(old_options_free_view_on != options_free_view_on) {
         options_free_view_on = old_options_free_view_on;
         }
   }
}

/***********************************************************************
 *              Copy a player to another player struct                 *
 ***********************************************************************/

void player_copy(struct Player * player, struct Player src)
{
    player->is_AI          = src.is_AI;
    player->is_net         = src.is_net;
    player->half_full      = src.half_full;
    strcpy(player->name,src.name);
    player->Xque           = src.Xque;
    player->Zque           = src.Zque;
    player->cue_x          = src.cue_x;
    player->cue_y          = src.cue_y;
    player->strength       = src.strength;
    player->queue_view     = src.queue_view;
    player->place_cue_ball = src.place_cue_ball;
    player->winner         = src.winner;
    player->err            = src.err;
    if(player->text) textObj_setText(player->text, player->name);
    if(player->score_text) textObj_setText(player->score_text, "0");
    player->snooker_on_red = src.snooker_on_red;
    player->score          = src.score;
    player->cue_ball       = src.cue_ball;
}

/***********************************************************************
 *                         Initialize Player                           *
 ***********************************************************************/

void init_player(struct Player * player, int ai)
{
    player->is_AI=ai;
    player->is_net=0;
    player->half_full=BALL_ANY;
    //ai?"AI-Player":"Human"
    strcpy(player->name,ai?localeText[55]:localeText[56]);
    player->Xque=-83.0;
    player->Zque=0.0;
    player->cue_x=0.0;
    player->cue_y=0.0;
    player->strength=0.7;
    player->queue_view=ai?0:1;
    player->place_cue_ball=0;
    player->winner=0;
    player->err=0;
    player->text = 0;
    player->score_text = 0;
    player->snooker_on_red=1;
    player->score=0;
    player->cue_ball=0;
}

/***********************************************************************
 *                    Initialize player Roster                         *
 ***********************************************************************/

void init_ai_player_roster(struct PlayerRoster * roster)
{
    int i;
    char str[256];

    for(i=0;i<roster->nr;i++){
#ifdef WETAB
  #define PLAYER1  "Arthur Dent"
  #define PLAYER2  "Ford Prefect"
  #define PLAYER3  "Zaphod Beeblebrox"
  #define PLAYER4  "Trillian McMillan"
  #define PLAYER5  "Slartibartfast"
  #define PLAYER6  "Marvin"
  #define PLAYER7  "Questular Rontok"
  #define PLAYER8  "Deep Thought"
  #define PLAYER9  "The Guide"
  #define PLAYER10 "Humma Kavula"
  #define PLAYER11 "Eddie the Computer"
  #define PLAYER12 "Jeltz"
  #define PLAYER13 "Fook"
  #define PLAYER14 "Lunkwill"
  #define PLAYER15 "Gag Halfrunt"
#else
  #define PLAYER1  "Billardo Bill"
  #define PLAYER2  "Suzy Cue"
  #define PLAYER3  "Pooledo Pete"
  #define PLAYER4  "Billie Ball"
  #define PLAYER5  "Snookie"
  #define PLAYER6  "Diamond Dan"
  #define PLAYER7  "Tom Tuxedo"
  #define PLAYER8  "Sally Silver"
  #define PLAYER9  "Wicked Wendy"
  #define PLAYER10 "Bald Ben"
  #define PLAYER11 "Badino Buck"
  #define PLAYER12 "Worse Will"
  #define PLAYER13 "Rita Rookie"
  #define PLAYER14 "Don Dumb"
  #define PLAYER15 "Dana Dummy"
#endif
        init_player(&(roster->player[i]),1);

        if (i==roster->nr-1) { /* human player */
            roster->player[i]=human_player_roster.player[0];
        } else if (i==0){
            strcpy(roster->player[i].name,PLAYER1);
            roster->player[i].err=0.0;
        } else if (i==1) {
            strcpy(roster->player[i].name,PLAYER2);
            roster->player[i].err=0.02;
        } else if (i==2) {
            strcpy(roster->player[i].name,PLAYER3);
            roster->player[i].err=0.05;
        } else if (i==3) {
            strcpy(roster->player[i].name,PLAYER4);
            roster->player[i].err=0.1;
        } else if (i==4) {
            strcpy(roster->player[i].name,PLAYER5);
            roster->player[i].err=0.2;
        } else if (i==5) {
            strcpy(roster->player[i].name,PLAYER6);
            roster->player[i].err=0.4;
        } else if (i==6) {
            strcpy(roster->player[i].name,PLAYER7);
            roster->player[i].err=0.6;
        } else if (i==7) {
            strcpy(roster->player[i].name,PLAYER8);
            roster->player[i].err=0.7;
        } else if (i==8) {
            strcpy(roster->player[i].name,PLAYER9);
            roster->player[i].err=0.8;
        } else if (i==9) {
            strcpy(roster->player[i].name,PLAYER10);
            roster->player[i].err=0.9;
        } else if (i==10) {
            strcpy(roster->player[i].name,PLAYER11);
            roster->player[i].err=0.10;
        } else if (i==11) {
            strcpy(roster->player[i].name,PLAYER12);
            roster->player[i].err=0.11;
        } else if (i==12) {
            strcpy(roster->player[i].name,PLAYER13);
            roster->player[i].err=0.12;
        } else if (i==13) {
            strcpy(roster->player[i].name,PLAYER14);
            roster->player[i].err=0.15;
        } else if (i==14) {
            strcpy(roster->player[i].name,PLAYER15);
            roster->player[i].err=0.19;
        } else {
            //sprintf(str,"dumb for %d",i-13);
            sprintf(str,localeText[57],i-13);
            strcpy(roster->player[i].name,str);
            roster->player[i].err=0.1*(VMfloat)(i-13);
        }
        roster->player[i].text = textObj_new(roster->player[i].name, options_roster_fontname, 20); //28 old
    }
}

/***********************************************************************
 *         Initialize variables for two players on startup             *
 ***********************************************************************/

void init_player_roster(struct PlayerRoster * roster)
{
    roster->nr=2;
    init_player(&(roster->player[0]),0);
    init_player(&(roster->player[1]),1);
    init_player(&player[0],0);
    init_player(&player[1],1);
    strcpy(roster->player[0].name,player_names[0]);
    strcpy(roster->player[1].name,player_names[1]);
    strcpy(player[0].name,player_names[0]);
    strcpy(player[1].name,player_names[1]);
#ifdef __MINGW32__
    if(getenv("USERNAME"))
        strcpy(roster->player[0].name,getenv("USERNAME"));
        strcpy(player[0].name,getenv("USERNAME"));
#else
    if(getenv("USER"))
        strcpy(roster->player[0].name,getenv("USER"));
        strcpy(player[0].name,getenv("USER"));
#endif
    roster->player[0].err=(VMfloat)0/10.0;
    roster->player[1].err=(VMfloat)0.30; //now medium
    roster->player[0].text = 0;
    roster->player[1].text = 0;

}

/***********************************************************************
 *        Set the Name textobject for two players on startup           *
 ***********************************************************************/

void create_human_player_roster_text(struct PlayerRoster * roster)
{
    int i;

    for(i=0;i<roster->nr;i++){
        if(roster->player[i].text == 0){
            roster->player[i].text = textObj_new(roster->player[i].name, options_roster_fontname, 28);
        } else {
            textObj_setText(roster->player[i].text, roster->player[i].name);
        }
    }
}

/***********************************************************************
 *                  Initialize a new Tournament game                   *
 ***********************************************************************/

void init_tournament_state( struct TournamentState_ * ts )
{
    static int init_me=1;
    int i,j,k, dummy, game;
    int players[100];

    ts->round_num=TOURNAMENT_ROUND_NUM;
    ts->game_ind=0;
    ts->round_ind=0;
    ts->wait_for_next_match=1;
    ts->tournament_over=0;
    ts->overall_winner=-1;
    ts->ai_fast_motion=options_tourfast;
    for( i=0 ; i<(1<<ts->round_num) ; i++ ) players[i]=i;
    /* mix players for tournament */
    for( k=0 ; k<1000 ; k++ ){
        i =   k   %(1<<ts->round_num);
        j = rand()%(1<<ts->round_num);
        dummy=players[i];
        players[i]=players[j];
        players[j]=dummy;
    }
    /* set up pairings */
    for( game=0 ; game<(1<<(ts->round_num-1)) ; game++ ){
        ts->game[0][game].roster_player1 = players[(game*2)];
        ts->game[0][game].roster_player2 = players[(game*2)+1];
        ts->game[0][game].winner = -1;
    }
    ts->roster.nr = 1<<ts->round_num;

    if(init_me){
        //fprintf(stderr,"init_tournament_state: initializing player roster\n");
        init_ai_player_roster(&(ts->roster));
        init_me=0;
    }
}

/***********************************************************************
 *            Setup the next round in a tournament game                *
 ***********************************************************************/

void tournament_state_setup_next_round( struct TournamentState_ * ts )
{
    int i;
    int players[100];

    (ts->round_ind)++;

    if( ts->round_ind == ts->round_num ){ /* tournament over ? */
        ts->tournament_over=1;
        ts->round_ind = ts->round_num-1;
        if(ts->game[ts->round_ind][0].winner==0){
            ts->overall_winner=ts->game[ts->round_ind][0].roster_player1;
        } else if(ts->game[ts->round_ind][0].winner==1){
            ts->overall_winner=ts->game[ts->round_ind][0].roster_player2;
        } else {
            fprintf(stderr,"error: nobody won the tournament !?\n");
            sys_exit(1);
        }
    } else {
        for(i=0;i<(1<<(ts->round_num-ts->round_ind));i++){
            if(ts->game[ts->round_ind-1][i].winner==0){
                players[i]=ts->game[ts->round_ind-1][i].roster_player1;
            } else if(ts->game[ts->round_ind-1][i].winner==1){
                players[i]=ts->game[ts->round_ind-1][i].roster_player2;
            } else {
                fprintf(stderr,"error: sbdy didnt win one of the last matches !?\n");
                sys_exit(1);
            }
        }
        // Pairings
        for(i=0;i<(1<<(ts->round_num-ts->round_ind-1));i++){
            ts->game[ts->round_ind][i].roster_player1=players[2*i];
            ts->game[ts->round_ind][i].roster_player2=players[2*i+1];
            ts->game[ts->round_ind][i].winner = -1;
            /* fprintf(stderr,"%d vs. %d\n",
                   ts->game[ts->round_ind][i].roster_player1,
                   ts->game[ts->round_ind][i].roster_player2
                  );
            fprintf(stderr,"%s vs. %s\n",
                   ts->roster.player[ts->game[ts->round_ind][i].roster_player1].name,
                   ts->roster.player[ts->game[ts->round_ind][i].roster_player2].name
                  ); */

        }
    }
}

/***********************************************************************
 *      Evaluate the last whole match inside a tournament game         *
 ***********************************************************************/

void tournament_evaluate_last_match( struct TournamentState_ * ts )
{
    if( player[0].winner ){
        ts->game[ts->round_ind][ts->game_ind].winner=0;
    } else if( player[1].winner ){
        ts->game[ts->round_ind][ts->game_ind].winner=1;
    } else {
        ts->game[ts->round_ind][ts->game_ind].winner=-1;
    }
    ts->game_ind++;
    if( ts->game_ind >= (1<<(ts->round_num-ts->round_ind-1)) ){
        tournament_state_setup_next_round(ts);
        ts->game_ind=0;
    }
}

/***********************************************************************
 *           Set the next match inside a tournament game               *
 ***********************************************************************/

void tournament_state_setup_next_match( struct TournamentState_ * ts )
{
    player_copy(&(player[0]),ts->roster.player[ts->game[ts->round_ind][ts->game_ind].roster_player1]);
    player_copy(&(player[1]),ts->roster.player[ts->game[ts->round_ind][ts->game_ind].roster_player2]);
    player[0].winner=0;
    player[1].winner=0;
    if( player[0].is_AI && player[1].is_AI ){
        g_motion_ratio=ts->ai_fast_motion;
    } else {
        g_motion_ratio=1.0;
    }
    restart_game_common();
    act_player=0;
    queue_view=player[act_player].queue_view;
}

/***********************************************************************
 *    Set the Nametext of two players on the machine for displaying    *
 ***********************************************************************/

void create_players_text(void)
{
    player[0].text = textObj_new(player[0].name, options_player_fontname, 28);
    player[1].text = textObj_new(player[1].name, options_player_fontname, 28);
    player[0].score_text = textObj_new("0", options_score_fontname, 20);
    player[1].score_text = textObj_new("0", options_score_fontname, 20);
}

/***********************************************************************
 *            Copy the state of all balls in a game                    *
 ***********************************************************************/

void copy_balls( BallsType * balls1, BallsType * balls2 )
{
    int i;
    if ( balls2->nr != balls1->nr ){
        balls2->nr = balls1->nr;
        free( balls2->ball );
        balls2->ball=(BallType *)malloc(balls2->nr*sizeof(BallType));
    }
    for(i=0;i<balls1->nr;i++){
        balls2->ball[i] = balls1->ball[i];
    }
    balls2->gametype = balls1->gametype;
}

/***********************************************************************
 *             The shoot with the cue (german: queue)                  *
 ***********************************************************************/

void queue_shot(void)
{
    VMvect dir, nx, ny, hitpoint;
    int i;
    int cue_ball = CUE_BALL_IND;

#ifdef NETWORKING
    if( !balls_moving && !player[0].winner && !player[1].winner && !wait_key){
#else
     if( !balls_moving && !player[0].winner && !player[1].winner){
#endif
        /* backup actual ball setup */
        copy_balls(&balls,&bakballs);
        dir = vec_xyz(sin(Zque*M_PI/180.0)*sin(Xque*M_PI/180.0),
                      cos(Zque*M_PI/180.0)*sin(Xque*M_PI/180.0),
                      cos(Xque*M_PI/180.0));
        nx = vec_unit(vec_cross(vec_ez(),dir));  /* parallel to table */
        ny = vec_unit(vec_cross(nx,dir));        /* orthogonal to dir and nx */
        hitpoint = vec_add(vec_scale(nx,queue_point_x),vec_scale(ny,queue_point_y));
        //fprintf(stderr,"queue_shot: Zque=%f\n",Zque);
        balls.ball[cue_ball].v =  vec_scale(dir,-CUEBALL_MAXSPEED*queue_strength);
        if(!options_jump_shots) {
           balls.ball[cue_ball].v.z =  0.0;
           }
        if(vec_abssq(hitpoint)==0.0){
            balls.ball[cue_ball].w = vec_xyz(0.0,0.0,0.0);
        } else {
            /* w = roll speed if hit 1/3of radius above center */
//            balls.ball[cue_ball].w = vec_scale(vec_cross(dir,hitpoint),4.0*3.0*CUEBALL_MAXSPEED*queue_strength/balls.ball[cue_ball].d/balls.ball[cue_ball].d);
            /* hmm, this one works better */
            balls.ball[cue_ball].w = vec_scale(vec_cross(dir,hitpoint),2.0*3.0*CUEBALL_MAXSPEED*queue_strength/balls.ball[cue_ball].d/balls.ball[cue_ball].d);
        }

#ifdef USE_SOUND
        if(options_gamemode==options_gamemode_tournament && player[0].is_AI && player[1].is_AI) {
          //nosound
        } else {
          PlaySound(&ball_cue_snd,options_snd_volume*queue_strength/2.0);
        }
#endif

        /* clear recorded ballpaths */
        for( i=0 ; i<balls.nr ; i++ ){
            BM_clearpath( &balls.ball[i] );
        }

        /* reset offset parameters */
        queue_point_x=0.0;
        queue_point_y=0.0;
    }
}

/***********************************************************************
 *               Computer move or proposal for a move                  *
 ***********************************************************************/

void do_computer_move( int doit )
{
    VMvect dir;

    //fprintf(stderr,"do_computermove: begin ai_get_strike_dir\n");

    ai_set_err(player[act_player].err);
    dir = ai_get_stroke_dir(&balls,&walls,&player[act_player]);

    //fprintf(stderr,"do_computermove: end ai_get_strike_dir\n");

    Zque = atan2(dir.x,dir.y)/M_PI*180.0;
    if(doit){
        shoot( !queue_view );
    }
    comp_dir=dir;
}

/***********************************************************************
 *   calculate cue offset func called from queue_offs_func (cos)       *
 ***********************************************************************/

VMfloat queue_offs_func1( VMfloat t )
{
    return( 1.0-cos(t*2.0*M_PI) );
}

/***********************************************************************
 *   calculate cue offset func called from queue_offs_func (sin)       *
 ***********************************************************************/

VMfloat queue_offs_func2( VMfloat t )
{
    return( sin(t*M_PI) );
}

/***********************************************************************
 *                    calculate cue offset func main                   *
 ***********************************************************************/

VMfloat queue_offs_func( VMfloat t )
{
    VMfloat tx6, rval;
    rval=0.0;
    tx6=t*4.5;
    if ( tx6 >= 0.0 && tx6 < 1.0  ){
        rval = 1.0*queue_offs_func1(tx6);
    } else if( tx6 >= 1.0 && tx6 < 1.4  ){
        rval = 0.0;
    } else if( tx6 >= 1.4 && tx6 < 2.4  ){
        rval = 1.0*queue_offs_func1((tx6-1.4)/1.0);
    } else if( tx6 >= 2.4 && tx6 < 2.8  ){
        rval = 0.0;
    } else if( tx6 >= 2.8 && tx6 < 3.8  ){
        rval = 1.3*queue_offs_func1((tx6-2.8)/1.0);
    } else if( tx6 >= 3.8 && tx6 < 4.5  ){
        rval = 6.0*queue_offs_func2((tx6-3.8)/0.7*1.06);
    }
    return 0.5*(0.7+rval);
}

/***********************************************************************
 *                Do a Shot (with cue-animation or not)                *
 ***********************************************************************/

void shoot( int ani )
{
    int other_player;
#ifdef NETWORKING
    if(wait_key) {
       return;
    }
#endif
    other_player = (act_player==0) ? 1 : 0 ;
    if ( player[other_player].is_net ){
       // other player is net
    }

    if( ani ){
        //fprintf(stderr,"shoot: animate-shot\n");
        queue_anim=30;
    } else {
        //fprintf(stderr,"shoot: direct shot\n");
        queue_shot();
    }
}

/***********************************************************************
 *         Is the cue ball inside the region of the table ?            *
 ***********************************************************************/

int in_cue_ball_region( VMvect pos )
{
    //fprintf(stderr,"in_cue_ball_region\n");
    switch(gametype){
    case GAME_8BALL:
    case GAME_9BALL:
        if( pos.x >  (TABLE_W-BALL_D)/2.0 || pos.x < -(TABLE_W-BALL_D)/2.0
        	|| pos.y > -TABLE_L/4.0 || pos.y < -(TABLE_L-BALL_D)/2.0) {
        	return(0);
        }
        break;
    case GAME_CARAMBOL:
        break;
    case GAME_SNOOKER:
#define TABLE_SCALE (TABLE_L/(3.571042))
        if( pos.y > -TABLE_L/2.0+TABLE_SCALE*0.737 || vec_abs(vec_diff(pos,vec_xyz(0,-TABLE_L/2.0+TABLE_SCALE*0.737,0))) > TABLE_SCALE*0.292) {
        	return(0);
        }
#undef TABLE_SCALE
        break;
    }
    return(1);
}

/***********************************************************************
 *          Is the pos inside the playground of the table ?            *
 ***********************************************************************/

int in_table_region( VMvect pos )
{
	   //fprintf(stderr,"in_table_region\n");
    if( pos.x >  (TABLE_W-BALL_D)/2.0 || pos.x < -(TABLE_W-BALL_D)/2.0
    	|| pos.y >  (TABLE_L-BALL_D)/2.0 || pos.y < -(TABLE_L-BALL_D)/2.0) {
    	return(0);
    }
    return(1);
}

/***********************************************************************
 *           no ball split into another (for one balls)                *
 *                called from all_balls_free_place                     *
 ***********************************************************************/

static void ball_free_place( int ind, BallsType * pballs )
{
        int i,exitloop;
        VMfloat x,y,x0,y0,r,phi;
        x=pballs->ball[ind].r.x; y=pballs->ball[ind].r.y;
        x0=x;
        y0=y;
        phi=0.0;

        do{
            exitloop=1;
            r=floor(phi/2.0/M_PI)*0.01;
            x=x0+r*cos(phi);
            y=y0+r*sin(phi);
            //fprintf(stderr,"phi=%f\n", phi);
            //fprintf(stderr,"ind=%d, CUE_BALL_IND=%d\n", ind, CUE_BALL_IND);
            if( ( ind==CUE_BALL_IND && in_cue_ball_region(vec_xyz(x,y,0)) && player[act_player].place_cue_ball ) || in_table_region(vec_xyz(x,y,0))) {
            } else {
            	exitloop=0;
            }
            //fprintf(stderr,"1:exitloop=%d\n", exitloop);
            for(i=0;i<pballs->nr;i++) if( i!=ind && pballs->ball[i].in_game ){
                if ( vec_abs(vec_diff(vec_xyz(x,y,0),pballs->ball[i].r)) <
                     (pballs->ball[ind].d+pballs->ball[i].d)/2.0 )
                {
                	exitloop=0;
                	break;
                }
            }
            //fprintf(stderr,"2:exitloop=%d\n", exitloop);
            phi+=0.01;
        } while(!exitloop);
        pballs->ball[ind].r.x=x;
        pballs->ball[ind].r.y=y;
}

/***********************************************************************
 *           no ball split into another (for all balls)                *
 ***********************************************************************/

static void all_balls_free_place(BallsType * pballs)
{
    int i;
    for(i=0;i<pballs->nr;i++) if( pballs->ball[i].in_game ){
        ball_free_place( i, pballs );
    }
}

/***********************************************************************
 *           Check for correct values for the strength bar             *
 ***********************************************************************/

VMfloat strength01( VMfloat value )
{
    if( value > 1.0 ) value=1.0;
    if( value < 0.0 ) value=0.0;
    return value;
}

/***********************************************************************
 *             Shoot with mouse middle or Enter key                    *
 ***********************************************************************/

void enter_mousemiddle(void)
{
    if (options_gamemode==options_gamemode_tournament && tournament_state.wait_for_next_match) {
      tournament_state_setup_next_match(&tournament_state);
      tournament_state.wait_for_next_match=0;
#ifdef NETWORKING
    } else if(!player[act_player].is_net && !player[act_player].is_AI && !wait_key){
#else
    } else if(!player[act_player].is_AI){
#endif
      g_shot_due=0;
      shoot( !queue_view );
    }
}

/***********************************************************************
 *         Special Keystroke-function of the Slider-Menues             *
 ***********************************************************************/

void Key1( int key )
{
  Key(key,0);
  b1_hold=0;
#ifdef USE_SOUND
  if(key !=13) {
       PlaySound(&ball_cue_snd,options_snd_volume);
  }
#endif
}

/***********************************************************************
 *                     Event for the mousebuttons                      *
 ***********************************************************************/

void MouseEvent(MouseButtonEnum button,MouseButtonState state, int x, int y,int key_modifiers)
{

    GLdouble newy;
    int newy_int, newx_int;

    newy = (GLdouble)win_height-y;
    newy_int = win_height-y;
    newx_int = win_width-x;
    if ( g_act_menu != (menuType *)0 ) {

        //fprintf(stderr,"x,y=%d,%d\n",x,y);
        menu_select_by_coord( g_act_menu, x-win_width/2, -y+win_height/2 );
#ifndef TOUCH
        if ( button==MOUSE_LEFT_BUTTON && state==MOUSE_DOWN && menu_choose_by_coord(g_act_menu, x-win_width/2, -y+win_height/2 )) {
#else
        if ( button==MOUSE_LEFT_BUTTON && state==MOUSE_UP && menu_choose_by_coord(g_act_menu, x-win_width/2, -y+win_height/2 )) {
#endif
#ifdef USE_SOUND
              PlaySound(&ball_cue_snd,options_snd_volume);
#endif
          menu_choose(&g_act_menu);
         }
    } else {

        if (button == MOUSE_LEFT_BUTTON) {
            if (state == MOUSE_DOWN) {
                if (options_gamemode==options_gamemode_tournament && tournament_state.wait_for_next_match) {
                   // we need it here for touch devices - shoot are not accessible
                   tournament_state_setup_next_match(&tournament_state);
                   tournament_state.wait_for_next_match=0;
                }
                if(!introtexture) introtexture++;
                if(b2_hold){
                    b2_b1_hold = 1;
                    Key1('m'); //m
                } else {
#ifdef NETWORKING
                // check on end of network game button upper right corner (only server is allowed to do that)
                if(active_net_game) {
                 //fprintf(stderr,"x: %i newy: %i winx: %i winy: %i\n",x,newy_int,win_width,win_height);
                 if(x > win_width-60 && x < win_width-14 && newy_int > win_height-53 && newy_int < win_height-14) {
                  if(server!=NULL) {
#ifdef USE_SOUND
                    PlaySound(&ball_cue_snd,options_snd_volume);
#endif
                    displaystring(localeText[262]);
                    close_listener(); // end network game
                  } else {
                    displaystring(localeText[263]);
                  }
                 }
                }
#endif
                //check for up/down/back/next/shoot button click or touch on the strengthbar
                if(!(options_gamemode==options_gamemode_tournament && tournament_state.wait_for_next_match) && !(player[act_player].is_net || player[act_player].is_AI) && !balls_moving) {
                   if((GLdouble)x > x_strengthbar && (GLdouble)x < x_strengthbar_end && newy > y_strengthbar && newy < y_strengthbar_end) {
                     //((xMausklick-XBeginn) * ((XEnde-XAnfang)/100))*0.01
                     queue_strength = strength01((((GLdouble)x-x_strengthbar) / ((x_strengthbar_end-x_strengthbar)/100.0))*0.01 ); // set strenghtbar value direct
#ifdef USE_SOUND
                     PlaySound(&ball_cue_snd,options_snd_volume);
#endif
                }
                  if(options_show_buttons) { // only, if the control buttons are shown
                   if((GLdouble)x-10 > x_downbutton && (GLdouble)x < (x_downbutton+40.0) && newy-10.0 > y_downbutton && newy < (y_downbutton + 40.0)) {
                     // zoom-
                     hudbuttonpressed = 1;
#ifdef USE_SOUND
                     PlaySound(&ball_cue_snd,options_snd_volume);
#endif

                   }
                   if((GLdouble)x-10 > x_upbutton && (GLdouble)x < (x_upbutton+40.0) && newy-10.0 > y_upbutton && newy < (y_upbutton + 40.0)) {
                     // zoom+
                     hudbuttonpressed = 2;
#ifdef USE_SOUND
                     PlaySound(&ball_cue_snd,options_snd_volume);

#endif
                   }
                   if((GLdouble)x-10 > x_backbutton && (GLdouble)x < (x_backbutton+40.0) && newy-10.0 > y_backbutton && newy < (y_backbutton + 40.0)) {
                     // strength down
                     hudbuttonpressed = 3;
#ifdef USE_SOUND
                     PlaySound(&ball_cue_snd,options_snd_volume);
#endif
                   }
                   if((GLdouble)x-10 > x_nextbutton && (GLdouble)x < (x_nextbutton+40.0) && newy-10.0 > y_nextbutton && newy < (y_nextbutton + 40.0)) {
                     // strength up
                     hudbuttonpressed = 4;
#ifdef USE_SOUND
                     PlaySound(&ball_cue_snd,options_snd_volume);
#endif
                   }
                   if((GLdouble)x-10 > x_shootbutton && (GLdouble)x < (x_shootbutton+40.0) && newy-10.0 > y_shootbutton && newy < (y_shootbutton + 40.0)) {
                     Key1(13); // make shoot over the key-routine
                   }
                  } //end, if the control buttons are shown
                }
                    b1_hold = 1;
                    start_x = x;
                    start_y = y;
                    if(!player[act_player].is_net && !balls_moving) {
                     switch(leftmenu) {
                         case 0:  // open left menu
                           if(x > 6 && x < 30 && newy_int > 475 && newy_int < 517) {
                             Key1('l');
                           }
                         break;
                         case 2:  // left menu is open
                           if(x > 185 && x < 203 && newy_int > 480 && newy_int < 517) {
                             Key1('l'); // close left menu
                           }
                           if(x > 11 && x < 80) {
                             //left button side
                             if(newy_int > 656 && newy_int < 693) {
                                Key1(KSYM_F1); //F1
                             } else  if(newy_int > 601 && newy_int < 632) {
                                Key1(KSYM_F3); //F3
                             } else if(newy_int > 542 && newy_int < 574) {
                                Key1(KSYM_F5); //F5
                             } else if(newy_int >  483 && newy_int < 516) {
                                Key1(KSYM_F7); //F7
                             } else if(newy_int > 422 && newy_int < 457) {
                                Key1(KSYM_F9); //F9
                             } else if(newy_int > 362 && newy_int < 398) {
                                Key1('s'); //s
                             } else if(newy_int > 304 && newy_int < 337) {
                                Key1('e'); //e
                             } else if(newy_int > 244 && newy_int < 279) {
                                Key1('f'); //f
                             } else if(options_gamemode == options_gamemode_training && newy_int > 183 && newy_int < 220) {
                                Key1(9); //Tab
                             }
                           } else if(x > 101 && x < 168) {
                             //right button side
                             if(newy_int > 656 && newy_int < 693) {
                                Key1(KSYM_F2); //F2
                             } else  if(newy_int > 601 && newy_int < 632) {
                                Key1(KSYM_F4); //F4
                             } else if(newy_int > 542 && newy_int < 574) {
                                Key1(KSYM_F6); //F6
                             } else if(newy_int >  483 && newy_int < 516) {
                                Key1(KSYM_F8); //F8
                             } else if(newy_int > 422 && newy_int < 457) {
                                Key1(KSYM_F10); //F10
                             } else if(newy_int > 362 && newy_int < 398) {
                                Key1('b'); //b
                             } else if(newy_int > 304 && newy_int < 337) {
                                Key1('m'); //m
                             } else if(newy_int > 244 && newy_int < 279) {
                                Key1(27); //ESC/Menu
                             } else if(options_gamemode == options_gamemode_training && newy_int > 183 && newy_int < 220) {
                                Key1('u'); //u
                             }
                           }
                         break;
                     }
                     switch(rightmenu) {
                         case 0:  // open right menu
                           if(newx_int > 6 && newx_int < 30 && newy_int > 475 && newy_int < 517) {
                             Key1('r');
                           }
                         break;
                         case 2:  // close right menu
                           if(newx_int > 185 && newx_int < 203 && newy_int > 480 && newy_int < 517) {
                             Key1('r');
                           }
                           if(newx_int < 175 && newy_int > 175 && newy_int < 701) {
                             // parse right slider menu
                             if(newy_int > 536) { // cursor cross
                              if(newx_int > 69 && newx_int < 109) { //cursor up/down
                               if(newy_int < 595) { // cursor down
                                //fprintf(stderr,"cursor down\n");
                                hudbuttonpressed = 5;
#ifdef USE_SOUND
                                PlaySound(&ball_cue_snd,options_snd_volume);
#endif
                               } else if(newy_int > 653) { // cursor up
                                //fprintf(stderr,"cursor up\n");
                                hudbuttonpressed = 6;
#ifdef USE_SOUND
                                PlaySound(&ball_cue_snd,options_snd_volume);
#endif
                               }
                              } else if(newy_int > 601 && newy_int < 641) { //cursor left right
                                if(newx_int < 57) { // cursor right
                                 //fprintf(stderr,"cursor right\n");
                                 hudbuttonpressed = 7;
#ifdef USE_SOUND
                                 PlaySound(&ball_cue_snd,options_snd_volume);
#endif
                                } else if(newx_int > 120) { // cursor left
                                 //fprintf(stderr,"cursor left\n");
                                 hudbuttonpressed = 8;
#ifdef USE_SOUND
                                 PlaySound(&ball_cue_snd,options_snd_volume);
#endif
                                }
                              }
                             } else if(newy_int > 254 && newy_int < 492 && newx_int > 51 && newx_int < 129) { // shoot, button up/down
                              if(newy_int < 318) { //shoot
                               //fprintf(stderr,"shoot\n");
                               Key1(13);
                              } else if(newy_int < 411 && newy_int > 371) { // button down
                               //fprintf(stderr,"button down\n");
                               hudbuttonpressed = 1;
#ifdef USE_SOUND
                               PlaySound(&ball_cue_snd,options_snd_volume);
#endif
                              } else if(newy_int < 491 && newy_int > 452) { //button up
                               //fprintf(stderr,"button up\n");
                               hudbuttonpressed = 2;
#ifdef USE_SOUND
                               PlaySound(&ball_cue_snd,options_snd_volume);
#endif
                              }
                             } else if(newy_int < 216) { //button left/right
                              if(newx_int < 78) { // button right
                               //fprintf(stderr,"button right\n");
                               hudbuttonpressed = 4;
#ifdef USE_SOUND
                               PlaySound(&ball_cue_snd,options_snd_volume);
#endif
                              } else if (newx_int > 97) { //button left
                               //fprintf(stderr,"button left\n");
                               hudbuttonpressed = 3;
#ifdef USE_SOUND
                               PlaySound(&ball_cue_snd,options_snd_volume);
#endif
                              }
                             }
                           }
                         break;
                     }
                    }
                }
            }
            if (state == MOUSE_UP) {
                hudbuttonpressed = 0;
                button_anim = 0.7;
                step = 0.03; // Keys Accelerator back to start
                freeview_step = 0.03; // Keys Accelerator back to start
#ifndef TOUCH
                if(control__english) control_unset(&control__english);
                if(control__place_cue_ball) control_unset(&control__place_cue_ball);
                if(control__cue_butt_updown) control_unset(&control__cue_butt_updown);
                if(control__fov) control_unset(&control__fov);
                if(control__mouse_shoot) control_unset(&control__mouse_shoot);
#endif
                b1_hold = 0;
                b2_b1_hold = 0;
                if(control__active) control__updated=1;
            }
        }
        if (button == MOUSE_RIGHT_BUTTON){
            hudbuttonpressed = 0;
            button_anim = 0.7;
            if (state == MOUSE_DOWN) {
                mouse_moved_after_b1_dn = 0;
                if ( b1_hold ){
                    b1_b2_hold = 1;
                    Key1('e'); //e
                } else {
                    b2_hold = 1;
                    scaling_start = y;
                    scaling_start2 = x;
                }
            }
            if (state == MOUSE_UP) {
                if(b1_b2_hold && !mouse_moved_after_b1_dn && !options_birdview_on) toggle_queue_view();
                b1_b2_hold = 0;
                b2_hold = 0;
            }
        }
        if (button == MOUSE_MIDDLE_BUTTON) {
            hudbuttonpressed = 0;
            button_anim = 0.7;
            if (state == MOUSE_UP) {
                enter_mousemiddle();
            }
        }
        if (button == MOUSE_WHEEL_UP_BUTTON) {
            hudbuttonpressed = 0;
            button_anim = 0.7;
            if(!player[act_player].is_AI && !balls_moving)
                queue_strength = strength01( queue_strength+0.01 );
        }
        if (button == MOUSE_WHEEL_DOWN_BUTTON) {
            hudbuttonpressed = 0;
            button_anim = 0.7;
            if(!player[act_player].is_AI && !balls_moving)
                queue_strength = strength01( queue_strength-0.01 );
        }

    }
//    fprintf(stderr,"button=%d\n", button);

}

/***********************************************************************
 *  Newpos of the cueball on manually place cueball inside table range *
 ***********************************************************************/

void ball_displace_clip( VMvect * cue_pos, VMvect offs )
{
    VMvect newpos;

    newpos = vec_add( *cue_pos, offs );

    if(options_gamemode==options_gamemode_training){

        if( newpos.x >  (TABLE_W-BALL_D)/2.0 ) newpos.x= (TABLE_W-BALL_D)/2.0 ;
        if( newpos.x < -(TABLE_W-BALL_D)/2.0 ) newpos.x=-(TABLE_W-BALL_D)/2.0 ;
        if( newpos.y >  (TABLE_L-BALL_D)/2.0 ) newpos.y= (TABLE_L-BALL_D)/2.0 ;
        if( newpos.y < -(TABLE_L-BALL_D)/2.0 ) newpos.y=-(TABLE_L-BALL_D)/2.0 ;
    } else {
        switch(gametype){
        case GAME_8BALL:
        case GAME_9BALL:
            if( newpos.x >  (TABLE_W-BALL_D)/2.0 ) newpos.x= (TABLE_W-BALL_D)/2.0 ;
            if( newpos.x < -(TABLE_W-BALL_D)/2.0 ) newpos.x=-(TABLE_W-BALL_D)/2.0 ;
            if( newpos.y > -TABLE_L/4.0          ) newpos.y= -TABLE_L/4.0 ;
            if( newpos.y < -(TABLE_L-BALL_D)/2.0 ) newpos.y=-(TABLE_L-BALL_D)/2.0 ;
            break;
        case GAME_CARAMBOL:
            break;
        case GAME_SNOOKER:
#define TABLE_SCALE (TABLE_L/(3.571042))
            if( newpos.y > -TABLE_L/2.0+TABLE_SCALE*0.737 ) newpos.y= -TABLE_L/2.0+TABLE_SCALE*0.737 ;
            newpos = vec_diff(newpos,vec_xyz(0,-TABLE_L/2.0+TABLE_SCALE*0.737,0));
            if( vec_abs(newpos) > TABLE_SCALE*0.292 ) newpos = vec_scale( vec_unit(newpos), TABLE_SCALE*0.292 ) ;
            newpos = vec_add(newpos,vec_xyz(0,-TABLE_L/2+TABLE_SCALE*0.737,0));
#undef TABLE_SCALE
            break;
        }
    }

    *cue_pos=newpos;
}

/***********************************************************************
 *                 Event for the motion of the mouse                   *
 ***********************************************************************/

void MouseMotion(int x, int y, int key_modifiers)
{

    static VMfloat acc;
    int place_cue_ball=0;
    int cue_ball = CUE_BALL_IND;
    int move_ok = 1;
    int i;
    VMfloat dx, dy, abspos;
    VMfloat Xoffs,Zoffs;
    VMvect whitepos, xv, yv;

    if (control__updated) {
        //fprintf(stderr,"updated\n");
        start_x = x;
        start_y = y;
        scaling_start = y;
        scaling_start2 = x;
        control__updated=0;
    }

    mouse_moved_after_b1_dn = 1;

    if( g_act_menu != (menuType *)0 ){
        menu_select_by_coord( g_act_menu, x-win_width/2, -y+win_height/2 );
    } else {

    acc=1.0;

    if(control__active){
        if (control__place_cue_ball){
            if( player[act_player].place_cue_ball && !balls_moving && !player[act_player].is_AI && !player[act_player].is_net ){
                dx=(VMfloat)(x-start_x);
                dx=dx*0.0001+fabs(dx)*dx*0.0002;
                dy=(VMfloat)(y-start_y);
                dy=dy*0.0001+fabs(dy)*dy*0.0002;
                xv=vec_xyz(+dx*cos(Zrot/180.0*M_PI),-dx*sin(Zrot/180.0*M_PI),0.0);
                yv=vec_xyz(-dy*sin(Zrot/180.0*M_PI),-dy*cos(Zrot/180.0*M_PI),0.0);
                whitepos=balls.ball[cue_ball].r;
                ball_displace_clip( &(balls.ball[cue_ball].r), vec_add(xv,yv));
                for(i=0;i<balls.nr;i++){
                  if(i!=cue_ball){
                     move_ok = move_ok &&
                               ( vec_abs(vec_diff(balls.ball[cue_ball].r,balls.ball[i].r))>(balls.ball[cue_ball].d+balls.ball[i].d)/2.0 ||
                               (!balls.ball[i].in_game) );
                     }
                  }
                if(!move_ok) balls.ball[cue_ball].r=whitepos;
            }
        } else if ( control__mouse_shoot ){
            if( (!queue_view) && (!balls_moving) && !player[act_player].is_AI && !player[act_player].is_net ) {  /* dynamic cue shot */
             abspos = angle_pm360(angle_pm360(Zrot+Zrot_offs)-angle_pm360(Zque));
             //fprintf(stderr,"Abspos: %f cue: %f\n",abspos,queue_offs);
             //fprintf(stderr,"x: %i y: %i\n",x,y);
             if(options_mouseshoot) { //real mouseshoot
              if(abspos >= 0.0f && abspos < 180.0f) { //left side of the cue
                queue_offs += ((VMfloat)(x-start_x)*0.002)/2;
                queue_strength = +(-0.02*(VMfloat)(x-start_x))/2;
                if(abspos < 90.0f) {
                   queue_offs+=((VMfloat)(y-start_y)*0.002)/2;
                   queue_strength += (-0.02*(VMfloat)(y-start_y))/2;
                } else {
                   queue_offs-=((VMfloat)(y-start_y)*0.002)/2;
                   queue_strength -= (-0.02*(VMfloat)(y-start_y))/2;
                }
              queue_strength=strength01(queue_strength);
              } else { //right side of the cue
                queue_offs-=((VMfloat)(x-start_x)*0.002)/2;
                queue_strength = -(-0.02*(VMfloat)(x-start_x))/2;
                if(abspos < 260.0f) {
                   queue_offs-=((VMfloat)(y-start_y)*0.002)/2;
                   queue_strength -= (-0.02*(VMfloat)(y-start_y))/2;
                } else {
                   queue_offs+=((VMfloat)(y-start_y)*0.002)/2;
                   queue_strength += (-0.02*(VMfloat)(y-start_y))/2;
                }
              queue_strength=strength01(queue_strength);
              }
             } else { //classic up/down
                queue_offs+=(VMfloat)(y-start_y)*0.002;
                queue_strength=strength01( -0.02*(VMfloat)(y-start_y) );
              }
              if(queue_offs > 1.0) {
                queue_offs = 1.0;
                }
              if( queue_offs < balls.ball[cue_ball].d/2.0 ){
                queue_offs=0.06;
                shoot(0);
                }
              start_x = x;
              start_y = y;
            }
        } else if ( control__english ){
            setenglish((x-scaling_start2)*0.0005, (y-scaling_start)*0.0005);
            scaling_start = y;
            scaling_start2 = x;
        } else if (control__cue_butt_updown){
            if(queue_view) toggle_queue_view();
            Xoffs =  (VMfloat)(y-start_y)*0.02*acc;
            Xoffs +=  (VMfloat)(y-start_y)*fabs(y-start_y)*0.01*acc;
            if( Xque-Xoffs < -90.0  ) Xoffs=-90.0-Xque;
            if( Xque-Xoffs >   0.0  ) Xoffs=  0.0-Xque;
            Xque-=Xoffs;
        } else if (control__fov){ //special key handling FOV
            setfov((y-scaling_start)*0.05);
        }
        start_x = x;
        start_y = y;
        scaling_start = y;
        scaling_start2 = x;

    } else if(b1_hold){
        //fprintf(stderr,"x: %i y: %i\n",x, win_height-y);
        if( b1_b2_hold ) {
            place_cue_ball=player[act_player].place_cue_ball;
        }
        if( !place_cue_ball) {
          if(!options_birdview_on) {
            //not birdview, then normal moving
            Xoffs =  (VMfloat)(y-start_y)*0.02*acc;
            Zoffs =  (VMfloat)(x-start_x)*0.02*acc;
            if(!FREE_VIEW) {
               Xoffs += (VMfloat)(y-start_y)*fabs(y-start_y)*0.01*acc;
               Zoffs += (VMfloat)(x-start_x)*fabs(x-start_x)*0.01*acc;
               if(y <= win_height/2) {
                 Zoffs = -Zoffs;
               }
               //fprintf(stderr,"Z %f w %f\n",Zoffs,whatoffs);
            } else {
               Xoffs -=  (VMfloat)(y-start_y)*fabs(y-start_y)*0.01*acc;
               Zoffs -=  (VMfloat)(x-start_x)*fabs(x-start_x)*0.01*acc;
            }
            if( Xrot+Xoffs < -90.0  ) Xoffs=-90.0-Xrot;
            if( Xrot+Xoffs >   0.0  ) Xoffs=  0.0-Xrot;
            Xrot += Xoffs;
            Zrot = angle_pm360(Zrot+Zoffs);
            Xrot_offs -= Xoffs;
            Zrot_offs -= Zoffs;
            if( queue_view ){
                Xque=Xrot;
                Zque=Zrot;
            }
          } else if (!balls_moving && !player[act_player].is_AI && !player[act_player].is_net ){
            // in birdview on, the cue is moving
            Zoffs = (VMfloat)(x-start_x)/2;
            if(Zque >180.0 && Zque <360.0) {
             Zoffs = -Zoffs;
            }
            Zque = angle_pm360(Zque + Zoffs);
            Zoffs = (VMfloat)(y-start_y)/2;
            if(Zque <90.0 || Zque >270.0) {
             Zoffs = -Zoffs;
            }
            Zque = angle_pm360(Zque + Zoffs);
          }
        } else if( place_cue_ball && !balls_moving && !player[act_player].is_AI && !player[act_player].is_net ){
            dx=(VMfloat)(x-start_x);
            dx=dx*0.0001+fabs(dx)*dx*0.0002;
            dy=(VMfloat)(y-start_y);
            dy=dy*0.0001+fabs(dy)*dy*0.0002;
            xv=vec_xyz(+dx*cos(Zrot/180.0*M_PI),-dx*sin(Zrot/180.0*M_PI),0.0);
            yv=vec_xyz(-dy*sin(Zrot/180.0*M_PI),-dy*cos(Zrot/180.0*M_PI),0.0);
            whitepos=balls.ball[cue_ball].r;
            ball_displace_clip( &(balls.ball[cue_ball].r), vec_add(xv,yv));
            for(i=0;i<balls.nr;i++){
              if(i!=cue_ball){
                 move_ok = move_ok &&
                           ( vec_abs(vec_diff(balls.ball[cue_ball].r,balls.ball[i].r))>(balls.ball[cue_ball].d+balls.ball[i].d)/2.0 ||
                           (!balls.ball[i].in_game) );
                 }
              }
            if(!move_ok) balls.ball[cue_ball].r=whitepos;
        }
        start_x = x;
        start_y = y;
    } else if(b2_hold){
        if(b2_b1_hold && !player[act_player].is_AI && !player[act_player].is_net ){
            control_set(&control__english);
        } else {
            zoom_in_out(y-scaling_start);
            scaling_start = y;
        }
      }
    } //end menuselect
}

/***********************************************************************
 *              draw the rectangles for the buttons                    *
 ***********************************************************************/

void myRect2D_texture(void)
{
  static int myrect_id= -1;             // glcompile-id
  if(myrect_id == -1) {
    myrect_id = glGenLists(1);
    glNewList(myrect_id, GL_COMPILE_AND_EXECUTE);
    glBegin(GL_QUADS);
      glTexCoord2f(0,1);
      glVertex3f(0,0,0);
      glTexCoord2f(0,0);
      glVertex3f(0,48,0);
      glTexCoord2f(1,0);
      glVertex3f(48,48,0);
      glTexCoord2f(1,1);
      glVertex3f(48,0,0);
    glEnd();
    glEndList();
  } else {
    //fprintf(stderr,"myrect %i\n",myrect_id);
    glCallList(myrect_id);
  }
}

/***********************************************************************
 *              draw the Rectangles for the strength bar               *
 ***********************************************************************/

void myRect2D( VMfloat x1, VMfloat y1, VMfloat x2, VMfloat y2, VMfloat color_strength, VMfloat standard )
{
    glBegin(GL_QUADS);
      glColor3f(standard, standard + color_strength * 1, standard);
      glVertex3f(x1,y1,-0.5);
      glColor3f(standard,standard + color_strength * 1 , standard);
      glVertex3f(x1,y2,-0.5);
      glColor3f(standard + color_strength * 1, standard, standard);
      glVertex3f(x2,y2,-0.5);
      glColor3f(standard + color_strength * 1, standard, standard);
      glVertex3f(x2,y1,-0.5);
    glEnd();
}

/***********************************************************************
 *       Interpolate balls if the ifdef for that is set                *
 ***********************************************************************/

#ifdef TIME_INTERPOLATE
void interpolate_balls( BallsType * balls1, BallsType * balls2, BallsType * newballs, VMfloat ratio )
{
    int i,j;
    // fprintf(stderr,"ratio=%f",ratio);
    for(i=0;i<balls2->nr;i++){
        newballs->ball[i]=balls2->ball[i];
        newballs->ball[i].r = vec_add(vec_scale(balls1->ball[i].r,1.0-ratio),vec_scale(balls2->ball[i].r,ratio));
        for(j=0;j<3;j++) {
            newballs->ball[i].b[j] = vec_unit(vec_add(vec_scale(balls1->ball[i].b[j],1.0-ratio),vec_scale(balls2->ball[i].b[j],ratio)));
        }
    }
}
#endif

/***********************************************************************
 *      Display Winner Text after a whole Tournament game in 3D        *
 ***********************************************************************/

void draw_3D_winner_tourn_text(void)
{
    static VMfloat ang=0.0;
    static VMfloat tprev=0.0;
    VMfloat t;
    VMfloat dt;

    t=SDL_GetTicks();
    dt=(t-tprev)/1000.0;
    tprev=t;
    if (options_birdview_on && old_birdview_ai) {
       old_birdview_ai = queue_view;
       queue_view=0;
       birdview();
       queue_view=old_birdview_ai;
       old_birdview_ai = 0;
    }
#ifdef USE_SOUND
    if(!playonce) {
      if(!(player[player[0].winner?0:1].is_AI || player[player[0].winner?0:1].is_net)) {
        Playwavdata(cvt_applause.buf,cvt_applause.len_cvt,options_snd_volume);
      } else {
        Playwavdata(cvt_ooh.buf,cvt_ooh.len_cvt,options_snd_volume);
      }
      playonce++;
    }

#endif
    textObj_setText( winner_name_text_obj, player[player[0].winner?0:1].name );
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
    glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
    glColor3f(1.0,1.0,1.0);
    glBindTexture(GL_TEXTURE_2D, spheretexbind);
    glPushMatrix();
    glRotatef(ang+=60.0*dt,0,0,1);
    glTranslatef(0,0,0.2);
    glRotatef(90,1,0,0);
    textObj_draw_centered(tourn_winner_obj);
    glTranslatef(0,0.2,0);
    glRotatef(-ang*2.0,0,1,0);
    textObj_draw_centered(winner_name_text_obj);
    glPopMatrix();
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glEnable(GL_LIGHTING);
}

/***********************************************************************
 *        Display Winner Text after game in 3D (normal games)          *
 *          and on one ended match inside tournament games             *
 ***********************************************************************/

void draw_3D_winner_text(void)
{
    static VMfloat ang=0.0;
    static VMfloat tprev=0.0;
    VMfloat t;
    VMfloat dt;

    if (options_birdview_on && old_birdview_ai) {
       old_birdview_ai = queue_view;
       queue_view=0;
       birdview();
       queue_view=old_birdview_ai;
       old_birdview_ai = 0;
    }
    t=SDL_GetTicks();
    dt=(t-tprev)/1000.0;
    tprev=t;
#ifdef USE_SOUND
    if(!playonce && options_gamemode!=options_gamemode_tournament) {
      if(!(player[player[0].winner?0:1].is_AI || player[player[0].winner?0:1].is_net)) {
        Playwavdata(cvt_applause.buf,cvt_applause.len_cvt,options_snd_volume);
      } else {
        Playwavdata(cvt_ooh.buf,cvt_ooh.len_cvt,options_snd_volume);
      }
      playonce++;
    }
#endif

    textObj_setText( winner_name_text_obj, player[player[0].winner?0:1].name );
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
    glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
    glColor3f(1.0,1.0,1.0);
    glBindTexture(GL_TEXTURE_2D, spheretexbind);
    glPushMatrix();
    glRotatef(ang+=60.0*dt,0,0,1);
    glTranslatef(0,0,0.2);
    glRotatef(90,1,0,0);
    textObj_draw_centered(winner_text_obj);
    glTranslatef(0,0.2,0);
    glRotatef(-ang*2.0,0,1,0);
    textObj_draw_centered(winner_name_text_obj);
    glPopMatrix();
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glEnable(GL_LIGHTING);
}

/***********************************************************************
 *      Display the textures (cuberef reflections) for one ball        *
 *                 called from create_cuberef_map                      *
 ***********************************************************************/

void DisplayFunc_cubemap( int ballnr, int side, VMvect cam_pos, int cube_res )
{

   GLfloat light_position[] = { 0.0, 0.0, 0.7, 0.0 };  //the [3] is 0.0 for better performance
   GLfloat light_diff[]     = { 0.2, 0.2, 0.2, 1.0 };
   GLfloat light_amb[]      = { 0.05, 0.05, 0.05, 1.0 };

   GLfloat mv_matr[16] = {0.0f};
   mv_matr[15]=1.0f;

   switch( side ){
// only change the values without 0.0. They are well defined at the beginning of the function
   case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:  /* (s,t)=(-z,-y) */
//     -y*ey    -z*ex
       mv_matr[8] =-1.0;
       mv_matr[5] =-1.0;
       mv_matr[2] = -1.0;
       break;
   case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:  /* (s,t)=(+z,-y) */
//     -y*ey    +z*ex
       mv_matr[8] = 1.0;
       mv_matr[5] =-1.0;
       mv_matr[2] = 1.0;
       break;
   case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:  /* (s,t)=(+x,+z) */
//     +x*ex    +z*ey
       mv_matr[0] = 1.0;
       mv_matr[9] = 1.0;
       mv_matr[6] =-1.0;
       break;
   case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:  /* (s,t)=(+x,-z) */
//     +x*ex    -z*ey
       mv_matr[0] = 1.0;
       mv_matr[9] =-1.0;
       mv_matr[6] = 1.0;
       break;
   case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:  /* (s,t)=(+x,-y) */
//     +x*ex   -y*ey
       mv_matr[0] = 1.0;
       mv_matr[5] =-1.0;
       mv_matr[10] =-1.0;
       break;
   case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:  /* (s,t)=(-x,-y) */
//    -x*ex     -y*ey
       mv_matr[0] =-1.0;
       mv_matr[5] =-1.0;
       mv_matr[10] = 1.0;
       break;
   }

   if(cubemap1_id == -1) {
     cubemap1_id = glGenLists(1);
     glNewList(cubemap1_id, GL_COMPILE_AND_EXECUTE);
     glDisable(GL_LIGHT1);
     glMatrixMode( GL_PROJECTION );
     glLoadIdentity();
     glFrustum( -0.01f, +0.01f, -0.01f, +0.01f, +0.01f, +3.0f );
     glMatrixMode( GL_MODELVIEW );
     glLoadIdentity();
     glEndList();
   } else {
     //fprintf(stderr,"cubemap1 %i\n",cubemap1_id);
     glCallList(cubemap1_id);
   }
   glLoadMatrixf(mv_matr);

 if(cubemap_id == -1) {
   cubemap_id = glGenLists(1);
   glNewList(cubemap_id, GL_COMPILE_AND_EXECUTE);
   glPushMatrix();

   glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diff);
   glLightfv(GL_LIGHT0, GL_AMBIENT,  light_amb);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);

   /* light rects */
   glDisable(GL_LIGHTING);
   glDisable(GL_TEXTURE_2D);

   glColor4f(1.0,1.0,1.0,1.0);
   glBegin( GL_QUADS );
     glNormal3f( 0.0,0.0,-1.0 );
     glVertex3f(  0.18, 0.15, 1.0 );
     glVertex3f(  0.18, 0.76, 1.0 );
     glVertex3f( -0.18, 0.76, 1.0 );
     glVertex3f( -0.18, 0.15, 1.0 );
     glVertex3f( -0.18, -0.15, 1.0 );
     glVertex3f( -0.18, -0.76, 1.0 );
     glVertex3f(  0.18, -0.76, 1.0 );
     glVertex3f(  0.18, -0.15, 1.0 );
   glEnd();

   glColor4f(0.6,0.6,0.6,1.0);
   glBegin( GL_QUADS );
     glVertex3f(  0.20, 0.13, 1.001 );
     glVertex3f(  0.20, 0.78, 1.001 );
     glVertex3f( -0.20, 0.78, 1.001 );
     glVertex3f( -0.20, 0.13, 1.001 );
     glVertex3f( -0.20, -0.13, 1.001 );
     glVertex3f( -0.20, -0.78, 1.001 );
     glVertex3f(  0.20, -0.78, 1.001 );
     glVertex3f(  0.20, -0.13, 1.001 );
   glEnd();

   glColor4f(0.3,0.3,0.3,1.0);
   glBegin( GL_QUADS );
     glVertex3f(  0.28,-0.86, 1.002 );
     glVertex3f(  0.28, 0.86, 1.002 );
     glVertex3f( -0.28, 0.86, 1.002 );
     glVertex3f( -0.28,-0.86, 1.002 );
   glEnd();

   glColor4f(0.15,0.2,0.15,1.0);
   glBegin( GL_QUADS );
     glVertex3f(  0.38,-0.96, 1.004 );
     glVertex3f(  0.38, 0.96, 1.004 );
     glVertex3f( -0.38, 0.96, 1.004 );
     glVertex3f( -0.38,-0.96, 1.004 );
   glEnd();

   glEnable(GL_LIGHTING);
   glEnable(GL_TEXTURE_2D);

   glPopMatrix();
   glEndList();
 } else {
   //fprintf(stderr,"cubemap %i\n",cubemap_id);
   glCallList(cubemap_id);
 }
}

/***********************************************************************
 * Create cuberef maps for one ball (called from create_cuberef_maps)  *
 ***********************************************************************/

void create_cuberef_map(int ballnr, int texbind, VMvect cam_pos)
{
    int i, w, target = 0, level;
    int xpos, ypos;
    VMfloat d, ang, ang1, ang2;
    VMfloat th, ph, cam_FOV2, cam_FOV3;
    VMvect dvec, ballvec, right, up, cam_pos_;

    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, texbind);

    /* calc necessary detail level */

    cam_FOV3 = cam_FOV*M_PI/180.0/2.0;
    cam_FOV2=(2.0*180.0/M_PI*atan(tan(cam_FOV3)/win_width*win_height))*M_PI/180.0/2.0;

    th=(Xrot+Xrot_offs)/180.0*M_PI;
    ph=(Zrot+Zrot_offs)/180.0*M_PI;
    dvec  = vec_xyz(-sin(th)*sin(ph),-sin(th)*cos(ph),-cos(th));
    cam_pos_ = vec_diff( cam_pos, vec_scale(dvec,balls.ball[ballnr].d/2.0/sin(cam_FOV3)));
    right = vec_unit(vec_xyz(dvec.y,-dvec.x,0));
    up    = vec_cross(right,dvec);
    ballvec = vec_diff(balls.ball[ballnr].r,cam_pos_);
    d = vec_mul(ballvec,dvec);
    ang1  = atan2( vec_mul(ballvec, right), d);
    ang2  = atan2( vec_mul(ballvec, up), d);
    ang = d/vec_abs(ballvec);
    ang = (fabs(ang)<1.0)?acos(ang):0.0;

    if(fabs(ang1) < cam_FOV3 && fabs(ang2) < cam_FOV2) {
    //only draw, if the ball is in view and in quality only on near distance
        // level=log(d/0.2)/log(2.0)-1.0; //too complex math
        level=fabs(d);
        if (level<0) level=0;
        if (level>6) level=6;
        //fprintf(stderr,"%f %i\n",d,level);
        w=options_cuberef_res>>level;

        for(i=0;i<6;i++){
            xpos = (i%3)*options_cuberef_res;
            ypos = (i/3)*options_cuberef_res;
            glViewport( xpos, ypos, w, w);
            switch(i){
            case 0: target=GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
                    break;
            case 1: target=GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
                    break;
            case 2: target=GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
                    break;
            case 3: target=GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
                    break;
            case 4: target=GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
                    break;
            case 5: target=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
                    break;
            }
            DisplayFunc_cubemap( ballnr , target, cam_pos, w );
            glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_BASE_LEVEL, level);
            glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAX_LEVEL, level);
            glCopyTexSubImage2D(target, level, 0, 0, xpos, ypos, w, w );
        }
    }
}

/***********************************************************************
 *         Create the cuberef binds / textures for all balls           *
 ***********************************************************************/

void create_cuberef_maps(VMvect cam_pos)
{
    int i;
    glColorMask(1, 1, 1, 1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    for(i=0;i<balls.nr;i++) {
      if(balls.ball[i].in_game){
        //fprintf(stderr,"creating cubemaps for ball #%d\n",i);
        create_cuberef_map(i,cuberef_allballs_texbind[i],cam_pos);
      }
    }
}

/***********************************************************************
 *            Display the Tournament players on Screen                 *
 ***********************************************************************/

void Display_tournament_tree( struct TournamentState_ * ts )
{
    int i,j;
    static textObj * title[TOURNAMENT_ROUND_NUM]={ 0 };
    static textObj * bottom=0;
    char newTournText[TOURNAMENT_ROUND_NUM][200];

    //fprintf(stderr,"Display_tournament_tree: 1");
    for (i = 0;i<TOURNAMENT_ROUND_NUM;i++) {
      if( title[i] == 0 ){
        //title=textObj_new("Tournament",options_menu_fontname,32);
        sprintf(newTournText[i],localeText[59],i+1);
        //printf("%s\n",newTournText[i]);
        title[i]=textObj_new(newTournText[i],options_menu_fontname,32);
      }
    }
    if( bottom == 0 ){
        //bottom=textObj_new("<fire> to continue",options_menu_fontname,16);
        bottom=textObj_new(localeText[60],options_menu_fontname,16);
    }

    if(tourn_id == -1) {
      tourn_id = glGenLists(1);
      glNewList(tourn_id, GL_COMPILE_AND_EXECUTE);
      glDisable(GL_LIGHTING);
      glDisable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

      glPushMatrix();
      glLoadIdentity();
      glScalef(0.8,0.8,1.0);

      glColor4f(0.4,0.4,0.4,0.7);
      glDisable(GL_TEXTURE_2D);
      /* top line */
      glBegin(GL_QUAD_STRIP);
        glVertex3f(-1, 0.98, 0);
        glVertex3f( 1, 0.98, 0);
        glVertex3f(-1, 0.82, 0);
        glVertex3f( 1, 0.82, 0);
      glEnd();
      /* bottom line */
      glBegin(GL_QUAD_STRIP);
        glVertex3f(-1, -0.82, 0);
        glVertex3f( 1, -0.82, 0);
        glVertex3f(-1, -0.98, 0);
        glVertex3f( 1, -0.98, 0);
      glEnd();

      glColor4f(0.6,0.6,0.6,0.85);
      glBindTexture(GL_TEXTURE_2D,fblogotexbind);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glEnable(GL_TEXTURE_2D);
      glBegin(GL_QUAD_STRIP);
        glTexCoord2f(-0.3-0.15, 0.06-0.15);
        glVertex3f(-1, 0.8, 0);
        glTexCoord2f( 1.3-0.15, 0.06-0.15);
        glVertex3f( 1, 0.8, 0);
        glTexCoord2f(-0.3+0.15, 0.94+0.15);
        glVertex3f(-1,-0.8, 0);
        glTexCoord2f( 1.3+0.15, 0.94+0.15);
        glVertex3f( 1,-0.8, 0);
      glEnd();

      glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
      glEnable(GL_TEXTURE_2D);

      glPushMatrix();
        glTranslatef(0,0.9,0);
        glPushMatrix();
        glScalef(0.004,0.005,1.0);
        glEndList();
      } else {
        //fprintf(stderr,"tourn_id %i\n",tourn_id);
        glCallList(tourn_id);
      }
      textObj_draw_bound(title[ts->round_ind],HBOUND_CENTER,VBOUND_CENTER);
      glPopMatrix();
      glTranslatef(0,-1.8,0);
      glPushMatrix();
        glScalef(0.004,0.004,1.0);
        textObj_draw_bound(bottom,HBOUND_CENTER,VBOUND_CENTER);
      glPopMatrix();
    glPopMatrix();

    glTranslatef(-1.0,0.8,0);
    glTranslatef(0.5*2.0/ts->round_num,0,0);


    //fprintf(stderr,"Display_tournament_tree: 2");
    for(i=0;i<=ts->round_ind;i++){
        glPushMatrix();
        glTranslatef(0,-0.5*1.6/(VMfloat)(1<<(ts->round_num-i)),0);
        for(j=0;j<(1<<(ts->round_num-i-1));j++){
            //fprintf(stderr,"Display_tournament_tree: drawing player %s\n",ts->roster.player[ts->game[i][j].roster_player1].text->str);
            if(ts->roster.player[ts->game[i][j].roster_player1].text){
                glPushMatrix();
                glScalef(0.003,0.003,1.0);
                if (ts->game[i][j].winner==0) {
                   glColor3f(0.0,1.0,1.0);
                } else {
                   if(ts->game[i][j].winner==1) {
                      glColor3f(0.5,0.5,0.5);
                   } else {
                    glColor3f(1.0,1.0,1.0);
                   }
                }
                textObj_draw_bound(ts->roster.player[ts->game[i][j].roster_player1].text,HBOUND_CENTER,VBOUND_CENTER);
                glPopMatrix();
            }
            glTranslatef(0,-1.6/(VMfloat)(1<<(ts->round_num-i)),0);
            //fprintf(stderr,"Display_tournament_tree: drawing player %s\n",ts->roster.player[ts->game[i][j].roster_player2].text->str);
            if(ts->roster.player[ts->game[i][j].roster_player2].text){
                glPushMatrix();
                glScalef(0.003,0.003,1.0);
                if (ts->game[i][j].winner==1) {
                   glColor3f(0.0,1.0,1.0);
                } else {
                   if(ts->game[i][j].winner==0) {
                     glColor3f(0.5,0.5,0.5);
                   } else {
                     glColor3f(1.0,1.0,1.0);
                   }
                }
                textObj_draw_bound(ts->roster.player[ts->game[i][j].roster_player2].text,HBOUND_CENTER,VBOUND_CENTER);
                glPopMatrix();
            }
            glTranslatef(0,-1.6/(VMfloat)(1<<(ts->round_num-i)),0);
        }
        glPopMatrix();
        glTranslatef(2.0/ts->round_num,0,0);
    }

    glPopMatrix(); /* position here = glScalef(0.8,0.8,1.0) */

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

/***********************************************************************
 *                   The idle & displaying function                    *
 ***********************************************************************/

void DisplayFunc( void )
{

  char str[256];                    // for string operations and network countdown
  int i,j,k,m;                      // some loop variables
  int minballnr;                    // next ball for 9ball
  static int balls_were_moving=0;   // balls were moved just one step before
  static int first_time=1;
  int t_act,in_game = 0;
  static int dt;
  static int t_prev=-1;
  static int frametime_rest=0;
  static int dt_rest=0;
  static int count=0;
  static VMfloat dt_s_rest=0.0;
  VMfloat fact;
#ifdef USE_SOUND                    // some stuff for ball-sounds
  VMfloat bhitstrength=0.0;
  VMfloat whitstrength=0.0;
  VMfloat toffs=0.0;
  int index;
#endif
  int old_cueball_ind;              // cueball index just one move before
  int col;                          // actual color for 9ball
  int cue_ball = CUE_BALL_IND;      // actual cueball index

  VMfloat th,ph,cam_dist0;
  VMfloat znear=0.03;
  VMfloat zfar=15.0;
  VMfloat eye_offs, zeye;        //for stereo view
  VMfloat eye_offs0, eye_offs1;  //for stereo view

  GLfloat light_position[] = { 0.0, 0.0, 0.7, 1.0 };  // all the postions of the lights
  GLfloat light0_position[] = { 0.0, 0.7, 0.7, 1.0 };
  GLfloat light0_diff[]     = { 0.6, 0.6, 0.6, 1.0 };
  GLfloat light0_amb[]      = { 0.35, 0.35, 0.35, 1.0 };
  GLfloat light1_position[] = { 0.0, -0.7, 0.7, 1.0 };
  GLfloat light1_diff[]     = { 0.6, 0.6, 0.6, 1.0 };
  GLfloat light1_amb[]      = { 0.35, 0.35, 0.35, 1.0 };

  GLfloat cam_FOV_tan = tan(cam_FOV*M_PI/360.0); //for quicker computing of the cam_FOV inside this function
  myvec cam_pos;
  VMmatrix4 mv_matr;
  VMmatrix4 prj_matr;
  VMvect dpos,dpos1,actpos,centpos,right,up; // for the Lensflare
#ifndef WETAB
  VMvect bx, by, bz, p, p1, p2;  //for the Helpline Cross on the ball
#endif
  static GLfloat real_dist=0.0;
  static GLfloat rg_eye_dist=0.05;
  static int introxanimate = 0; //animate the intro (don't change this!!)
  static int introyanimate = 0; //dto.
  static GLfloat introblendxanimate = 0.0; //dto.
  static GLfloat introblendyanimate = 0.0; //dto.

#ifdef NETWORKING
   // Network Game
   network_game();
   //fprintf(stderr,"networking\n");
#endif

  count++;
  t_act=SDL_GetTicks();
  if (t_prev==-1) t_prev=t_act;
  dt += t_act-t_prev;
  dt_s_rest += (t_act-t_prev)/1000.0;
  t_prev = t_act;
  if(count==1){
    count=0;
    frametime_ms=dt;
    if( frametime_ms<1 ) frametime_ms=1;
    if( frametime_ms>frametime_ms_max ) frametime_ms=frametime_ms_max;
    dt=0;
    dt_rest+=frametime_ms;
  }

 //    fprintf(stderr,"dt=%d\n",dt);
  fact=pow(0.85,(VMfloat)frametime_ms/50.0);
  Xrot_offs *= fact;
  Zrot_offs *= fact;
  fact=pow(0.94,(VMfloat)frametime_ms/50.0);
  cam_dist = (cam_dist*fact)+(cam_dist_aim+vec_abs(balls.ball[CUE_BALL_IND].v)*0.4)*(1.0-fact);

  free_view_pos = vec_add( vec_scale( free_view_pos, fact ), vec_scale( free_view_pos_aim, 1.0-fact ) );

  while(dt_s_rest>0.0) {
         //fprintf(stderr,"dtsrest\n");
         /* assure constant time flow */
         first_time=0; /* to get into loop when balls not moving */
 #ifdef TIME_INTERPOLATE
         copy_balls(&balls,&g_lastballs);
 #endif

 #define TIMESTEP 0.01
         dt_s_rest-=TIMESTEP/0.75/g_motion_ratio;
         // fprintf(stderr,"g_motion_ratio=%f\n",g_motion_ratio);
         // fprintf(stderr,"dt_s_rest=%f\n",dt_s_rest);
         balls_moving = proceed_dt( &balls, &walls, TIMESTEP, player );
         if(balls_moving) {
          balls_were_moving = 1;
         }
 #ifdef USE_SOUND
         index=0;
         do{
           BM_get_balls_hit_strength_last_index( index++ ,&bhitstrength, &toffs );
           bhitstrength = 1.75 * (0.3 * bhitstrength / CUEBALL_MAXSPEED + 0.7 * bhitstrength*bhitstrength / CUEBALL_MAXSPEED / CUEBALL_MAXSPEED);
           if(bhitstrength!=0.0){
             if( toffs>TIMESTEP || toffs<0.0 ){
                fprintf(stderr,"Error: toffs>TIMESTEP || toffs<0.0\n");
                sys_exit(0);
             } /* else{
                fprintf(stderr,"toffs/TIMESTEP=%f\n",toffs/TIMESTEP);
             } */
             if(options_gamemode==options_gamemode_tournament && player[0].is_AI && player[1].is_AI) {
               //nosound
             } else {
               PlaySound_offs(&ball_ball_snd,options_snd_volume*((bhitstrength>1.0)?1.0:bhitstrength), SOUND_NULLOFFS-(TIMESTEP-toffs)*22050);
             }
           }
         } while(bhitstrength!=0.0);
         index=0;
         do{
           BM_get_walls_hit_strength_last_index( index++ ,&whitstrength, &toffs );
           whitstrength = 0.4 * (0.3 * whitstrength / CUEBALL_MAXSPEED + 0.7 * whitstrength*whitstrength / CUEBALL_MAXSPEED / CUEBALL_MAXSPEED);
           if(whitstrength!=0.0){
             if(options_gamemode==options_gamemode_tournament && player[0].is_AI && player[1].is_AI) {
              //nosound
             } else {
 #ifndef WETAB
              PlaySound_offs(&ball_wall_snd,options_snd_volume*((whitstrength>1.0)?1.0:whitstrength), SOUND_NULLOFFS-(TIMESTEP-toffs)*22050);
 #else
              PlaySound_offs(&ball_wall_snd,(options_snd_volume+1.0)*((whitstrength>1.0)?1.0:whitstrength), SOUND_NULLOFFS-(TIMESTEP-toffs)*22050);
 #endif
             }
           }
         } while(whitstrength!=0.0);
 #endif
     if (!balls_moving) break;
     }
     // Check for button pressed on the Hud
     if(hudbuttonpressed) {
       if ( button_anim > 0.0 ){
          button_anim-=(VMfloat)frametime_ms/120.0;
          if (button_anim<0.0) button_anim=0.0;
       } //anim end
       //check for up/down/back/next/shoot button click
       if(button_anim < 0.01 && !(options_gamemode==options_gamemode_tournament && tournament_state.wait_for_next_match) && !(player[act_player].is_net || player[act_player].is_AI) && !balls_moving) {
          button_anim = 0.3;
          switch(hudbuttonpressed) {
            case 1:
              zoom_in_out(-20); // zoom-
              break;
            case 2:
              zoom_in_out(+20); // zoom+
              break;
            case 3:
              queue_strength = strength01( queue_strength-0.01 ); // strength down
              break;
            case 4:
              queue_strength = strength01( queue_strength+0.01 ); // strength up
              break;
            case 5:
              Key(KSYM_DOWN,0); // cursor down
              break;
            case 6:
              Key(KSYM_UP,0); // cursor up
              break;
            case 7:
              Key(KSYM_RIGHT,0); // cursor right
              break;
            case 8:
              Key(KSYM_LEFT,0); // cursor left
              break;
          }
       }
     }
     //Check for button pressed on hud end
     if (dt_s_rest>0.0) {
      dt_s_rest=0.0; /* to move on if last move was completely in last simulation step */
     }
 #ifdef TIME_INTERPOLATE
     if((frametime_ms+frametime_rest)/10>0)
         g_frametime_laststep = (frametime_ms+frametime_rest)/10*10;
     g_frametime_fromlast = frametime_rest;
 #endif
     frametime_rest = (frametime_ms+frametime_rest) % 10;

     /*************************************************************
      * the following "if" is only proceed if no balls are moving *
      *************************************************************/

     if(!balls_moving && balls_were_moving ){
         /* allways a shot to be due when balls just stopped moving */
         g_shot_due=1;
         balls_were_moving=0;
         if(options_gamemode!=options_gamemode_training){
#ifdef NETWORKING
             old_actplayer = act_player; // save the state of the actual player for network game
#endif
             evaluate_last_move( player, &act_player, &balls, &queue_view, &Xque );
#ifdef NETWORKING
             if(old_actplayer != act_player) {
                // change the network player
                if(active_net_timer!=NULL) {
                   netorder = 1;
                }
             }
#endif
             if(!tournament_state.wait_for_next_match && options_gamemode==options_gamemode_tournament && (player[0].winner || player[1].winner)) {
               tournament_evaluate_last_match( &tournament_state );
               tournament_state.wait_for_next_match=1;
             }
         } else {
             player[act_player].place_cue_ball=1;
             /* find a ball still in game */
             old_cueball_ind=CUE_BALL_IND;
             while(!balls.ball[CUE_BALL_IND].in_game){
                 CUE_BALL_IND++;
                 if(CUE_BALL_IND==balls.nr) CUE_BALL_IND=0;
                 if(CUE_BALL_IND==old_cueball_ind) break;
             }
             for(i=0;i<balls.nr;i++) {
               in_game += balls.ball[i].in_game;
             }
             if(!in_game) { //no balls in game, start a new training
              restart_game();
             }
         }
         all_balls_free_place(&balls); // no balls should overlap

         for(i=0;i<2;i++) { // score text
           switch(gametype){
             case GAME_8BALL:
                 str[0]= '0';
                 str[1] = 0;
                 break;
             case GAME_9BALL:
                 minballnr=15;
                 for(j=0;j<balls.nr;j++){
                   if(balls.ball[j].nr<minballnr && balls.ball[j].nr!=0 && balls.ball[j].in_game)
                       minballnr=balls.ball[j].nr;
                 }
                 player[i].next_9ball = minballnr;
                 // next: %d for the localeText
                 sprintf( str, localeText[176], minballnr );
                 if(minballnr == 15) {
                   str[0]=0;
                   }
                 break;
             case GAME_CARAMBOL:
                 sprintf( str, "%+04d", player[i].score );
                 break;
             case GAME_SNOOKER:
                 // col, red, yellow, green, brown, blue, pink, black for localeText
                 snooker_color(str,abs(player[i].score),act_player,i);
                 break;
           }
           textObj_setText( player[i].score_text, str );
         }
         // after shoot switch back to freeview if set
         if(options_auto_freemove && !queue_view && !balls_moving && !(player[act_player].is_net || player[act_player].is_AI)) {
               if(options_birdview_on) {
                  birdview();
               } else {
                  toggle_queue_view();
               }
            }

         // bird-view for AI or net player
         if(options_ai_birdview) {
           if((player[act_player].is_AI || player[act_player].is_net) && !(player[0].winner || player[1].winner) && !options_birdview_on) {
             old_birdview_ai = 1;
             birdview();
           } else if (!player[act_player].is_AI && !player[act_player].is_net && options_birdview_on && old_birdview_ai) {
             old_birdview_ai = queue_view;
             queue_view=0;
             birdview();
             queue_view=old_birdview_ai;
             old_birdview_ai = 0;
           }
         }

         // unset mouseshoot
         if(control__active && options_control_kind) {
            control_unset(&control__mouse_shoot);
         }
     }

     /******************************************
      * only called if no balls are moving end *
      ******************************************/

     if(g_shot_due && !( options_gamemode==options_gamemode_tournament && (tournament_state.wait_for_next_match || tournament_state.tournament_over))) {
         g_shot_due=0;
         if( player[act_player].is_AI && !(player[act_player].winner || player[(act_player+1)%2].winner) ){
             do_computer_move(1);
         }
     }
     if ( queue_anim > 0.0 ){
         queue_anim-=(VMfloat)frametime_ms/120.0*g_motion_ratio;
         if (queue_anim<0.0) queue_anim=0.0;
         queue_offs=0.16*queue_offs_func((30.0-queue_anim)/30.0);
         if( queue_anim==0.0 ){
             queue_shot();
             queue_offs=0.06;
         }
     }

   if( old_queue_view==1 && queue_view==0  ) { /* this is sloppy and ugly */
       /* set free_view_pos to actual view */
       th=Xrot/180.0*M_PI;
       ph=Zrot/180.0*M_PI;
       free_view_pos_aim = vec_scale(vec_xyz(sin(th)*sin(ph),sin(th)*cos(ph),cos(th)), cam_dist);
       free_view_pos_aim = vec_add( free_view_pos_aim, CUE_BALL_XYPOS );
       free_view_pos = free_view_pos_aim ;
   }
   old_queue_view=queue_view;

   if(!FREE_VIEW) {
       th=(Xrot+Xrot_offs)/180.0*M_PI;
       ph=(Zrot+Zrot_offs)/180.0*M_PI;
       cam_pos=vec_scale(vec_xyz(sin(th)*sin(ph),sin(th)*cos(ph),cos(th)), real_dist);
       cam_pos=vec_add(cam_pos,balls.ball[cue_ball].r);
   } else {
       cam_pos=free_view_pos;
   }

   if(options_cuberef) {
       create_cuberef_maps(cam_pos);
   }

   // Begin displaying from here

   glViewport( 0, 0, win_width, win_height);

#ifdef TIME_INTERPOLATE
   interpolate_balls( &g_lastballs, &balls, &g_drawballs, (VMfloat)g_frametime_fromlast/(VMfloat)g_frametime_laststep );
#endif

   if(!options_positional_light){
   // only set if direct light. for postional light is the init on top of the function
       light_position[3]=0.0;
       light0_position[3]=0.0;
       light1_position[3]=0.0;
   }

   if(!options_deco){ //fog only without walls and so on
     if(!FREE_VIEW){
       glFogf (GL_FOG_START, (cam_dist/2.0>cam_dist-1.0) ? cam_dist/2.0 : cam_dist-1.0 );
       glFogf (GL_FOG_END, cam_dist+6.0);
     } else {
       cam_dist0 = vec_abs(cam_pos);
       glFogf (GL_FOG_START, (cam_dist0/2.0>cam_dist0-1.0) ? cam_dist0/2.0 : cam_dist0-1.0 );
       glFogf (GL_FOG_END, cam_dist0+6.0);
     }
   }

   real_dist = cam_dist;

   if(options_rgstereo_on) {
       glColorMask(1, 1, 1, 1);
       glClear( GL_COLOR_BUFFER_BIT );
   }
   // max. two loops for stereo view
   for(i=0;i<=options_rgstereo_on;i++) {

   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();

   if (options_rgstereo_on){
       zeye = (VMfloat)win_width/2.0/scr_dpi*0.025/cam_FOV_tan;
       eye_offs = rg_eye_dist/24.0*znear/zeye; // ###TODO### Slider for eye_offs inside the options
       glLoadIdentity();
       switch(i) {
         case 0:
           eye_offs0 = 0.0;
           if (options_rgaim == 0) eye_offs0 = -eye_offs;
           if (options_rgaim == 1) eye_offs0 = -2.0*eye_offs;
           if (options_rgaim == 2) eye_offs0 = 0.0;
           glColorMask(1, 0, 0, 1);
           glMatrixMode( GL_PROJECTION );
           glFrustum( -znear*cam_FOV_tan+eye_offs0, znear*cam_FOV_tan+eye_offs0,
                     -znear*cam_FOV_tan*(VMfloat)win_height/(VMfloat)win_width,
                     +znear*cam_FOV_tan*(VMfloat)win_height/(VMfloat)win_width, znear, zfar);
           glMatrixMode( GL_MODELVIEW );
           glTranslatef( eye_offs0/znear*zeye, 0.0, 0.0 );
           break;
         case 1:
           eye_offs1 = 0.0;
           if (options_rgaim == 0) eye_offs1 = +eye_offs;
           if (options_rgaim == 1) eye_offs1 = 0.0;
           if (options_rgaim == 2) eye_offs1 = +2.0*eye_offs;
           glColorMask(0, 1, 1, 1);
           glMatrixMode( GL_PROJECTION );
           glFrustum( -znear*cam_FOV_tan+eye_offs1, znear*cam_FOV_tan+eye_offs1,
                     -znear*cam_FOV_tan*(VMfloat)win_height/(VMfloat)win_width,
                     +znear*cam_FOV_tan*(VMfloat)win_height/(VMfloat)win_width, znear, zfar);
           glMatrixMode( GL_MODELVIEW );
           glTranslatef( eye_offs1/znear*zeye, 0.0, 0.0 );
           break;
       }
   } else {
       glMatrixMode( GL_PROJECTION );
       glFrustum( -znear*cam_FOV_tan, znear*cam_FOV_tan,
                  -znear*cam_FOV_tan*(VMfloat)win_height/(VMfloat)win_width,
                  +znear*cam_FOV_tan*(VMfloat)win_height/(VMfloat)win_width, znear, zfar);
/*       {
           // for Debugging the Matrix
           GLfloat m[16];
           glGetFloatv(GL_PROJECTION_MATRIX,m);
           printf("\nmatrix=\n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n",
                  m[0],m[4],m[8],m[12],
                  m[1],m[5],m[9],m[13],
                  m[2],m[6],m[10],m[14],
                  m[3],m[7],m[11],m[15]
                 );


       }*/
       glMatrixMode( GL_MODELVIEW );
     }

   if(FREE_VIEW) {
       glRotatef(Xrot+Xrot_offs, 1.0, 0.0, 0.0);
       glRotatef(Yrot+Yrot_offs, 0.0, 1.0, 0.0);
       glRotatef(Zrot+Zrot_offs, 0.0, 0.0, 1.0);
       glTranslatef( -free_view_pos.x, -free_view_pos.y, -free_view_pos.z );
       glPushMatrix();
   } else {
       glTranslatef( 0.0, 0.0, -real_dist );
       glPushMatrix();
       glRotatef(Xrot+Xrot_offs, 1.0, 0.0, 0.0);
       glRotatef(Yrot+Yrot_offs, 0.0, 1.0, 0.0);
       glRotatef(Zrot+Zrot_offs, 0.0, 0.0, 1.0);
       glTranslatef( -balls.ball[cue_ball].r.x, -balls.ball[cue_ball].r.y, -balls.ball[cue_ball].r.z );
   }
   glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diff);
   glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_amb);
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
   glEnable(GL_LIGHT1);
   glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diff);
   glLightfv(GL_LIGHT1, GL_AMBIENT,  light1_amb);
   glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // Tron Special Gamemode - and for debugging too.... ;-))
   if(options_tronmode) {
     glLineWidth (1.5);
     glEnable (GL_LINE_SMOOTH);
     glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
     glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
   } // End Tron Mode

   glCallList(table_obj); // draw table

   glPushMatrix();
   glCallList(floor_obj); // draw floor
   if(options_deco) {     // draw room if option on
     glCallList(wall1_2_obj);  // room
     glRotatef(90.0,0.0,0.0,1.1);
     if(Zrot>50 && Zrot<320) {
       //fprintf(stderr,"Zrot: %f\n",Zrot);
       // draw the window with the skyline behind, problem......
       glCallList(wall3_obj);
     }
     glCallList(wall4_c_obj);
   } else {
   	 glRotatef(-90.0,0.0,0.0,1.1);
   }

   if(options_tronmode) { //switch Tron Gamemode off
     glDisable (GL_LINE_SMOOTH);
     glPolygonMode(GL_FRONT,GL_FILL);
   } //end tron-mode off

   // draw some meshes (furniture)
   if(options_furniture) {
   	 if(!options_tronmode) {
   	   glCallList(carpet_obj); // first must draw the carpet
     } else {
 			   glMaterialfv(GL_FRONT,GL_AMBIENT, ambient_torus);
 			   glMaterialfv(GL_FRONT,GL_DIFFUSE, diffuse_torus);
 			   glMaterialfv(GL_FRONT,GL_SPECULAR, specular_torus);
 			   glMaterialf (GL_FRONT, GL_SHININESS, 51);
     }
     glTranslatef(3.5,-4.0,0.65);
     glRotatef(180.0,0.0,0.0,1.0);
     glScalef(1.2,1.2,1.2);
     glCullFace(GL_FRONT);  // This is a must for blender export models
     glPolygonMode(GL_BACK,GL_FILL); // fill the back of the polygons
     if(Zrot>180.0) {
     	 glPushMatrix();
       glScalef(0.5,1.0,0.7);
       glCallList(bartable_id); //table window
       glPopMatrix();
     }
   	 if(options_tronmode) {
       glDisable(GL_TEXTURE_2D);
   	 }
     glTranslatef(2.0,0.0,0.0);
     if(Zrot>180.0) {
       glCallList(sofa_id); //sofa 1
     }
     glTranslatef(2.5,0.0,0.0);
     if(Zrot>180.0) {
   	   glCallList(sofa_id); //sofa 2
     }
     glTranslatef(1.0,0.0,0.0);
     if(Zrot>180.0) {
     	 glPushMatrix();
       glScalef(0.5,1.0,0.7);
       glCallList(bartable_id); //table sofa 2
       glPopMatrix();
     }

     glTranslatef(-6.0,-6.0,0.2);
     glScalef(0.7,0.7,0.7);
     if(Zrot<190.0) {
   	   glCallList(chair_id); //chair 1
     }
     glRotatef(65.0,0.0,0.0,1.0);
     glTranslatef(1.0,-3.0,0.0);
     if(Zrot<190.0) {
   	   glCallList(chair_id); //chair 2
     }
     glRotatef(115.0,0.0,0.0,1.0);
     glTranslatef(1.5,0.0,0.2);
     glScalef(1.1,1.1,1.7);
     if(Zrot<190.0) {
   	   glCallList(bartable_id); //bar table
     }
     glTranslatef(-5.5,0.6,-0.2);
     glScalef(1.0,1.3,1.0);
     if(Zrot<190.0 || Zrot>320.0) {
    	  if(options_tronmode) {
        glDisable(GL_TEXTURE_2D);
    	  }
   	   glCallList(sofa_id); //sofa 3
     }
     glRotatef(90.0,0.0,0.0,1.0);
     glTranslatef(-4.0,2.4,-0.43);
     glScalef(0.6,0.4,0.45);

     if(Zrot<90.0 || Zrot>280.0) {
    	 if(options_tronmode) {
    	  glDisable(GL_TEXTURE_2D);
    	 }
   	   glCallList(camin_id); //Camin
     }
     if(!options_birdview_on) {
       glPopMatrix();
       glPushMatrix();
       glScalef(0.1,0.7,0.05);
       glTranslatef(0.0,0.0,20.0);
       glCallList(lamp_id);
     }
     glCullFace(GL_BACK);   // This is a must for blender export models
     glPolygonMode(GL_BACK,GL_LINE);  // fill the back of the polygons
   } // end furniture
   glPopMatrix();

   /* draw balls with reflections and shadows */
#ifdef TIME_INTERPOLATE
    if(options_cuberef) {
        draw_balls(g_drawballs,cam_pos,cam_FOV,win_width,spheretexbind,lightpos,lightnr, cuberef_allballs_texbind);
    } else {
        draw_balls(g_drawballs,cam_pos,cam_FOV,win_width,spheretexbind,lightpos,lightnr, (unsigned int *)0);
    }
#else
    if(options_cuberef) {
        draw_balls(balls,cam_pos,cam_FOV,win_width,spheretexbind,lightpos,lightnr, cuberef_allballs_texbind);
    } else {
        draw_balls(balls,cam_pos,cam_FOV,win_width,spheretexbind,lightpos,lightnr, (unsigned int *)0);
    }

#endif
   if( !queue_view && !balls_moving ) {  /* draw queue */
       draw_queue( balls.ball[cue_ball].r, Xque, Zque, queue_offs, queue_point_x, queue_point_y, spheretexbind, lightpos, lightnr );
   }

   if (player[act_player].place_cue_ball && !balls_moving) {
//      glMaterialfv(GL_FRONT, GL_DIFFUSE, col_shad);
        glDepthMask (GL_FALSE);
        glEnable(GL_BLEND);
        glDisable (GL_LIGHTING);
        glBlendFunc (GL_ONE, GL_ONE);
        glColor3f(0.5,0.5,0.5);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,placecueballtexbind);
#define SH_SZ 0.087
        glBegin( GL_QUADS );
          glNormal3f( 0.0,0.0,1.0 );
          glTexCoord2f(0.0,1.0);
          glVertex3f( balls.ball[cue_ball].r.x-SH_SZ, balls.ball[cue_ball].r.y+SH_SZ, balls.ball[cue_ball].r.z-balls.ball[cue_ball].d/2.02 );
          glTexCoord2f(1.0,1.0);
          glVertex3f( balls.ball[cue_ball].r.x+SH_SZ, balls.ball[cue_ball].r.y+SH_SZ, balls.ball[cue_ball].r.z-balls.ball[cue_ball].d/2.02 );
          glTexCoord2f(1.0,0.0);
          glVertex3f( balls.ball[cue_ball].r.x+SH_SZ, balls.ball[cue_ball].r.y-SH_SZ, balls.ball[cue_ball].r.z-balls.ball[cue_ball].d/2.02 );
          glTexCoord2f(0.0,0.0);
          glVertex3f( balls.ball[cue_ball].r.x-SH_SZ, balls.ball[cue_ball].r.y-SH_SZ, balls.ball[cue_ball].r.z-balls.ball[cue_ball].d/2.02 );
        glEnd();
        glDisable(GL_BLEND);
        glDepthMask (GL_TRUE);
#undef SH_SZ
        glEnable (GL_LIGHTING);
   }

   if( options_balltrace ) {
     glDisable(GL_TEXTURE_2D);
     glDisable(GL_LIGHTING);
#ifdef WETAB_ALIASING
     if(options_antialiasing) {
      glLineWidth(1.5);
      glEnable(GL_LINE_SMOOTH);
      glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);
     }
#endif
     for(m=0;m<balls.nr;m++) {
        draw_ballpath(&balls.ball[m]);
     }
#ifdef WETAB_ALIASING
    if(options_antialiasing) {
      glDisable(GL_LINE_SMOOTH);
      glDisable(GL_BLEND);
    }
#endif
     glEnable(GL_LIGHTING);
     glEnable(GL_TEXTURE_2D);
   }

   if( (player[0].winner || player[1].winner) ) {
       control_unset(&control__cue_butt_updown);
       control_unset(&control__english);
       control_unset(&control__place_cue_ball);
       control_unset(&control__fov);
       control_unset(&control__mouse_shoot);
       if(options_3D_winnertext){
           if(options_gamemode==options_gamemode_tournament && tournament_state.overall_winner>=0) {
              draw_3D_winner_tourn_text();
           } else {
              draw_3D_winner_text();
           }
       }
#ifdef NETWORKING
       if(active_net_game) { // end an active netgame
        close_listener();
       }
#endif
   }

   if( options_lensflare ) {
       glDepthMask (GL_FALSE);
       glEnable(GL_BLEND);
       glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

       for(m=0;m<1/*lightnr*/;m++){
           glBlendFunc ( GL_ONE, GL_ONE );

           glBindTexture(GL_TEXTURE_2D,blendetexbind);
           glGetFloatv(GL_MODELVIEW_MATRIX,mv_matr.m);
           glGetFloatv(GL_PROJECTION_MATRIX,prj_matr.m);

           dpos1    = matr4_rdot( mv_matr, vec_xyz(0,0,0.77) );
           centpos = vec_xyz(0,0,-0.5);
           dpos    = vec_unit(vec_diff( dpos1, centpos ));

           glMatrixMode(GL_MODELVIEW);
           glPushMatrix();
           glLoadIdentity();
           glDisable(GL_LIGHTING);

           for(k=0;k<3;k++) {
             for(j=-1;j<20;j++){
               VMfloat zact;
               if(!options_rgstereo_on){
                   glColor3f(1.0*(VMfloat)(k%3!=1), 1.0*(VMfloat)(k%3!=2), 1.0*(VMfloat)(k%3!=0));
               } else {
                   glColor3f(0.5+0.25*(VMfloat)(k%3), 0.5+0.25*(VMfloat)(k%3), 0.5+0.25*(VMfloat)(k%3));
               }

               if(j==-1 && k==0){
                   glColor3f(1.0,1.0,1.0);
                   glBindTexture(GL_TEXTURE_2D,lightflaretexbind);
                   zact = dpos1.z;
                   actpos = dpos1;
                   right = vec_xyz(0.02/0.4*(0.5-zact),0,0);
                   up    = vec_xyz(0,0.02/0.4*(0.5-zact),0);
               } else if(j>=0 && j<10){
                   glBindTexture(GL_TEXTURE_2D,blendetexbind);
                   zact = 0.32-0.25*exp((j-3)+k*1.4345);
                   actpos = vec_add( centpos , vec_scale(dpos,zact/dpos.z) );
                   right = vec_xyz(0.008*(1.0-k*0.23)/0.4*(0.5-zact),0,0);
                   up    = vec_xyz(0,0.008*(1.0-k*0.23)/0.4*(0.5-zact),0);
               } else {
                   glBindTexture(GL_TEXTURE_2D,blendetexbind);
                   zact = 0.282-0.127*exp((j-3-10)+k*1.2453);
                   actpos = vec_add( centpos , vec_scale(dpos,zact/dpos.z) );
                   right = vec_xyz(0.003*(1.0-k*0.23)/0.4*(0.5-zact),0,0);
                   up    = vec_xyz(0,0.003*(1.0-k*0.23)/0.4*(0.5-zact),0);
               }
               glBegin( GL_QUADS );
                 glTexCoord2f(0.0,0.0);
                 glVertex3f( actpos.x+up.x-right.x, actpos.y+up.y-right.y, actpos.z+up.z-right.z );
                 glTexCoord2f(1.0,0.0);
                 glVertex3f( actpos.x+up.x+right.x, actpos.y+up.y+right.y, actpos.z+up.z+right.z );
                 glTexCoord2f(1.0,1.0);
                 glVertex3f( actpos.x-up.x+right.x, actpos.y-up.y+right.y, actpos.z-up.z+right.z );
                 glTexCoord2f(0.0,1.0);
                 glVertex3f( actpos.x-up.x-right.x, actpos.y-up.y-right.y, actpos.z-up.z-right.z );
               glEnd();
             }
           }
           glPushMatrix();
           glTranslatef( 0,0,-0.5 );
           glScalef(0.0005,0.0005,1.0);
           glPopMatrix();
           glPopMatrix();
           glMatrixMode(GL_MODELVIEW);
       }
       glDisable(GL_BLEND);
       glDepthMask (GL_TRUE);
   }

      /* HUD stuff */
       glDisable(GL_DEPTH_TEST);
#ifndef WETAB
       // the WeTab is to slow for this !!
       if((vline_on || (control__active && control__english)) && queue_view && !balls_moving){
           bz=vec_unit(vec_diff(cam_pos,balls.ball[cue_ball].r));
           bx=vec_unit(vec_xyz(-bz.y, bz.x, 0));
           by=vec_cross(bz,bx);
           p=vec_add(vec_scale(bx,queue_point_x),vec_scale(by,-queue_point_y));
           p=vec_add(p,balls.ball[cue_ball].r);
           glEnable(GL_LINE_STIPPLE);
           glLineStipple(1, 0x3333);
             glBegin( GL_LINES );
               p1=vec_add(p,vec_scale(bx,-0.01));
               p2=vec_add(p,vec_scale(bx,+0.01));
               glVertex3f( p.x, p.y, p.z );
               glVertex3f( p1.x, p1.y, p1.z );
               glVertex3f( p.x, p.y, p.z );
               glVertex3f( p2.x, p2.y, p2.z );
               p1=vec_add(p,vec_scale(by,-0.01));
               p2=vec_add(p,vec_scale(by,+0.01));
               glVertex3f( p.x, p.y, p.z );
               glVertex3f( p1.x, p1.y, p1.z );
               glVertex3f( p.x, p.y, p.z );
               glVertex3f( p2.x, p2.y, p2.z );
             glEnd();
           glDisable(GL_LINE_STIPPLE);
       }
#endif
     if(hudstuff_id == -1) {
       hudstuff_id = glGenLists(1);
       glNewList(hudstuff_id, GL_COMPILE_AND_EXECUTE);
       glMatrixMode( GL_TEXTURE );
       glPushMatrix();

       glLoadIdentity();

       glMatrixMode( GL_PROJECTION );
       glPushMatrix();
       glLoadIdentity();

       glMatrixMode( GL_MODELVIEW );
       glPushMatrix();
       glLoadIdentity();

       glColor3f(1.0,1.0,1.0);
       glDisable(GL_LIGHTING);
       glDisable(GL_TEXTURE_2D);
       glDisable(GL_DEPTH_TEST);
       glDisable(GL_TEXTURE_GEN_S);
       glDisable(GL_TEXTURE_GEN_T);

       glEnable(GL_TEXTURE_2D);
       glEnable(GL_BLEND);
       glBlendFunc(GL_ONE,GL_ONE);
       glEndList();
     } else {
       //fprintf(stderr,"hudstuff_id %i\n",hudstuff_id);
       glCallList(hudstuff_id);
     }
       // draw statusline
       if(!(player[0].winner || player[1].winner)) {
         drawstatustext(win_width, win_height);
         }
       /* act player */
       glPushMatrix();
       glTranslatef(-0.94,-0.94,-1.0);
       glScalef(2.0/win_width,2.0/win_height,1.0);
       if( player[act_player].text != 0 ){
           textObj_draw( player[act_player].text );
       }
       glTranslatef(0,30,0);
       switch(gametype) {
        case GAME_8BALL:
           switch(player[act_player].half_full){
              case BALL_HALF:
                glBindTexture(GL_TEXTURE_2D,halfsymboltexbind);
                break;
              case BALL_FULL:
                glBindTexture(GL_TEXTURE_2D,fullsymboltexbind);
                break;
              case BALL_ANY:
                glBindTexture(GL_TEXTURE_2D,fullhalfsymboltexbind);
                break;
           }
           myRect2D_texture();
           break;
        case GAME_9BALL:
           if( player[act_player].next_9ball != 8 ){
               col = options_col_ball[player[act_player].next_9ball];
           } else {
               col = 0x888888;
           }
           glColor3ub( col>>16, (col>>8)&0xFF, col&0xFF );
           textObj_draw( player[act_player].score_text );
           break;
        case GAME_SNOOKER:
        case GAME_CARAMBOL:
           textObj_draw( player[act_player].score_text );
           break;
       }
       glPopMatrix();
       /* 2nd player */
       glPushMatrix();
       glColor3f(0.0,0.0,1.0);
       glTranslatef(0.94,-0.94,-1.0);
       glScalef(2.0/win_width,2.0/win_height,1.0);
       if( player[act_player?0:1].text != 0 ){
         textObj_draw_bound( player[act_player?0:1].text, HBOUND_RIGHT, VBOUND_BOTTOM );
       }
       if (gametype==GAME_SNOOKER || gametype==GAME_CARAMBOL){
         glTranslatef(0,30,0);
    	    textObj_draw_bound( player[act_player?0:1].score_text, HBOUND_RIGHT, VBOUND_BOTTOM );
       }
       glPopMatrix();

       if(show_disc) { // save config was choosen
         glPushMatrix();
         glColor3f(1.0,1.0,1.0);
         glTranslatef(0.03,-0.94,0.0);
         glScalef(2.0/win_width,2.0/win_height,0.0);
         glBindTexture(GL_TEXTURE_2D,discbind); //disc png texture
         myRect2D_texture();
         glPopMatrix();
       }

       // now, set the menu bar on left and right
       if(!player[act_player].is_net && !balls_moving) {
       glPushMatrix();
       glColor3f(1.0,1.0,1.0);
       glScalef(2.0/win_width,2.0/win_height,0.0);
#ifndef WETAB
 #define MENUSTEP 5
#else
 #define MENUSTEP 20
#endif
       if(leftmenu == 1) { leftcount += MENUSTEP; }
       if(leftmenu == 3) { leftcount -= MENUSTEP; }
       if(leftcount <0) { leftcount = 0; leftmenu = 0; }
       if(leftcount > MENUCOUNT) { leftcount = MENUCOUNT; leftmenu = 2; }
       glTranslatef(-((VMfloat)win_width/2+180-leftcount),-(VMfloat)win_height/2+170,0.0);
       if( options_gamemode == options_gamemode_training ) {
          if(mleft_id == -1) {
            mleft_id = glGenLists(1);
            glNewList(mleft_id, GL_COMPILE_AND_EXECUTE);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D,mleftbind); // left menu bar training
            glBegin(GL_QUADS);
              glTexCoord2f(0,1);
              glVertex3f(0,0,0);
              glTexCoord2f(0,0);
              glVertex3f(0,535,0);
              glTexCoord2f(1,0);
              glVertex3f(215,535,0);
              glTexCoord2f(1,1);
              glVertex3f(215,0,0);
            glEnd();
            glPopMatrix();
            glEndList();
          } else {
            glCallList(mleft_id);
          }
       } else {
        if(mleftnormal_id == -1) {
          mleftnormal_id = glGenLists(1);
          glNewList(mleftnormal_id, GL_COMPILE_AND_EXECUTE);
          glEnable(GL_TEXTURE_2D);
          glEnable(GL_BLEND);
          glBindTexture(GL_TEXTURE_2D,mleftnormalbind); // left menu bar normal
          glBegin(GL_QUADS);
            glTexCoord2f(0,1);
            glVertex3f(0,0,0);
            glTexCoord2f(0,0);
            glVertex3f(0,535,0);
            glTexCoord2f(1,0);
            glVertex3f(215,535,0);
            glTexCoord2f(1,1);
            glVertex3f(215,0,0);
          glEnd();
          glPopMatrix();
          glEndList();
        } else {
          glCallList(mleftnormal_id);
        }
       }
       glPushMatrix();
       glScalef(2.0/win_width,2.0/win_height,0.0);
       if(rightmenu == 1) { rightcount += MENUSTEP; }
       if(rightmenu == 3) { rightcount -= MENUSTEP; }
#undef MENUSTEP
       if(rightcount <0) { rightcount = 0; rightmenu = 0; }
       if(rightcount > MENUCOUNT) { rightcount = MENUCOUNT; rightmenu = 2; }
       glTranslatef((VMfloat)win_width/2-35-rightcount,-(VMfloat)win_height/2+170,0.0);
       if(mright_id == -1) {
         mright_id = glGenLists(1);
         glNewList(mright_id, GL_COMPILE_AND_EXECUTE);
         glEnable(GL_TEXTURE_2D);
         glEnable(GL_BLEND);
         glBindTexture(GL_TEXTURE_2D,mrightbind); // right menu bar
         glBegin(GL_QUADS);
           glTexCoord2f(0,1);
           glVertex3f(0,0,0);
           glTexCoord2f(0,0);
           glVertex3f(0,535,0);
           glTexCoord2f(1,0);
           glVertex3f(215,535,0);
           glTexCoord2f(1,1);
           glVertex3f(215,0,0);
         glEnd();
         glPopMatrix();
         glEndList();
       } else {
         glCallList(mright_id);
       }
       } //End Menu bar left and right

       glDisable(GL_LIGHTING);
       glDisable(GL_TEXTURE_2D);
       glEnable(GL_BLEND);
       glBlendFunc(GL_ONE,GL_ONE);

       /* strength bar */
       if(!(options_gamemode==options_gamemode_tournament && tournament_state.wait_for_next_match) && !player[act_player].is_AI && !balls_moving) {
       /* disable strength bar if tournament window, player is net or ai is active and no balls where moving */
    	   myRect2D( -0.5, -0.755, 0.5, -0.675, 0.25, 0.2 );
           myRect2D( -0.5, -0.745,-0.5+queue_strength, -0.685, 0.0, 0.3 );
           glPushMatrix();
           glTranslatef(-0.5,-0.755, 0.0);
           glScalef(2.0/win_width,2.0/win_height,1.0);
           glGetDoublev(GL_MODELVIEW_MATRIX,b_modelview); //get the whole world OpenGL offset of the strength bar begin
           glGetDoublev(GL_PROJECTION_MATRIX,b_projection);
           glGetIntegerv(GL_VIEWPORT,b_viewport);
           gluProject(-0.5,-0.755,0.0,b_modelview,b_projection,b_viewport,&x_strengthbar,&y_strengthbar,&z_dummy);
           glPopMatrix();
           glPushMatrix();
           glTranslatef(0.5,-0.675, 0.0);
           glScalef(2.0/win_width,2.0/win_height,1.0);
           glGetDoublev(GL_MODELVIEW_MATRIX,b_modelview); //get the whole world OpenGL offset of the strength bar end
           glGetDoublev(GL_PROJECTION_MATRIX,b_projection);
           glGetIntegerv(GL_VIEWPORT,b_viewport);
           gluProject(0.5,-0.675,0.0,b_modelview,b_projection,b_viewport,&x_strengthbar_end,&y_strengthbar_end,&z_dummy);
           glPopMatrix();
           //percent on the strength bar
           sprintf(stbar_text,"%03u%%",(unsigned int)(queue_strength*100));
           textObj_setText(stbar_text_obj,stbar_text);
           glEnable(GL_TEXTURE_2D);
           glPushMatrix();
           glTranslatef(-0.02,-0.745, 0.0);
           glScalef(2.0/win_width,2.0/win_height,1.0);
           glColor3f(0.8,0.8,0.8);
           textObj_draw(stbar_text_obj);    //Draw the strength adjustment percent
           glPopMatrix();
           //Show the control Buttons on the Screen ?
           if(options_show_buttons && !player[act_player].is_net) {
             glPushMatrix();
             glColor3f(1.0,1.0,1.0);   // Begin draw the buttons
             glTranslatef(-0.72,-0.72,0.0);
             glScalef(2.0/win_width,2.0/win_height,1.0);
             glGetDoublev(GL_MODELVIEW_MATRIX,b_modelview); //get the whole world OpenGL offset of the buttons
             glGetDoublev(GL_PROJECTION_MATRIX,b_projection);
             glGetIntegerv(GL_VIEWPORT,b_viewport);
             gluProject(-0.72,-0.72,0.0,b_modelview,b_projection,b_viewport,&x_upbutton,&y_upbutton,&z_dummy);
             //fprintf(stderr,"up button x %f y %f\n",x_upbutton,y_upbutton);
             glBindTexture(GL_TEXTURE_2D,utexbind); //Up button - zoom+
             myRect2D_texture();

             glTranslatef(0.0,-40.0,0.0);
             glGetDoublev(GL_MODELVIEW_MATRIX,b_modelview); //get the whole world OpenGL offset of the buttons
             glGetDoublev(GL_PROJECTION_MATRIX,b_projection);
             glGetIntegerv(GL_VIEWPORT,b_viewport);
             gluProject(-0.72,-0.82,0.0,b_modelview,b_projection,b_viewport,&x_downbutton,&y_downbutton,&z_dummy);
             glBindTexture(GL_TEXTURE_2D,dtexbind); //Down button - zoom-
             myRect2D_texture();

             glTranslatef(50.0,18,0.0);
             glGetDoublev(GL_MODELVIEW_MATRIX,b_modelview); //get the whole world OpenGL offset of the buttons
             glGetDoublev(GL_PROJECTION_MATRIX,b_projection);
             glGetIntegerv(GL_VIEWPORT,b_viewport);
             gluProject(-0.62,-0.77,0.0,b_modelview,b_projection,b_viewport,&x_backbutton,&y_backbutton,&z_dummy);
             glBindTexture(GL_TEXTURE_2D,btexbind); //Back button
             myRect2D_texture();

             glPopMatrix();
             glPushMatrix();
             glTranslatef(0.515,-0.77,0.0);
             glScalef(2.0/win_width,2.0/win_height,1.0);
             glGetDoublev(GL_MODELVIEW_MATRIX,b_modelview); //get the whole world OpenGL offset of the buttons
             glGetDoublev(GL_PROJECTION_MATRIX,b_projection);
             glGetIntegerv(GL_VIEWPORT,b_viewport);
             gluProject(0.515,-0.77,0.0,b_modelview,b_projection,b_viewport,&x_nextbutton,&y_nextbutton,&z_dummy);
             glBindTexture(GL_TEXTURE_2D,ntexbind);  //Next Button
             myRect2D_texture();

             glTranslatef(60.0,0.0,0.0);
             glGetDoublev(GL_MODELVIEW_MATRIX,b_modelview); //get the whole world OpenGL offset of the buttons
             glGetDoublev(GL_PROJECTION_MATRIX,b_projection);
             glGetIntegerv(GL_VIEWPORT,b_viewport);
             gluProject(0.643,-0.77,0.0,b_modelview,b_projection,b_viewport,&x_shootbutton,&y_shootbutton,&z_dummy);
             glBindTexture(GL_TEXTURE_2D,stexbind);  //Shoot Button
             myRect2D_texture();
             glPopMatrix();
           }
           glDisable(GL_TEXTURE_2D);
       }
#ifdef NETWORKING
       // show the network game end button if network is active
       if(active_net_game) {
           glEnable(GL_TEXTURE_2D);
           glPushMatrix();
           glScalef(2.0/win_width,2.0/win_height,1.0);
           glTranslatef((VMfloat)win_width/2-60,(VMfloat)win_height/2-60,0.0);
           glBindTexture(GL_TEXTURE_2D,networkbind); //network game close button
           myRect2D_texture();
           glPopMatrix();
           glDisable(GL_TEXTURE_2D);
       }
#endif
       // show the helpline
       if (vline_on && queue_view && !balls_moving ) {
    	    if(vline_id == -1) {
           vline_id = glGenLists(1);
    	      glNewList(vline_id, GL_COMPILE_AND_EXECUTE);
           glPushMatrix();
           glColor3f(0.3,0.3,0.3);
           glLineStipple( 1, 0xF0F0 );
           glEnable(GL_LINE_STIPPLE);
           glBegin( GL_LINES );
             glVertex3f( 0.0, 1.00, 0.5);
             glVertex3f( 0.0, 0.08, 0.5);
           glEnd();
           glDisable(GL_LINE_STIPPLE);
           glPopMatrix();
           glEndList();
    	   } else {
           //fprintf(stderr,"vline_id %i\n",vline_id);
           glCallList(vline_id);
        }
       }
       glEnable(GL_BLEND);
       glBlendFunc(GL_ONE,GL_ONE);
       glEnable(GL_TEXTURE_2D);
       glColor3f(0.9,0.9,0.9);

       //Special Keys are active ?
       if(!balls_moving && !player[act_player].is_AI && !player[act_player].is_net) {
         //place cue ball - Stipple in almost blue over it
         if (!FREE_VIEW && control__place_cue_ball) {
           if(cueball_id == -1) {
             cueball_id = glGenLists(1);
        	    glNewList(cueball_id, GL_COMPILE_AND_EXECUTE);
             glPushMatrix();
             glDisable(GL_BLEND);
             glDisable(GL_TEXTURE_2D);
             glLineWidth(20.0);
             glColor3f(0.2,0.2,1.0);
             glLineStipple( 1, 0x5555 );
             glEnable(GL_LINE_STIPPLE);
             glBegin( GL_LINES );
               glVertex3f( 0.0, 0.20, 0.5);
               glVertex3f( 0.0, 0.08, 0.5);
             glEnd();
             glLineWidth(1.0);
             glDisable(GL_LINE_STIPPLE);
             glColor3f(0.9,0.9,0.9);
             glPopMatrix();
             glEnable(GL_TEXTURE_2D);
             glEnable(GL_BLEND);
             glEndList();
      	    } else {
             //fprintf(stderr,"place_cue_ball %i\n",cueball_id);
             glCallList(cueball_id);
           }
         }
         // english moving (white ball, cross left and special graphic right corner)
         if(control__english) {
           if(english_id == -1) {
             english_id = glGenLists(1);
             glNewList(english_id, GL_COMPILE_AND_EXECUTE);
             glPushMatrix();
             glTranslatef(-0.95,0.15,0.0);
             glScalef(2.0/win_width,2.0/win_height,1.0);
             glBindTexture(GL_TEXTURE_2D,englishbind); //English Control if set
             glBegin(GL_QUADS);
               glTexCoord2f(0,1);
               glVertex3f(0,0,0);
               glTexCoord2f(0,0);
               glVertex3f(0,256,0);
               glTexCoord2f(1,0);
               glVertex3f(256,256,0);
               glTexCoord2f(1,1);
               glVertex3f(256,0,0);
             glEnd();
             glEndList();
    	      } else {
             //fprintf(stderr,"english move %i\n",english_id);
             glCallList(english_id);
           }
           //draw the cross on the big white ball if english set is active
           //then middle position is 106,100,0.0
           glTranslatef(106.0+(queue_point_x*5300),100.0-(queue_point_y*5300),0.0);
           glBindTexture(GL_TEXTURE_2D,kreuzbind); //English control cross png texture
           if(english1_id == -1) {
             english1_id = glGenLists(1);
             glNewList(english1_id, GL_COMPILE_AND_EXECUTE);
             myRect2D_texture();
             glPopMatrix();
             glPushMatrix();
             glTranslatef(0.03,-0.94,0.0);
             glScalef(2.0/win_width,2.0/win_height,0.0);
             glBindTexture(GL_TEXTURE_2D,ebind); //English control mode png texture
             myRect2D_texture();
             glPopMatrix();
             glEndList();
  	        } else {
           //fprintf(stderr,"english move %i\n",english1_id);
           glCallList(english1_id);
           }
         }
         if(control__mouse_shoot) {
           if(shoot_id == -1) {
             shoot_id = glGenLists(1);
             glNewList(shoot_id, GL_COMPILE_AND_EXECUTE);
             glPushMatrix();
             glTranslatef(0.03,-0.94,0.0);
             glScalef(2.0/win_width,2.0/win_height,0.0);
             glBindTexture(GL_TEXTURE_2D,sbind); //mouse shoot mode png texture
             myRect2D_texture();
             glPopMatrix();
             glEndList();
	          } else {
             //fprintf(stderr,"mouse shoot %i\n",shoot_id);
             glCallList(shoot_id);
           }
         }
         if(control__cue_butt_updown) {
           if(cuebutt_id == -1) {
             cuebutt_id = glGenLists(1);
             glNewList(cuebutt_id, GL_COMPILE_AND_EXECUTE);
             glPushMatrix();
             glTranslatef(0.03,-0.94,0.0);
             glScalef(2.0/win_width,2.0/win_height,0.0);
             glBindTexture(GL_TEXTURE_2D,bbind); //cue up/down control mode png texture
             myRect2D_texture();
             glPopMatrix();
             glEndList();
	          } else {
             //fprintf(stderr,"cue butt up/down %i\n",cuebutt_id);
             glCallList(cuebutt_id);
           }
         }
         if(control__place_cue_ball) {
           if(cueball1_id == -1) {
             cueball1_id = glGenLists(1);
             glNewList(cueball1_id, GL_COMPILE_AND_EXECUTE);
             glPushMatrix();
             glTranslatef(0.03,-0.94,0.0);
             glScalef(2.0/win_width,2.0/win_height,0.0);
             glBindTexture(GL_TEXTURE_2D,mbind); //set cue ball control mode png texture
             myRect2D_texture();
             glPopMatrix();
             glEndList();
	          } else {
             //fprintf(stderr,"place cue ball %i\n",cueball1_id);
             glCallList(cueball1_id);
           }
         }
         if(control__fov) {
           if(fov_id == -1) {
             fov_id = glGenLists(1);
             glNewList(fov_id, GL_COMPILE_AND_EXECUTE);
             glPushMatrix();
             glTranslatef(0.03,-0.94,0.0);
             glScalef(2.0/win_width,2.0/win_height,0.0);
             glBindTexture(GL_TEXTURE_2D,fbind); //FOV control mode png texture
             myRect2D_texture();
             glPopMatrix();
             glEndList();
	          } else {
             //fprintf(stderr,"FOV %i\n",fov_id);
             glCallList(fov_id);
           }
         }
       }
       if(options_free_view_on && !options_birdview_on) {    // Freeview png if freeview on
         if(freeview_id == -1) {
           freeview_id = glGenLists(1);
           glNewList(freeview_id, GL_COMPILE_AND_EXECUTE);
           glPushMatrix();
           glTranslatef(-0.09,-0.94,0.0);
           glScalef(2.0/win_width,2.0/win_height,0.0);
           glBindTexture(GL_TEXTURE_2D,freeviewbind); //Freeview control mode png texture
           myRect2D_texture();
           glPopMatrix();
           glEndList();
         } else {
           //fprintf(stderr,"Freeview %i\n",freeview_id);
           glCallList(freeview_id);
         }
       }
       glDisable(GL_TEXTURE_2D);
       glDisable(GL_BLEND);
    if(!options_3D_winnertext) {
       if(options_gamemode==options_gamemode_tournament && tournament_state.overall_winner>=0 && g_act_menu==(menuType *)0) {
           // here is nothing todo, only an else (need better statement for if)
       } else {
         if( (player[0].winner || player[1].winner) && g_act_menu==(menuType *)0 ){
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE,GL_ONE);
            if( !options_rgstereo_on ){
                glColor3f(1.0,1.0,0.0);
            } else {
                glColor3f(1.0,1.0,1.0);
            }
            glPushMatrix();
            glTranslatef(0,0,-0.5);
            glScalef(2.0/win_width,2.0/win_height,1.0);
            glTranslatef( 0, 30,-0.5);
            textObj_setText( winner_name_text_obj, player[player[0].winner?0:1].name );
            textObj_draw_centered( winner_name_text_obj );
            glTranslatef( 0,-60, 0.0);
            textObj_draw_centered( winner_text_obj );
            glPopMatrix();
            glDisable(GL_BLEND);
         }
       }
    }
    if(helpscreen_on){
        glColor3f(0.7,0.7,0.7);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE,GL_ONE);
        glPushMatrix();
        draw_help_screen(win_width, win_height);
        glPopMatrix();
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }

    if(options_gamemode==options_gamemode_tournament && tournament_state.wait_for_next_match && tournament_state.overall_winner==-1) {
        Display_tournament_tree(&tournament_state);
           /* the following lines only for debugging
           int i;
           struct TournamentState_ * ts;
           ts = &tournament_state;
           printf("Pairings: \n");
           for(i=0;i<(1<<(ts->round_num-ts->round_ind-1));i++){
               printf("%s vs. %s\n",
                      ts->roster.player[ts->game[ts->round_ind][i].roster_player1].name,
                      ts->roster.player[ts->game[ts->round_ind][i].roster_player2].name
                     );
               printf("%d vs. %d\n",
                      ts->game[ts->round_ind][i].roster_player1,
                      ts->game[ts->round_ind][i].roster_player2
                     );
           }*/
       }

    if(!introtexture) {
        if((introxanimate+=12) >768) introxanimate = 768;
        if((introyanimate+=8) >512) introyanimate = 512;
        if((introblendxanimate-=6) < -384.0) introblendxanimate = -384.0;
        if((introblendyanimate-=3.22) < -206.0) introblendyanimate = -206.0;
        glEnable(GL_TEXTURE_2D);
        glColor4f(0.9,0.9,0.9,1.0);
        glEnable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glBlendFunc (GL_ONE, GL_ONE);
        glPushMatrix();
        glScalef(2.0/win_width,2.0/win_height,1.0);
        glTranslatef(introblendxanimate,introblendyanimate,0.0);
        glBindTexture(GL_TEXTURE_2D,introtexbind);
        glBegin(GL_QUADS); // Introsequenz graphic
          glTexCoord2f(0,1);
          glVertex3f(0,0,0);
          glTexCoord2f(0,0);
          glVertex3f(0,introyanimate,0);
          glTexCoord2f(1,0);
          glVertex3f(introxanimate,introyanimate,0);
          glTexCoord2f(1,1);
          glVertex3f(introxanimate,0,0);
        glEnd();
        glPopMatrix();
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
    }

    if( g_act_menu != (menuType *)0 ){
        glColor3f(1.0,1.0,1.0);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glBlendFunc(GL_ONE,GL_ONE);
        glPushMatrix();
        glTranslatef(0.0,0.0,-1.0);
        glScalef(2.0/win_width,2.0/win_height,1.0);
        menu_draw( g_act_menu );
        glPopMatrix();
        glDisable(GL_BLEND);
    }

#ifdef NETWORKING
    if( wait_seconds > 0 ) {
        // draw the countdown
        sprintf(str,"%01u:%02u",wait_seconds/60,wait_seconds%60);
        textObj_setText( seconds_text_obj, str );
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glTranslatef(0.0,0.5,-1.0);
        glScalef(2.0/win_width,2.0/win_height,1.0);
        glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
        glColor3f(1.0,1.0,1.0);
        glTranslatef(2,2,0);
        textObj_draw_centered(seconds_text_obj);
        glTranslatef(-4,0,0);
        textObj_draw_centered(seconds_text_obj);
        glTranslatef(0,-4,0);
        textObj_draw_centered(seconds_text_obj);
        glTranslatef(4,0,0);
        textObj_draw_centered(seconds_text_obj);
        glTranslatef(-2,2,0);
        glBlendFunc(GL_ONE,GL_ONE);
        textObj_draw_centered(seconds_text_obj);
        glTranslatef(0.0,-100,0.0);
        //draw on Host game the listener ip Adresses
        if(server!=NULL) {
           textObj_draw_centered( ip1_text_obj);
           glTranslatef(0.0,-35.0,0.0);
           textObj_draw_centered( ip2_text_obj);
           glTranslatef(0.0,-21.0,0.0);
           for(m=0;m<9;m++) {
             if(ipptr[m]) {
               textObj_draw_centered( ip_text_obj[m]);
               glTranslatef(0.0,-21.0,0.0);
             }
           }
        }
        glTranslatef(0.0,-14,0.0);
        textObj_draw_centered(esc_stop_obj);
        glPopMatrix();
        glDisable(GL_BLEND);
    }
#endif

    if(enddisp_id == -1) {
      enddisp_id = glGenLists(1);
      glNewList(enddisp_id, GL_COMPILE_AND_EXECUTE);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_LIGHTING);
      glEnable(GL_TEXTURE_2D);
      glPopMatrix();
      glMatrixMode( GL_PROJECTION );
      glPopMatrix();
      glMatrixMode( GL_MODELVIEW );
      glPopMatrix();
      glEndList();
    } else {
      //fprintf(stderr,"enddisp %i\n",enddisp_id);
      glCallList(enddisp_id);
    }
   } /* rg stereo */

}

/***********************************************************************
 *                        Resizing an OpenGL window                    *
 ***********************************************************************/

void ResizeWindow( int width, int height )
{
   if(width < 958) width = 958;      // don't resize below this
   if(height < 750) height = 750;
   win_width=width;
   win_height=height;
   glViewport( 0, 0, width, height );
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
}

/***********************************************************************
 *                Set the table in correct view                        *
 ***********************************************************************/

void tableinfov(void)
{
     int switchvalue;
     switchvalue= (int)(options_table_size/0.3048);
     //fprintf(stderr,"table: %i\n",switchvalue);
     switch (switchvalue) {
       case 6:
       case 7:
          cam_dist_aim = 2.5; //for 7 foot or default table
          break;
       case 8:
          cam_dist_aim = 2.8; //for 8 foot table
          break;
       case 9:
          cam_dist_aim = 3.1; //for 9 foot table
          break;
       default:
          cam_dist_aim = 3.8; //for 12 foot table
          break;
     }
}

/***********************************************************************
 *                        Restart a common game                        *
 ***********************************************************************/

void restart_game_common(void)
{

    char statusstr[100];  // make a better statusline
#ifdef NETWORKING
    if(wait_key || active_net_game) {
       return;
    }
#endif
    playonce = 0;         //applause allowed for one time
    player[0].half_full=BALL_ANY;
    player[1].half_full=BALL_ANY;
    player[0].place_cue_ball=0;
    player[1].place_cue_ball=0;
    player[0].winner=0;
    player[1].winner=0;
    player[0].score=0;
    player[1].score=0;
    textObj_setText(player[0].score_text,"0");
    textObj_setText(player[1].score_text,"0");
    create_walls( &walls );
    create_scene( &balls );
    tableinfov();
    g_shot_due=1;
#ifdef USE_SOUND
        if(options_gamemode==options_gamemode_tournament && player[0].is_AI && player[1].is_AI) {
          //nosound
        } else {
          Playwavdata(cvt_shuffle.buf,cvt_shuffle.len_cvt,options_snd_volume);
        }
#endif
    if(gametype==GAME_CARAMBOL) {
       emptyworkstring();
       sprintf(statusstr,localeText[175],options_maxp_carambol);
       concatworkstring(statusstr);
       setst_text();
      player[0].cue_ball=0;
      player[1].cue_ball=1;
    } else {
      player[0].cue_ball=0;
      player[1].cue_ball=0;
    }

    // switch back to a start position
    Xrot = -70.0;
    Yrot = 0.0;
    Zrot = 0.0;
    Xque = -83.0;
    Zque = 0.0;
    Xrot_offs=0.0;
    Yrot_offs=0.0;
    Zrot_offs=0.0;

    if(options_birdview_on) { // ### FIXME ### not so good code here (ugly)
     birdview();
    }
}

/***********************************************************************
 *                        Restart a Tournament                         *
 ***********************************************************************/

void restart_game_tournament(void)
{
    restart_game_common();
    g_motion_ratio=1.0;
    init_tournament_state(&tournament_state);
    act_player=0;
    queue_view=player[act_player].queue_view;
    close_screen();
}

/***********************************************************************
 *                      Restart a training game                        *
 ***********************************************************************/

void restart_game_training(void)
{
    restart_game_common();
    g_motion_ratio=1.0;
    human_player_roster.player[0].winner=0;
    human_player_roster.player[0].score=0;
    player_copy(&player[0],human_player_roster.player[0]);
    player_copy(&player[1],human_player_roster.player[0]);
    act_player=0;
    queue_view=player[act_player].queue_view;
    copy_balls(&balls,&bakballs); //for undo problem if someone pick undo at startup
    close_screen();
}

/***********************************************************************
 *                        Restart a game match                         *
 ***********************************************************************/

void restart_game_match(void)
{
    restart_game_common();
    g_motion_ratio=1.0;

    human_player_roster.player[0].winner=0;
    human_player_roster.player[1].winner=0;
    human_player_roster.player[0].score=0;
    human_player_roster.player[1].score=0;
    player_copy(&player[0],human_player_roster.player[0]);
    player_copy(&player[1],human_player_roster.player[1]);
    act_player=0;
    queue_view=player[act_player].queue_view;
    close_screen();
}

/**********************************************************************
 *                        Restart a whole game                        *
 **********************************************************************/

void restart_game(void)
{

    displaystring (" "); // clear the statusline
    switch(options_gamemode){
    case options_gamemode_tournament:
        restart_game_tournament();
        break;
    case options_gamemode_training:
        restart_game_training();
        break;
    case options_gamemode_match:
        restart_game_match();
        break;
    }
}

/***********************************************************************
 *                Control set for keys s b e m f                       *
 ***********************************************************************/

void control_set(int * control_param)
{
    if(!control__active){
        *control_param=1;
        control__updated=1;
        control__active=1;
    }
}

/***********************************************************************
 *                Control unset for keys s b e m f                     *
 ***********************************************************************/

void control_unset(int * control_param)
{
    if(control__active && *control_param){
      *control_param=0;
      control__active=0;
    }
}

/***********************************************************************
 *                Control Toggle for keys s b e m f                    *
 ***********************************************************************/

void control_toggle(int * control_param) {

    if(control__active) {
        control_unset(control_param);
    } else {
        control_set(control_param);
    }
}

/***********************************************************************
 *            Zooming the view out or in in direction steps            *
 ***********************************************************************/

void zoom_in_out(int direction) {
    VMfloat th, ph;
    VMvect dvec;
    if(!FREE_VIEW){
        cam_dist_aim += direction*0.005;
        if( cam_dist_aim<0.2 ) cam_dist_aim=0.2;
        if( cam_dist_aim>5.8 ) cam_dist_aim=5.8;
        //fprintf(stderr,"Dist: %f\n",cam_dist_aim);
    } else {
        th=Xrot/180.0*M_PI;
        ph=Zrot/180.0*M_PI;
        dvec = vec_xyz(sin(th)*sin(ph),sin(th)*cos(ph),cos(th));
        dvec = vec_scale( dvec , direction*0.005 );
        free_view_pos_aim = vec_add( free_view_pos_aim, dvec );
        //fprintf(stderr,"x %f y %f z %f\n",free_view_pos_aim.x,free_view_pos_aim.y,free_view_pos_aim.z);
        if( free_view_pos_aim.z < 0.1 ) free_view_pos_aim.z = 0.1;
        if( free_view_pos_aim.z > 5.6 ) free_view_pos_aim.z = 5.6;
        if( free_view_pos_aim.y < -5.5 ) free_view_pos_aim.y = -5.5;
        if( free_view_pos_aim.y > 0.27 ) free_view_pos_aim.y = 0.27;
        if( free_view_pos_aim.x < -0.3 ) free_view_pos_aim.x = -0.3;
        if( free_view_pos_aim.x > 0.0 ) free_view_pos_aim.x = 0.0;
    }
}

/***********************************************************************
 *  Set the cue-point in english setting mode (Keyboard and mouse)     *
 ***********************************************************************/

void setenglish(VMfloat addx, VMfloat addy) {

   VMfloat queue_point_abs;

   queue_point_x += addx;
   queue_point_y += addy;
   queue_point_abs = sqrt(queue_point_y*queue_point_y+queue_point_x*queue_point_x);
   if(queue_point_abs>(BALL_D-QUEUE_D2)/2.0) {
     queue_point_x=queue_point_x/queue_point_abs*(BALL_D-QUEUE_D2)/2.0;
     queue_point_y=queue_point_y/queue_point_abs*(BALL_D-QUEUE_D2)/2.0;
   }
}

/***********************************************************************
 *  Set the cue-ball in english setting mode (Keyboard only)           *
 *  Function checks overlapping of cueball and other ball for the      *
 *  next move                                                          *
 ***********************************************************************/

void setcueball(VMvect *cueball, VMfloat x, VMfloat y, int cue_ball) {

   int i, move_ok;
   VMvect whitepos, posoffs;
   posoffs.x = x;
   posoffs.y = y;
   whitepos=*cueball;
   ball_displace_clip(cueball, posoffs);
   move_ok = 1;
   for(i=0;i<balls.nr;i++) {
      if(i!=cue_ball) {
         move_ok = move_ok &&
          (vec_abs(vec_diff(balls.ball[cue_ball].r,balls.ball[i].r))>(balls.ball[cue_ball].d+balls.ball[i].d)/2.0 ||
          (!balls.ball[i].in_game));
        }
     }
   if(!move_ok) {
     balls.ball[cue_ball].r=whitepos;
   }
}

/***********************************************************************
 *                    Set the FOV (Keyboard only)                      *
 ***********************************************************************/

void setfov(VMfloat fovstep) {

   VMfloat old_FOV=cam_FOV;

   cam_FOV  +=  fovstep; //step;
   if( cam_FOV<30.0 ) cam_FOV=30.0;
   if( cam_FOV>110.0 ) cam_FOV=110.0;
   cam_dist_aim = cam_dist_aim*tan(old_FOV*M_PI/360.0)/tan(cam_FOV*M_PI/360.0);
   cam_dist = cam_dist_aim;
}

/***********************************************************************
 *                        passes a keystroke                           *
 ***********************************************************************/

void Key( int key, int modifiers ) {

   int cue_ball = CUE_BALL_IND;
   int old_cue_ball;    // only for trainig mode the holder for the state of the old cueball

   displaystring(" ");  // the statustext is now empty, display blank lines
   if(!introtexture) introtexture++;
   if( g_act_menu != (menuType *)0 ){
#ifdef NETWORKING
       if(wait_key || active_net_game) {
          return;
       }
#endif
       /* menu keys */
       switch (key) {
       case KSYM_PAGE_UP:
       case KSYM_UP:
           menu_select_prev(g_act_menu);
           break;
       case KSYM_PAGE_DOWN:
       case KSYM_DOWN:
           menu_select_next(g_act_menu);
           break;
       case 13:
#ifdef USE_SOUND
             PlaySound(&ball_cue_snd,options_snd_volume);
#endif
           menu_choose( &g_act_menu );
           break;
       case 27:
           menu_exit( &g_act_menu );
           break;
       default:
           menu_text_keystroke( g_act_menu, key );
           break;
       }

   } else if (control__active){
       // Handling of special control are only active, nothing else works then
      switch(key) {
      //handling of special-special keys
      case 's':
          if(options_control_kind) {
            control_toggle(&control__mouse_shoot);
          }
          break;
      case 'b':
          if(options_control_kind) {
            control_toggle(&control__cue_butt_updown);
          }
          break;
      case 'e':
          if(options_control_kind) {
            control_toggle(&control__english);
          }
          break;
      case 'm':
          if(options_control_kind) {
            control_toggle(&control__place_cue_ball);

          }
          break;
      case 'f':
          if(options_control_kind) {
            control_toggle(&control__fov);
          }
          break;
      //handling of special-special keys end
        case KSYM_DOWN:
           //cue down
           if(control__cue_butt_updown) {
             Xque -= step;
             step += CUESTEP;
             if(step > CUESTEPMAX) step = CUESTEPMAX;
             if(Xque < -90.0) Xque = -90.0;
           //place cue ball
           } else if (control__place_cue_ball && player[act_player].place_cue_ball && !balls_moving && !player[act_player].is_AI && !player[act_player].is_net) {
               setcueball(&balls.ball[cue_ball].r, 0.0, -0.01, cue_ball);
           //set shoot angle
           } else if (control__english && !player[act_player].is_AI && !player[act_player].is_net) {
               setenglish(0.0, 0.001);
           //set front of view
           } else if(control__fov) {
               setfov(-0.5);
           }
           break;
        case KSYM_UP:
           //cue up
           if(control__cue_butt_updown) {
             Xque += step;
             step += CUESTEP;
             if(step > CUESTEPMAX) step = CUESTEPMAX;
             if(Xque > 0.0) Xque = 0.0;
           //place cue ball
           } else if (control__place_cue_ball && player[act_player].place_cue_ball && !balls_moving && !player[act_player].is_AI && !player[act_player].is_net) {
               setcueball(&balls.ball[cue_ball].r, 0.0, +0.01, cue_ball);
           //english set
           } else if (control__english && !player[act_player].is_AI && !player[act_player].is_net) {
               setenglish(0.0, -0.001);
           //set front of view
           } else if(control__fov) {
               setfov(0.5);
           }
           break;
        case KSYM_LEFT:
           //place cue ball
           if (control__place_cue_ball && player[act_player].place_cue_ball && !balls_moving && !player[act_player].is_AI && !player[act_player].is_net) {
             setcueball(&balls.ball[cue_ball].r, -0.01, 0.0, cue_ball);
           //english set
           } else  if (control__english && !player[act_player].is_AI && !player[act_player].is_net) {
             setenglish(-0.001,0.0);
           }
           break;
        case KSYM_RIGHT:
           //place cue ball
           if (control__place_cue_ball && player[act_player].place_cue_ball && !balls_moving && !player[act_player].is_AI && !player[act_player].is_net) {
             setcueball(&balls.ball[cue_ball].r, +0.01, 0.0, cue_ball);
           //english set
           } else  if (control__english && !player[act_player].is_AI && !player[act_player].is_net) {
             setenglish(0.001,0.0);
           }
           break;
      }
   } else {

   /* general keys */

      switch (key) {
      case KSYM_UP: //
            if(options_birdview_on) {
             // in birdview on, the cue is moving
             Zque = angle_pm360(Zque);
             if(Zque >90.0 && Zque <270.0) {
              Zque--;
             } else {
              Zque++;
             }
             break;
            }
            if(!FREE_VIEW){
              Xrot -= step;
              step += STEP1;
              if (step > STEPMAX) step = STEPMAX;
            } else {
              Xrot += freeview_step;
              freeview_step += FREEVIEW_STEP1;
              if(freeview_step > FREEVIEW_STEPMAX) freeview_step = FREEVIEW_STEPMAX;
            }
            if(Xrot < -90.0) Xrot = -90.0;
         break;
      case KSYM_DOWN:
            if(options_birdview_on) {
            // in birdview on, the cue is moving
            Zque = angle_pm360(Zque);
            if(Zque >90.0 && Zque <270.0) {
              Zque++;
            } else {
              Zque--;
             }
            break;
            }
            if(!FREE_VIEW){
              Xrot += step;
              step += STEP1;
              if (step > STEPMAX) step = STEPMAX;
            } else {
              Xrot -= freeview_step;
              freeview_step += FREEVIEW_STEP1;
              if(freeview_step > FREEVIEW_STEPMAX) freeview_step = FREEVIEW_STEPMAX;
            }
            if(Xrot > 0.0) Xrot = 0.0;
         break;
      case KSYM_RIGHT:
            if(options_birdview_on) {
            // in birdview on, the cue is moving
            Zque = angle_pm360(Zque);
            if(Zque >0.0 && Zque <180.0) {
              Zque++;
            } else {
              Zque--;
            }
            break;
            }
            if(!FREE_VIEW) {
              Zrot = angle_pm360(Zrot+step);
              step += STEP1;
              if (step > STEPMAX) step = STEPMAX;
            } else {
              Zrot = angle_pm360(Zrot-freeview_step);
              Zque += freeview_step;
              freeview_step += FREEVIEW_STEP1;
              if(freeview_step > FREEVIEW_STEPMAX) freeview_step = FREEVIEW_STEPMAX;
            }
         break;
      case KSYM_LEFT:
         if(options_birdview_on) {
         // in birdview on, the cue is moving
         Zque = angle_pm360(Zque);
         if(Zque >0.0 && Zque <180.0) {
           Zque--;
         } else {
           Zque++;
         }
         break;
         }
         if(!FREE_VIEW){
             Zrot = angle_pm360(Zrot-step);
             step += STEP1;
             if (step > STEPMAX) step = STEPMAX;
         } else {
             Zrot = angle_pm360(Zrot+freeview_step);
             Zque -= freeview_step;
             freeview_step += FREEVIEW_STEP1;
             if(freeview_step > FREEVIEW_STEPMAX) freeview_step = FREEVIEW_STEPMAX;
         }
         break;
      case KSYM_PAGE_DOWN:
         if(!player[act_player].is_AI && !balls_moving)
             queue_strength = strength01( queue_strength-0.01 );
         break;
      case KSYM_PAGE_UP:
         if(!player[act_player].is_AI && !balls_moving)
            queue_strength = strength01( queue_strength+0.01 );
         break;
      case KSYM_F1:
         if(g_act_menu==(menuType *)0){
              helpscreen_on = !helpscreen_on;
             }
         break;
      case KSYM_F2:
         if(!old_birdview_ai) {
             birdview();
         } else {
             displaystring(localeText[413]);
         }
         break;
      case KSYM_F3:
         if (!options_birdview_on) {
             toggle_queue_view();
         } else {
             displaystring(localeText[413]);
         }
         break;
      case KSYM_F4:
         if(!options_birdview_on) {
             options_free_view_on = (options_free_view_on==0)?1:0;
         } else {
             displaystring(localeText[413]);
         }
         break;
      case KSYM_QUIT:
          save_config();
          break;
      case KSYM_F5:
          save_config();
          if(SDL_AddTimer(2000,notshow_disc,NULL) != NULL) {
            show_disc = 1;
          }
         break;
      case KSYM_F6:
#ifdef NETWORKING
          if(!player[0].is_net && !player[1].is_net && !wait_key &&  !active_net_game) {
#endif
             player[act_player].is_AI = (player[act_player].is_AI==0)?1:0;
             if(player[act_player].is_AI) {
                displaystring(localeText[411]);
                player[act_player].queue_view=0;
                if(queue_view) toggle_queue_view();
                do_computer_move(1);
                } else {
                 displaystring(localeText[412]);
                }
#ifdef NETWORKING
          } else {
             displaystring(localeText[413]);
          }
#endif
         break;
      case KSYM_F7:
          if((options_balltrace = (options_balltrace==0)?1:0)) {
             displaystring(localeText[414]);
          } else {
             displaystring(localeText[415]);
          }
         break;
      case KSYM_F8:
          if(!options_birdview_on) {
             vline_on = (vline_on==0)?1:0;
          } else {
             displaystring(localeText[413]);
          }
         break;
      case KSYM_F9:
#ifdef NETWORKING
          if(!player[0].is_net && !player[1].is_net && !wait_key && !active_net_game) {
#endif
             do_computer_move(0);
             displaystring(localeText[416]);
#ifdef NETWORKING
          } else {
             displaystring(localeText[413]);
          }
#endif
         break;
      case KSYM_F10:
#ifdef NETWORKING
          if(!player[0].is_net && !player[1].is_net && !wait_key && !active_net_game) {
#endif
            restart_game();
#ifdef NETWORKING
          } else {
            displaystring(localeText[413]);
          }
#endif
         break;
      case 27:
          helpscreen_on=0;
#ifdef NETWORKING
          if(!wait_key && !active_net_game) {
#endif
            g_act_menu = g_main_menu;
#ifdef NETWORKING
          }
          if(wait_key && !active_net_game) {
            close_listener();
          }
#endif
         break;
      case '1':
          //zooming in
          zoom_in_out(-20);
         break;
      case '2':
          //zooming out
          zoom_in_out(+20);
         break;
      case ' ':
      case 13:
          if(modifiers == 0){
            enter_mousemiddle();
          }
#ifndef TOUCH
          if(modifiers & KEY_MODIFIER_ALT){
            sys_toggle_fullscreen();
          }
          break;
      case KSYM_KP_ENTER:
          if(modifiers & KEY_MODIFIER_ALT){
            sys_toggle_fullscreen();
            }
#endif
          break;
      case 's':
          if(queue_view) toggle_queue_view();
          if(options_control_kind) {
            control_toggle(&control__mouse_shoot);
          } else {
            control_set(&control__mouse_shoot);
          }
          break;
      case 'b':
          if(queue_view) toggle_queue_view();
          if(options_control_kind) {
            control_toggle(&control__cue_butt_updown);
          } else {
            control_set(&control__cue_butt_updown);
          }
          break;
      case 'e':
          if(options_control_kind) {
            control_toggle(&control__english);
          } else {
            control_set(&control__english);
          }
          break;
      case 'm':
          if(options_control_kind) {
            control_toggle(&control__place_cue_ball);
          } else {
            control_set(&control__place_cue_ball);
          }
          break;
      case 'f':
          if(options_birdview_on) {
            displaystring(localeText[413]);
            break;
          }
          if(!queue_view) toggle_queue_view();
          if(options_control_kind) {
            control_toggle(&control__fov);
          } else {
            control_set(&control__fov);
          }
          break;
      case 9:  /* TAB */
          if( options_gamemode == options_gamemode_training ){
             old_cue_ball=player[act_player].cue_ball;
             do {
                  player[act_player].cue_ball++;
                  if( player[act_player].cue_ball >= balls.nr )
                      player[act_player].cue_ball=0;
                  if( player[act_player].cue_ball == old_cue_ball ) break;
                  //fprintf(stderr,"cue_ball=%d\n",player[act_player].cue_ball);
             } while ( !balls.ball[player[act_player].cue_ball].in_game );
          } else {
             displaystring(localeText[413]);
          }
         break;
      case'u':  /* undo */
          if( options_gamemode == options_gamemode_training ){
             copy_balls(&bakballs,&balls);
          } else {
             displaystring(localeText[413]);
          }
         break;
      case'l':  /* sliding left menu */
         if(!leftmenu) {
           leftmenu = 1;
         } else if(leftmenu == 2) {
           leftmenu = 3;
         }
         break;
      case'r':  /* sliding right menu */
         if(!rightmenu) {
           rightmenu = 1;
         } else if(rightmenu == 2) {
           rightmenu = 3;
         }
         break;
      }
   }  /* no menu active */
}

/***********************************************************************
 *             Keyboard key up function for s b e m f                  *
 ***********************************************************************/

void KeyUp( int key, int modifiers )
{
    if( g_act_menu == (menuType *)0 && !options_control_kind){
        switch (key) {
        case 's':
           control_unset(&control__mouse_shoot);
           break;
        case 'b':
           control_unset(&control__cue_butt_updown);
           break;
        case 'e':
           control_unset(&control__english);
           break;
        case 'm':
           control_unset(&control__place_cue_ball);
           break;
        case 'f':
           control_unset(&control__fov);
           break;
        }
    }
    step = 0.03; // Keys Accelerator back to start
    freeview_step = 0.03; // Keys Accelerator back to start
}

/***********************************************************************
 *        unallocate all cuberef textures in memory allocation         *
 ***********************************************************************/

void free_cuberef_tex(void)
{
    int i;
    for( i=0 ; i<cuberef_allballs_texbind_nr ; i++ ){

        glDeleteTextures(1,&cuberef_allballs_texbind[i]);
    }
    free(cuberef_allballs_texbind);
    cuberef_allballs_texbind=0;
}

/***********************************************************************
 *       reassign / build all cuberef textures allocate memory         *
 ***********************************************************************/

void reassign_and_gen_cuberef_tex(void)
{
    int i,j,k,l, layer, w, target = 0;
    char * data;

    if( cuberef_allballs_texbind!=0 || balls.nr!=cuberef_allballs_texbind_nr ) {
       free_cuberef_tex();
    }
    cuberef_allballs_texbind_nr=balls.nr;
    cuberef_allballs_texbind=malloc(cuberef_allballs_texbind_nr*sizeof(*cuberef_allballs_texbind));

    for( i=0 ; i<cuberef_allballs_texbind_nr ; i++ ){
        glGenTextures(1,&cuberef_allballs_texbind[i]);
    }
            for(i=0;i<6;i++){
                switch(i){
                case 0: target=GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB; break;
                case 1: target=GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB; break;
                case 2: target=GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB; break;
                case 3: target=GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB; break;
                case 4: target=GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB; break;
                case 5: target=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB; break;
                }
                w=options_cuberef_res;
                //fprintf(stderr,".... w=%d,h=%d\n",w,w);

                data=malloc(w*w*3);
                for(j=0;j<w*w;j++){
                    if( ((j%w)%9)<1 || ((j/w)%9)<1 ){
                        data[j*3+0]=255*(j%w)/w;
                        data[j*3+1]=255*(j/w)/w;
                        data[j*3+2]=0;
                    }else{
                        data[j*3+0]=((i%3)==0 || i==5)?0xFF:0;
                        data[j*3+1]=((i%3)==1 || i==3)?0xFF:0;
                        data[j*3+2]=((i%3)==2 || i==4)?0xFF:0;

                    }
                }
                for(k=0;k<cuberef_allballs_texbind_nr;k++){
                    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cuberef_allballs_texbind[k]);
                    for( l=options_cuberef_res,layer=0 ; l>0 ; l>>=1,layer++){
                        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                        glTexImage2D(target,layer,
                           GL_RGB, /*3,*/
                           l,  /* width */
                           l,  /* height */
                           0,GL_RGB,GL_UNSIGNED_BYTE,data);
                    }
                }
                free( data );
            }
}

#ifdef NETWORKING

/***********************************************************************
 *                        Start a network game                         *
 *  Use strings to transport over a network, because we can have many  *
 *  different clients like 32/64 Bit little/big endian Win/Linux etc.  *
 ***********************************************************************/

int host_network_game(void)
{
    char name1[256],name2[256];

    displaystring (" "); // clear the statusline
    options_jump_shots_net_bak = options_jump_shots;      // for restore jump_shots after net game
    options_net_compatible_bak = options_net_compatible;  // for restore net compatible after net game
    options_net_speed_bak = options_net_speed;            // for restore net_speed after net game
    if(server == NULL || g_socket == NULL) {
      displaystring (localeText[248]);
      close_listener();
       return(0);
    }
    strcpy(net_data,"42"); //testing connection
    //fprintf(stderr,"Test Client Connection\n");
    if(socket_write(g_socket,net_data,10)!=10) {
      displaystring (localeText[249]);
      close_listener();
      return(0);
    }
    if(socket_read(g_socket,net_data,10)!=10) { // read the answer from client
      //fprintf(stderr,"Read Client Answer\n");
      displaystring (localeText[250]);
      close_listener();
      return(0);
    }
    if(atoi(net_data)!=7) {
      //fprintf(stderr,"check the answer again 7\n");
      displaystring (localeText[251]);
      close_listener(); // not the correct connection - terminating
      return (0);
    }
    if(socket_read(g_socket,net_data,10)!=10) { // read math precision from client
      //fprintf(stderr,"Read Client Answer\n");
      displaystring (localeText[250]);
      close_listener();
      return(0);
    }
#ifdef VMATH_SINGLE_PRECISION
    if(atoi(net_data)!=1) {
#else
     if(atoi(net_data)!=0) {
#endif
      displaystring (localeText[264]);
      close_listener(); // not the correct connection - terminating
      return (0);
    }
    sprintf(net_data,"%i",options_net_speed); //Send server network speed
    //fprintf(stderr,"Send Server Network speed\n");
    if(socket_write(g_socket,net_data,10)!=10) {
      displaystring (localeText[252]);
      close_listener();
      return(0);
    }

    sprintf(net_data,"%i",options_net_compatible); //Send server network mode
    //fprintf(stderr,"Send Server Network mode\n");
    if(socket_write(g_socket,net_data,10)!=10) {
      displaystring (localeText[252]);
      close_listener();
      return(0);
    }
    if(options_net_compatible) {
       net_send_data = net_send_data_soft;
       net_get_data = net_get_data_soft;
    } else {
       net_send_data = net_send_data_hard;
       net_get_data = net_get_data_hard;
    }

    player[1].queue_view=0;
    player[0].queue_view=1;

    strcpy_whtspace_2_uscore(name1,player[0].name);
    strcpy_whtspace_2_uscore(name2,player[1].name);
#ifdef VMATH_SINGLE_PRECISION
    sprintf(net_data,"%i %i %f %i %s %s",options_jump_shots,gametype,options_table_size,options_maxp_carambol,name1,name2);
#else
    sprintf(net_data,"%i %i %lf %i %s %s",options_jump_shots,gametype,options_table_size,options_maxp_carambol,name1,name2);
#endif
    //fprintf(stderr,"Write Gameplay options\n");
    if(socket_write(g_socket,net_data,700)!=700) {
      displaystring (localeText[247]);
      close_listener();
      return(0);
    }

    player[0].is_AI=0;  /* FIXME maybe one can leave this away someday */
    player[1].is_AI=0;  /* FIXME maybe one can leave this away someday */
    player[1].is_net=1;
    player[0].is_net=0;
    wait_key = 0;
    active_net_game = 1; // now, the network game is active
    displaystring (localeText[253]);
    return (1);
}

/***********************************************************************
 *                       Join a network game                           *
 *  Use strings to transport over a network, because we can have many  *
 *  different clients like 32/64 Bit little/big endian Win/Linux etc.  *
 ***********************************************************************/

int join_network_game(void)
{

    char name1[256],name2[256];

    displaystring (" "); // clear the statusline
    options_jump_shots_net_bak = options_jump_shots;     // for restore jump_shots after net game
    options_net_compatible_bak = options_net_compatible; // for restore net compatible mode
    options_net_speed_bak = options_net_speed;           // for restore net_speed after net game
    if(client == NULL || g_socket == NULL) {
      return(0);
    }
    if(socket_read(g_socket,net_data,10)!=10) {
      displaystring (localeText[254]);
      close_listener(); //failure - terminating
      return (0);
    }
    //fprintf(stderr,"Read Server\n");
    if(atoi(net_data)!=42) {
      displaystring (localeText[255]);
      close_listener(); //not the correct connection - terminating
      return (0);
    }
    strcpy(net_data,"7"); // answer the server
    //fprintf(stderr,"Send 7 to Server\n");
    if(socket_write(g_socket,net_data,10)!=10) {
      displaystring (localeText[256]);
      close_listener(); //failure - terminating
      return (0);
   }
    sprintf(net_data,"%u",(unsigned int)sizeof(VMfloat));
#ifdef VMATH_SINGLE_PRECISION
    if(socket_write(g_socket,"1",10)!=10) {
#else
     if(socket_write(g_socket,"0",10)!=10) {
#endif
      displaystring (localeText[254]);
      close_listener(); //failure - terminating
      return (0);
    }
    if(socket_read(g_socket,net_data,10)!=10) {
      displaystring (localeText[252]);
      close_listener(); //failure - terminating
      return (0);
    }
    //fprintf(stderr,"Read Server connection speed\n");
    options_net_speed = atoi(net_data);
    if(options_net_speed <NET_SPEED_LOW) { options_net_speed = NET_SPEED_LOW; }
    if(options_net_speed >NET_SPEED_HIGH) { options_net_speed = NET_SPEED_HIGH; }

    if(socket_read(g_socket,net_data,10)!=10) {
      displaystring (localeText[252]);
      close_listener(); //failure - terminating
      return (0);
    }
    //fprintf(stderr,"Read Server network mode speed\n");
    options_net_compatible = atoi(net_data);
    if(options_net_compatible <0) { options_net_compatible =0; }
    if(options_net_compatible >1) { options_net_compatible = 1; }
    if(options_net_compatible) {
       net_send_data = net_send_data_soft;
       net_get_data = net_get_data_soft;
    } else {
       net_send_data = net_send_data_hard;
       net_get_data = net_get_data_hard;
    }

    player[0].queue_view=0;
    player[1].queue_view=1;
    queue_view=0;

    //fprintf(stderr,"Read Game play options\n");
    if(socket_read(g_socket,net_data,700)!=700) {
     displaystring (localeText[257]);
     close_listener();
     return (0);
    }
#ifdef VMATH_SINGLE_PRECISION
    sscanf(net_data,"%i %i %f %i %s %s",&options_jump_shots,(int *)&gametype,&options_table_size,&options_maxp_carambol,name1,name2);
#else
    sscanf(net_data,"%i %i %lf %i %s %s",&options_jump_shots,(int *)&gametype,&options_table_size,&options_maxp_carambol,name1,name2);
#endif
    strcpy_uscore_2_whtspace(player[0].name,name1);
    strcpy_uscore_2_whtspace(player[1].name,name2);
    textObj_setText(player[0].text, player[0].name);
    textObj_setText(player[1].text, player[1].name);
    set_gametype( gametype );  
    create_scene( &balls );
    create_walls( &walls );
    table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
    reassign_and_gen_cuberef_tex();

    player[0].is_AI=0;  /* ###FIXME### maybe one can leave this away someday */
    player[1].is_AI=0;  /* ###FIXME### maybe one can leave this away someday */
    player[1].is_net=0;
    player[0].is_net=1;
    wait_key = 0;
    active_net_game = 1; // now, the network game is active
    return (1);
}

/***********************************************************************
 *          Close all listener etc. for network game                   *
 ***********************************************************************/

void close_listener(void)
{
     if(net_id) {
      SDL_RemoveTimer(net_id);
      net_id = NULL;
     }
     wait_seconds = 0;
     if(active_net_timer) {
      SDL_RemoveTimer(active_net_timer);
      active_net_timer = NULL;
      SDL_Delay(20);
     }
     wait_key = 0;
     wait_client = 0;
     wait_server = 0;
     if(g_socket) {
       net_close_listener(g_socket);
       SDL_Delay(20);
     }
     if(server) {
        net_close_listener(server);
        SDL_Delay(20);
     }
     network_game = no_network;
     active_net_game = 0;
     netorder = 0;
     server = NULL;
     client = NULL;
     g_socket = NULL;
     SDLNet_Quit();
     player[0].is_AI=0;  /* ###FIXME### maybe one can leave this away someday */
     player[1].is_AI=1;  /* ###FIXME### maybe one can leave this away someday */
     player[0].is_net=0;
     player[1].is_net=0;
     options_jump_shots = options_jump_shots_net_bak;      // restore old state of jump_shots
     options_net_compatible = options_net_compatible_bak;  // restore net compatible mode
     options_net_speed = options_net_speed_bak;            // restore net_speed after net game
}

/***********************************************************************
 *            Opens a listener for network game (Client)               *
 *            NULL on failure                                          *
 ***********************************************************************/

void open_client_listener(void)
{
     //TCPsocket socket=NULL;

     if(wait_key) {
        return;
     }
       if((net_id=SDL_AddTimer(1000,wait_for_server_connect,NULL)) != NULL) {
        network_game = play_network;
        wait_key = 1; // stop some key interactions and new game start etc.
        wait_seconds = 120; // how long in seconds to wait
        close_screen();
       } else {
        displaystring (localeText[258]);
       }
}

/***********************************************************************
 *             Opens a listener for network game (Server)              *
 *             NULL on failure                                         *
 ***********************************************************************/

TCPsocket open_server_listener(void)
{
     TCPsocket socket=NULL;
     char host_info[300];
     int i=0;

     if(wait_key) {
        return(NULL);
     }
     restart_game();
     table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
     reassign_and_gen_cuberef_tex();
     if((socket=host_create_socket(NULL))!=NULL) { //network game listen
       network_game = play_network;
       wait_key = 1; // now, wait for connect
       wait_seconds = 120; // how long in seconds to wait
       SDL_Delay(20);
       if((net_id=SDL_AddTimer(1000,wait_for_connect,NULL)) == NULL) {
        //problem with timer, so no network game
        wait_key = 0;
        displaystring (localeText[258]);
        close_listener();
       }
     } else {
       displaystring (localeText[259]);
       close_listener();
     }
     if(socket!=NULL) {
       ip_adresses = get_ip_address();
       ipptr[i] = strtok(ip_adresses,";");
       while(ipptr[i] != NULL && i<8) {
        i++;
        ipptr[i] = strtok(NULL, ";");
       }
       ipptr[++i] = NULL;
       for(i=0;i<9;i++) {
         if(ipptr[i]) {
           textObj_setText( ip_text_obj[i], ipptr[i]);
         }
       }
       sprintf(host_info,"Host: %s",get_hostname());
       textObj_setText( ip2_text_obj,host_info);
     }
     close_screen();
     return(socket);
}

#endif //NETWORKING

/***********************************************************************
 *                   What is choosen in a menu ?                       *
 ***********************************************************************/

void menu_cb( int id, void * arg , VMfloat value)
{
    switch(id){
    case MENU_ID_MSHOOT_NEW:
        options_mouseshoot = 1;
        break;
    case MENU_ID_MSHOOT_CLASSIC:
        options_mouseshoot = 0;
        break;
    case MENU_ID_TOURFAST_NO:
        options_tourfast = 1.0;
        if(options_gamemode==options_gamemode_tournament) {
          tournament_state.ai_fast_motion=options_tourfast;
        }
        break;
    case MENU_ID_TOURFAST_SLOW:
        options_tourfast = 2.0;
        if(options_gamemode==options_gamemode_tournament) {
          tournament_state.ai_fast_motion=options_tourfast;
        }
        break;
    case MENU_ID_TOURFAST_MIDDLE:
        options_tourfast = 5.0;
        if(options_gamemode==options_gamemode_tournament) {
          tournament_state.ai_fast_motion=options_tourfast;
        }
        break;
    case MENU_ID_TOURFAST_QUICK:
        options_tourfast = 10.0;
        if(options_gamemode==options_gamemode_tournament) {
          tournament_state.ai_fast_motion=options_tourfast;
        }
        break;
    case MENU_ID_TOURFAST_ULTRA:
        options_tourfast = 30.0;
        if(options_gamemode==options_gamemode_tournament) {
          tournament_state.ai_fast_motion=options_tourfast;
        }
        break;
    case MENU_ID_MAIN_QUIT:
        save_config();
        sys_exit(0);
        break;
    case MENU_ID_AVATAR_ON:
        options_avatar_on = 1;
        break;
    case MENU_ID_AVATAR_OFF:
        options_avatar_on = 0;
        break;
    case MENU_ID_CONTROLS_ON:
        options_show_buttons = 1;
        break;
    case MENU_ID_CONTROLS_OFF:
        options_show_buttons = 0;
        break;
    case MENU_ID_JUMP_SHOT_ON:
        options_jump_shots = 1;
        restart_game();
        break;
    case MENU_ID_JUMP_SHOT_OFF:
        options_jump_shots = 0;
        restart_game();
        break;
    case MENU_ID_ALIAS_ON:
        options_antialiasing = 1;
#ifndef WETAB
        if(options_fsaa) {
         glEnable(GL_MULTISAMPLE_ARB);
        	options_fsaa_value = 8; //#### FIXME ### fsaa max. 8
        }
#endif
        break;
    case MENU_ID_ALIAS_OFF:
        options_antialiasing = 0;
#ifndef WETAB
        if(options_fsaa) {
          glDisable(GL_MULTISAMPLE_ARB);
        }
#endif
        options_fsaa_value = 0;
        break;
    case MENU_ID_LIGHT_POS:
        options_positional_light = 1;
        break;
    case MENU_ID_LIGHT_DIR:
        options_positional_light = 0;
        break;
    case MENU_ID_STATUS_ON:
        options_status_text = 1;
        break;
    case MENU_ID_STATUS_OFF:
        options_status_text = 0;
        break;
#ifdef USE_SOUND
    case MENU_ID_SOUND_ON:
        options_use_sound = 1;
        break;
    case MENU_ID_SOUND_OFF:
        options_use_sound = 0;
        break;
#endif
    case MENU_ID_BIRDVIEW_AI_ON:
        options_ai_birdview = 1;
        break;
    case MENU_ID_BIRDVIEW_AI_OFF:
        options_ai_birdview = 0;
        break;
    case MENU_ID_MAXP_CARAMBOL:
        options_maxp_carambol = atoi((char*)arg);
        if(options_maxp_carambol <=0 || options_maxp_carambol >100) { 
           options_maxp_carambol = 10;
        }
        break;
    case MENU_ID_MANUAL:
        if(manualthere) {
#ifndef WETAB
          fullscreen = sys_get_fullscreen();
          if(fullscreen) {
#endif
          sys_fullscreen(0);
          SDL_Delay(20);
#ifndef WETAB
          }
#endif
          system(foomanual);
#ifndef WETAB
          if(fullscreen) {
          set_checkkey();
          //wait for sdl_event to transform the window back to fullscreen
          while(checkkey()) { SDL_Delay(100); }
#endif
          sys_fullscreen(1);
          SDL_Delay(20);
#ifndef WETAB
          }
#endif
        }
        break;
    case MENU_RES_REND_LOW:
        options_cuberef_res = 16;
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_RES_REND_MED:
        options_cuberef_res = 32;
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_RES_REND_HIGH:
        options_cuberef_res = 64;
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_RES_REND_VERYHIGH:
        options_cuberef_res = 128;
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_ID_TABLESIZE_7FOOT:
        options_table_size = 7.0*2.54*12.0/100.0;
        create_scene( &balls );
        create_walls( &walls );
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        tableinfov();
        break;
    case MENU_ID_TABLESIZE_8FOOT:
        options_table_size = 8.0*2.54*12.0/100.0;
        create_scene( &balls );
        create_walls( &walls );
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        tableinfov();
        break;
    case MENU_ID_TABLESIZE_9FOOT:
        options_table_size = 9.0*2.54*12.0/100.0;
        create_scene( &balls );
        create_walls( &walls );
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        tableinfov();
        break;
    case MENU_ID_TABLESIZE_12FOOT:
        options_table_size = 11.708*2.54*12.0/100.0;
        create_scene( &balls );
        create_walls( &walls );
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        tableinfov();
        break;
    case MENU_ID_TABLETHEME_GOLDGREEN:
        options_table_color   = options_table_color_green;
        options_diamond_color = options_diamond_color_gold;
        options_frame_color   = options_frame_color_default;
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        break;
    case MENU_ID_TABLETHEME_GOLDRED:
        options_table_color   = options_table_color_red;
        options_diamond_color = options_diamond_color_gold;
        options_frame_color   = options_frame_color_default;
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        break;
    case MENU_ID_TABLETHEME_CHROMEBLUE:
        options_table_color   = options_table_color_blue;
        options_diamond_color = options_diamond_color_chrome;
        options_frame_color   = options_frame_color_default;
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        break;
    case MENU_ID_TABLETHEME_BLACKBEIGE:
        options_table_color   = options_table_color_beige;
        options_diamond_color = options_diamond_color_black;
        options_frame_color   = options_frame_color_default;
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        break;
    case MENU_ID_TABLETHEME_TRON:
        options_tronmode  = (options_tronmode==0)?1:0;
        break;
    case MENU_ID_ROOM_ON:
        options_deco=1;
        glFogf (GL_FOG_END, 16.0);
        break;
    case MENU_ID_ROOM_OFF:
        options_deco=0;
        glFogf (GL_FOG_END, 12.5);
        break;
    case MENU_ID_FURNITURE_ON:
        options_furniture=1;
        break;
    case MENU_ID_FURNITURE_OFF:
        options_furniture=0;
        break;
    case MENU_ID_HELPLINE_ON:
        vline_on=1;
        break;
    case MENU_ID_HELPLINE_OFF:
        vline_on=0;
        break;
    // Training local Play
    case MENU_ID_TRAINING_8BALL:
        set_gametype( GAME_8BALL );
        options_gamemode=options_gamemode_training;
        restart_game();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_ID_TRAINING_9BALL:
        set_gametype( GAME_9BALL );
        options_gamemode=options_gamemode_training;
        restart_game();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_ID_TRAINING_CARAMBOL:
        set_gametype( GAME_CARAMBOL );
        options_gamemode=options_gamemode_training;
        restart_game();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_ID_TRAINING_SNOOKER:
        set_gametype( GAME_SNOOKER );
        options_gamemode=options_gamemode_training;
        restart_game();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        reassign_and_gen_cuberef_tex();
        break;
    // Tourn local Play
    case MENU_ID_8BALL_TOURN:
        set_gametype( GAME_8BALL );
        options_gamemode=options_gamemode_tournament;
        init_tournament_state(&tournament_state);
        restart_game();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_ID_9BALL_TOURN:
        set_gametype( GAME_9BALL );
        options_gamemode=options_gamemode_tournament;
        init_tournament_state(&tournament_state);
        restart_game();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_ID_CARAMBOL_TOURN:
        set_gametype( GAME_CARAMBOL );
        options_gamemode=options_gamemode_tournament;
        init_tournament_state(&tournament_state);
        restart_game();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_ID_SNOOKER_TOURN:
        set_gametype( GAME_SNOOKER );
        options_gamemode=options_gamemode_tournament;
        init_tournament_state(&tournament_state);
        restart_game();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        reassign_and_gen_cuberef_tex();
        break;
    // Local Play
    case MENU_ID_GAMETYPE_8BALL:
        set_gametype( GAME_8BALL );
        options_gamemode=options_gamemode_match;
        restart_game();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_ID_GAMETYPE_9BALL:
        set_gametype( GAME_9BALL );
        options_gamemode=options_gamemode_match;
        restart_game();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_ID_GAMETYPE_CARAMBOL:
        set_gametype( GAME_CARAMBOL );
        options_gamemode=options_gamemode_match;
        restart_game();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_ID_GAMETYPE_SNOOKER:
        set_gametype( GAME_SNOOKER );
        options_gamemode=options_gamemode_match;
        restart_game();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        reassign_and_gen_cuberef_tex();
        break;
#ifndef TOUCH
    case MENU_ID_FULLSCREEN_ON:
        sys_fullscreen( 1 );
        break;
    case MENU_ID_FULLSCREEN_OFF:
        sys_fullscreen( 0 );
        break;
#endif
    case MENU_ID_RGSTEREO_ON:
        options_rgstereo_on=1;
        delete_queue_texbind();
        create_texbinds(&balls);
        create_queue_texbind();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        break;
    case MENU_ID_RGSTEREO_OFF:
        options_rgstereo_on=0;
        glColorMask(1, 1, 1, 1);
        delete_queue_texbind();
        create_texbinds(&balls);
        create_queue_texbind();
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        break;
    case MENU_ID_RGAIM_LEFT:
        options_rgaim=1;
        break;
    case MENU_ID_RGAIM_RIGHT:
        options_rgaim=2;
        break;
    case MENU_ID_RGAIM_MIDDLE:
        options_rgaim=0;
        break;
    case MENU_ID_LENSFLARE_ON:
        options_lensflare=1;
        break;
    case MENU_ID_LENSFLARE_OFF:
        options_lensflare=0;
        break;
    case MENU_ID_RESTART:
        restart_game();
        break;
    case MENU_ID_CONTROL_KIND_ON:
        options_control_kind = 1;
        break;
    case MENU_ID_CONTROL_KIND_OFF:
        options_control_kind = 0;
        break;
    case MENU_ID_PLAYER1_NAME:
        strcpy(player[0].name,(char *)arg);
        textObj_setText(player[0].text,player[0].name);
        player_copy(&human_player_roster.player[0],player[0]);
        break;
    case MENU_ID_PLAYER2_NAME:
        strcpy(player[1].name,(char *)arg);
        textObj_setText(player[1].text,player[1].name);
        player_copy(&human_player_roster.player[1],player[1]);
        break;
    case MENU_ID_PLAYER1_SKILL_EXCEL:
        player[0].err=0.0;
        player_copy(&human_player_roster.player[0],player[0]);
        break;
    case MENU_ID_PLAYER1_SKILL_GOOD:
        player[0].err=0.1;
        player_copy(&human_player_roster.player[0],player[0]);
        break;
    case MENU_ID_PLAYER1_SKILL_MEDIUM:
        player[0].err=0.3;
        player_copy(&human_player_roster.player[0],player[0]);
        break;
    case MENU_ID_PLAYER1_SKILL_BAD:
        player[0].err=0.6;
        player_copy(&human_player_roster.player[0],player[0]);
        break;
    case MENU_ID_PLAYER1_SKILL_WORSE:
        player[0].err=1.0;
        player_copy(&human_player_roster.player[0],player[0]);
        break;
    case MENU_ID_PLAYER2_SKILL_EXCEL:
        player[1].err=0.0;
        player_copy(&human_player_roster.player[1],player[1]);
        break;
    case MENU_ID_PLAYER2_SKILL_GOOD:
        player[1].err=0.1;
        player_copy(&human_player_roster.player[1],player[1]);
        break;
    case MENU_ID_PLAYER2_SKILL_MEDIUM:
        player[1].err=0.3;
        player_copy(&human_player_roster.player[1],player[1]);
        break;
    case MENU_ID_PLAYER2_SKILL_BAD:
        player[1].err=0.6;
        player_copy(&human_player_roster.player[1],player[1]);
        break;
    case MENU_ID_PLAYER2_SKILL_WORSE:
        player[1].err=1.0;
        player_copy(&human_player_roster.player[1],player[1]);
        break;
    case MENU_ID_PLAYER1_TYPE_HUMAN:
        player[0].is_AI=0;
        player_copy(&human_player_roster.player[0],player[0]);
        break;
    case MENU_ID_PLAYER2_TYPE_HUMAN:
        player[1].is_AI=0;
        player_copy(&human_player_roster.player[1],player[1]);
        break;
    case MENU_ID_PLAYER1_TYPE_AI:
        if(act_player==0){
            player[0].is_AI=1;
            player[0].queue_view=0;
            if(queue_view) toggle_queue_view();
            do_computer_move(1);
        } else {
            player[0].is_AI=1;
            player[0].queue_view=1;
        }
        player_copy(&human_player_roster.player[0],player[0]);
        break;
    case MENU_ID_PLAYER2_TYPE_AI:
        if(act_player==1){
            player[1].is_AI=1;
            player[1].queue_view=0;
            if(queue_view) toggle_queue_view();
            do_computer_move(1);
        } else {
            player[1].is_AI=1;
            player[1].queue_view=1;
        }
        player_copy(&human_player_roster.player[1],player[1]);
        break;
    case MENU_ID_BALL_DETAIL_LOW:
        options_max_ball_detail     = options_max_ball_detail_LOW;
        options_ball_detail_nearmax = options_ball_detail_nearmax_LOW;
        options_ball_detail_farmin  = options_ball_detail_farmin_LOW;
        break;
    case MENU_ID_BALL_DETAIL_MED:
        options_max_ball_detail     = options_max_ball_detail_MED;
        options_ball_detail_nearmax = options_ball_detail_nearmax_MED;
        options_ball_detail_farmin  = options_ball_detail_farmin_MED;
        break;
    case MENU_ID_BALL_DETAIL_HIGH:
        options_max_ball_detail     = options_max_ball_detail_HIGH;
        options_ball_detail_nearmax = options_ball_detail_nearmax_HIGH;
        options_ball_detail_farmin  = options_ball_detail_farmin_HIGH;
        break;
    case MENU_ID_BALL_DETAIL_VERYHIGH:
#ifndef WETAB
        options_max_ball_detail     = options_max_ball_detail_VERYHIGH;
        options_ball_detail_nearmax = options_ball_detail_nearmax_VERYHIGH;
        options_ball_detail_farmin  = options_ball_detail_farmin_VERYHIGH;
#else
        options_max_ball_detail     = options_max_ball_detail_HIGH;
        options_ball_detail_nearmax = options_ball_detail_nearmax_HIGH;
        options_ball_detail_farmin  = options_ball_detail_farmin_HIGH;
#endif
        break;
#ifndef WETAB
    case MENU_ID_VIDMODE:
        {
        sysResolution *mode;
        mode = (sysResolution *)arg;
        sys_resize(mode->w, mode->h);
        }
        break;
    case MENU_ID_BROWSER:
#ifndef WETAB
        strcpy(options_browser,(char *)arg);
        initLanguage(0);
#endif
        break;
#endif
    case MENU_ID_ANISOTROP:
        options_value_anisotrop = value;
        break;
    case MENU_ID_MAIN_HELP:
        close_screen();
        helpscreen_on = 1;
        break;
#ifdef NETWORKING
    case MENU_ID_NETWORK_HOST8:
        set_gametype( GAME_8BALL );
        options_gamemode=options_gamemode_match;
        server = open_server_listener();
        break;
    case MENU_ID_NETWORK_HOST9:
        set_gametype( GAME_9BALL );
        options_gamemode=options_gamemode_match;
        server = open_server_listener();
        break;
    case MENU_ID_NETWORK_HOSTC:
        set_gametype( GAME_CARAMBOL );
        options_gamemode=options_gamemode_match;
        server = open_server_listener();
        break;
    case MENU_ID_NETWORK_HOSTS:
        set_gametype( GAME_SNOOKER );
        options_gamemode=options_gamemode_match;
        server = open_server_listener();
        break;
    case MENU_ID_NETWORK_JOIN:
        open_client_listener();
        close_screen();
        break;
    case MENU_ID_NETWORK_IP:
        strcpy(options_net_hostname,(char *)arg);
        break;
    case MENU_ID_NETSPEED_SLOW:
        options_net_speed = NET_SPEED_LOW;
        break;
    case MENU_ID_NETSPEED_NORMAL:
        options_net_speed = NET_SPEED_NORMAL;
        break;
    case MENU_ID_NETSPEED_MIDDLE:
        options_net_speed = NET_SPEED_MIDDLE;
        break;
    case MENU_ID_NETSPEED_HIGH:
        options_net_speed = NET_SPEED_HIGH;
        break;
    case MENU_ID_NETWORK_PORTNUM:
        sscanf((char *)arg,"%d",&options_net_portnum);
        break;
    case MENU_ID_NETCOMP_ON:  // only, if no netgame is active
        if(!active_net_game) {
           options_net_compatible = 1;
           net_send_data = net_send_data_soft;
           net_get_data = net_get_data_soft;
        }
        break;
    case MENU_ID_NETCOMP_OFF:  // only, if no netgame is active
        if(!active_net_game) {
           options_net_compatible = 0;
           net_send_data = net_send_data_hard;
           net_get_data = net_get_data_hard;
        }
        break;
#endif //NETWORKING
    case MENU_ID_REFLECTION_SPHERE:
        options_cuberef = 0;
        free_cuberef_tex();
        break;
    case MENU_ID_REFLECTION_RENDERED:
        options_cuberef = 1;
#ifdef WETAB
        options_max_ball_detail     = options_max_ball_detail_HIGH;
        options_ball_detail_nearmax = options_ball_detail_nearmax_HIGH;
        options_ball_detail_farmin  = options_ball_detail_farmin_HIGH;
#endif
        reassign_and_gen_cuberef_tex();
        break;
    case MENU_ID_GLASSBALLS:
        options_glassballs = (options_glassballs==0)?1:0;
        break;
    case MENU_ID_BALLTRACE_ON:
        options_balltrace = 1;
        break;
    case MENU_ID_BALLTRACE_OFF:
        options_balltrace = 0;
        break;
    case MENU_ID_BUMPREF_ON:
        options_bumpref = 1;
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        break;
    case MENU_ID_BUMPREF_OFF:
        options_bumpref = 0;
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        break;
    case MENU_ID_BUMPWOOD_ON:
        options_bumpwood = 1;
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        break;
    case MENU_ID_BUMPWOOD_OFF:
        options_bumpwood = 0;
        table_obj = create_table( spheretexbind, &walls, gametype==GAME_CARAMBOL );
        break;
    case MENU_ID_AUTOFREEMOVE_ON:
        options_auto_freemove = 1;
        break;
    case MENU_ID_AUTOFREEMOVE_OFF:
        options_auto_freemove = 0;
        break;
    case MENU_ID_IDLE:
        break;
    }
}

/***********************************************************************
 *                Check for contain a string s1 a value s2?            *
 ***********************************************************************/

int str_contains(char *s1, char *s2)
{
    int i,j;
    int rval=0;

    for( i=0 ; s1[i]!=0 ; i++ ){
        for( j=0 ; s2[j]!=0 && s1[i+j]!=0 && s2[j]==s1[i+j] ; j++ ){
        }
        if(s2[j]==0){
            rval=1; break;
        }

    }
    return rval;
}

/***********************************************************************
 *                 Check the needed OpenGL extensions                  *
 ***********************************************************************/

void parse_gl_extensions_string( void )
{
    char * str;
    str = (char *)glGetString( GL_EXTENSIONS );

    extension_cubemap      = (str_contains(str,"GL_ARB_texture_cube_map"))  ? 1 : 0 ;
    fprintf(stderr,"extension_cubemap_ARB=%d\n",extension_cubemap);

    extension_multitexture = (str_contains(str,"GL_ARB_multitexture"))      ? 1 : 0 ;
    fprintf(stderr,"extension_multitexture_ARB=%d\n",extension_multitexture);

    extension_combine      = (str_contains(str,"GL_ARB_texture_env_combine" ))  ? 1 : 0 ;
    fprintf(stderr,"extension_combine_ARB: %i\n",extension_combine);

    extension_dot3         = (str_contains(str,"GL_ARB_texture_env_dot3" )) ? 1 : 0 ;
    fprintf(stderr,"extension_dot3_ARB: %i\n",extension_dot3);

    extension_vp_ARB       = (str_contains(str,"GL_ARB_vertex_program" )) ? 1 : 0 ;
    fprintf(stderr,"extension_vp_ARB: %i\n",extension_vp_ARB);

    extension_ts_NV        = (str_contains(str,"GL_NV_texture_shader"))     ? 1 : 0 ;
    fprintf(stderr,"extension_ts_NV=%d\n",extension_ts_NV);

    extension_rc_NV        = (str_contains(str,"GL_NV_register_combiners")) ? 1 : 0 ;
    fprintf(stderr,"extension_rc_NV=%d\n",extension_rc_NV);

    extension_vp_NV        = (str_contains(str,"GL_NV_vertex_program"))     ? 1 : 0 ;
    fprintf(stderr,"extension_vp_NV=%d\n",extension_vp_NV);

    options_fsaa           = (str_contains(str,"GL_ARB_multisample" ))      ? 1 : 0 ;
    fprintf(stderr,"options_multisample: %i\n",options_fsaa);

    options_anisotrop      = (str_contains(str,"GL_EXT_texture_filter_anisotropic" )) ? 1 : 0 ;
    fprintf(stderr,"Anisotropic Filter %d.\n",options_anisotrop);
    if(options_anisotrop) {
       glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &options_maxanisotrop);
       //fprintf(stderr,"max. anisotrop: %f\n",options_maxanisotrop);
    }
}

/***********************************************************************
 *        The Init function for the whole game (called once)           *
 ***********************************************************************/

static void Init( void )
{
    GLfloat fogColor[4] = {0.0, 0.0, 0.0, 1.0};

    parse_gl_extensions_string();

    lightpos[0]=vec_xyz(0.0,+0.4,0.7);
    lightpos[1]=vec_xyz(0.0,-0.4,0.7);
    lightnr=2;

    glDisable(GL_DITHER);
    glEnable(GL_TEXTURE_2D);
    create_png_texbind("sphere_map_128x128.png", &spheretexbind, 3, GL_RGB);

#ifdef WETAB
    create_png_texbind("tabletex_wetab_128x128.png", &fblogotexbind, 1, GL_LUMINANCE);
#else
    create_png_texbind("tabletex_fB_128x128.png", &fblogotexbind, 1, GL_LUMINANCE);
#endif

    create_png_texbind("place_cue_ball.png", &placecueballtexbind, 3, GL_RGB);
    create_png_texbind("blende.png", &blendetexbind, 1, GL_LUMINANCE);
    create_png_texbind("lightflare.png", &lightflaretexbind, 1, GL_LUMINANCE);
    create_png_texbind("full_symbol.png", &fullsymboltexbind, 3, GL_RGBA);
    create_png_texbind("half_symbol.png", &halfsymboltexbind, 3, GL_RGBA);
    create_png_texbind("fullhalf_symbol.png", &fullhalfsymboltexbind, 3, GL_RGBA);

// intro graphic

    create_png_texbind("intro.png", &introtexbind, 3, GL_RGBA);

// graphics for the control under the table

    create_png_texbind("b.png", &btexbind, 3, GL_RGBA);
    create_png_texbind("n.png", &ntexbind, 3, GL_RGBA);
    create_png_texbind("shot.png", &stexbind, 3, GL_RGBA);
    create_png_texbind("up.png", &utexbind, 3, GL_RGBA);
    create_png_texbind("down.png", &dtexbind, 3, GL_RGBA);

// white ball left for English moving

    create_png_texbind("english.png", &englishbind, 3, GL_RGBA);

// Cross for the white ball moving

    create_png_texbind("kreuz.png", &kreuzbind, 3, GL_RGBA);

// Graphics for the special keys s,b,e,m,f

    create_png_texbind("s.png", &sbind, 3, GL_RGBA);
    create_png_texbind("b1.png", &bbind, 3, GL_RGBA);
    create_png_texbind("e.png", &ebind, 3, GL_RGBA);
    create_png_texbind("m.png", &mbind, 3, GL_RGBA);
    create_png_texbind("fov.png", &fbind, 3, GL_RGBA);
    create_png_texbind("f.png", &freeviewbind, 3, GL_RGBA);
    create_png_texbind("disc.png", &discbind, 3, GL_RGBA);

    // Graphics for menubar left and right

    create_png_texbind("mleft.png", &mleftbind, 3, GL_RGBA);
    create_png_texbind("mleftnormal.png", &mleftnormalbind, 3, GL_RGBA);
    create_png_texbind("network.png", &networkbind, 3, GL_RGBA);
#ifdef WETAB
    create_png_texbind("mright-wetab.png", &mrightbind, 3, GL_RGBA);
#else
    create_png_texbind("mright.png", &mrightbind, 3, GL_RGBA);
#endif
      glEnable(GL_FOG);
      glFogi (GL_FOG_MODE, GL_LINEAR);
      glHint (GL_FOG_HINT, GL_FASTEST);
      glFogf (GL_FOG_START, 0.0);
      if(options_deco){
        glFogf (GL_FOG_END, 16.0);
      } else {
        glFogf (GL_FOG_END, 12.5);
      }
      glFogfv (GL_FOG_COLOR, fogColor);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    walls.hole=NULL;
    walls.border=NULL;
    create_walls( &walls );
    balls.ball=NULL;
    bakballs.ball=NULL;
    create_scene( &balls );

#ifdef TIME_INTERPOLATE
    g_lastballs.ball=NULL;
    create_scene( &g_lastballs );
    g_drawballs.ball=NULL;
    create_scene( &g_drawballs );
#endif

    table_obj = create_table(spheretexbind, &walls, gametype==GAME_CARAMBOL);
    create_room(&floor_obj,&wall1_2_obj,&wall3_obj,&wall4_c_obj,&carpet_obj);

    /* lighting */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

   //fprintf(stderr,"enabling depth test\n");
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
   glDepthFunc( GL_LEQUAL );

   if(options_cuberef){
       reassign_and_gen_cuberef_tex();
   }

   glEnable(GL_LIGHTING);
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T);

}

/***********************************************************************
 *                               C-Main                                *
 ***********************************************************************/

int main( int argc, char *argv[] )
{

   int auxnr,i;
   int act_option,option_index;

   int confc;
   char ** confv;

   // things for network -standard is no network game set
   // and quick not so compatible hadcoding
#ifdef NETWORKING
   network_game = no_network;
   net_send_data = net_send_data_hard;
   net_get_data = net_get_data_hard;
#endif
#ifdef __MINGW32__	//RB
	  glActiveTextureARB = 0;
#endif

   /* initialize hostname with a default address */
   strcpy(options_net_hostname,"192.168.1.1");

   /* initialize random seed */
   srand(SDL_GetTicks());

   /* Initialize Language and folders */
   initLanguage(1);

   /* Initialize all player variables for two players */
   init_player_roster(&human_player_roster);

#ifdef _MSC_VER //RB For only Windows-MSVC
#else
   print_help(long_options,appname_str);
#endif

   /* config file */
   load_config( &confv, &confc, argv, argc );
   while( ( act_option = getopt_long_only(confc, confv, "+", long_options, &option_index) ) >= 0){
       //fprintf(stderr,"processing option %d=%s\n",act_option,optarg);
       process_option(act_option);
   }

   /* Initialize Manual (not in function before. we use some from the conf-file) */
#ifndef WETAB
   initLanguage(0);
#endif
   sys_create_display( &argc, argv, win_width, win_height);
#ifdef __MINGW32__	//RB
	  glActiveTextureARB = (void *) SDL_GL_GetProcAddress("glActiveTextureARB");
#endif
#ifndef WETAB
   if( fullscreen ) sys_fullscreen( 1 );
#endif
   Init();

   /* Initialize the statusline */
   initstatustext();

   create_human_player_roster_text(&human_player_roster);
   create_players_text();

   if(options_gamemode==options_gamemode_tournament){
       init_tournament_state(&tournament_state);
   }

   /* this is a glory shit at this place. But we need it
      for some things that are loaded from the config-file on startup.
   */
   restart_game();

   glGetIntegerv(GL_AUX_BUFFERS, &auxnr);
   //fprintf(stderr,"# of AUX-buffers:%d\n",auxnr);

   glEnable(GL_LIGHTING);

#ifdef USE_SOUND
   init_sound();
#endif


   //mixaudio(); //checkit!!!! there is a lot ###TODO###

   if(!options_3D_winnertext){
       //wins
       winner_text_obj = textObj_new(localeText[144], options_winner_fontname, 60);
       tourn_winner_obj = textObj_new(localeText[174], options_winner_fontname, 60);
       //hallo
       winner_name_text_obj = textObj_new(localeText[145], options_winner_fontname, 60);
   } else {
       //wins
       winner_text_obj = textObj3D_new(localeText[144], options_winner_fontname, 0.2, 0.05);
       winner_name_text_obj = textObj3D_new(localeText[145], options_winner_fontname, 0.2, 0.05);
       tourn_winner_obj = textObj3D_new(localeText[174], options_winner_fontname, 0.2, 0.05);
   }

   stbar_text_obj = textObj_new("0", options_status_fontname, 16);  // the percent in the strength bar
   seconds_text_obj = textObj_new("0", options_score_fontname, 100);   // show a countdown for network connections
   for(i=0;i<9;i++) {
    ip_text_obj[i] = textObj_new("0", options_help_fontname, 16);   // shows the possible ip-adresses for network play
   }
   ip1_text_obj = textObj_new(localeText[238], options_help_fontname, 20);
   ip2_text_obj = textObj_new("0", options_help_fontname, 16);
   esc_stop_obj = textObj_new(localeText[245], options_help_fontname, 20);
   init_menu();

   // Things for the Intro
#ifdef USE_SOUND
      Playwavdata(cvt_intro.buf,cvt_intro.len_cvt,options_snd_volume);
#endif
   InitMesh();
   sys_main_loop();

   return 0;
}
