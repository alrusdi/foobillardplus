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

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>    // us time measure
#include <getopt.h>
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#ifdef NETWORKING
  #include <SDL/SDL_net.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include "sys_stuff.h"
#include "options.h"

/***********************************************************************/

static int fullscreen = 0;
static int keymodif =0;
static int vidmode_bpp=0;
static int sdl_on = 0;
SDL_Surface * vid_surface = NULL;

/***********************************************************************
 *        New transparent mousecursor for touch-devices (WETAB)        *
 *    We don't use SDL_Showcursor which is not really on function      *
 *                          on some devices                            *
 ***********************************************************************/

#ifdef WETAB
  static Uint8 cursorMask[16] = { 0 };
  static Uint8 cursorData[16] = { 0 };
  static SDL_Cursor* cursor;
#endif

/***********************************************************************
 *                 gives back time in microseconds                     *
 ***********************************************************************/

int time_us()
{
#ifndef _WIN32
    struct timeval tv;
    struct timezone tz;
    tz.tz_minuteswest = 0;
    tz.tz_dsttime     = 0;
    gettimeofday(&tv,&tz);
    return ( tv.tv_sec*1000000+tv.tv_usec );
#else
    struct timeb t;
    return( t.time*1000000+t.millitm*1000 );
#endif
}

/***********************************************************************
 *                     gives back time in seconds                      *
 ***********************************************************************/

VMfloat time_s()

{
    return (((VMfloat)time_us())*1E-6);
}

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
      Key(KSYM_QUIT, 0); //save config and so on
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

void sys_create_display(int *argc, char **argv, int width,int height)
{
  /* Information about the current video settings. */
  const SDL_VideoInfo* info = NULL;
  int vidmode_flags=0, samplingerror = 0;

  /* First, initialize SDL's video subsystem. */
  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 ) {
    fprintf( stderr, "Video initialization failed: %s\n",
    SDL_GetError( ) );
    sys_exit(1);
  }
  atexit(SDL_Quit);

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
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

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
#ifdef WETAB
  cursor = SDL_CreateCursor(cursorData, cursorMask, 16, 16, 0, 0);
  SDL_SetCursor (cursor);
  sys_fullscreen(1);
#endif

  glPolygonMode(GL_FRONT,GL_FILL);  // fill the front of the polygons
  glPolygonMode(GL_BACK,GL_LINE);   // only lines for back (better seeing on zooming)
}

/***********************************************************************
 *                          Fullscreen active ?                        *
 ***********************************************************************/

int sys_get_fullscreen(void)
{
    return fullscreen;
}

/***********************************************************************
 *            Set a fullscreen(1) or window(0) window                  *
 ***********************************************************************/

void sys_fullscreen( int fullscr )
{
    SDL_Surface * screen;

    fullscreen = fullscr;
    screen = SDL_GetVideoSurface();
//#ifdef WETAB
    // show Mousecursor
//    SDL_ShowCursor(SDL_ENABLE);
//#endif
    if ( fullscreen!=0 && (screen->flags & SDL_FULLSCREEN)==0 ){
        SDL_WM_ToggleFullScreen(screen);
    } else if( fullscreen==0 && (screen->flags & SDL_FULLSCREEN)!=0 ){
        SDL_WM_ToggleFullScreen(screen);
    }
//#ifdef WETAB
    // hide Mousecursor
//    SDL_Delay(20);
//    SDL_ShowCursor(SDL_DISABLE);
//#endif
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
  
  MouseEvent(b,s,e->x,e->y,keymodif) ; 
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
      KeyUp(keysym, keymodif);
  }
}

/***********************************************************************
 *                Resize the SDL Surface handle                        *
 ***********************************************************************/

void sys_resize( int width, int height )
{
    if(width < 958) width = 958;      // don't resize below this
    if(height < 750) height = 750;
    SDL_Surface * screen;
    screen = SDL_GetVideoSurface();
    SDL_EnableKeyRepeat( 0, 0 );
    SDL_SetVideoMode( width, height, screen->format->BitsPerPixel, screen->flags);
    SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL );
    ResizeWindow(width,height) ;
}

/***********************************************************************
 *              Handle for the reshape event of SDL                    *
 ***********************************************************************/

static void handle_reshape_event( int width, int height ) 
{
    sys_resize( width, height );
}

/***********************************************************************
 *                 work for the SDL mousemotion event                  *
 ***********************************************************************/

void handle_motion_event(SDL_MouseMotionEvent *e) 
{
  update_key_modifiers();
  MouseMotion(e->x,e->y,keymodif);
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
 *           get all resolution modes for SDL/OpenGL                   *
 ***********************************************************************/

sysResolution *sys_list_modes( void )
{
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

void sys_main_loop(void)
{
  /* These are to calculate our fps */
  //static GLint T0 = 0;
  //static GLint Frames = 0;
  //GLint t;
  //GLfloat seconds, fps;
  /* End of calculate fps */

  while(1) {
    process_events();
    DisplayFunc();
    // the following two lines only on problems with old SDL-Versions on some machines
    //glFlush();
    //glFinish();
    SDL_GL_SwapBuffers();
    /* Calculate fps */
    //Frames++;
    //t = SDL_GetTicks ();
    //if ( t - T0 >= 5000 ) {
    //  seconds = ( t - T0 ) / 1000.0;
    //  fps = Frames / seconds;
    //  fprintf ( stderr,"%d frames in %g seconds = %g FPS\n", Frames, seconds, fps );
    //  T0 = t;
    //  Frames = 0;
    //}
    /* End calculate fps */
    SDL_Delay(15); //### TODO ### make this delay better with other code
  }

}
