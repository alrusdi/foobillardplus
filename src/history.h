/* sys_stuff.h
**
**    code for game history
**
**    Version foobillard++ started at 12/2010
**    Copyright (C) 2011 - 2013 Holger Schaekel (foobillardplus@go4more.de)
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

// don't use functions of history unless the language is initialized!

#ifndef HISTORY_H
#define HISTORY_H

#include "billmove.h"
#include "billard3d.h"

void file_tournament_history( struct TournamentState_ * ts, char *winner, enum gameType gametype);
void file_history(char *player1, char *player2, char *winner, int hits, int rounds, enum gameType gametype);
void get_history(char *strpointer);
void init_history(void);
int history_free(void);
void history_clear(void);
void history_set(void);
int check_xml(char *filecheck);
void show_history(char *historyfile);

#endif
