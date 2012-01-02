/* sys_stuff.c
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

#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#ifdef USE_WIN
  #include <windows.h>
  #include <shellapi.h>
#else
  #include <sys/stat.h>
#endif

#ifdef NETWORKING
  #include <SDL/SDL_net.h>
#endif
#include "sound_stuff.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include "sys_stuff.h"
#include "billard3d.h"

/***********************************************************************/

static char browser[256];
static int fullscreen = 0;
static int keymodif =0;
static int vidmode_bpp=0;
static int sdl_on = 0;
static int check_SDL;           // check for mousebutton for manual from fullscreen
static int ignore = 0;          // SDL bug set videomode calls reshape event twice SDL 1.2.8 and > ?
SDL_Surface * vid_surface = NULL;

/***************************************************
 *    replace a string (max. 2048 Bytes long)       *
 ***************************************************/

char *replace(char *st, char *orig, char *repl) {
  static char buffer[2048];
  char *ch;
  if (!(ch = strstr(st, orig)))
   return st;
  strncpy(buffer, st, ch-st);
  buffer[ch-st] = 0;
  sprintf(buffer+(ch-st), "%s%s", repl, ch+strlen(orig));
  return buffer;
  }

/***********************************************************************
 *                    copy binary a file                               *
 ***********************************************************************/

int filecopy(char *filefrom,char *fileto)
{
  FILE *from, *to;
  char ch;

  /* open source file */
  if((from = fopen(filefrom, "rb"))==NULL) {
  	 fprintf(stderr,"Error: open source file (%s) for copy\n",filefrom);
    return(0);
  }

  /* open destination file */
  if((to = fopen(fileto, "wb"))==NULL) {
  	 fprintf(stderr,"Error: open destination file (%s) for copy\n",fileto);
    return(0);
  }

  /* copy the file */
  while(!feof(from)) {
    ch = fgetc(from);
    if(ferror(from)) {
      return(0);
    }
    if(!feof(from)) fputc(ch, to);
    if(ferror(to)) {
      return(0);
    }
  }

  fclose(from);

  if(fclose(to)==EOF) {
    return(0);
  }
  return(1);
}

/***********************************************************************
 *                    init internetbrowser in string                   *
 *                    get it from options or os                        *
 * WeTab Standard Browser can't show xml, xsl data. So another         *
 * must be used here..... and have to store in the options             *
 ***********************************************************************/

void init_browser(void) {
#ifdef USE_WIN
  char *cp;
  GetModuleFileName(NULL,browser,sizeof(browser));
  if((cp = strrchr(browser,'\\'))) { //extract the program name from path
    cp[0] = 0;
  }
  strcat(browser,"\\data\\");
#else
  if(!strcmp(options_browser,"browser")) {
    strcpy(options_browser,"./browser.sh");
  }
  sprintf(browser,"%s file://",options_browser);
#endif
}

/***********************************************************************
 *                    copy internetbrowser in string                   *
 *                    string length minimum 256                        *
 ***********************************************************************/

void get_browser(char *strpointer) {
	 strcpy(strpointer,browser);
}

/***********************************************************************
 *          Transparent mousecursor for touch-devices (WETAB)          *
 *    We don't use SDL_Showcursor which is not really on function      *
 *                          on some devices                            *
 ***********************************************************************/

#ifdef TOUCH
  static Uint8 cursorMask[16] = { 0 };
  static Uint8 cursorData[16] = { 0 };
  static SDL_Cursor* cursor;
#endif

/***********************************************************************
 *                      Exit with SDL-Support                          *
 ***********************************************************************/

void sys_exit( int code )
{
  if (sdl_on) {
    /*
     * Quit SDL so we can release the fullscreen
     * mode and restore the previous video settings,
     * etc.
     */
     save_config(); //save the config (must!!!)
#ifdef USE_SOUND
     exit_sound();
#endif
#ifdef NETWORKING
      SDLNet_Quit();  //in case of open Netgame
#endif
      SDL_Quit( );
  }

  /* Exit program. */
  exit( code );
}

/***********************************************************************
 *      Initialize SDL and make a SDL-Window / Fullscreen              *
 ***********************************************************************/

void sys_create_display(int width,int height)
{
  /* Information about the current video settings. */
  const SDL_VideoInfo* info = NULL;
  int vidmode_flags=0, samplingerror = 0;

  /* First, initialize SDL's video subsystem. */
#ifdef USE_SOUND
  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0 ) {
    fprintf( stderr, "Video or Audio initialization failed: %s\n",
#else
  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 ) {
    fprintf( stderr, "Video initialization failed: %s\n",
#endif
    SDL_GetError( ) );
    sys_exit(1);
  }

  sdl_on = 1 ; 

  /* Let's get some video information. */
  info = SDL_GetVideoInfo( );
  
  if( !info ) {
    /* This should probably never happen. */
    fprintf( stderr, "Video query failed: %s\n",
    SDL_GetError( ) );
    sys_exit(1);
  }
  
  vidmode_bpp = info->vfmt->BitsPerPixel;

  /*
   * Now, we want to setup our requested
   * window attributes for our OpenGL window.
   * We want *at least* 5 bits of red, green
   * and blue. We also want at least a 16-bit
   * depth buffer.
   *
   * The last thing we do is request a VMfloat
   * buffered window. '1' turns on VMfloat
   * buffering, '0' turns it off.
   *
   * Note that we do not use SDL_DOUBLEBUF in
   * the flags to SDL_SetVideoMode. That does
   * not affect the GL attribute state, only
   * the standard 2D blitting setup.
   */

  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  if (SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ) <0) {
  	 fprintf(stderr, "SDL_GL_DOUBLEBUFFER error: %s\n", SDL_GetError());
  	 options_vsync = 0;
  } else {
#ifdef __APPLE__
	long swapInterval = 1;
	CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &swapInterval);
#endif
//compile without errors, if SDL is < Version 1.2.10 at compile time
#if SDL_MAJOR_VERSION == 1 && SDL_MINOR_VERSION == 2 && SDL_PATCHLEVEL > 9
// The next works only with fsaa options off!!!!
  if(!options_fsaa_value) {
	   if(SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 ) < 0 ) {
				  fprintf( stderr, "Unable to guarantee accelerated visual with libSDL < 1.2.10\n");
	   }
  }
  if(vsync_supported()) {
  	 if(options_vsync) {
      if (SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1) < 0) { // since SDL v1.2.10
        fprintf(stderr, "SDL_GL_SWAP_CONTROL error: %s\n", SDL_GetError());
        options_vsync = 0;
      }
  	 }
  } else {
    fprintf(stderr,"SDL-System without control of vsync. Scrolling may stutter\n");
  }
#endif
  }

#ifndef WETAB
  if(options_fsaa_value) {
    samplingerror = SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1);
    if(!samplingerror) {
      samplingerror = SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES,options_fsaa_value);
    }
  }
  if (samplingerror == -1) {
#endif
    options_fsaa_value = 0;
#ifndef WETAB
  }
#endif
  SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL );
  /* key repeat caused problem when toggling fullscreen !!! */

  vidmode_flags = SDL_OPENGL;

  if ( info->hw_available ) {
    vidmode_flags |= SDL_HWSURFACE;
    vidmode_flags |= SDL_HWPALETTE; /* Store the palette in hardware */
  } else {
    vidmode_flags |= SDL_SWSURFACE;
  }

  if ( info->blit_hw ) { /* checks if hardware blits can be done */
    vidmode_flags |= SDL_HWACCEL;
  }
#ifndef WETAB
    if (fullscreen) {
      vidmode_flags |= SDL_FULLSCREEN;
    }else{
#endif
      vidmode_flags |= SDL_RESIZABLE;
#ifndef WETAB
  }
#endif

  //Set the window icon
  SDL_WM_SetIcon(SDL_LoadBMP("icon.bmp"),NULL);

#ifndef WETAB
  while (vid_surface == NULL) {
#endif
   if((vid_surface=SDL_SetVideoMode( width, height, vidmode_bpp, vidmode_flags )) == NULL) {
    if(!options_fsaa_value) {
     fprintf( stderr, "Video mode set failed: %s\nPlease restart Foobillard++\n", SDL_GetError());
     sys_exit(1);
    }
#ifndef WETAB
    fprintf( stderr, "Video mode set failed: %s\nSwitch to other mode\n", SDL_GetError());
    if(options_fsaa_value) {
      options_fsaa_value >>= 1;
      fprintf(stderr,"FSAA %i\n",options_fsaa_value);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, options_fsaa_value);
    } else {
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,0);
    }
#endif
   }
#ifndef WETAB
  }
  // Check new settings for better output the next line is a must for multisample
  if(!samplingerror && options_fsaa_value){
     glEnable(GL_MULTISAMPLE);
     //glHint(GL_MULTISAMPLE_FILTER_HINT_NV,GL_NICEST); //be careful (Nvidia-specific), set over an option ?
  }
#endif
  SDL_WM_SetCaption("Foobillardplus","Foobillardplus");
#ifdef TOUCH
  cursor = SDL_CreateCursor(cursorData, cursorMask, 16, 16, 0, 0);
  SDL_SetCursor (cursor);
  sys_fullscreen(1);
#endif

  glPolygonMode(GL_FRONT,GL_FILL);  // fill the front of the polygons
  glPolygonMode(GL_BACK,GL_LINE);   // only lines for back (better seeing on zooming)
  glCullFace(GL_BACK);              // Standards for rendering only front of textures
  glEnable(GL_CULL_FACE);
}

/***********************************************************************
 *                          Fullscreen active ?                        *
 ***********************************************************************/

int sys_get_fullscreen(void)
{
    return fullscreen;
}

/**************************************************************************
 *            Set a fullscreen(1) or window(0) window                     *
 * SDL_WM_ToggleFullScreen(screen) works only on X11 and there not stable *
 **************************************************************************/

void sys_fullscreen( int fullscr )
{

#ifdef USE_WIN
	   // MS-Windows and SDL 1.2 with OpenGL are not really friends
	   // and at the time I don't want to rebuild the whole OpenGL context
	   // so only a window resize to fullscreen and back is done
	   SDL_SysWMinfo info;
	   SDL_VERSION(&info.version);
	   SDL_GetWMInfo(&info);
	   if(fullscr) {
	      ShowWindow(info.window, SW_MAXIMIZE);
	   } else {
       ShowWindow(info.window, SW_RESTORE);
	   }
	   fullscreen = fullscr;
#else
    SDL_Surface * screen;
    Uint32 flags;

    screen = SDL_GetVideoSurface();
    flags = screen->flags; /* Save the current flags in case toggling fails */
    SDL_EnableKeyRepeat( 0, 0 );
    if ( fullscr!=0 && (screen->flags & SDL_FULLSCREEN)==0 ){
    	   screen = SDL_SetVideoMode( 0, 0, 0, screen->flags | SDL_FULLSCREEN );
    } else if( fullscr==0 && (screen->flags & SDL_FULLSCREEN)!=0 ){
    	   screen = SDL_SetVideoMode( 0, 0, 0, screen->flags & ~SDL_FULLSCREEN);
    }
    if(screen == NULL) {
    	   screen = SDL_SetVideoMode(0, 0, 0, flags); /* If toggle FullScreen failed, then switch back */
    } else {
    	   fullscreen = fullscr;
    }
    SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL );
    if(screen == NULL) {
    	   fprintf(stderr,"Video-Error on set full-screen/windowed mode. Terminating\n");
    	   sys_exit(1); /* If you can't switch back for some reason, then epic fail */
    }
#endif
}

/***********************************************************************
 *          Toggle between Fullscreen and windowed mode                *
 ***********************************************************************/

void sys_toggle_fullscreen( void )
{
    if (fullscreen){
        sys_fullscreen( 0 );
    } else {
        sys_fullscreen( 1 );
    }
}

/***********************************************************************
 *        Update the keystroke modifiers (alt, strg etc.)              *
 ***********************************************************************/

static void update_key_modifiers(void)
{
  SDLMod m ;
  m=SDL_GetModState();
  keymodif=0 ;
  if (KMOD_CTRL  & m) keymodif |= KEY_MODIFIER_CTRL ;
  if (KMOD_SHIFT & m) keymodif |= KEY_MODIFIER_SHIFT ;
  if (KMOD_ALT   & m) keymodif |= KEY_MODIFIER_ALT ;
   
}

/***********************************************************************
 *                    handle for the mouse buttons                     *
 ***********************************************************************/

static void handle_button_event(SDL_MouseButtonEvent *e)
{
  MouseButtonEnum b ;
  MouseButtonState s ;

  update_key_modifiers() ;

  /* then the mouse buttons */
  switch(e->button) {
  case SDL_BUTTON_LEFT:   
    b = MOUSE_LEFT_BUTTON; 
    break ;
  case SDL_BUTTON_RIGHT: 
    b = MOUSE_RIGHT_BUTTON;
    break ;
  case SDL_BUTTON_MIDDLE: 
    b = MOUSE_MIDDLE_BUTTON;
    break ;
  case 4:
    b = MOUSE_WHEEL_UP_BUTTON;
    break ;
  case 5:
    b = MOUSE_WHEEL_DOWN_BUTTON;
    break ;
  default:
    /* Unknown button: ignore */
    return ; 
  }

  s = -1;
  if(e->state==SDL_PRESSED)  s=MOUSE_DOWN;
  if(e->state==SDL_RELEASED) s=MOUSE_UP;
  
  MouseEvent(b,s,e->x,e->y) ;
}

/***********************************************************************
 *        Translate the keystrokes from SDL for foobillard++           *
 ***********************************************************************/

static int translate_key(SDL_KeyboardEvent* e)
{
  int keysym=0;

  switch (e->keysym.sym) {
  case SDLK_PAGEUP:
    keysym = KSYM_PAGE_UP ;
    break;
  case SDLK_UP:
    keysym = KSYM_UP ;
    break;
  case SDLK_PAGEDOWN:
    keysym = KSYM_PAGE_DOWN ;
    break;
  case SDLK_DOWN:
    keysym = KSYM_DOWN ;
    break;
  case SDLK_LEFT:
    keysym = KSYM_LEFT ;
    break;
  case SDLK_RIGHT:
    keysym = KSYM_RIGHT ;
    break;
  case SDLK_F1:
    keysym = KSYM_F1 ;    
    break;

  case SDLK_F2:
    keysym = KSYM_F2 ;
    break;
  case SDLK_F3:
    keysym = KSYM_F3 ;
    break;
  case SDLK_F4:
    keysym = KSYM_F4 ;
    break;
  case SDLK_F5:
    keysym = KSYM_F5 ;
    break;
  case SDLK_F6:
    keysym = KSYM_F6 ;
    break;
  case SDLK_F7:
    keysym = KSYM_F7 ;
    break;
  case SDLK_F8:
    keysym = KSYM_F8 ;
    break;
  case SDLK_F9:
    keysym = KSYM_F9 ;
    break;
  case SDLK_F10:
    keysym = KSYM_F10 ;
    break;
  case SDLK_F11:
    keysym = KSYM_F11 ;
    break;
  case SDLK_F12:
    keysym = KSYM_F12 ;
    break;
  case SDLK_KP_ENTER:
    keysym = KSYM_KP_ENTER ;
    break;
  default:
    //fprintf(stderr,"%i\n",e->keysym.sym);
    if (e->keysym.sym>0 && e->keysym.sym<=127) {
      keysym = (int) e->keysym.sym ;
      if((e->keysym.mod & KMOD_LSHIFT) || (e->keysym.mod & KMOD_RSHIFT) || (e->keysym.mod & KMOD_CAPS)) {
        if(keysym >= SDLK_a && keysym <= SDLK_z) {
           keysym = keysym-32;
        }
      }
    } else {
      /* ignore */
      return -1;
    }
  }
  return keysym;
}

/***********************************************************************
 *                  Handle for keystroke down                          *
 ***********************************************************************/

static void handle_key_down(SDL_KeyboardEvent* e)
{
  int keysym;

  update_key_modifiers();
  keysym = translate_key(e);
  if(keysym!=-1){
      Key(keysym, keymodif);
  }
}

/***********************************************************************
 *                         Handle for key up                           *
 ***********************************************************************/

static void handle_key_up(SDL_KeyboardEvent* e)
{
  int keysym;

  update_key_modifiers();
  keysym = translate_key(e);
  if(keysym!=-1){
      KeyUp(keysym);
  }
}

/***********************************************************************
 *                Resize the SDL Surface handle                        *
 ***********************************************************************/

void sys_resize( int width, int height, int callfrom )
{

    SDL_Surface * screen;
    Uint32 flags;

    if(width < 958) width = 958;      // don't resize below this
    if(height < 750) height = 750;
    ignore = callfrom;
    screen = SDL_GetVideoSurface();
    flags = screen->flags; /* Save the current flags in case toggling fails */
    SDL_EnableKeyRepeat( 0, 0 );
    screen = SDL_SetVideoMode( width, height, screen->format->BitsPerPixel, screen->flags);
    SDL_Delay(300);
    //fprintf(stderr,"Called x: %i y: %i\n",width,height);
    SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL );
    if(screen == NULL) {
    	   screen = SDL_SetVideoMode(0, 0, 0, flags); /* If failed, then switch back */
    }
    SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL );
    if(screen == NULL) {
    	   fprintf(stderr,"Video-Error on window resize. Terminating\n");
    	   sys_exit(1); /* If you can't switch back for some reason, then epic fail */
    }
    ResizeWindow(width,height);
}

/***********************************************************************
 *              Handle for the reshape event of SDL                    *
 ***********************************************************************/

static void handle_reshape_event( int width, int height ) 
{
	  if(!ignore) {
     sys_resize( width, height, 0 );
	  }
	  ignore = 0;
}

/***********************************************************************
 *                 work for the SDL mousemotion event                  *
 ***********************************************************************/

void handle_motion_event(SDL_MouseMotionEvent *e) 
{
  update_key_modifiers();
  MouseMotion(e->x,e->y);
}

/***********************************************************************
 *                     Process all the SDL events                      *
 ***********************************************************************/

static void  process_events( void )
{
  /* Our SDL event placeholder. */
  SDL_Event event;

    /* Grab all the events off the queue. */
  while( SDL_PollEvent( &event ) ) 
   {
    switch( event.type ) {
      case SDL_KEYUP:
        handle_key_up( &event.key );
	       break;
      case SDL_KEYDOWN:
        /* Handle key presses. */
        handle_key_down( &event.key );
	       break;
      case SDL_QUIT:
	       /* Handle quit requests (like Ctrl-c). */
	       sys_exit(0);
	       break;
      case SDL_MOUSEMOTION:
        handle_motion_event(&(event.motion)) ;
	       break ;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
	       handle_button_event(&(event.button)) ;
	       check_SDL = 0;
        break ;
      case SDL_VIDEORESIZE:
        handle_reshape_event(event.resize.w,event.resize.h);
        break;
      default:
        //	fprintf( stderr,"EVENT: %d\n", (int) event.type ) ;
        break;
    }
   }
}

/***********************************************************************
 *   set and return SDL_Event status for manual from fullscreen        *
 ***********************************************************************/

void set_checkkey(void) {
    check_SDL = 1;
}

int checkkey(void) {
	   process_events();
    return(check_SDL);
}

/***********************************************************************
 *           get all resolution modes for SDL/OpenGL                   *
 ***********************************************************************/

sysResolution *sys_list_modes( void ) {
    sysResolution * sysmodes;
    SDL_Rect ** modes;
    int i, modenr;

    modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
    for(i=0;modes[i];i++);
    modenr=i;
    sysmodes = (sysResolution *) malloc((modenr+1)*sizeof(sysResolution));
    for(i=0;modes[i];i++){
        sysmodes[i].w = modes[i]->w;
        sysmodes[i].h = modes[i]->h;
    }
    sysmodes[i].w=0;  /* terminator */
    sysmodes[i].h=0;  /* terminator */

    return( sysmodes );
}

/***********************************************************************
 *                            SDL main loop                            *
 ***********************************************************************/

void sys_main_loop(void) {
  // we want a good smooth scrolling
  GLint old_t, t;
  GLint sleeptime;

  old_t = SDL_GetTicks();
  while(1) {
  	 if(options_vsync) {
       process_events();
       DisplayFunc();
       SDL_GL_SwapBuffers();
    } else {
       process_events();
       DisplayFunc();
       SDL_GL_SwapBuffers();
       t = SDL_GetTicks();
       sleeptime = 15-(t-old_t); //wish sleeptime is 15 milliseconds
       old_t = t;
       if(sleeptime > 0) {
         SDL_Delay(sleeptime);
       }
    }
  }

}

/***********************************************************************
 *      Find the program's "data" directory and chdir into it          *
 ***********************************************************************/

static char data_dir[512];

void enter_data_dir() {
    int success = 1;

#ifdef POSIX
    char proc_exe[20];
    char *slash_pos;
#endif

    do {
        success = 0;

#ifdef POSIX
        snprintf(proc_exe, sizeof(proc_exe), "/proc/%d/exe", getpid());
        if (readlink(proc_exe, data_dir, sizeof(data_dir)) < 0) {
            perror("readlink failed");
            break;
        }

        // Remove program name
        slash_pos = strrchr(data_dir, '/');
        if (!slash_pos) break;
        *slash_pos = '\0';

        // Go one dir up
        slash_pos = strrchr(data_dir, '/');
        if (!slash_pos) break;

        // Add "/data"
        strncpy(slash_pos, "/data", sizeof(data_dir) - (slash_pos - data_dir));
#else
        /* ### TODO ### Get the working directory of the program
         * Mac OS X: _NSGetExecutablePath() (man 3 dyld)
         * Solaris: getexecname()
         * FreeBSD: sysctl CTL_KERN KERN_PROC KERN_PROC_PATHNAME -1
         * BSD with procfs: readlink /proc/curproc/file
         * Windows: GetModuleFileName() with hModule = NULL
         */
        strncpy(data_dir, "data", sizeof(data_dir));
#endif

        if (chdir(data_dir) < 0) {
            break;
        }

        success = 1;
    } while (0);

    if (!success) {
        fprintf(stderr,
            "Foobillard++ seems not to be correctly installed\n"
            "Cannot find valid data directory\n"
            "(assuming the current directory contains the data)\n");
    }
}

/***********************************************************************
 *           returns the "data" directory and chdir into it            *
 ***********************************************************************/

const char *get_data_dir() {
#ifdef POSIX
    return data_dir;
#else
    return ".";
#endif
}

/***********************************************************************
 *      Check whether a given file exists                              *
 ***********************************************************************/

int file_exists(const char *path) {
#ifdef POSIX
    struct stat buf;
    return stat(path, &buf) == 0;
#else
    FILE *fp = fopen(path, "r");
    if (!fp) return 0;
    fclose(fp);
    return 1;
#endif
}

/***********************************************************************
 *      Launch an external command                                     *
 ***********************************************************************/

int launch_command(const char *command) {
#ifdef USE_WIN
	   ShellExecute(NULL,"open",command,NULL,NULL,SW_SHOWNORMAL);
	   return (0);
#else
    return system(command);
#endif
}

/***********************************************************************
 *      check if vsync is supported with SDL                           *
 ***********************************************************************/

int vsync_supported(void) {
//compile without errors, if SDL is < Version 10 at compile time
#if SDL_MAJOR_VERSION == 1 && SDL_MINOR_VERSION == 2 && SDL_PATCHLEVEL > 9
	   SDL_version v;

    v = *SDL_Linked_Version();
    if(v.major == 1 && v.minor == 2 && v.patch > 9) {
    	 return 1;
    }
#endif
    options_vsync = 0; //if not supported by SDL turn every time off
    return 0;
}
