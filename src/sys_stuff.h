/* sys_stuff.h
**
**    code for system-behaviour
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

#ifndef SYS_STUFF_H
#define SYS_STUFF_H

#include "vmath.h"

typedef enum 
{
  MOUSE_LEFT_BUTTON=1,
  MOUSE_RIGHT_BUTTON=2,
  MOUSE_MIDDLE_BUTTON=3,
  MOUSE_WHEEL_UP_BUTTON=4,
  MOUSE_WHEEL_DOWN_BUTTON=5
} MouseButtonEnum ;


typedef enum 
{
  MOUSE_UP=1,
  MOUSE_DOWN=2
} MouseButtonState ;

#define KEY_MODIFIER_SHIFT 0x01
#define KEY_MODIFIER_CTRL  0x02
#define KEY_MODIFIER_ALT   0x04

enum {
  KSYM_UP=257 ,
  KSYM_DOWN ,
  KSYM_LEFT ,
  KSYM_RIGHT ,
  KSYM_PAGE_DOWN ,
  KSYM_PAGE_UP ,
  KSYM_F1 ,
  KSYM_F2 ,
  KSYM_F3 ,
  KSYM_F4 ,
  KSYM_F5 ,
  KSYM_F6 ,
  KSYM_F7 ,
  KSYM_F8 ,
  KSYM_F9 ,
  KSYM_F10 ,
  KSYM_F11 ,
  KSYM_F12 ,
  KSYM_KP_ENTER,
  KSYM_QUIT
} ;

typedef struct{
    int w, h;
} sysResolution;


// system functions 

void set_checkkey(void);
int checkkey(void);
void sys_create_display(int width,int height, int _fullscreen) ;
int  sys_get_fullscreen(void);
void sys_fullscreen( int fullscr);
void sys_toggle_fullscreen( void );
void sys_main_loop(void) ;
void sys_resize( int width, int height, int callfrom);
void sys_redisplay(void) ;
void sdl_exit( void ) ;
void sys_exit( int code ) ;
void sys_fullscreen( int fullscr );
void init_browser(void);
void get_browser(char *strpointer);
int filecopy(char *filefrom,char *fileto);
char *replace(char *st, char *orig, char *repl);
int vsync_supported(void);
sysResolution * sys_list_modes(void);

void enter_data_dir();
const char *get_data_dir();
const char *get_prog();
int file_exists(const char *path);
void error_print(char *error_message, char *error_extend);
int get_dialogprog(void);

int launch_command(const char *command);

#endif  /* SYS_STUFF_H */
