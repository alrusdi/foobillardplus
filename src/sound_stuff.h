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
#include <SDL_mixer.h>
#include "vmath.h"

extern struct SoundStruct{
    short int * data;
    int len;  /* length in bytes */
} TSound;

extern struct TSound ball_ball_snd;
extern struct TSound ball_wall_snd;
extern struct TSound ball_cue_snd;

/* Mix_Music actually holds the music information. */

extern Mix_Music *music;

/* Mix_Chunk actually holds the noise information. */

extern Mix_Chunk *ball_hole;
extern Mix_Chunk *wave_applause;
extern Mix_Chunk *wave_intro;
extern Mix_Chunk *wave_shuffle;
extern Mix_Chunk *wave_smack;
extern Mix_Chunk *wave_bomb;
extern Mix_Chunk *wave_oneball;
extern Mix_Chunk *wave_outball;
extern Mix_Chunk *wave_ooh;
extern Mix_Chunk *cue_sound;
extern Mix_Chunk *wall_sound;
extern Mix_Chunk *ball_sound;

void init_sound(void);
void exit_sound(void);
void PlaySound(Mix_Chunk *chunkdata, int volume);
void PlayNextSong(void);

#endif /* #ifdef USE_SOUND */

#endif /* SOUND_STUFF_H */
