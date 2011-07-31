/* menu.h
**
**    code for Menu-Handling
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

#ifndef MENU_H
#define MENU_H

#include "textobj.h"
#include "vmath.h"

#define MAX_MENU_ENTRY_NUM 50

enum entryType {
    ENTRY_TYPE_ID,
    ENTRY_TYPE_SUBMENU,
    ENTRY_TYPE_TEXTFIELD,
    ENTRY_TYPE_EXIT
};

enum keyType {
    KEY_TYPE_NOTHING,
    KEY_TYPE_NUM,
    KEY_TYPE_TEXT,
    KEY_TYPE_MIXED,
    KEY_TYPE_NETADDRESS
};

struct menu_entry_struct;
struct menu_struct;

typedef struct menu_entry_struct menuEntry;
typedef struct menu_struct menuType;

/*typedef*/ struct menu_entry_struct{

    char            text[256];
    char            settingtext[256];
    enum entryType  type;
    menuType *      submenu;
    int             id;
    char *          fontname;
    int             fontsize;
    textObj *       text_obj;
    char *          help_line;
    int             show_subsetting;
    void *          arg;
    int             fixedlen; /* fixed length that may not be changed in textfields */
    int             modus;    // see enum keyType
    int             minvalue; // minvalue for num
    int             maxvalue; // maxvalue for num
    int             maxfield; // max. nun m of char for input
    int             value; /* a value to work with it */

}/* menuEntry*/;

/*typedef */struct menu_struct{

    menuEntry   entry[MAX_MENU_ENTRY_NUM];
    int         nr;
    void        (* callback)( int, void * ,VMfloat);
    int         select_index;
    int         select_id;
    int *       p_select_id;
    char *      fontname;
    int         fontsize;
    int         textedit_mode;
    menuType *  parent;
    menuEntry * parent_entry;

}/* menuType*/;

enum menu_ID {

    MENU_RES_REND_LOW,
    MENU_RES_REND_MED,
    MENU_RES_REND_HIGH,
    MENU_RES_REND_VERYHIGH,
    MENU_ID_LIGHT_POS,
    MENU_ID_LIGHT_DIR,
    MENU_ID_ALIAS_ON,
    MENU_ID_ALIAS_OFF,
    MENU_ID_JUMP_SHOT_ON,
    MENU_ID_JUMP_SHOT_OFF,
    MENU_ID_AVATAR_ON,
    MENU_ID_AVATAR_OFF,
    MENU_ID_STATUS_ON,
    MENU_ID_STATUS_OFF,
    MENU_ID_GAMETYPE_8BALL,
    MENU_ID_8BALL_TOURN,
    MENU_ID_TRAINING_8BALL,
    MENU_ID_GAMETYPE_9BALL,
    MENU_ID_9BALL_TOURN,
    MENU_ID_TRAINING_9BALL,
    MENU_ID_MAXP_CARAMBOL,
    MENU_ID_GAMETYPE_CARAMBOL,
    MENU_ID_CARAMBOL_TOURN,
    MENU_ID_TRAINING_CARAMBOL,
    MENU_ID_GAMETYPE_SNOOKER,
    MENU_ID_SNOOKER_TOURN,
    MENU_ID_TRAINING_SNOOKER,
    MENU_ID_ANISOTROP,
    MENU_ID_SOUND_ON,
    MENU_ID_SOUND_OFF,
    MENU_ID_MANUAL,
    MENU_ID_RESTART,
    MENU_ID_CONTROL_KIND_ON,
    MENU_ID_CONTROL_KIND_OFF,
    MENU_ID_BIRDVIEW_AI_ON,
    MENU_ID_BIRDVIEW_AI_OFF,
    MENU_ID_CONTROLS_ON,
    MENU_ID_CONTROLS_OFF,

    MENU_ID_VIDMODE,

    MENU_ID_BUMPREF_ON,
    MENU_ID_BUMPREF_OFF,

    MENU_ID_BUMPWOOD_ON,
    MENU_ID_BUMPWOOD_OFF,

    MENU_ID_BALLTRACE_ON,
    MENU_ID_BALLTRACE_OFF,

    MENU_ID_REFLECTION_SPHERE,
    MENU_ID_REFLECTION_RENDERED,
    MENU_ID_GLASSBALLS,

    MENU_ID_BALL_DETAIL_LOW,
    MENU_ID_BALL_DETAIL_MED,
    MENU_ID_BALL_DETAIL_HIGH,
    MENU_ID_BALL_DETAIL_VERYHIGH,

    MENU_ID_RGAIM_MIDDLE,
    MENU_ID_RGAIM_LEFT,
    MENU_ID_RGAIM_RIGHT,

    MENU_ID_RGSTEREO_OFF,
    MENU_ID_RGSTEREO_ON,

    MENU_ID_LENSFLARE_OFF,
    MENU_ID_LENSFLARE_ON,

    MENU_ID_FULLSCREEN_ON,
    MENU_ID_FULLSCREEN_OFF,

    MENU_ID_TABLETHEME_GOLDGREEN,
    MENU_ID_TABLETHEME_GOLDRED,
    MENU_ID_TABLETHEME_CHROMEBLUE,
    MENU_ID_TABLETHEME_BLACKBEIGE,
    MENU_ID_TABLETHEME_TRON,

    MENU_ID_TABLESIZE_7FOOT,
    MENU_ID_TABLESIZE_8FOOT,
    MENU_ID_TABLESIZE_9FOOT,
    MENU_ID_TABLESIZE_12FOOT,

    MENU_ID_HELPLINE_ON,
    MENU_ID_HELPLINE_OFF,

#ifdef NETWORKING
    MENU_ID_NETWORK_HOST8,
    MENU_ID_NETWORK_HOST9,
    MENU_ID_NETWORK_HOSTC,
    MENU_ID_NETWORK_HOSTS,
    MENU_ID_NETWORK_JOIN,
    MENU_ID_NETWORK_IP,
    MENU_ID_NETWORK_PORTNUM,
    MENU_ID_NETSPEED_SLOW,
    MENU_ID_NETSPEED_NORMAL,
    MENU_ID_NETSPEED_MIDDLE,
    MENU_ID_NETSPEED_HIGH,
    MENU_ID_NETCOMP_ON,
    MENU_ID_NETCOMP_OFF,
#endif

    MENU_ID_MAIN_RESUME,
    MENU_ID_MAIN_HELP,
    MENU_ID_MAIN_QUIT,

    MENU_ID_PLAYER1_NAME,
    MENU_ID_PLAYER2_NAME,
    MENU_ID_PLAYER1_TYPE_AI,
    MENU_ID_PLAYER2_TYPE_AI,
    MENU_ID_PLAYER1_TYPE_HUMAN,
    MENU_ID_PLAYER2_TYPE_HUMAN,
    MENU_ID_PLAYER1_SKILL_EXCEL,
    MENU_ID_PLAYER1_SKILL_GOOD,
    MENU_ID_PLAYER1_SKILL_MEDIUM,
    MENU_ID_PLAYER1_SKILL_BAD,
    MENU_ID_PLAYER1_SKILL_WORSE,
    MENU_ID_PLAYER2_SKILL_EXCEL,
    MENU_ID_PLAYER2_SKILL_GOOD,
    MENU_ID_PLAYER2_SKILL_MEDIUM,
    MENU_ID_PLAYER2_SKILL_BAD,
    MENU_ID_PLAYER2_SKILL_WORSE,

    MENU_ID_TOURFAST_NO,
    MENU_ID_TOURFAST_SLOW,
    MENU_ID_TOURFAST_MIDDLE,
    MENU_ID_TOURFAST_QUICK,
    MENU_ID_TOURFAST_ULTRA,

    MENU_ID_MSHOOT_NEW,
    MENU_ID_MSHOOT_CLASSIC,

    MENU_ID_BROWSER,

    MENU_ID_AUTOFREEMOVE_ON,
    MENU_ID_AUTOFREEMOVE_OFF,

    MENU_ID_IDLE
};

enum optionType {
    OPT_PLAYER1,
    OPT_PLAYER2,
    OPT_NAME1,
    OPT_NAME2,
    OPT_HELP,
    OPT_8BALL,
    OPT_9BALL,
    OPT_CARAMBOL,
    OPT_SNOOKER,
    OPT_CHROMEBLUE,
    OPT_GOLDGREEN,
    OPT_GOLDRED,
    OPT_BLACKBEIGE,
    OPT_TRONMODE,
    OPT_TABLESIZE,
    OPT_LENSFLARE,
    OPT_NOLENSFLARE,
    OPT_POSLIGHT,
    OPT_DIRLIGHT,
    OPT_AI1ERR,
    OPT_AI2ERR,
    OPT_BALLDETAIL,
    OPT_GLASSBALL,
    OPT_RGSTEREO,
    OPT_RGAIM,
    OPT_HOSTADDR,
    OPT_PORTNUM,
#ifndef WETAB
    OPT_GEOMETRY,
    OPT_FULLSCREEN,
    OPT_BROWSER,
#endif
#ifdef NETWORKING
    OPT_NET_SPEED,
    OPT_NET_COMPATIBLE,
#endif
    OPT_FREEMOVE,
    OPT_CUBEREF,
    OPT_CUBERES,
    OPT_BUMPREF,
    OPT_BUMPWOOD,
    OPT_BALLTRACE,
    OPT_GAMEMODE,
    OPT_AVATAR,
    OPT_TOURFAST,
    OPT_SHOW_BUTTONS,
    OPT_JUMP_SHOTS,
    OPT_ANTIALIASING,
    OPT_STATUSTEXT,
    OPT_USE_SOUND,
    OPT_MAXP_CARAMBOL,
    OPT_CONTROL_KIND,
    OPT_AI_BIRDVIEW,
    OPT_VALUE_ANISOTROP,
    OPT_MSHOOT,
    OPT_FREEMOVE1,
    OPT_FSAA,
    OPT_DUMMY
};

menuType * menu_new( void (* callback)( int, void *, VMfloat ) );
void menu_add_submenu( menuType * menu, char * text, menuType * submenu, int show_subsetting, char * subtext, char * help_line );
void menu_add_entry( menuType * menu, char * text, int id, char * help_line );
void menu_add_arg_entry( menuType * menu, char * text, int id, void * arg, char * help_line );
void menu_add_value_entry( menuType * menu, char * text, int id, VMfloat value, char * help_line );
void menu_add_textfield( menuType * menu, char * text, int id, int fixedlen , int modus, int minvalue, int maxvalue, int maxfield, char * help_line);
void menu_add_exit( menuType * menu, char *text, char * help_line );
void menu_select_by_coord( menuType * menu, int x, int y );
int menu_choose_by_coord( menuType * menu, int x, int y );
void menu_select_next( menuType * menu );
void menu_select_prev( menuType * menu );
void menu_choose(menuType ** menu);
void menu_exit(menuType ** menu);
void menu_text_keystroke( menuType * menu, int key );
void menu_draw( menuType * menu );
void menu_texObj_cleanup(menuType * menu);
void init_menu(void);

#endif /* MENU_H */
