/* language.h
**
**    code for Localisation
**    Version foobillard++ started at 12/2010
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

#ifndef LANGUAGE_H
#define LANGUAGE_H

#define MAX_TEXT_ENTRIES 480
#define MAX_TEXT_ENTRY_LEN 200

extern char localeText[MAX_TEXT_ENTRIES][MAX_TEXT_ENTRY_LEN];

void init_language();
int manual_available();
void launch_manual();

#endif // language.h
