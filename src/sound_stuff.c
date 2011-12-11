/* sound_stuff.c
**
**    code for sound sample data
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
#ifdef USE_SOUND

#include "options.h"
#include "sys_stuff.h"

#include <SDL.h>
#include <SDL_mixer.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <string.h>

/***********************************************************************/

typedef struct SoundStruct{
  short int * data;
  int len;  /* length in bytes */
} TSound;

TSound ball_ball_snd;
TSound ball_wall_snd;
TSound ball_cue_snd;

// Placeholder for max. 500 Songs (that's hopefully enough, I think)

char musicfile[500][255];
char playfile[300];
int shufflesong[500];
int songs = 0;
int actualsong = 0;
int give_up = 0;
int music_to_play = 0;
int music_finished = 1;

// Mix_Music actually holds the music information.

Mix_Music *music = NULL;

// Mix_Chunk actually holds the noise information.

Mix_Chunk *ball_hole = NULL;
Mix_Chunk *wave_applause = NULL;
Mix_Chunk *wave_intro = NULL;
Mix_Chunk *wave_shuffle = NULL;
Mix_Chunk *wave_smack = NULL;
Mix_Chunk *wave_bomb = NULL;
Mix_Chunk *wave_oneball = NULL;
Mix_Chunk *wave_outball = NULL;
Mix_Chunk *wave_ooh = NULL;
Mix_Chunk *cue_sound = NULL;
Mix_Chunk *wall_sound = NULL;
Mix_Chunk *ball_sound = NULL;

/***********************************************************************/

/* Internal Routines */

void init_sound(void);
void exit_sound(void);
Mix_Chunk* loadsound (char *filename);
void PlaySound(Mix_Chunk *chunkdata, int volume);
void create_cue_sound(short int ** data, int * len, int length, VMfloat randmul, VMfloat maxrand, VMfloat sinfloat, VMfloat expfloat );
void create_wall_sound(short int ** data, int * len );

/***********************************************************************
 *              Check the given string extension mp3 or ogg            *
 ***********************************************************************/

int strsound ( char s1[] )
{
   int i = 0;
   char s[10];
   if(strlen(s1) > 4) {
      strcpy(s,&s[strlen(s)-4]);
      while (s[i]) {
         s[i] = toupper(s[i]);
         ++i;
      }
 	    if(strcmp(s,".MP3") || strcmp(s,".OGG")) {
         return(1);
 	    }
   }
   return(0);
}

/***********************************************************************
 *                music finished callback function                     *
 ***********************************************************************/

void musicFinished()
{
    music_finished = 1;
}

/***********************************************************************
 *              Play the next song if no music is playing              *
 ***********************************************************************/

void PlayNextSong (void) {
	   if(options_use_music) {
	   	 if (!music_to_play || !music_finished) {
	   		  return;
	   	 }
	   	 if(give_up >= songs) {
	   	 	 give_up = 0;
	   	 	 options_use_music = 0;
	   	 	 fprintf(stderr,"background-music failed too many times. Give up.\n");
	   	 	 return;
	   	 }
	     if(music) {
	   	   Mix_FreeMusic(music);
	   	   music = NULL;
	     }
	     if(++actualsong >= songs) {
	   	   actualsong = 0;
	     }
	     sprintf(playfile,"music/%s",musicfile[shufflesong[actualsong]]);
      if(!(music = Mix_LoadMUS(playfile))) {
         //fprintf(stderr,"background-music %s failed. Goto next one.\n",musicfile[shufflesong[actualsong]]);
         give_up ++;
         PlayNextSong();
      } else {
      	  fprintf(stderr,"Now playing %s.\n",musicfile[shufflesong[actualsong]]);
      	  Mix_PlayMusic(music,0);
         Mix_VolumeMusic(options_mus_volume);
         give_up = 0;
         //fprintf(stderr,"music is%s playing.\n", Mix_PlayingMusic()?"":" not");
         if(Mix_PlayingMusic()) {
         	 //fprintf(stderr,"Wait for next song\n");
           music_finished = 0;
         }
      }
	   }
}

/***********************************************************************
 *                             load sounds                             *
 ***********************************************************************/

Mix_Chunk* loadsound (char *filename) {

	   Mix_Chunk *chunkname;
    if(!(chunkname = Mix_LoadWAV(filename))) {
 	    fprintf(stderr,"Initializing %s failed. No sound in game.\n",filename);
 	    options_use_sound=0;
    }
    return (chunkname);
}

/***********************************************************************
 *                       Initialize the sound system                   *
 ***********************************************************************/

void init_sound(void)
{
    FILE * f;
    DIR * d;
    struct dirent * dp;
    int i,j,k,dummy;
    /* ball-ball sounds from samuele catuzzi's kbilliards - thanx */
    if((f=fopen("ball_ball.raw", "rb"))==NULL){
        fprintf(stderr,"couldn't open ball_ball.raw\n");
        sys_exit(1);
    }
    fseek(f, 0L, SEEK_END);
    ball_ball_snd.len = ftell(f)+1;
    fseek(f, 0L, SEEK_SET);
    ball_ball_snd.data = malloc(ball_ball_snd.len);
    fread(&ball_ball_snd.data[0], 1, ball_ball_snd.len,f );
    fclose(f);

    // Manipulate the ballsounds a little bit (for better sound)
    for(i=0;i<ball_ball_snd.len/2/2;i++){
        ball_ball_snd.data[(i)*2]*=exp(-(VMfloat)i/(VMfloat)(ball_ball_snd.len/2/4));
        ball_ball_snd.data[(i)*2+1]*=exp(-(VMfloat)i/(VMfloat)(ball_ball_snd.len/2/4));
    }
    for(i=0;i<ball_ball_snd.len/2/2-1;i++){
        ball_ball_snd.data[i*2]=ball_ball_snd.data[i*2]*0.7+ball_ball_snd.data[(i+1)*2]*0.3;
        ball_ball_snd.data[i*2+1]=ball_ball_snd.data[i*2+1]*0.7+ball_ball_snd.data[(i+1)*2+1]*0.3;
    }

    create_wall_sound(&ball_wall_snd.data, &ball_wall_snd.len);
    create_cue_sound(&ball_cue_snd.data, &ball_cue_snd.len,2640,0.6,0.4,20.0,220.0); //length 2*2*3*220

    if(Mix_OpenAudio(22050, AUDIO_S16, 2, 1024)) {
      fprintf(stderr,"Unable to open audio!\n");
      options_use_sound=0;
      options_use_music=0;
    } else {
    	 Mix_AllocateChannels(20); // max. 20 Channels
    	 Mix_Volume(-1,MIX_MAX_VOLUME);

    	 // Extended Init for Version higher then SDL 1.2.10
#if SDL_MIXER_MAJOR_VERSION > 2 || (SDL_MIXER_MINOR_VERSION == 2 && SDL_MIXER_PATCHLEVEL > 9)
    	 // load support for the MP3, OGG music formats
    	 int flags=MIX_INIT_OGG|MIX_INIT_MP3;
    	 int initted=Mix_Init(flags);
    	 if(initted&flags != flags) {
    	     fprintf(stderr,"Mix_Init: Failed to init required ogg and mp3 support!\n");
    	     fprintf(stderr,"Mix_Init: %s\n", Mix_GetError());
    	     options_use_music=0;
    	 }

#endif
   	  /* Actually loads up the sounds */
    	 ball_hole = loadsound ("ballinhole.wav");
    	 wave_applause = loadsound ("applause.wav");
    	 wave_intro = loadsound ("intro.wav");
    	 wave_shuffle = loadsound ("shuffleballs.wav");
    	 wave_smack = loadsound ("smack.wav");
    	 wave_bomb = loadsound ("bomb.wav");
    	 wave_oneball = loadsound ("oneballontable.wav");
    	 wave_outball = loadsound ("balloutoftable.wav");
    	 wave_ooh = loadsound ("ooh.wav");
    	 if(!(ball_sound = Mix_QuickLoad_RAW((Uint8*)ball_ball_snd.data,ball_ball_snd.len))) {
    	 	fprintf(stderr,"Initializing internal ball-sound failed. No sound in game\n");
    	 	options_use_sound=0;
    	 }
    	 if(!(wall_sound = Mix_QuickLoad_RAW((Uint8*)ball_wall_snd.data,ball_wall_snd.len))) {
    	 	fprintf(stderr,"Initializing internal wall-sound failed. No sound in game\n");
    	 	options_use_sound=0;
    	 }
    	 if(!(cue_sound = Mix_QuickLoad_RAW((Uint8*)ball_cue_snd.data,ball_cue_snd.len))) {
    	 	fprintf(stderr,"Initializing internal cue-sound failed. No sound in game\n");
    	 	options_use_sound=0;
    	 }

   	  /* Actually loads up the music */
#if SDL_MIXER_MAJOR_VERSION > 2 || (SDL_MIXER_MINOR_VERSION == 2 && SDL_MIXER_PATCHLEVEL > 9)
    	 if(initted&flags == flags) {
#endif
    	 if((d = opendir("music"))) {
    	 	 i = 0;
   	    while ((dp = readdir(d))!= NULL && i <= 500) {
   	    	 if(strsound(dp->d_name)) {
   	    	 	  strcpy(musicfile[i++],dp->d_name);
   		        //fprintf(stderr,"%s\n", musicfile[i-1]);
   	    	 }
   	    }
   	    closedir(d);
   	    if(i>0) { //music to play
          if(i == 1) { // only one song, so play forever
          	sprintf(playfile,"music/%s",musicfile[0]);
          	if((music = Mix_LoadMUS(playfile))) {
          		  Mix_FadeInMusic(music,-1,4000);
          		  Mix_VolumeMusic(options_mus_volume);
          	}
          } else {
          	songs = i;
          	if(i > 499) {
          		  fprintf(stderr,"Max. 500 Songs are playable. Only the first 500 Songs are considered.\n");
          		  songs = 500;
          	}
          	// shuffle the playing-list
          	for (i=0;i<songs;i++) {
          		 shufflesong[i] = i;
          	}
          	for (k = 0; k < songs - 1; k++) {
          	  j = k + rand() / (RAND_MAX / (songs - k) + 1);
          	  dummy = shufflesong[j];
          	  shufflesong[j] = shufflesong[k];
          	  shufflesong[k] = dummy;
          	}
          	//for(i=0;i<songs;i++) {	fprintf(stderr,"%i, ",shufflesong[i]);	}
          	music_to_play = 1;
           PlayNextSong();
           Mix_HookMusicFinished(musicFinished);
          }
   	    } else {
   	    	 options_use_music=0;
   	    }
    	 } else {
   	     fprintf(stderr,"Initializing background-music failed. No background-music in game\n");
   	     options_use_music=0;
    	 }
#if SDL_MIXER_MAJOR_VERSION > 2 || (SDL_MIXER_MINOR_VERSION == 2 && SDL_MIXER_PATCHLEVEL > 9)
    	 }
#endif

    }
}

/***********************************************************************
 *                    plays sound with volume set                      *
 ***********************************************************************/
void PlaySound(Mix_Chunk *chunkdata, int volume)
{
	   if(options_use_sound) {
	   	 if (chunkdata != NULL) {
	   	   //fprintf(stderr,"Volume: %i\n",volume);
	       Mix_VolumeChunk(chunkdata, volume);
	       Mix_PlayChannel(-1, chunkdata, 0);
	   	 }
	   }
}


/***********************************************************************
 *                      close the sound system                         *
 ***********************************************************************/
void exit_sound(void)
{
#if SDL_MIXER_MAJOR_VERSION > 2 || (SDL_MIXER_MINOR_VERSION == 2 && SDL_MIXER_PATCHLEVEL > 9)
	   Mix_Quit();
#endif
	   Mix_CloseAudio();
}

/***********************************************************************
 *      Generates the sound for the cue. There is no file for it!      *
 ***********************************************************************/

void create_cue_sound(short int ** data, int * len, int length, VMfloat randmul, VMfloat maxrand, VMfloat sinfloat, VMfloat expfloat )
{
    int i;
    *len=length;
    *data=(short int *)malloc(*len);
    for(i=0;i<(*len)/2/2;i++){
        (*data)[i*2]=32000.0*(randmul*(VMfloat)rand()/(VMfloat)RAND_MAX+(maxrand)*sin((VMfloat)i/sinfloat*2.0*M_PI))*exp(-(VMfloat)i/expfloat);
        (*data)[i*2+1]=(*data)[i*2];
    }
    for(i=1;i<(*len)/2/2;i++){
        (*data)[i*2]=0.5*(*data)[i*2]+0.5*(*data)[(i-1)*2];
        (*data)[i*2+1]=(*data)[i*2];
    }
    for(i=30;i<(*len)/2/2;i++){
        (*data)[i*2]=0.7*(*data)[i*2]+0.3*(*data)[(i-30)*2];
        (*data)[i*2+1]=(*data)[i*2];
    }
}

/***********************************************************************
*   Generates the sound for the ball-wall. There is no file for it!    *
************************************************************************/

void create_wall_sound(short int ** data, int * len)
{
    int i;
    create_cue_sound(data,len,5584,0.1,0.9,220.0,465.0); // length 2*2*3*465+2*2
    for(i=0;i<(*len)/2/2;i++){
        (*data)[i*2]*=exp(-(VMfloat)(i)/40.0*(VMfloat)(i)/40.0);
        (*data)[i*2+1]*=exp(-(VMfloat)(i)/40.0*(VMfloat)(i)/40.0);
    }
}

#endif  /* #ifdef USE_SOUND */
