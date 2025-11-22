/* sys_stuff.c
**
**    code for system-behaviour
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

#define _POSIX_C_SOURCE 200809L

#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#ifdef USE_WIN
  #include <windows.h>
  #include <shellapi.h>
#else
  #include <sys/stat.h>
#endif

#ifdef NETWORKING
  #include <SDL2/SDL_net.h>
#endif
#include "sound_stuff.h"
#ifdef __APPLE__
 #include <OpenGL/OpenGL.h>
 #include <OpenGL/gl.h>
 #include <OpenGL/glu.h>
 #include <OpenGL/glext.h>
 #include <CoreFoundation/CoreFoundation.h>
#else
 #include <GL/gl.h>
 #include <GL/glu.h>
 #include <GL/glext.h>
#endif
#include "sys_stuff.h"
#include "billard3d.h"

/***********************************************************************/

static char browser[256];
static int fullscreen = 0;
static int keymodif =0;
static int sdl_on = 0;
static int check_SDL;           // check for mousebutton for manual from fullscreen
static int ignore = 0;          // SDL bug set videomode calls reshape event twice SDL 1.2.8 and > ?
static SDL_Window *main_window = NULL;
static SDL_GLContext main_glcontext = NULL;
// Note: vid_surface is no longer needed but keep it for now for compatibility
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

/***************************************************
 *          Split a string with delimeter          *
 ***************************************************/
struct split
{
  char *pointers[512];
  int count;
};

struct split split (char *in, char delim)
{
  struct split sp;
  sp.count = 1;
  sp.pointers[0] = in;

  while (*++in) {
    if (*in == delim) {
      *in = 0;
      sp.pointers[sp.count++] = in+1;
    }
  }
  return sp;
}

/***************************************************
 * Get dialog program (deal with path environment  *
 * return index to the program array               *
 * -1 = error                                      *
 * 0 = gnome zenity                                *
 * 1 = kde kdialog                                 *
 * 2 = X11 xmessage                                *
 ***************************************************/

int get_dialogprog(void) {
    char path[2048];
    char file[1024];
    int i;
    struct split sp;
    char *env_path = getenv("PATH");

    if(env_path == NULL) {
        fprintf(stderr, "PATH environment variable not set\n");
        return(-1);
    }
    strcpy(path, env_path);
    fprintf(stderr,"Check for Dialog-Program\n");
      // extract every path and check for zenity or kdialog
      sp = split(path, ':');
      for (i=0; i<sp.count; i++) {
        snprintf(file,sizeof(file),"%s/%s",sp.pointers[i],"zenity");
        if(file_exists(file)) {
            fprintf(stderr,"Dialog Program zenity found\n");
            return(0);
        }
        snprintf(file,sizeof(file),"%s/%s",sp.pointers[i],"kdialog");
        if(file_exists(file)) {
            fprintf(stderr,"Dialog Program kdialog found\n");
            return(1);
        }
      }
      // extract every path and check last for xmessage
      sp = split(path, ':');
      for (i=0; i<sp.count; i++) {
        snprintf(file,sizeof(file),"%s/%s",sp.pointers[i],"xmessage");
        if(file_exists(file)) {
            fprintf(stderr,"Only Dialog Program xmessage found\n");
            return(2);
        }
      }
    return(-1);
}

/***************************************************
 *  print an error string (max. 2048 Bytes long)   *
 ***************************************************/

void error_print(char *error_message, char *error_extend) {

    char message[2048];

    if(error_extend) {
	snprintf(message,sizeof(message),error_message,error_extend);
    } else {
	snprintf(message,sizeof(message),"%s",error_message);
    }
    fprintf(stderr,"%s\n",message); // print error to stderr every time
#ifdef USE_WIN
    MessageBox(0,message,"Foobillard++ Error",MB_OK);
#else
#ifdef __APPLE__
  // needs -framework CoreFoundation
    SInt32 nRes = 0;
    CFUserNotificationRef pDlg = NULL;
    const void* keys[] = { kCFUserNotificationAlertHeaderKey,
    kCFUserNotificationAlertMessageKey };
    const void* vals[] = {
	CFSTR("Foobillard++ Error"),
	CFStringCreateWithCString(NULL, message, kCFStringEncodingMacRoman)
    };
    if(!sys_get_fullscreen()) {
	// display a dialog window only if fullscreen is not active
	CFDictionaryRef dict = CFDictionaryCreate(0, keys, vals,
		    sizeof(keys)/sizeof(*keys),
		    &kCFTypeDictionaryKeyCallBacks,
		    &kCFTypeDictionaryValueCallBacks);
	pDlg = CFUserNotificationCreate(kCFAllocatorDefault, 0,
			kCFUserNotificationPlainAlertLevel,
			&nRes, dict);
    }
#else
    char *dialog_prog[] = {"zenity --error --text=\"%s\"","kdialog --error \"%s\"","xmessage -center %s"};
    char newmessage[2048];
    // display a dialog window only if fullscreen is not active
    if(dialog>=0 && dialog < 2 && !sys_get_fullscreen()) {
	snprintf(newmessage,sizeof(newmessage),dialog_prog[dialog],message);
	system(newmessage);
    }
#endif
#endif
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
 *                         Exit SDL-Support                            *
 ***********************************************************************/

void sdl_exit()
{
    if (sdl_on) {
        save_config();
#ifdef USE_SOUND
        exit_sound();
#endif
#ifdef NETWORKING
        SDLNet_Quit();
#endif
        if(main_glcontext) {
            SDL_GL_DeleteContext(main_glcontext);
            main_glcontext = NULL;
        }
        if(main_window) {
            SDL_DestroyWindow(main_window);
            main_window = NULL;
        }
        SDL_Quit();
        sdl_on = 0;
    }
}

/***********************************************************************
 *                      Exit with SDL-Support                          *
 ***********************************************************************/

void sys_exit( int code )
{
 	sdl_exit();
  exit( code );
}

/***********************************************************************
 *      Initialize SDL and make a SDL-Window / Fullscreen              *
 ***********************************************************************/

void sys_create_display(int width,int height,int _fullscreen)
{
    fullscreen = _fullscreen;
    int samplingerror = 0;
    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

    /* First, initialize SDL's video subsystem. */
#ifdef USE_SOUND
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0 ) {
        error_print("Video or Audio initialization failed: %s", (char*)SDL_GetError());
#else
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 ) {
        error_print("Video initialization failed: %s", (char*)SDL_GetError());
#endif
        sys_exit(1);
    }

    sdl_on = 1;

    /* Set OpenGL attributes before window creation */
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif

    /* VSync support */
    if(options_vsync) {
        SDL_GL_SetSwapInterval(1);
    } else {
        SDL_GL_SetSwapInterval(0);
    }

#ifndef WETAB
    /* FSAA/Multisample support */
    if(options_fsaa_value) {
        samplingerror = SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1);
        if(!samplingerror) {
            samplingerror = SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, options_fsaa_value);
        }
    }
    if (samplingerror == -1) {
#endif
        options_fsaa_value = 0;
        fprintf(stderr,"FSAA Multisample not available\n");
#ifndef WETAB
    }
#endif

    /* Set window flags */
#ifndef WETAB
    if (fullscreen) {
        window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    } else {
#ifndef __APPLE__
        window_flags |= SDL_WINDOW_RESIZABLE;
#endif
    }
#else
    window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif

    /* Enable key repeat */
    // Note: SDL 2.0 handles this differently, but we'll keep it for now

#ifndef WETAB
    if(options_fsaa_value > options_maxfsaa) {
        options_fsaa_value = options_maxfsaa;
    }
    
    /* Try different FSAA values if window creation fails */
    while (main_window == NULL && options_fsaa_value >= 0) {
#endif
        /* Create window */
        main_window = SDL_CreateWindow(
            "Foobillardplus",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            window_flags
        );

        if(main_window == NULL) {
            if(!options_fsaa_value) {
                error_print("Window creation failed. Please restart Foobillard++", NULL);
                sys_exit(1);
            }
#ifndef WETAB
            fprintf(stderr, "Window creation failed: %s\nSwitch to other mode\n", SDL_GetError());
            if(options_fsaa_value) {
                options_fsaa_value >>= 1;
                fprintf(stderr,"FSAA %i\n", options_fsaa_value);
                SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, options_fsaa_value);
            } else {
                SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
                break;  // Stop trying, will fail next iteration anyway
            }
        } else {
            break;  // Success!
#endif
        }
#ifndef WETAB
    }
#endif

    /* Create OpenGL context */
    if(main_window) {
        main_glcontext = SDL_GL_CreateContext(main_window);
        if(!main_glcontext) {
            error_print("OpenGL context creation failed: %s", (char*)SDL_GetError());
            SDL_DestroyWindow(main_window);
            main_window = NULL;
            sys_exit(1);
        }
    }

    /* Apply VSync setting */
    if(options_vsync) {
        SDL_GL_SetSwapInterval(1);
    }

#ifndef WETAB
    /* Check and enable FSAA if requested */
    if(!samplingerror && options_fsaa_value) {
        fprintf(stderr,"Attempt to initialize FSAA Multisample (Antialiasing)\n");
        glEnable(GL_MULTISAMPLE);
    }
#endif

    /* Set window icon */
    SDL_Surface *icon = SDL_LoadBMP("icon.bmp");
    if(icon) {
        SDL_SetWindowIcon(main_window, icon);
        SDL_FreeSurface(icon);
    }

#ifdef TOUCH
    cursor = SDL_CreateCursor(cursorData, cursorMask, 16, 16, 0, 0);
    SDL_SetCursor(cursor);
    sys_fullscreen(1);
#endif

    /* OpenGL initialization */
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    
    /* For compatibility - set vid_surface to a dummy value */
    vid_surface = (SDL_Surface*)1;  // Non-NULL to indicate success
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
#ifdef __APPLE__
    // would need to rebuild context for toggling fullscreen
    fullscreen = fullscr;
#elif defined(USE_WIN)
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
    /* SDL 2.0 fullscreen toggle */
    if(main_window) {
        if(fullscr != 0) {
            SDL_SetWindowFullscreen(main_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            fullscreen = 1;
        } else {
            SDL_SetWindowFullscreen(main_window, 0);
            fullscreen = 0;
        }
    }
#endif
}

/***********************************************************************
 *          Toggle between Fullscreen and windowed mode                *
 ***********************************************************************/

void sys_toggle_fullscreen( void )
{
    if (fullscreen){
        sys_fullscreen(0);
    } else {
        sys_fullscreen(1);
    }
}

/***********************************************************************
 *        Update the keystroke modifiers (alt, strg etc.)              *
 ***********************************************************************/

static void update_key_modifiers(void)
{
  SDL_Keymod m;
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
#ifdef __APPLE__
    // would need to reload whole opengl context just like with fullscreen toggling
#else
    if(width < 958) width = 958;      // don't resize below this
    if(height < 750) height = 750;
    
    ignore = callfrom;
    
    if(main_window) {
        SDL_SetWindowSize(main_window, width, height);
        SDL_Delay(300);
    }
    
    ResizeWindow(width, height);
#endif
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
    SDL_Event event;

    while( SDL_PollEvent( &event ) ) 
    {
        switch( event.type ) {
            case SDL_KEYUP:
                handle_key_up( &event.key );
                break;
            case SDL_KEYDOWN:
                handle_key_down( &event.key );
                break;
            case SDL_QUIT:
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
            case SDL_WINDOWEVENT:  // CHANGED: was SDL_VIDEORESIZE
                if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    handle_reshape_event(event.window.data1, event.window.data2);
                }
                break;
            // OPTIONAL: Add touch support for Android
            case SDL_FINGERDOWN:
            case SDL_FINGERUP:
            case SDL_FINGERMOTION:
                // TODO: Convert touch to mouse events for Android
                break;
            default:
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

sysResolution *sys_list_modes(void) 
{
    sysResolution * sysmodes;
    int display_index = 0;  // Primary display
    int num_modes = SDL_GetNumDisplayModes(display_index);
    int i;

    sysmodes = (sysResolution *) malloc((num_modes + 1) * sizeof(sysResolution));
    
    for(i = 0; i < num_modes; i++) {
        SDL_DisplayMode mode;
        if(SDL_GetDisplayMode(display_index, i, &mode) == 0) {
            sysmodes[i].w = mode.w;
            sysmodes[i].h = mode.h;
        }
    }
    
    sysmodes[i].w = 0;  /* terminator */
    sysmodes[i].h = 0;  /* terminator */

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
       SDL_GL_SwapWindow(main_window);
    } else {
       process_events();
       DisplayFunc();
       SDL_GL_SwapWindow(main_window);
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
 *      and the program executable and directory to it
 ***********************************************************************/

static char data_dir[512];
static char exe_prog[512];

void enter_data_dir() {
    int success = 1;

#ifdef POSIX
    char proc_exe[20];
    char *slash_pos;
#endif

    do {
        success = 0;
#ifdef USE_WIN
        GetModuleFileName(NULL,exe_prog,sizeof(exe_prog));
#endif
#ifdef __APPLE__
        char *get_mac_data_directory();
        char *data_directory = get_mac_data_directory();

        strncpy(data_dir, data_directory, sizeof(data_dir));
        strncpy(exe_prog, data_directory, sizeof(exe_prog));
        free(data_directory);
#elif defined(POSIX)
        snprintf(proc_exe, sizeof(proc_exe), "/proc/%d/exe", getpid());
        if (readlink(proc_exe, data_dir, sizeof(data_dir)) < 0) {
            perror("readlink failed");
            break;
        }
        strncpy(exe_prog, data_dir, sizeof(exe_prog));
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
        //check for Linux Default Directory if possible
#ifdef USE_DEBIAN
        if(!chdir(DATA_DIRECTORY)){
           strncpy(data_dir, DATA_DIRECTORY, sizeof(data_dir));
        } else {
#endif
           fprintf(stderr,
            "Foobillard++ seems not to be correctly installed\n"
            "Cannot find valid data directory\n"
            "(assuming the current directory contains the data)\n");
#ifdef USE_DEBIAN
        }
#endif
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
 *           returns the "exe" directory and applicationname           *
 ***********************************************************************/

const char *get_prog() {
    return exe_prog;
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

int vsync_supported(void) 
{
    // SDL 2.0 always supports vsync control via SDL_GL_SetSwapInterval
    return 1;
}
