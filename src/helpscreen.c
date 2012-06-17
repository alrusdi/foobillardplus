/* helpscreen.c
**
**    helpscreen using textobj
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
**    along with this program; if not, write tto the Free Software
**    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
*/

#include <stdlib.h>
#include "font.h"
#include "textobj.h"
#include "options.h"
#include "language.h"

static int help_screen_obj = -1;
static textObj ** text;
static textObj ** text0;

/***********************************************************************/

void create_help_screen(int win_width, int win_height)
{
        text = malloc(100*sizeof(textObj *));
        text0=text;
        help_screen_obj = glGenLists(1);

        *text = textObj_new( localeText[148], options_help_fontname, 21 );
        text++;
        *text = textObj_new( localeText[149], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[150], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[151], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[152], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[153], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[154], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[155], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[156], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[157], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[158], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[159], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[160], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[161], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[162], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[163], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[164], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[165], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[166], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[167], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[168], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[169], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[170], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[171], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[172], options_help_fontname, 16);
        text++;
        *text = textObj_new( localeText[173], options_help_fontname, 16);
        text++;
        *text = (textObj *)0;

        text=text0;
        glNewList(help_screen_obj, GL_COMPILE);
           glTranslatef(-0.57,0.77,-1.0);
           glScalef(2.0/win_width,2.0/win_height,1.0);
           for(text=text0 ; *text!=0 ; text++ ){
               textObj_draw( *text );
               glTranslatef(0.0,-21.0,-0.0);
           }
        glEndList();
}

/***********************************************************************/

int draw_help_screen(int win_width, int win_height)
{

    if( help_screen_obj==-1 ){
        create_help_screen( win_width, win_height );
    } else {
        glCallList(help_screen_obj);
    }

    return 0;
}
