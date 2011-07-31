/* sound_stuff.h
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

#ifndef SOUND_STUFF_H
#define SOUND_STUFF_H

#ifdef USE_SOUND

#include <SDL.h>
#include <SDL_audio.h>
#include "vmath.h"

extern struct SoundStruct{
    short int * data;
    int len;  /* length in bytes */
} TSound;

#define NUM_SOUNDS 20

extern struct TSound ball_ball_snd;
extern struct TSound ball_wall_snd;
extern struct TSound ball_cue_snd;

extern Uint8 *ballinhole_data, *applause_data, *intro_data, *shuffle_data, *smack_data, *bomb_data, *oneball_data, *outball_data, *ooh_data;
extern Uint32 ballinhole_dlen, applause_dlen, intro_dlen, shuffle_dlen, smack_dlen, bomb_dlen, oneball_dlen, outball_dlen, ooh_dlen;

extern SDL_AudioSpec wave_applause;
extern SDL_AudioCVT cvt_applause;
extern SDL_AudioSpec wave_ballinhole;
extern SDL_AudioCVT cvt_ballinhole;
extern SDL_AudioSpec wave_intro;
extern SDL_AudioCVT cvt_intro;
extern SDL_AudioSpec wave_shuffle;
extern SDL_AudioCVT cvt_shuffle;
extern SDL_AudioSpec wave_smack;
extern SDL_AudioCVT cvt_smack;
extern SDL_AudioSpec wave_bomb;
extern SDL_AudioCVT cvt_bomb;
extern SDL_AudioSpec wave_oneball;
extern SDL_AudioCVT cvt_oneball;
extern SDL_AudioSpec wave_outball;
extern SDL_AudioCVT cvt_outball;
extern SDL_AudioCVT cvt_ooh;

#ifndef SOUND_NULLOFFS
  #define SOUND_NULLOFFS 10000
#endif

#define TSoundENTRY(snd,index) (( (index)<(snd).len/2 && (index)>=0 )?(snd).data[(index)]:0)

void PlaySound(struct TSound *snd, VMfloat ampl);
void PlaySound_offs(struct TSound *snd, VMfloat ampl, int offs_samps);
void Playwavdata(Uint8 *data, int len, VMfloat ampl);
int init_sound(void);
int exit_sound(void);

void apply_bandpass( VMfloat period_samps, VMfloat width_samps, struct TSound snd );
void apply_attack( int offset, VMfloat attack, short int ** data, int * len );
void create_expsinerr( VMfloat period_samps, VMfloat tau_samps, VMfloat err, short int ** data, int * len );
void create_delayed_expsinerr( VMfloat period_samps, VMfloat tau_samps, int delay_samps, VMfloat err, short int ** data, int * len );
void create_expsinerr_attack( VMfloat period_samps, VMfloat tau_samps, VMfloat err, VMfloat attack, short int ** data, int * len );
void create_expsin( VMfloat period_samps, VMfloat tau_samps, short int ** data, int * len );

#endif /* #ifdef USE_SOUND */

#endif /* SOUND_STUFF_H */
