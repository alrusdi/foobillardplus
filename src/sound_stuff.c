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
#include <SDL_audio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/***********************************************************************/

#define NUM_SOUNDS 20

#ifndef SOUND_NULLOFFS
  #define SOUND_NULLOFFS 10000
#endif

#define TSoundENTRY(snd,index) (( (index)<(snd).len/2 && (index)>=0 )?(snd).data[(index)]:0)

typedef struct SoundStruct{
  short int * data;
  int len;  /* length in bytes */
} TSound;

TSound ball_ball_snd;
TSound ball_wall_snd;
TSound ball_cue_snd;

Uint8 *ballinhole_data, *applause_data, *intro_data, *shuffle_data, *smack_data, *bomb_data, *oneball_data, *outball_data, *ooh_data;
Uint32 ballinhole_dlen, applause_dlen, intro_dlen, shuffle_dlen, smack_dlen, bomb_dlen, oneball_dlen, outball_dlen, ooh_dlen;

SDL_AudioSpec wave_applause;
SDL_AudioCVT cvt_applause;
SDL_AudioSpec wave_ballinhole;
SDL_AudioCVT cvt_ballinhole;
SDL_AudioSpec wave_intro;
SDL_AudioCVT cvt_intro;
SDL_AudioSpec wave_shuffle;
SDL_AudioCVT cvt_shuffle;
SDL_AudioSpec wave_smack;
SDL_AudioCVT cvt_smack;
SDL_AudioSpec wave_bomb;
SDL_AudioCVT cvt_bomb;
SDL_AudioSpec wave_oneball;
SDL_AudioCVT cvt_oneball;
SDL_AudioSpec wave_outball;
SDL_AudioCVT cvt_outball;
SDL_AudioSpec wave_ooh;
SDL_AudioCVT cvt_ooh;

static struct sample {
    Uint8 *data;
    Uint32 dpos;
    Uint32 dlen;
    int    vol;
} sounds[NUM_SOUNDS];

/***********************************************************************/

/* Intern Routines */

void PlaySound(TSound * snd, VMfloat ampl);
void PlaySound_offs(TSound * snd, VMfloat ampl, int offs_samps);
void PlayData(short int *data, int len, VMfloat ampl);
int init_sound(void);
int exit_sound(void);

void apply_bandpass( VMfloat period_samps, VMfloat width_samps, TSound snd );
void apply_attack( int offset, VMfloat attack, short int ** data, int * len );
void create_expsinerr( VMfloat period_samps, VMfloat tau_samps, VMfloat err, short int ** data, int * len );
void create_delayed_expsinerr( VMfloat period_samps, VMfloat tau_samps, int delay_samps, VMfloat err, short int ** data, int * len );
void create_expsinerr_attack( VMfloat period_samps, VMfloat tau_samps, VMfloat err, VMfloat attack, short int ** data, int * len );
void create_expsin( VMfloat period_samps, VMfloat tau_samps, short int ** data, int * len );

/***********************************************************************/

static void mixaudio(void *userdata, Uint8 *stream, int len)
{
    int i;
    Uint32 amount;

    for ( i=0; i<NUM_SOUNDS; ++i ) {
        amount = (sounds[i].dlen-sounds[i].dpos);
        if ( amount > len ) {
            amount = len;
        }
/*        SDL_MixAudio(stream, &sounds[i].data[sounds[i].dpos], amount, SDL_MIX_MAXVOLUME);*/
        SDL_MixAudio(stream, &sounds[i].data[sounds[i].dpos], amount, sounds[i].vol);
        sounds[i].dpos += amount;
    }
}

/***********************************************************************/

/*
  This Routine is called inside this file only
  It plays the sound of the raw soundfile
  with not really correct pointer handling (solve this some time)
*/

void PlayData(short int *data, int len, VMfloat ampl)
{
    int index;

    if(options_use_sound){
        /* Look for an empty (or finished) sound slot */
        for ( index=0; index<NUM_SOUNDS; ++index ) {
            if ( sounds[index].dpos == sounds[index].dlen ) {
                break;
            }
        }
        if ( index == NUM_SOUNDS )
            return;

        /* Put the sound data in the slot (it starts playing immediately) */
        SDL_LockAudio();
        sounds[index].data = (Uint8 *)data;
        sounds[index].dlen = len;
        sounds[index].dpos = 0;
        sounds[index].vol  = (int)(128.0*ampl);
        SDL_UnlockAudio();
    }
}

/***********************************************************************/

/*
  This Routine is called outside this file only
  It plays the sound of the wav converted soundfiles
  with correct pointers
*/

void Playwavdata(Uint8 *data, int len, VMfloat ampl)
{
    int index;

    if(options_use_sound){
        /* Look for an empty (or finished) sound slot */
        for ( index=0; index<NUM_SOUNDS; ++index ) {
            if ( sounds[index].dpos == sounds[index].dlen ) {
                break;
            }
        }
        if ( index == NUM_SOUNDS )
            return;

        /* Put the sound data in the slot (it starts playing immediately) */
        SDL_LockAudio();
        sounds[index].data = data;

        sounds[index].dlen = len;
        sounds[index].dpos = 0;
        sounds[index].vol  = (int)(128.0*ampl);
        SDL_UnlockAudio();
    }
}

/***********************************************************************/

void PlaySound(TSound * snd, VMfloat ampl)
{
    PlayData(snd->data, snd->len, ampl);
}

/***********************************************************************/

void PlaySound_offs(TSound * snd, VMfloat ampl, int offs_samps)
{
    PlayData(&snd->data[offs_samps*2], snd->len-offs_samps*2*2, ampl);
}

/***********************************************************************/

int init_sound(void)
{
    FILE * f;
    int i;
    //fprintf(stderr,"loading ball-ball sound\n");
    /* ball-ball sounds from samuele catuzzi's kbilliards - thanx */
    if((f=fopen("ball_ball.raw", "rb"))==NULL){
        fprintf(stderr,"couldn't open ball_ball.raw\n");
        sys_exit(1);
    }
    fseek(f, 0L, SEEK_END);
    ball_ball_snd.len = ftell(f)+1+SOUND_NULLOFFS*2*2;
    fseek(f, 0L, SEEK_SET);
    ball_ball_snd.data = malloc(ball_ball_snd.len);
    fread( &ball_ball_snd.data[SOUND_NULLOFFS*2], 1, ball_ball_snd.len-SOUND_NULLOFFS*2*2 , f );
    fclose(f);

    /* Load the sound file and convert it to 16-bit stereo at 22kHz */
    if ( SDL_LoadWAV("ballinhole.wav", &wave_ballinhole, &ballinhole_data, &ballinhole_dlen) == NULL ) {
        fprintf(stderr, "Couldn't load %s: %s\n", "ballinhole.wav", SDL_GetError());
        sys_exit(1);
    }
    SDL_BuildAudioCVT(&cvt_ballinhole, wave_ballinhole.format, wave_ballinhole.channels, wave_ballinhole.freq, AUDIO_S16, 2, 22050);
    cvt_ballinhole.buf = malloc(ballinhole_dlen*cvt_ballinhole.len_mult);
    memcpy(cvt_ballinhole.buf, ballinhole_data, ballinhole_dlen);
    cvt_ballinhole.len = ballinhole_dlen;
    SDL_ConvertAudio(&cvt_ballinhole);
    SDL_FreeWAV(ballinhole_data);

    if ( SDL_LoadWAV("applause.wav", &wave_applause, &applause_data, &applause_dlen) == NULL ) {
        fprintf(stderr, "Couldn't load %s: %s\n", "applause.wav", SDL_GetError());
        sys_exit(1);
    }
    SDL_BuildAudioCVT(&cvt_applause, wave_applause.format, wave_applause.channels, wave_applause.freq, AUDIO_S16, 2, 22050);
    cvt_applause.buf = malloc(applause_dlen*cvt_applause.len_mult);
    memcpy(cvt_applause.buf, applause_data, applause_dlen);
    cvt_applause.len = applause_dlen;
    SDL_ConvertAudio(&cvt_applause);
    SDL_FreeWAV(applause_data);

    if ( SDL_LoadWAV("intro.wav", &wave_intro, &intro_data, &intro_dlen) == NULL ) {
        fprintf(stderr, "Couldn't load %s: %s\n", "intro.wav", SDL_GetError());
        sys_exit(1);
    }
    SDL_BuildAudioCVT(&cvt_intro, wave_intro.format, wave_intro.channels, wave_intro.freq, AUDIO_S16, 2, 22050);
    cvt_intro.buf = malloc(intro_dlen*cvt_intro.len_mult);
    memcpy(cvt_intro.buf, intro_data, intro_dlen);
    cvt_intro.len = intro_dlen;
    SDL_ConvertAudio(&cvt_intro);
    SDL_FreeWAV(intro_data);

    if ( SDL_LoadWAV("shuffleballs.wav", &wave_shuffle, &shuffle_data, &shuffle_dlen) == NULL ) {
        fprintf(stderr, "Couldn't load %s: %s\n", "shuffleballs.wav", SDL_GetError());
        sys_exit(1);
    }
    SDL_BuildAudioCVT(&cvt_shuffle, wave_shuffle.format, wave_shuffle.channels, wave_shuffle.freq, AUDIO_S16, 2, 22050);
    cvt_shuffle.buf = malloc(shuffle_dlen*cvt_shuffle.len_mult);
    memcpy(cvt_shuffle.buf, shuffle_data, shuffle_dlen);
    cvt_shuffle.len = shuffle_dlen;
    SDL_ConvertAudio(&cvt_shuffle);
    SDL_FreeWAV(shuffle_data);

    if ( SDL_LoadWAV("smack.wav", &wave_smack, &smack_data, &smack_dlen) == NULL ) {
        fprintf(stderr, "Couldn't load %s: %s\n", "smack.wav", SDL_GetError());
        sys_exit(1);
    }
    SDL_BuildAudioCVT(&cvt_smack, wave_smack.format, wave_smack.channels, wave_smack.freq, AUDIO_S16, 2, 22050);
    cvt_smack.buf = malloc(smack_dlen*cvt_smack.len_mult);
    memcpy(cvt_smack.buf, smack_data, smack_dlen);
    cvt_smack.len = smack_dlen;
    SDL_ConvertAudio(&cvt_smack);
    SDL_FreeWAV(smack_data);

    if ( SDL_LoadWAV("bomb.wav", &wave_bomb, &bomb_data, &bomb_dlen) == NULL ) {
        fprintf(stderr, "Couldn't load %s: %s\n", "bomb.wav", SDL_GetError());
        sys_exit(1);
    }
    SDL_BuildAudioCVT(&cvt_bomb, wave_bomb.format, wave_bomb.channels, wave_bomb.freq, AUDIO_S16, 2, 22050);
    cvt_bomb.buf = malloc(bomb_dlen*cvt_bomb.len_mult);
    memcpy(cvt_bomb.buf, bomb_data, bomb_dlen);
    cvt_bomb.len = bomb_dlen;
    SDL_ConvertAudio(&cvt_bomb);
    SDL_FreeWAV(bomb_data);

    if ( SDL_LoadWAV("oneballontable.wav", &wave_oneball, &oneball_data, &oneball_dlen) == NULL ) {
        fprintf(stderr, "Couldn't load %s: %s\n", "oneballontable.wav", SDL_GetError());
        sys_exit(1);
    }
    SDL_BuildAudioCVT(&cvt_oneball, wave_oneball.format, wave_oneball.channels, wave_oneball.freq, AUDIO_S16, 2, 22050);
    cvt_oneball.buf = malloc(oneball_dlen*cvt_oneball.len_mult);
    memcpy(cvt_oneball.buf, oneball_data, oneball_dlen);
    cvt_oneball.len = oneball_dlen;
    SDL_ConvertAudio(&cvt_oneball);
    SDL_FreeWAV(oneball_data);

    if ( SDL_LoadWAV("balloutoftable.wav", &wave_outball, &outball_data, &outball_dlen) == NULL ) {
        fprintf(stderr, "Couldn't load %s: %s\n", "balloutoftable.wav", SDL_GetError());
        sys_exit(1);
    }
    SDL_BuildAudioCVT(&cvt_outball, wave_outball.format, wave_outball.channels, wave_outball.freq, AUDIO_S16, 2, 22050);
    cvt_outball.buf = malloc(outball_dlen*cvt_outball.len_mult);
    memcpy(cvt_outball.buf, outball_data, outball_dlen);
    cvt_outball.len = outball_dlen;
    SDL_ConvertAudio(&cvt_outball);
    SDL_FreeWAV(outball_data);

    if ( SDL_LoadWAV("ooh.wav", &wave_ooh, &ooh_data, &ooh_dlen) == NULL ) {
        fprintf(stderr, "Couldn't load %s: %s\n", "ooh.wav", SDL_GetError());
        sys_exit(1);
    }
    SDL_BuildAudioCVT(&cvt_ooh, wave_ooh.format, wave_ooh.channels, wave_ooh.freq, AUDIO_S16, 2, 22050);
    cvt_ooh.buf = malloc(ooh_dlen*cvt_ooh.len_mult);
    memcpy(cvt_ooh.buf, ooh_data, ooh_dlen);
    cvt_ooh.len = ooh_dlen;
    SDL_ConvertAudio(&cvt_ooh);
    SDL_FreeWAV(ooh_data);

#if __BYTE_ORDER == __BIG_ENDIAN
       {
          char *snd=ball_ball_snd.data;
          for(i=0;i<ball_ball_snd.len;i+=2)
          {
             char t=snd[i];
             snd[i]=snd[i+1];
             snd[i+1]=t;
          }
       }
#endif
       for(i=0;i<ball_ball_snd.len/2/2-SOUND_NULLOFFS;i++){
           ball_ball_snd.data[(i+SOUND_NULLOFFS)*2+0]*=/*0.5*/1.0*exp(-(VMfloat)i/(VMfloat)((ball_ball_snd.len-SOUND_NULLOFFS*2*2)/2/4));
           ball_ball_snd.data[(i+SOUND_NULLOFFS)*2+1]*=/*0.5*/1.0*exp(-(VMfloat)i/(VMfloat)((ball_ball_snd.len-SOUND_NULLOFFS*2*2)/2/4));
       }
       for(i=0;i<ball_ball_snd.len/2/2-1;i++){
           ball_ball_snd.data[i*2+0]=ball_ball_snd.data[i*2+0]*0.7+ball_ball_snd.data[(i+1)*2+0]*0.3;
           ball_ball_snd.data[i*2+1]=ball_ball_snd.data[i*2+1]*0.7+ball_ball_snd.data[(i+1)*2+1]*0.3;
       }
       for(i=0;i<SOUND_NULLOFFS*2;i++){
           ball_ball_snd.data[i]=0;
       }

    create_delayed_expsinerr( 220.0, 465.0, SOUND_NULLOFFS, 0.1, &ball_wall_snd.data, &ball_wall_snd.len );
    apply_attack( SOUND_NULLOFFS, 40.0, &ball_wall_snd.data, &ball_wall_snd.len );
    create_expsinerr(  20.0, 220.0, 0.6, &ball_cue_snd.data,  &ball_cue_snd.len );

    SDL_AudioSpec fmt;

    /* Set 16-bit stereo audio at 22Khz */
    fmt.freq = 22050;
    fmt.format = AUDIO_S16SYS;
    fmt.channels = 2;
    fmt.samples = 512;        /* A good value for games */
    fmt.callback = mixaudio;
    fmt.userdata = NULL;

    /* Open the audio device and start playing sound! */
    if ( SDL_OpenAudio(&fmt, NULL) < 0 ) {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        options_use_sound=0;
    } else {
        options_use_sound=1;
    }

    SDL_PauseAudio(0);
    return 1;
}

/***********************************************************************/

int exit_sound(void)
{
    SDL_CloseAudio();
    return 1;
}

/***********************************************************************/

VMfloat errsin(VMfloat x, VMfloat err)
{
/*    VMfloat y;
    y=(x-M_PI/4.0)/M_PI;
    y-=floor(y);
    if( y<0.5 ){
        return (1.0-4.0*y);
    }
    return (-1.0+4.0*(y-0.5));*/
    return(err*(VMfloat)rand()/(VMfloat)RAND_MAX+(1.0-err)*sin(x));
}

/***********************************************************************/

void create_expsinerr( VMfloat period_samps, VMfloat tau_samps, VMfloat err, short int ** data, int * len )
{
    int i;
    *len=2*2*3*tau_samps;
    *data=(short int *)malloc(*len);
    for(i=0;i<(*len)/2/2;i++){
        (*data)[i*2]=32000.0*errsin((VMfloat)i/period_samps*2.0*M_PI,err)*exp(-(VMfloat)i/tau_samps);
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

/***********************************************************************/

void create_delayed_expsinerr( VMfloat period_samps, VMfloat tau_samps, int delay_samps, VMfloat err, short int ** data, int * len )
{
    int i;
    *len=2*2*3*tau_samps+2*2*delay_samps;
    *data=(short int *)malloc(*len);
    for(i=0;i<delay_samps;i++){
        (*data)[i*2]=0;
        (*data)[i*2+1]=0;
    }
    for(i=delay_samps+0;i<(*len)/2/2;i++){
        (*data)[i*2]=32000.0*errsin((VMfloat)(i-delay_samps)/period_samps*2.0*M_PI,err)*exp(-(VMfloat)(i-delay_samps)/tau_samps);
        (*data)[i*2+1]=(*data)[i*2];
    }
    for(i=delay_samps+1;i<(*len)/2/2;i++){
        (*data)[i*2]=0.5*(*data)[i*2]+0.5*(*data)[(i-1)*2];
        (*data)[i*2+1]=(*data)[i*2];
    }
    for(i=delay_samps+30;i<(*len)/2/2;i++){
        (*data)[i*2]=0.7*(*data)[i*2]+0.3*(*data)[(i-30)*2];
        (*data)[i*2+1]=(*data)[i*2];
    }
}

/***********************************************************************/

void apply_bandpass( VMfloat period_samps, VMfloat width_samps, TSound snd )
{
    int len2, i, j;
    VMfloat * filter, filterint=0.0; //
    VMfloat newdata1, newdata2; //Apply filter

#define FILTERENTRY(filter,len,index) (((index)<(len)&&(index)>=0)?(filter)[(index)]:0.0)
    len2 = 3*width_samps;
    filter = malloc( len2*sizeof(VMfloat) );

    /* construct filter */
    for(i=0;i<len2;i++){
        filter[i]=
            exp(-(VMfloat)(i-len2/2)*(VMfloat)(i-len2/2)/(VMfloat)width_samps/(VMfloat)width_samps) *
            cos(2.0*M_PI*(VMfloat)(i-len2/2)/(VMfloat)period_samps);
        filterint+=filter[i];
    }

    /* apply filter */
    for(i=0;i<snd.len/2/2;i++){
        newdata1=0.0;
        newdata2=0.0;
        for(j=0;j<len2;j++){
            newdata1 += (VMfloat)TSoundENTRY(snd,(i+(j-len2/2))*2)   * FILTERENTRY(filter,len2,j);
            newdata2 += (VMfloat)TSoundENTRY(snd,(i+(j-len2/2))*2+1) * FILTERENTRY(filter,len2,j);
        }
        snd.data[i*2]   = newdata1/filterint;
        snd.data[i*2+1] = newdata2/filterint;
    }

    free( filter );
}

/***********************************************************************/

void create_expsinerr_attack( VMfloat period_samps, VMfloat tau_samps, VMfloat err, VMfloat attack, short int ** data, int * len )
{
    int i;
    *len=2*2*3*tau_samps;
    *data=(short int *)malloc(*len);
    for(i=0;i<(*len)/2/2;i++){
        (*data)[i*2]=32000.0*errsin((VMfloat)i/period_samps*2.0*M_PI,err)*exp(-(VMfloat)i/tau_samps);
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

    for(i=0;i<(*len)/2/2;i++){
        (*data)[i*2]*=1.0-exp(-(VMfloat)i/(VMfloat)attack*(VMfloat)i/(VMfloat)attack);
        (*data)[i*2+1]=(*data)[i*2];
    }
}

/***********************************************************************/

void apply_attack( int offset, VMfloat attack, short int ** data, int * len )
{
    int i;

    for(i=offset;i<(*len)/2/2;i++){
        (*data)[i*2+0]*=1.0-exp(-(VMfloat)(i-offset)/(VMfloat)attack*(VMfloat)(i-offset)/(VMfloat)attack);
        (*data)[i*2+1]*=1.0-exp(-(VMfloat)(i-offset)/(VMfloat)attack*(VMfloat)(i-offset)/(VMfloat)attack);
    }
}

/***********************************************************************/

void create_expsin( VMfloat period_samps, VMfloat tau_samps, short int ** data, int * len )
{
    int i;
    *len=2*2*3*tau_samps;
    *data=(short int *)malloc(*len);
    for(i=0;i<(*len)/2/2;i++){
        (*data)[i*2]=32000.0*sin((VMfloat)i/period_samps*2.0*M_PI)*exp(-(VMfloat)i/tau_samps);
        (*data)[i*2+1]=(*data)[i*2];
    }
}

/***********************************************************************/

void create_expsin_otones( VMfloat period_samps, VMfloat tau_samps, VMfloat otonefact, short int ** data, int * len )
{
    int i,j;
    VMfloat fact;
    *len=2*2*3*tau_samps;
    *data=(short int *)malloc(*len);
    for(i=0;i<(*len)/2/2;i++){
        fact=1.0;
        (*data)[i*2]=0.0;
        for(j=0;j<6;j++){
            (*data)[i*2]+=fact*25000.0*sin((VMfloat)i/(period_samps/(VMfloat)j)*2.0*M_PI)*exp(-(VMfloat)i/tau_samps);
            fact*=otonefact;
        }
        (*data)[i*2+1]=(*data)[i*2];
    }
}

#endif  /* #ifdef USE_SOUND */
