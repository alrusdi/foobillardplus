/* menu.c
**
**    code for Menu-Handling
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

#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <GL/gl.h>
#include "options.h"
#include "sys_stuff.h"
#include "sound_stuff.h"
#include "font.h"
#include "language.h"
#include "history.h"
#include "player.h"
#include "evaluate_move.h"

extern menuType  *g_act_menu;   // comes from billard3d.c
extern menuType  *g_main_menu;  // dto.

extern int vline_on;            // dto.

extern struct Player player[2]; // dto.

void menu_cb( int, void *, VMfloat); // dto.

/***********************************************************************
 *                   check for a valid IPv4 address                    *
 ***********************************************************************/

int is_valid_ip(const char *ip_str)
{
unsigned int n1,n2,n3,n4;

  if(sscanf(ip_str,"%u.%u.%u.%u", &n1, &n2, &n3, &n4) != 4) return 0;
  if((n1 != 0) && (n1 <= 255) && (n2 <= 255) && (n3 <= 255) && (n4 <= 255)) {
     char buf[64];
     sprintf(buf,"%u.%u.%u.%u",n1,n2,n3,n4);
     if(strcmp(buf,ip_str)) return 0;
     return 1;
  }
  return 0;
}

/***********************************************************************
 *       Set the text for the Statusline for an option or menu         *
 ***********************************************************************/

static void menu_statustext( char * help_line )
{
    if( help_line != NULL ){
      displaystring(help_line);
    } else {
      displaystring(" ");
    }
}
/***********************************************************************
 *                       Generate a new menu                           *
 ***********************************************************************/

menuType * menu_new( void (* callback)( int, void *, VMfloat ) )
{
    menuType * menu;

    //fprintf(stderr,"menu_new:\n");

    menu = malloc(sizeof(menuType));
    menu->nr           = 0;
    menu->select_id    = -1;
    menu->p_select_id  = &(menu->select_id);
    menu->select_index = 0;
    menu->textedit_mode = 0;
    menu->callback     = callback;
    menu->parent       = (menuType *)0;
    return menu;
}

/***********************************************************************
 *                Add a submenu to an existing menu                    *
 ***********************************************************************/

void menu_add_submenu( menuType * menu, char * text, menuType * submenu, int show_subsetting, char * subtext, char * help_line )
{
    //fprintf(stderr,"menu_add_submenu:\n");
    if(menu->nr<MAX_MENU_ENTRY_NUM){
        menu->entry[menu->nr].type     = ENTRY_TYPE_SUBMENU;
        strcpy( menu->entry[menu->nr].text, text);
        menu->entry[menu->nr].settingtext[0] = 0;
        menu->entry[menu->nr].submenu  = submenu;
        menu->entry[menu->nr].text_obj = (textObj *)0;
        menu->entry[menu->nr].help_line = help_line;
        menu->entry[menu->nr].fontname = options_menu_fontname;
        menu->entry[menu->nr].fontsize = 32;
        menu->entry[menu->nr].show_subsetting = show_subsetting;
        menu->entry[menu->nr].arg      = (void *) 0;
        if(submenu!=(menuType *)0){
            submenu->callback     = menu->callback;
            submenu->p_select_id  = menu->p_select_id;
            submenu->parent       = menu;
            submenu->parent_entry = &(menu->entry[menu->nr]);
        }
        if(show_subsetting && subtext!=NULL) {
            strcpy( menu->entry[menu->nr].settingtext, subtext);
        }
        menu->nr++;
    } else {
        fprintf(stderr,"menu_add_submenu: too many menu entries - ignoring this entry\n");
    }
}

/***********************************************************************
 *            Generate a new entry in a submenu/menu                   *
 ***********************************************************************/

void menu_add_entry( menuType * menu, char * text, int id, char * help_line )
{
    //fprintf(stderr,"menu_add_entry:\n");
    if(menu->nr<MAX_MENU_ENTRY_NUM){
        menu->entry[menu->nr].type     = ENTRY_TYPE_ID;
        strcpy( menu->entry[menu->nr].text, text );
        menu->entry[menu->nr].settingtext[0] = 0;
        menu->entry[menu->nr].id       = id;
        menu->entry[menu->nr].text_obj = (textObj *)0;
        menu->entry[menu->nr].help_line = help_line;
        menu->entry[menu->nr].fontname = options_menu_fontname;
        menu->entry[menu->nr].fontsize = 32;
        menu->entry[menu->nr].arg      = (void *) 0;
        menu->entry[menu->nr].value    = -1.0f;
        menu->nr++;
    } else {
        fprintf(stderr,"menu_add_entry: too many menu entries - ignoring this entry\n");
    }
}

/***********************************************************************
 *        Add an argument entry inside an existing menu/submenu        *
 ***********************************************************************/

void menu_add_arg_entry( menuType * menu, char * text, int id, void * arg, char * help_line )
{
    //fprintf(stderr,"menu_add_arg_entry:\n");
    if(menu->nr<MAX_MENU_ENTRY_NUM){
        menu->entry[menu->nr].type     = ENTRY_TYPE_ID;
        strcpy( menu->entry[menu->nr].text, text );
        menu->entry[menu->nr].settingtext[0] = 0;
        menu->entry[menu->nr].id       = id;
        menu->entry[menu->nr].text_obj = (textObj *)0;
        menu->entry[menu->nr].help_line = help_line;
        menu->entry[menu->nr].fontname = options_menu_fontname;
        menu->entry[menu->nr].fontsize = 32;
        menu->entry[menu->nr].arg      = arg;
        menu->entry[menu->nr].value    = -1.0f;
        menu->nr++;
    } else {
        fprintf(stderr,"menu_add_arg_entry: too many menu entries - ignoring this entry\n");
    }
}

/***********************************************************************
 *           Add a value entry inside an existing menu/submenu         *
 ***********************************************************************/

void menu_add_value_entry( menuType * menu, char * text, int id, VMfloat value, char * help_line )
{
    //fprintf(stderr,"menu_add_value_entry:\n");
    if(menu->nr<MAX_MENU_ENTRY_NUM){

        menu->entry[menu->nr].type     = ENTRY_TYPE_ID;
        strcpy( menu->entry[menu->nr].text, text );
        menu->entry[menu->nr].settingtext[0] = 0;
        menu->entry[menu->nr].id       = id;
        menu->entry[menu->nr].text_obj = (textObj *)0;
        menu->entry[menu->nr].help_line = help_line;
        menu->entry[menu->nr].fontname = options_menu_fontname;
        menu->entry[menu->nr].fontsize = 32;
        menu->entry[menu->nr].arg      = (void *) 0;
        menu->entry[menu->nr].value    = value;
        menu->nr++;
    } else {
        fprintf(stderr,"menu_add_value_entry: too many menu entries - ignoring this entry\n");
    }
}

/***********************************************************************
 *   Add an editable text-entryentry inside an existing menu/submenu   *
 ***********************************************************************/

void menu_add_textfield( menuType * menu, char * text, int id, int fixedlen, int modus, int minvalue, int maxvalue, int maxfield, char * help_line )
{
    //fprintf(stderr,"menu_add_arg_entry:\n");
    if(menu->nr<MAX_MENU_ENTRY_NUM){
        menu->entry[menu->nr].type     = ENTRY_TYPE_TEXTFIELD;
        strcpy( menu->entry[menu->nr].text, text );
        menu->entry[menu->nr].settingtext[0] = 0;
        menu->entry[menu->nr].id       = id;
        menu->entry[menu->nr].text_obj = (textObj *)0;
        menu->entry[menu->nr].help_line = help_line;
        menu->entry[menu->nr].fontname = options_menu_fontname;
        menu->entry[menu->nr].fontsize = 32;
        menu->entry[menu->nr].arg      = (void *)0;
        menu->entry[menu->nr].value    = -1.0f;
        menu->entry[menu->nr].fixedlen = fixedlen;
        menu->entry[menu->nr].modus = modus;
        menu->entry[menu->nr].minvalue = minvalue;
        menu->entry[menu->nr].maxvalue = maxvalue;
        menu->entry[menu->nr].maxfield = maxfield;
        menu->nr++;
    } else {
        fprintf(stderr,"menu_add_textfield: too many menu entries - ignoring this entry\n");
    }
}


/***********************************************************************
 *                Exit entry from a menu or submenu                    *
 ***********************************************************************/

void menu_add_exit( menuType * menu, char *text, char * help_line)
{
    //fprintf(stderr,"menu_add_exit:\n");
    if(menu->nr<MAX_MENU_ENTRY_NUM){
        menu->entry[menu->nr].type = ENTRY_TYPE_EXIT;
        strcpy( menu->entry[menu->nr].text, text );
        menu->entry[menu->nr].settingtext[0] = 0;
        menu->entry[menu->nr].text_obj = (textObj *)0;
        menu->entry[menu->nr].help_line = help_line;
        menu->entry[menu->nr].fontname = options_menu_fontname;
        menu->entry[menu->nr].fontsize = 32;
        menu->entry[menu->nr].arg      = (void *) 0;
        menu->entry[menu->nr].value    = -1.0f;
        menu->nr++;
    } else {
        fprintf(stderr,"menu_add_exit: too many menu entries - ignoring this entry\n");
    }
}

/***********************************************************************
 *            Set the text in a menu / submenu option                  *
 ***********************************************************************/

void menu_entry_set_settingtext( menuEntry * entry, char * text )
{
    char str[256];
    strcpy( entry->settingtext, text );
    if( entry->text_obj != (textObj *)0 ){
        if( entry->show_subsetting && entry->settingtext[0]!=0 ){
            sprintf(str,"%s : %s",entry->text,entry->settingtext);
            textObj_setText( entry->text_obj, str );
        } else {
            textObj_setText( entry->text_obj, text );
        }
    }
}

/***********************************************************************
 *          Create the menu textobject for opengl displaying           *
 ***********************************************************************/

void menu_create_textobj( menuEntry * entry )
{
    char str[256];
    if( entry->text_obj == (textObj *)0 ){


        if( entry->show_subsetting && entry->settingtext[0]!=0 ){
            sprintf(str,"%s : %s",entry->text,entry->settingtext);
            entry->text_obj = textObj_new( str, entry->fontname, entry->fontsize );
        } else {
            entry->text_obj = textObj_new( entry->text, entry->fontname, entry->fontsize );
        }
    }
}

/***********************************************************************
 *                selects the menupoint under the pos x,y              *
 ***********************************************************************/

void menu_select_by_coord( menuType * menu, int x, int y )

{
    int i,x1,y1,x2,y2, all_height;

    if(!menu->textedit_mode){
        all_height=0;
        for(i=0;i<menu->nr;i++){
            all_height+=menu->entry[i].fontsize;
        }

        for(i=0;i<menu->nr;i++){
            //        fprintf( stderr, "menu->entry[i].text_obj = %d\n", menu->entry[i].text_obj );
            if( menu->entry[i].text_obj == (textObj *)0 ){
                menu_create_textobj( &(menu->entry[i]) );
            }
            x1 = -menu->entry[i].text_obj->quad_w/2;
            y1 = all_height/2-i*menu->entry[i].fontsize-menu->entry[i].text_obj->quad_h/2;
            x2 = +menu->entry[i].text_obj->quad_w/2;
            y2 = all_height/2-i*menu->entry[i].fontsize+menu->entry[i].text_obj->quad_h/2;
            if( x<=x2 && x>=x1 && y<=y2 && y>=y1 ){
                menu->select_index=i;
                *(menu->p_select_id)=menu->entry[i].id;
                menu_statustext( menu->entry[i].help_line );
            }
        }
    }
}

/***********************************************************************
 *              choose the Menupoint under the pos x,y                 *
 ***********************************************************************/

int menu_choose_by_coord( menuType * menu, int x, int y )
{
    int i,x1,y1,x2,y2, all_height;

    if(!menu->textedit_mode){
        all_height=0;
        for(i=0;i<menu->nr;i++){
            all_height+=menu->entry[i].fontsize;
        }

        for(i=0;i<menu->nr;i++){
            if( menu->entry[i].text_obj == (textObj *)0 ){
                menu_create_textobj( &(menu->entry[i]) );
            }
            x1 = -menu->entry[i].text_obj->quad_w/2;
            y1 = all_height/2-i*menu->entry[i].fontsize-menu->entry[i].text_obj->quad_h/2;
            x2 = +menu->entry[i].text_obj->quad_w/2;
            y2 = all_height/2-i*menu->entry[i].fontsize+menu->entry[i].text_obj->quad_h/2;
            if( x<=x2 && x>=x1 && y<=y2 && y>=y1 ){
                return(1);
            }
        }
    }
return(0);
}

/***********************************************************************
 *          selects the next menupoint in the current submenu          *
 ***********************************************************************/

void menu_select_next( menuType * menu )
{
    if(!menu->textedit_mode){
        (menu->select_index)++;
        if( menu->select_index >= menu->nr ) menu->select_index=0;
        *(menu->p_select_id) = menu->entry[menu->select_index].id;
        menu_statustext( menu->entry[menu->select_index].help_line );
    }
}

/***********************************************************************
 *      selects the previous menupoint in the current submenu          *
 ***********************************************************************/

void menu_select_prev( menuType * menu )
{
    if(!menu->textedit_mode){
        (menu->select_index)--;
        if( menu->select_index < 0 ) menu->select_index=(menu->nr)-1;
        *(menu->p_select_id) = menu->entry[menu->select_index].id;
        menu_statustext( menu->entry[menu->select_index].help_line );
    }
}

/***********************************************************************
 *                  choose a selected menu option                      *
 ***********************************************************************/

void menu_choose(menuType ** menu)
{
    //fprintf(stderr,"menu_choose:\n");
    if( (*menu)->entry[(*menu)->select_index].type==ENTRY_TYPE_SUBMENU ) {

        //fprintf(stderr,"menu_choose: switch submenu\n");
        (*menu)=(*menu)->entry[(*menu)->select_index].submenu;

    } else if( (*menu)->entry[(*menu)->select_index].type==ENTRY_TYPE_ID ) {

        //fprintf(stderr,"menu_choose: id=%d\n",(*menu)->entry[(*menu)->select_index].id);
        if( (*menu)->parent_entry != 0 ){
            if( (*menu)->parent_entry->show_subsetting ){
                menu_entry_set_settingtext( (*menu)->parent_entry, (*menu)->entry[(*menu)->select_index].text );
            }
        }
        (*menu)->callback( (*menu)->entry[(*menu)->select_index].id,
                           (*menu)->entry[(*menu)->select_index].arg,
                           (*menu)->entry[(*menu)->select_index].value );
        menu_exit(menu);

    } else if( (*menu)->entry[(*menu)->select_index].type==ENTRY_TYPE_TEXTFIELD ) {
        //fprintf(stderr,"menu_choose: ENTRY_TYPE_TEXTFIELD\n");
        if(!(*menu)->textedit_mode){
            //fprintf(stderr,"menu_choose: ENTRY_TYPE_TEXTFIELD - !textedit_mode\n");
            (*menu)->textedit_mode=1;
#ifdef WETAB
            sys_fullscreen(0);
            SDL_Delay(20);
            launch_command("tiitoo-keyboard-toggle-daemon.sh on");
#endif
        } else {
            //fprintf(stderr,"menu_choose: ENTRY_TYPE_TEXTFIELD - textedit_mode\n");
           // check the new entries
            switch((*menu)->entry[(*menu)->select_index].modus) {
              case KEY_TYPE_NOTHING:
              case KEY_TYPE_TEXT:
              case KEY_TYPE_MIXED:
                //nothing to do?
                //to short
                if(utf8count((*menu)->entry[(*menu)->select_index].text_obj->str) < (size_t)((*menu)->entry[(*menu)->select_index].minvalue+(*menu)->entry[(*menu)->select_index].fixedlen)) {
                	  PLAY_NOISE(wave_bomb,options_snd_volume);
                return;
                }
                break;
              case KEY_TYPE_NUM:
                // to small or to big
               (*menu)->entry[(*menu)->select_index].arg = (void *)&((*menu)->entry[(*menu)->select_index].text_obj->str[(*menu)->entry[(*menu)->select_index].fixedlen]);
                if( atoi((*menu)->entry[(*menu)->select_index].arg) < (*menu)->entry[(*menu)->select_index].minvalue || atoi((*menu)->entry[(*menu)->select_index].arg) > (*menu)->entry[(*menu)->select_index].maxvalue) {
                	  PLAY_NOISE(wave_bomb,options_snd_volume);
                return;
                }
                break;
              case KEY_TYPE_NETADDRESS:
                // correct netaddress format
               (*menu)->entry[(*menu)->select_index].arg = (void *)&((*menu)->entry[(*menu)->select_index].text_obj->str[(*menu)->entry[(*menu)->select_index].fixedlen]);
                if(!is_valid_ip((*menu)->entry[(*menu)->select_index].arg)) {
                	  PLAY_NOISE(wave_bomb,options_snd_volume);
                return;
                }
                break;
            }
           // check end 
           (*menu)->textedit_mode=0;
#ifdef WETAB
           launch_command("tiitoo-keyboard-toggle-daemon.sh off");
            sys_fullscreen(1);
            SDL_Delay(20);
#endif
            strcpy((*menu)->entry[(*menu)->select_index].text,
                   (*menu)->entry[(*menu)->select_index].text_obj->str);
            /* give the entered string as arg */
            (*menu)->entry[(*menu)->select_index].arg = (void *)&((*menu)->entry[(*menu)->select_index].text[(*menu)->entry[(*menu)->select_index].fixedlen]);
            //fprintf(stderr,"printing arg:\n");
            //fprintf(stderr,"%s\n",(char *)((*menu)->entry[(*menu)->select_index].arg));
            (*menu)->callback( (*menu)->entry[(*menu)->select_index].id,
                               (*menu)->entry[(*menu)->select_index].arg,
                               (*menu)->entry[(*menu)->select_index].value );
        }
    } else if( (*menu)->entry[(*menu)->select_index].type==ENTRY_TYPE_EXIT ) {

        menu_exit(menu);

    }
}

/***********************************************************************
 *                          menu exit function                         *
 ***********************************************************************/

void menu_exit(menuType ** menu)
{
    //fprintf(stderr,"menu_exit:\n");
    if(!(*menu)->textedit_mode){
        //fprintf(stderr,"menu_exit: switch to parent\n");
        menu_texObj_cleanup(*menu);
        *menu=(*menu)->parent;
        menu_statustext(NULL);
        //fprintf(stderr,"menu_exit: switch to parent - end\n");
    } else {
        textObj_setText( (*menu)->entry[(*menu)->select_index].text_obj, (*menu)->entry[(*menu)->select_index].text );
        (*menu)->textedit_mode=0;
#ifdef WETAB
        launch_command("tiitoo-keyboard-toggle-daemon.sh off");
        sys_fullscreen(1);
        SDL_Delay(20);
#endif
    }
}

/***********************************************************************
 *           the function for keystroke into textobject                *
 ***********************************************************************/

void menu_text_keystroke( menuType * menu, int key )
{
    if(menu->textedit_mode){
        switch(key){
        case 8:
            if(utf8count(menu->entry[menu->select_index].text_obj->str) > (size_t)menu->entry[menu->select_index].fixedlen)
                textObj_delete_last( menu->entry[menu->select_index].text_obj );
            break;
        default:
            switch(menu->entry[menu->select_index].modus) {
              case KEY_TYPE_NOTHING:
                if (isprint(key))
                  textObj_append_char( menu->entry[menu->select_index].text_obj, key );
                break;
              case KEY_TYPE_NUM:
                if(utf8count(menu->entry[menu->select_index].text_obj->str) < (size_t)(menu->entry[menu->select_index].maxfield+menu->entry[menu->select_index].fixedlen)) {
                  if (isprint(key) && key >='0' && key<='9')
                    textObj_append_char( menu->entry[menu->select_index].text_obj, key );
                }
                break;
              case KEY_TYPE_TEXT:
                if(utf8count(menu->entry[menu->select_index].text_obj->str) < (size_t)(menu->entry[menu->select_index].maxfield+menu->entry[menu->select_index].fixedlen)) {
                  if (isprint(key) && ((key >='a' && key<='z') || (key >='A' && key<='Z')))
                    textObj_append_char( menu->entry[menu->select_index].text_obj, key );
                }
                break;
              case KEY_TYPE_MIXED:
                if(utf8count(menu->entry[menu->select_index].text_obj->str) < (size_t)(menu->entry[menu->select_index].maxfield+menu->entry[menu->select_index].fixedlen)) {
                  if (isprint(key) && ((key >='a' && key<='z') || (key >='A' && key<='Z') || (key >='0' && key<='9')))
                    textObj_append_char( menu->entry[menu->select_index].text_obj, key );
                }
                break;
              case KEY_TYPE_NETADDRESS:
                if(utf8count(menu->entry[menu->select_index].text_obj->str) < (size_t)(menu->entry[menu->select_index].maxfield+menu->entry[menu->select_index].fixedlen)) {
                if (isprint(key) && ((key >='0' && key<='9') || (key =='.')))
                  textObj_append_char( menu->entry[menu->select_index].text_obj, key );
                }
                break;
            }
            break;
        }
    }
}

/***********************************************************************
 *             Draw a menu or submenu entry to display                 *
 ***********************************************************************/

static void menu_draw_entry( menuEntry * entry )
{
    if( entry->text_obj == (textObj *)0 ){
        menu_create_textobj( entry );
    }
    textObj_draw_centered(entry->text_obj);
}

/***********************************************************************
 *           Draw a full menu or submenu to display                    *
 ***********************************************************************/

void menu_draw( menuType * menu )
{
    int i,all_height;

    all_height=0;
    for(i=0;i<menu->nr;i++){
        all_height+=menu->entry[i].fontsize;
    }
    glPushMatrix();
    glTranslatef(0,all_height/2,0);
    for(i=0;i<menu->nr;i++){
        /* hilight the entry with id=*p_select_id */
        if( i==menu->select_index ){     /* selected menu entry */
            if(menu->textedit_mode){     /* if textedit-field */
                glColor3f(1.0,0.0,0.0);
                menu_draw_entry(&(menu->entry[i]));
            } else {
                glPushMatrix();
                glScalef(1.1,1.1,1.0);
                glBlendFunc( GL_ZERO, GL_ONE_MINUS_SRC_COLOR );
                glColor3f(1.0,1.0,1.0);
                glTranslatef(2,2,0);
                menu_draw_entry(&(menu->entry[i]));
                glTranslatef(-4,0,0);
                menu_draw_entry(&(menu->entry[i]));
                glTranslatef(0,-4,0);
                menu_draw_entry(&(menu->entry[i]));
                glTranslatef(4,0,0);
                menu_draw_entry(&(menu->entry[i]));
                glTranslatef(-2,2,0);
                glBlendFunc(GL_ONE,GL_ONE);
                glColor3f(1.0,1.0,0.0);
                menu_draw_entry(&(menu->entry[i]));
                glPopMatrix();
            }
        } else {         /* normal menu entry */
            glPushMatrix();
            glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
            glColor3f(1.0,1.0,1.0);
            glTranslatef(2,2,0);
            menu_draw_entry(&(menu->entry[i]));
            glTranslatef(-4,0,0);
            menu_draw_entry(&(menu->entry[i]));
            glTranslatef(0,-4,0);
            menu_draw_entry(&(menu->entry[i]));
            glTranslatef(4,0,0);
            menu_draw_entry(&(menu->entry[i]));
            glTranslatef(-2,2,0);
            glBlendFunc(GL_ONE,GL_ONE);
            glColor3f(1.0,1.0,1.0);
            menu_draw_entry(&(menu->entry[i]));
            glPopMatrix();
        }

        glTranslatef(0,-menu->entry[i].fontsize,0);
    }
    glPopMatrix();
}

/***********************************************************************
 *                 clean a menu or submenu entry                       *
 ***********************************************************************/

void menu_texObj_cleanup(menuType * menu)
/* releases the gl-lists and the allocated textures */
{
    int i;

    //fprintf(stderr,"menu_texObj_cleanup:\n");
    if( menu != (menuType *)0 ){
        for ( i=0 ; i<menu->nr ; i++ ) {
            //fprintf(stderr,"menu_texObj_cleanup: i=%d (%s)\n",i,menu->entry[i].text);
            if( menu->entry[i].text_obj != (textObj *)0 )
                textObj_delete(menu->entry[i].text_obj);
            //fprintf(stderr,"menu_texObj_cleanup: 1\n");
            if( menu->entry[i].type == ENTRY_TYPE_SUBMENU ){
                //fprintf(stderr,"menu_texObj_cleanup: menu_texObj_cleanup\n");
                menu_texObj_cleanup(menu->entry[i].submenu);
            }
            //fprintf(stderr,"menu_texObj_cleanup: 2\n");
            if( menu->entry[i].text_obj != (textObj *)0 )
                free(menu->entry[i].text_obj);
            //fprintf(stderr,"menu_texObj_cleanup: 3\n");
            menu->entry[i].text_obj = (textObj *)0;
            //fprintf(stderr,"menu_texObj_cleanup: 4\n");
        }
    }
}

/***********************************************************************
 *                     Initialize the menu-system                      *
 ***********************************************************************/

void init_menu(void)
{
#ifndef WETAB
    int i,j;               // loop variables for getting video modes
    sysResolution * modes; // the video modes
    int entr_num=10;       // max. entries for Video modes
    int mode_num=0;        // Modus no. of the video mode
    int menu_num=0;        // the index number of the menue for resolution
    static menuType * videomode_menu;
    static menuType ** videomode_menus;
    static menuType * keypressed_menu;
#endif
    VMfloat maxi;          // in case for max Menu entry
#ifdef NETWORKING
    static menuType * network_speed_menu8;
    static menuType * network_speed_menu9;
    static menuType * network_speed_menuc;
    static menuType * network_speed_menus;
    static menuType * net8ball_menu;
    static menuType * net9ball_menu;
    static menuType * netcarambol_menu;
    static menuType * netsnooker_menu;
    static menuType * network_host8_menu;
    static menuType * network_join8_menu;
    static menuType * network_host9_menu;
    static menuType * network_join9_menu;
    static menuType * network_hostcarambol_menu;
    static menuType * network_joincarambol_menu;
    static menuType * network_hostsnooker_menu;
    static menuType * network_joinsnooker_menu;
    static menuType * netcompatible_menu8;
    static menuType * netcompatible_menu9;
    static menuType * netcompatible_menuc;
    static menuType * netcompatible_menus;
#endif
    static menuType  *g_options_menu;
    static menuType * quit_menu;
#ifdef USE_SOUND
    static menuType * sound_menu;
    static menuType * music_menu;
#endif
    static menuType * antialias_menu;
    static menuType * antialias_max;
    static menuType * fullscreen_menu;
    static menuType * reflection_menu;
    static menuType * render_menu;
    static menuType * resolution_menu;
    static menuType * bumpref_menu;
    static menuType * bumpwood_menu;
    static menuType * light_menu;
    static menuType * view_menu;
    static menuType * game_menu;
    static menuType * player_menu;
    static menuType * rgstereo_menu;
    static menuType * vsync_menu;
    static menuType * rgenable_menu;
    static menuType * rgaim_menu;
    static menuType * lensflare_menu;
    static menuType * balldetail_menu;
    static menuType * tabletheme_menu;
    static menuType * roomtexture_menu;
    static menuType * furnituretex_menu;
    static menuType * tablesize_menu;
    static menuType * helpline_menu;
    static menuType * birdview_ai_menu;
    static menuType * gametype_menu;
    static menuType * balltrace_menu;
    static menuType * status_menu;
    static menuType * control_menu;
    static menuType * avatar_menu;
    static menuType * jump_shot_menu;
    static menuType * ball8_menu;
    static menuType * ball9_menu;
    static menuType * carambol_menu;
    static menuType * snooker_menu;
    static menuType * player1_menu;
    static menuType * player2_menu;
    static menuType * player1_skill_menu;
    static menuType * player2_skill_menu;
    static menuType * player1_type_menu;
    static menuType * player2_type_menu;
    static menuType * tourfast_menu;
    static menuType * anisotrop_menu;
    static menuType * mshoot_menu;
    static menuType * freemove_menu;
    char str[256];
    int neuwert = 0;       // for the localeText variables inside some loops

    /* Building the Menus */

    /*
     MShoot Menu comes from Game Options
     */
    mshoot_menu = menu_new( menu_cb );
    // Real Mouse shoot
    menu_add_entry( mshoot_menu, localeText[234], MENU_ID_MSHOOT_NEW,localeText[270]);
    // CLassic Mouseshoot up/down
    menu_add_entry( mshoot_menu, localeText[235], MENU_ID_MSHOOT_CLASSIC,localeText[271]);
    //< back
    menu_add_exit ( mshoot_menu, localeText[63],localeText[266]);
    /*
     Tourfast Menu comes from Game Options
     */
    tourfast_menu = menu_new( menu_cb );
    //realtime 1.0
    menu_add_entry( tourfast_menu, localeText[230], MENU_ID_TOURFAST_NO,localeText[272]);
    //slow 2.0 times faster
    menu_add_entry( tourfast_menu, localeText[231], MENU_ID_TOURFAST_SLOW,localeText[273]);
    //middle 5.0 times faster
    menu_add_entry( tourfast_menu, localeText[70], MENU_ID_TOURFAST_MIDDLE,localeText[274]);
    //quicker 10.0 times faster
    menu_add_entry( tourfast_menu, localeText[71], MENU_ID_TOURFAST_QUICK,localeText[275]);
    //ultra quick 30.0 times faster
    menu_add_entry( tourfast_menu, localeText[72], MENU_ID_TOURFAST_ULTRA,localeText[276]);
    //< back
    menu_add_exit ( tourfast_menu, localeText[63],localeText[266]);

    /*
     Bumpwood Menu comes from Graphics/Reflections
     */
    bumpwood_menu = menu_new( menu_cb );
    //on
    menu_add_entry( bumpwood_menu, localeText[64], MENU_ID_BUMPWOOD_ON,localeText[277]);
    //off
    menu_add_entry( bumpwood_menu, localeText[65], MENU_ID_BUMPWOOD_OFF,localeText[278]);
    //< back
    menu_add_exit ( bumpwood_menu, localeText[63],localeText[266]);

    /*
     Bumpref Menu comes from Graphics/Reflections
     */
    bumpref_menu = menu_new( menu_cb );
    //on
    menu_add_entry( bumpref_menu, localeText[64], MENU_ID_BUMPREF_ON,localeText[279]);
    //off
    menu_add_entry( bumpref_menu, localeText[65], MENU_ID_BUMPREF_OFF,localeText[280]);
    //< back
    menu_add_exit ( bumpref_menu, localeText[63],localeText[266]);

    /*
     Resolution Menu comes from Graphics/Reflections, Resolution of Antialiasing
     */
     resolution_menu = menu_new( menu_cb );
    //low 16
    menu_add_entry(resolution_menu, localeText[69], MENU_RES_REND_LOW,localeText[281]);
    //medium 32
    menu_add_entry(resolution_menu, localeText[70], MENU_RES_REND_MED,localeText[282]);
    //high 64
    menu_add_entry(resolution_menu, localeText[71], MENU_RES_REND_HIGH,localeText[283]);
    //very high 128
    menu_add_entry(resolution_menu, localeText[72], MENU_RES_REND_VERYHIGH,localeText[284]);
    //back
    menu_add_exit(resolution_menu, localeText[63],localeText[266]);

    /*
     Render Menu comes from Graphics/Reflections
     */
    render_menu = menu_new( menu_cb );
    //spheremap standard
    menu_add_entry(render_menu, localeText[448], MENU_ID_REFLECTION_STANDARD,localeText[449]);
    //spheremap
    menu_add_entry(render_menu, localeText[66], MENU_ID_REFLECTION_SPHERE,localeText[285]);
    //rendered
    menu_add_entry(render_menu, localeText[67], MENU_ID_REFLECTION_RENDERED,localeText[286]);
    // Glass balls
    menu_add_entry(render_menu, localeText[68], MENU_ID_GLASSBALLS,localeText[287]);
    //< back
    menu_add_exit(render_menu, localeText[63],localeText[266]);

    /*
     Reflections Menu comes from Graphics Options
     */
    reflection_menu = menu_new( menu_cb );
    //Rendering
    menu_add_submenu(reflection_menu, localeText[208],render_menu, 1, NULL,localeText[288]);
    //Resolution
    switch(options_cuberef_res) {
      case 16:
        neuwert = 69;
        break;
      case 32:
        neuwert = 70;
        break;
      case 64:
        neuwert = 71;
        break;
      default:
        neuwert = 72;
      }
    menu_add_submenu(reflection_menu, localeText[209],resolution_menu, 1, localeText[neuwert],localeText[289]);
#ifndef WETAB
    //bump reflections
    menu_add_submenu(reflection_menu, localeText[106], bumpref_menu, 1, localeText[65-options_bumpref],localeText[290]);
    //bumpy wood frame
    menu_add_submenu(reflection_menu, localeText[107], bumpwood_menu, 1, localeText[65-options_bumpwood],localeText[291]);
#endif
    //< back
    menu_add_exit(reflection_menu, localeText[63],localeText[266]);

    /*
     Lights Menu comes from Graphics Options
     */
    light_menu = menu_new( menu_cb );
    //Positional
    menu_add_entry(light_menu, localeText[210], MENU_ID_LIGHT_POS,localeText[292]);
    //direct
    menu_add_entry(light_menu, localeText[211], MENU_ID_LIGHT_DIR,localeText[293]);
    //< back
    menu_add_exit (light_menu, localeText[63],localeText[266]);

    /*
     vsync come from Graphics Options
    */
    vsync_menu = menu_new( menu_cb );
    //on
    menu_add_entry(vsync_menu, localeText[64], MENU_ID_VSYNC_ON,localeText[452]);
    //off
    menu_add_entry(vsync_menu, localeText[65], MENU_ID_VSYNC_OFF,localeText[453]);
    //< back
    menu_add_exit (vsync_menu, localeText[63],localeText[266]);


#ifndef WETAB
    /*
    Videomode Menu comes from Graphics Options

    several pages for the modes: this should fix the problems with many display modes
    however the selected resolution displayed in the parent menu will be wrong
    when selected from other the 1st page
    */

    modes=sys_list_modes();
    for(mode_num=0;modes[mode_num].w > 957 && modes[mode_num].h > 749;mode_num++) {};
    //fprintf(stderr,"mode_num=%d\n",mode_num);
    menu_num=(mode_num+entr_num-1)/entr_num;
    if (menu_num<1) {
        menu_num=1;
        }
    //fprintf(stderr,"menu_num=%d\n",menu_num);
    videomode_menus = (menuType **)malloc(menu_num*sizeof(menuType *));

    for(i=0;i<menu_num;i++){
       videomode_menus[i] = menu_new( menu_cb );
       }
    videomode_menu=videomode_menus[0];

    if( mode_num==0 ){
        //<not available>
        menu_add_entry( videomode_menus[0], localeText[61], MENU_ID_IDLE,localeText[267] );
    }

    i=0;
    for( j=0 ; j<menu_num && i<mode_num ; j++ ){
       if(mode_num!=0) do {
            sprintf( str, "%dx%d", modes[i].w, modes[i].h );
            //fprintf(stderr, "%dx%d\n", modes[i].w, modes[i].h );
            menu_add_arg_entry(videomode_menus[j], str, MENU_ID_VIDMODE, (void *)&modes[i],NULL);
            i++;
       } while ( (i%entr_num)!=0 && i<mode_num );
       if( i<mode_num ){
            //more >
            menu_add_submenu(videomode_menus[j], localeText[62], videomode_menus[j+1], 0, NULL, localeText[269] );
       }
       //< back
       menu_add_exit (videomode_menus[j], localeText[63],localeText[268]);
    }
#endif

    /*
     Fullscreen Menu comes from Graphics Options
     */
    fullscreen_menu = menu_new( menu_cb );
    //fullscreen
    menu_add_entry(fullscreen_menu, localeText[82], MENU_ID_FULLSCREEN_ON,localeText[294]);
    //window
    menu_add_entry(fullscreen_menu, localeText[83], MENU_ID_FULLSCREEN_OFF,localeText[295]);
    //Back
    menu_add_exit(fullscreen_menu, localeText[63],localeText[266]);

    /*
      Antialias Max. Menu come from Antialias Menu
    */
    antialias_max = menu_new( menu_cb );
    //max 1
    menu_add_entry(antialias_max, localeText[456] , MENU_ID_ALIASMAX_1,localeText[455]);
    //max 2
    menu_add_entry(antialias_max, localeText[457], MENU_ID_ALIASMAX_2,localeText[455]);
    //max 4
    menu_add_entry(antialias_max, localeText[458], MENU_ID_ALIASMAX_4,localeText[455]);
    //max 8
    menu_add_entry(antialias_max, localeText[459], MENU_ID_ALIASMAX_8,localeText[455]);
    //Back
    menu_add_exit(antialias_max, localeText[63],localeText[266]);

    /*
      Antialias Menu come from Graphics Options
    */
    switch(options_maxfsaa) {
      case 8:
        neuwert = 3;
        break;
      case 4:
        neuwert = 2;
        break;
      case 1:
        neuwert = 0;
        break;
      default:
        neuwert = 1;
      }
    antialias_menu = menu_new( menu_cb );
    //Alias on
    menu_add_entry(antialias_menu, localeText[64] , MENU_ID_ALIAS_ON,localeText[296]);
    //Alias Off
    menu_add_entry(antialias_menu, localeText[65], MENU_ID_ALIAS_OFF,localeText[297]);
    //Alias FSAA Max (1,2,4, or 8, default: 2)
    menu_add_submenu(antialias_menu, localeText[454], antialias_max, 1, localeText[456+neuwert],localeText[455]);
    //Back
    menu_add_exit(antialias_menu, localeText[63],localeText[266]);

    /*
    Aim come from R/G Stereo
    */
    rgaim_menu = menu_new( menu_cb );
    //middle
    menu_add_entry(rgaim_menu, localeText[73], MENU_ID_RGAIM_MIDDLE,localeText[298]);
    //left
    menu_add_entry(rgaim_menu, localeText[74], MENU_ID_RGAIM_LEFT,localeText[299]);
    //right
    menu_add_entry(rgaim_menu, localeText[75], MENU_ID_RGAIM_RIGHT,localeText[300]);
    //< back
    menu_add_exit (rgaim_menu, localeText[63],localeText[266]);

    /*
    Stereo Enable/Disable come R/G Stereo
    */
    rgenable_menu = menu_new( menu_cb );
    //rg on
    menu_add_entry(rgenable_menu, localeText[76], MENU_ID_RGSTEREO_ON,localeText[301]);
    //rg off"
    menu_add_entry(rgenable_menu, localeText[77], MENU_ID_RGSTEREO_OFF,localeText[302]);
    //< back
    menu_add_exit (rgenable_menu, localeText[63],localeText[266]);

    /*
    R/G Stereo come from View Options
    */
    rgstereo_menu = menu_new( menu_cb );
    //enable
    menu_add_submenu( rgstereo_menu, localeText[78], rgenable_menu, 1, localeText[77-options_rgstereo_on],localeText[303]);
    //aim eye
    menu_add_submenu( rgstereo_menu, localeText[79], rgaim_menu, 1, localeText[75-options_rgaim],localeText[304]);
    //< back
    menu_add_exit ( rgstereo_menu, localeText[63],localeText[266]);

    /*
    Lensflare come from View Options
    */
    lensflare_menu = menu_new( menu_cb );
    //lensflare on
    menu_add_entry(lensflare_menu, localeText[80], MENU_ID_LENSFLARE_ON,localeText[305]);
    //lensflare off
    menu_add_entry(lensflare_menu, localeText[81], MENU_ID_LENSFLARE_OFF,localeText[306]);
    //< back
    menu_add_exit (lensflare_menu, localeText[63],localeText[266]);

    /*
    Balldetail come from View Options
    */
    balldetail_menu = menu_new( menu_cb );
    //low
    menu_add_entry(balldetail_menu, localeText[69], MENU_ID_BALL_DETAIL_LOW,localeText[307]);
    //medium
    menu_add_entry(balldetail_menu, localeText[70], MENU_ID_BALL_DETAIL_MED,localeText[308]);
    //high
    menu_add_entry(balldetail_menu, localeText[71], MENU_ID_BALL_DETAIL_HIGH,localeText[309]);
    //very high
    menu_add_entry(balldetail_menu, localeText[72], MENU_ID_BALL_DETAIL_VERYHIGH,localeText[310]);
    //< back
    menu_add_exit(balldetail_menu, localeText[63],localeText[266]);

    /*
     Table theme come from View Options
    */
    tabletheme_menu = menu_new( menu_cb );
    //gold-green
    menu_add_entry(tabletheme_menu, localeText[88], MENU_ID_TABLETHEME_GOLDGREEN,localeText[311]);
    //gold-red
    menu_add_entry(tabletheme_menu, localeText[89], MENU_ID_TABLETHEME_GOLDRED,localeText[312]);
    //chrome-blue
    menu_add_entry(tabletheme_menu, localeText[90], MENU_ID_TABLETHEME_CHROMEBLUE,localeText[313]);
    //black-beige
    menu_add_entry(tabletheme_menu, localeText[92], MENU_ID_TABLETHEME_BLACKBEIGE,localeText[314]);
    //black-beige
    menu_add_entry(tabletheme_menu, localeText[237], MENU_ID_TABLETHEME_TRON,localeText[315]);
    //< back
    menu_add_exit (tabletheme_menu, localeText[63],localeText[266]);

    /*
     roomtexture come from View Options
    */
    roomtexture_menu = menu_new( menu_cb );
    //on
    menu_add_entry(roomtexture_menu, localeText[64], MENU_ID_ROOM_ON,localeText[316]);
    //off
    menu_add_entry(roomtexture_menu, localeText[65], MENU_ID_ROOM_OFF,localeText[317]);
    //< back
    menu_add_exit (roomtexture_menu, localeText[63],localeText[266]);

    /*
     furnituretexture come from View Options
    */
    furnituretex_menu = menu_new( menu_cb );
    //on
    menu_add_entry(furnituretex_menu, localeText[64], MENU_ID_FURNITURE_ON,localeText[316]);
    //off
    menu_add_entry(furnituretex_menu, localeText[65], MENU_ID_FURNITURE_OFF,localeText[317]);
    //< back
    menu_add_exit (furnituretex_menu, localeText[63],localeText[266]);

    /*
     Controls come from Game Menu
    */
    control_menu = menu_new( menu_cb );
    //on
    menu_add_entry(control_menu, localeText[64], MENU_ID_CONTROLS_ON,localeText[316]);
    //off
    menu_add_entry(control_menu, localeText[65], MENU_ID_CONTROLS_OFF,localeText[317]);
    //< back
    menu_add_exit (control_menu, localeText[63],localeText[266]);

    /*
     Jump Shots come from Game Menu
    */
    jump_shot_menu = menu_new( menu_cb );
    //on
    menu_add_entry(jump_shot_menu, localeText[64], MENU_ID_JUMP_SHOT_ON,localeText[318]);
    //off
    menu_add_entry(jump_shot_menu, localeText[65], MENU_ID_JUMP_SHOT_OFF,localeText[319]);
    //< back
    menu_add_exit (jump_shot_menu, localeText[63],localeText[266]);

#ifndef TOUCH
    /*
     keypressed_menu come from Game Menu
    */
    keypressed_menu = menu_new( menu_cb );
    //on
    menu_add_entry(keypressed_menu, localeText[64], MENU_ID_CONTROL_KIND_ON,localeText[320]);
    //off
    menu_add_entry(keypressed_menu, localeText[65], MENU_ID_CONTROL_KIND_OFF,localeText[321]);
    //< back
    menu_add_exit (keypressed_menu, localeText[63],localeText[266]);
#endif

    /*
     Avatar come from Game Menu
    */
    avatar_menu = menu_new( menu_cb );
    //on
    menu_add_entry(avatar_menu, localeText[64], MENU_ID_AVATAR_ON,localeText[322]);
    //off
    menu_add_entry(avatar_menu, localeText[65], MENU_ID_AVATAR_OFF,localeText[323]);
    //< back
    menu_add_exit (avatar_menu, localeText[63],localeText[266]);

    /*
     Status Text come from Game Menu
    */
    status_menu = menu_new( menu_cb );
    //on
    menu_add_entry(status_menu, localeText[64], MENU_ID_STATUS_ON,localeText[324]);
    //off
    menu_add_entry(status_menu, localeText[65], MENU_ID_STATUS_OFF,localeText[325]);
    //< back
    menu_add_exit (status_menu, localeText[63],localeText[266]);

    /*
     Ball Traces come from Game Menu
    */
    balltrace_menu = menu_new( menu_cb );
    //on
    menu_add_entry(balltrace_menu, localeText[64], MENU_ID_BALLTRACE_ON,localeText[326]);
    //off
    menu_add_entry(balltrace_menu, localeText[65], MENU_ID_BALLTRACE_OFF,localeText[327]);
    //< back
    menu_add_exit (balltrace_menu, localeText[63],localeText[266]);

    /*
     freemove menu come from Game Menu
    */
    freemove_menu = menu_new( menu_cb );
    //on
    menu_add_entry(freemove_menu, localeText[64], MENU_ID_AUTOFREEMOVE_ON,localeText[328]);
    //off
    menu_add_entry(freemove_menu, localeText[65], MENU_ID_AUTOFREEMOVE_OFF,localeText[329]);
    //< back
    menu_add_exit (freemove_menu, localeText[63],localeText[266]);

    /*
     birdview_ai come from Game Menu
    */
    birdview_ai_menu = menu_new( menu_cb );
    //on
    menu_add_entry(birdview_ai_menu, localeText[64], MENU_ID_BIRDVIEW_AI_ON,localeText[330]);
    //off
    menu_add_entry(birdview_ai_menu, localeText[65], MENU_ID_BIRDVIEW_AI_OFF,localeText[331]);
    //< back
    menu_add_exit (birdview_ai_menu, localeText[63],localeText[266]);

    /*
     Help Lines come from Game Menu
    */
    helpline_menu = menu_new( menu_cb );
    //on
    menu_add_entry(helpline_menu, localeText[64], MENU_ID_HELPLINE_ON,localeText[332]);
    //off
    menu_add_entry(helpline_menu, localeText[65], MENU_ID_HELPLINE_OFF,localeText[333]);
    //< back
    menu_add_exit (helpline_menu, localeText[63],localeText[266]);

    /*
     Table Size come from Game Menu
    */
    tablesize_menu = menu_new( menu_cb );
    //7 foot
    menu_add_entry(tablesize_menu,localeText[84] , MENU_ID_TABLESIZE_7FOOT,localeText[334]);
    //8 foot
    menu_add_entry(tablesize_menu, localeText[85], MENU_ID_TABLESIZE_8FOOT,localeText[335]);
    //9 foot
    menu_add_entry(tablesize_menu, localeText[86], MENU_ID_TABLESIZE_9FOOT,localeText[336]);
    //12 foot
    menu_add_entry(tablesize_menu, localeText[87], MENU_ID_TABLESIZE_12FOOT,localeText[337]);
    //< back
    menu_add_exit (tablesize_menu, localeText[63],localeText[266]);

    /*
     8 Ball Network Host come from 8 Ball Network
    */
#ifdef NETWORKING
    /*
     netcompatible menu comes from 8,9,carambol and snooker network menu
     */
    netcompatible_menu8 = menu_new( menu_cb );
    // Net slow compatible mode on
    menu_add_entry( netcompatible_menu8, localeText[64], MENU_ID_NETCOMP_ON,localeText[338]);
    // Net slow compatible mode on
    menu_add_entry( netcompatible_menu8, localeText[65], MENU_ID_NETCOMP_OFF,localeText[339]);
    //< back
    menu_add_exit ( netcompatible_menu8, localeText[63],localeText[266]);
    netcompatible_menu9 = menu_new( menu_cb );
    // Net slow compatible mode on
    menu_add_entry( netcompatible_menu9, localeText[64], MENU_ID_NETCOMP_ON,localeText[338]);
    // Net slow compatible mode on
    menu_add_entry( netcompatible_menu9, localeText[65], MENU_ID_NETCOMP_OFF,localeText[339]);
    //< back
    menu_add_exit ( netcompatible_menu9, localeText[63],localeText[266]);
    netcompatible_menuc = menu_new( menu_cb );
    // Net slow compatible mode on
    menu_add_entry( netcompatible_menuc, localeText[64], MENU_ID_NETCOMP_ON,localeText[338]);
    // Net slow compatible mode on
    menu_add_entry( netcompatible_menuc, localeText[65], MENU_ID_NETCOMP_OFF,localeText[339]);
    //< back
    menu_add_exit ( netcompatible_menuc, localeText[63],localeText[266]);
    netcompatible_menus = menu_new( menu_cb );
    // Net slow compatible mode on
    menu_add_entry( netcompatible_menus, localeText[64], MENU_ID_NETCOMP_ON,localeText[338]);
    // Net slow compatible mode on
    menu_add_entry( netcompatible_menus, localeText[65], MENU_ID_NETCOMP_OFF,localeText[339]);
    //< back
    menu_add_exit ( netcompatible_menus, localeText[63],localeText[266]);
    /*
     netspeed menu comes from 8,9,carambol and snooker network menu
     */

    network_speed_menu8 = menu_new( menu_cb );
    //Slow (5 network packets per seconds)
    menu_add_entry(network_speed_menu8,localeText[241] , MENU_ID_NETSPEED_SLOW,localeText[340]);
    //Normal (10 network packets per seconds)
    menu_add_entry(network_speed_menu8,localeText[242] , MENU_ID_NETSPEED_NORMAL,localeText[341]);
    //Middle (15 network packets per seconds)
    menu_add_entry(network_speed_menu8,localeText[243] , MENU_ID_NETSPEED_MIDDLE,localeText[342]);
    //High (20 network packets per seconds)
    menu_add_entry(network_speed_menu8,localeText[244] , MENU_ID_NETSPEED_HIGH,localeText[343]);
    //< back
    menu_add_exit (network_speed_menu8, localeText[63],localeText[266]);
    network_speed_menu9 = menu_new( menu_cb );
    //Slow (5 network packets per seconds)
    menu_add_entry(network_speed_menu9,localeText[241] , MENU_ID_NETSPEED_SLOW,localeText[340]);
    //Normal (10 network packets per seconds)
    menu_add_entry(network_speed_menu9,localeText[242] , MENU_ID_NETSPEED_NORMAL,localeText[341]);
    //Middle (15 network packets per seconds)
    menu_add_entry(network_speed_menu9,localeText[243] , MENU_ID_NETSPEED_MIDDLE,localeText[342]);
    //High (20 network packets per seconds)
    menu_add_entry(network_speed_menu9,localeText[244] , MENU_ID_NETSPEED_HIGH,localeText[343]);
    //< back
    menu_add_exit (network_speed_menu9, localeText[63],localeText[266]);
    network_speed_menuc = menu_new( menu_cb );
    //Slow (5 network packets per seconds)
    menu_add_entry(network_speed_menuc,localeText[241] , MENU_ID_NETSPEED_SLOW,localeText[340]);
    //Normal (10 network packets per seconds)
    menu_add_entry(network_speed_menuc,localeText[242] , MENU_ID_NETSPEED_NORMAL,localeText[341]);
    //Middle (15 network packets per seconds)
    menu_add_entry(network_speed_menuc,localeText[243] , MENU_ID_NETSPEED_MIDDLE,localeText[342]);
    //High (20 network packets per seconds)
    menu_add_entry(network_speed_menuc,localeText[244] , MENU_ID_NETSPEED_HIGH,localeText[343]);
    //< back
    menu_add_exit (network_speed_menuc, localeText[63],localeText[266]);
    network_speed_menus = menu_new( menu_cb );
    //Slow (5 network packets per seconds)
    menu_add_entry(network_speed_menus,localeText[241] , MENU_ID_NETSPEED_SLOW,localeText[340]);
    //Normal (10 network packets per seconds)
    menu_add_entry(network_speed_menus,localeText[242] , MENU_ID_NETSPEED_NORMAL,localeText[341]);
    //Middle (15 network packets per seconds)
    menu_add_entry(network_speed_menus,localeText[243] , MENU_ID_NETSPEED_MIDDLE,localeText[342]);
    //High (20 network packets per seconds)
    menu_add_entry(network_speed_menus,localeText[244] , MENU_ID_NETSPEED_HIGH,localeText[343]);
    //< back
    menu_add_exit (network_speed_menus, localeText[63],localeText[266]);

    network_host8_menu = menu_new( menu_cb );
    //P1 Name:
    sprintf(str,localeText[122],player[0].name);
    menu_add_textfield( network_host8_menu, str, MENU_ID_PLAYER1_NAME, utf8count(localeText[122])-2,KEY_TYPE_MIXED,1,0,15,localeText[344]);
    //P2 Name:
    sprintf(str,localeText[123],player[1].name);
    menu_add_textfield( network_host8_menu, str, MENU_ID_PLAYER2_NAME, utf8count(localeText[123])-2,KEY_TYPE_MIXED,1,0,15,localeText[345]);
    //Port:
    sprintf(str,localeText[124],options_net_portnum);
    menu_add_textfield( network_host8_menu, str, MENU_ID_NETWORK_PORTNUM, utf8count(localeText[124])-2,KEY_TYPE_NUM,1024,65535,5,localeText[346]);
    //Start Game
    menu_add_entry (network_host8_menu, localeText[125], MENU_ID_NETWORK_HOST8,localeText[347]);
    //< back
    menu_add_exit (network_host8_menu, localeText[63],localeText[266]);

    /*
     8 Ball Network Join come from 8 Ball Network
    */
    network_join8_menu = menu_new( menu_cb );
    //Port:
    sprintf(str,localeText[124],options_net_portnum);
    menu_add_textfield( network_join8_menu, str, MENU_ID_NETWORK_PORTNUM, utf8count(localeText[124])-2,KEY_TYPE_NUM,1024,65535,5,localeText[346]);
    //IP:
    sprintf(str,localeText[126],options_net_hostname);
    menu_add_textfield( network_join8_menu, str, MENU_ID_NETWORK_IP, 4,KEY_TYPE_NETADDRESS,0,0,15,localeText[348]);
    //Start Game
    menu_add_entry (network_join8_menu, localeText[125], MENU_ID_NETWORK_JOIN,localeText[349]);
    //< back
    menu_add_exit (network_join8_menu, localeText[63],localeText[266]);

   /*
     8 Ball Network come from 8 Ball
    */
    net8ball_menu = menu_new( menu_cb );
    //As Host
    menu_add_submenu(net8ball_menu,localeText[127], network_host8_menu, 0, NULL,localeText[350]);
    //Join
    menu_add_submenu(net8ball_menu, localeText[128], network_join8_menu, 0, NULL,localeText[351]);
    // Network Speed
    menu_add_submenu (net8ball_menu, localeText[240], network_speed_menu8, 1, localeText[240+options_net_speed],localeText[352]);
    // Network slow compatibilty netcompatible_menu
    menu_add_submenu (net8ball_menu, localeText[265], netcompatible_menu8, 1, localeText[65-options_net_compatible],localeText[353]);
    //< back
    menu_add_exit (net8ball_menu, localeText[63],localeText[266]);
#endif

    /*
     8 Ball come from Game Type
    */
    ball8_menu = menu_new( menu_cb );
    //One Player
    menu_add_entry(ball8_menu, localeText[212], MENU_ID_GAMETYPE_8BALL,localeText[354]);
    //Tournament
    menu_add_entry(ball8_menu, localeText[120], MENU_ID_8BALL_TOURN,localeText[355]);
    //Network
#ifdef NETWORKING
    menu_add_submenu(ball8_menu, localeText[135], net8ball_menu, 0, NULL,localeText[356]);
#endif
    //Training

    menu_add_entry(ball8_menu, localeText[121], MENU_ID_TRAINING_8BALL,localeText[357]);
    //< back
    menu_add_exit (ball8_menu, localeText[63],localeText[266]);

    /*
     9 Ball Network Host come from 9 Ball Network
    */
#ifdef NETWORKING
    network_host9_menu = menu_new( menu_cb );
    //P1 Name:
    sprintf(str,localeText[122],player[0].name);
    menu_add_textfield( network_host9_menu, str, MENU_ID_PLAYER1_NAME, utf8count(localeText[122])-2,KEY_TYPE_MIXED,1,0,15,localeText[344]);
    //P2 Name:
    sprintf(str,localeText[123],player[1].name);
    menu_add_textfield( network_host9_menu, str, MENU_ID_PLAYER2_NAME, utf8count(localeText[123])-2, KEY_TYPE_MIXED,1,0,15,localeText[345]);
    //Port:
    sprintf(str,localeText[124],options_net_portnum);
    menu_add_textfield( network_host9_menu, str, MENU_ID_NETWORK_PORTNUM, utf8count(localeText[124])-2,KEY_TYPE_NUM,1024,65535,5,localeText[346]);
    //Start Game
    menu_add_entry (network_host9_menu, localeText[125], MENU_ID_NETWORK_HOST9,localeText[347]);
    //< back
    menu_add_exit (network_host9_menu, localeText[63],localeText[266]);

    /*
     9 Ball Network Join come from 9 Ball Network
    */
    network_join9_menu = menu_new( menu_cb );
    //Port:
    sprintf(str,localeText[124],options_net_portnum);
    menu_add_textfield( network_join9_menu, str, MENU_ID_NETWORK_PORTNUM, utf8count(localeText[124])-2,KEY_TYPE_NUM,1024,65535,5,localeText[346]);
    //IP:
    sprintf(str,localeText[126],options_net_hostname);
    menu_add_textfield( network_join9_menu, str, MENU_ID_NETWORK_IP, 4,KEY_TYPE_NETADDRESS,0,0,15,localeText[348]);
    //Start Game
    menu_add_entry (network_join9_menu, localeText[125], MENU_ID_NETWORK_JOIN,localeText[349]);
    //< back
    menu_add_exit (network_join9_menu, localeText[63],localeText[266]);

   /*
     9 Ball Network come from 9 Ball
    */
    net9ball_menu = menu_new( menu_cb );
    //As Host
    menu_add_submenu(net9ball_menu,localeText[127], network_host9_menu, 0, NULL,localeText[350]);
    //Join
    menu_add_submenu(net9ball_menu, localeText[128], network_join9_menu, 0, NULL,localeText[351]);
    // Network Speed
    menu_add_submenu (net9ball_menu, localeText[240], network_speed_menu9, 1, localeText[240+options_net_speed],localeText[352]);
    // Network slow compatibilty netcompatible_menu
    menu_add_submenu (net9ball_menu, localeText[265], netcompatible_menu9, 1, localeText[65-options_net_compatible],localeText[353]);
    //< back
    menu_add_exit (net9ball_menu, localeText[63],localeText[266]);
#endif

    /*
     9 Ball come from Game Type
    */
    ball9_menu = menu_new( menu_cb );
    //One Player
    menu_add_entry(ball9_menu, localeText[212], MENU_ID_GAMETYPE_9BALL,localeText[354]);
    //Tournament
    menu_add_entry(ball9_menu, localeText[120], MENU_ID_9BALL_TOURN,localeText[355]);
#ifdef NETWORKING
    //Network
    menu_add_submenu(ball9_menu, localeText[135], net9ball_menu, 0, NULL,localeText[356]);
#endif
    //Training
    menu_add_entry(ball9_menu, localeText[121], MENU_ID_TRAINING_9BALL,localeText[357]);
    //< back
    menu_add_exit (ball9_menu, localeText[63],localeText[266]);

    /*
     Carambol Network Host come from Carambol Network
    */
#ifdef NETWORKING
    network_hostcarambol_menu = menu_new( menu_cb );
    //P1 Name:
    sprintf(str,localeText[122],player[0].name);
    menu_add_textfield( network_hostcarambol_menu, str, MENU_ID_PLAYER1_NAME, utf8count(localeText[122])-2,KEY_TYPE_MIXED,1,0,15,localeText[344]);
    //P2 Name:
    sprintf(str,localeText[123],player[1].name);
    menu_add_textfield( network_hostcarambol_menu, str, MENU_ID_PLAYER2_NAME, utf8count(localeText[123])-2,KEY_TYPE_MIXED,1,0,15,localeText[345]);
    //Port:
    sprintf(str,localeText[124],options_net_portnum);
    menu_add_textfield( network_hostcarambol_menu, str, MENU_ID_NETWORK_PORTNUM, utf8count(localeText[124])-2,KEY_TYPE_NUM,1024,65535,5,localeText[346]);
    //Start Game
    menu_add_entry (network_hostcarambol_menu, localeText[125], MENU_ID_NETWORK_HOSTC,localeText[347]);
    //< back
    menu_add_exit (network_hostcarambol_menu, localeText[63],localeText[266]);

    /*
     Carambol Network Join come from Carambol Network
    */
    network_joincarambol_menu = menu_new( menu_cb );
    //Port:
    sprintf(str,localeText[124],options_net_portnum);
    menu_add_textfield( network_joincarambol_menu, str, MENU_ID_NETWORK_PORTNUM, utf8count(localeText[124])-2,KEY_TYPE_NUM,1024,65535,5,localeText[346]);
    //IP:
    sprintf(str,localeText[126],options_net_hostname);
    menu_add_textfield( network_joincarambol_menu, str, MENU_ID_NETWORK_IP, 4,KEY_TYPE_NETADDRESS,0,0,15,localeText[348]);
    //Start Game
    menu_add_entry (network_joincarambol_menu, localeText[125], MENU_ID_NETWORK_JOIN,localeText[349]);
    //< back
    menu_add_exit (network_joincarambol_menu, localeText[63],localeText[266]);

   /*
     Carambol Network come from Carambol
    */
    netcarambol_menu = menu_new( menu_cb );
    //As Host
    menu_add_submenu(netcarambol_menu,localeText[127], network_hostcarambol_menu, 0, NULL,localeText[350]);
    //Join
    menu_add_submenu(netcarambol_menu, localeText[128], network_joincarambol_menu, 0, NULL,localeText[351]);
    // Network Speed
    menu_add_submenu (netcarambol_menu, localeText[240], network_speed_menuc, 1, localeText[240+options_net_speed],localeText[352]);
    // Network slow compatibilty netcompatible_menu
    menu_add_submenu (netcarambol_menu, localeText[265], netcompatible_menuc, 1, localeText[65-options_net_compatible],localeText[353]);
    //< back
    menu_add_exit (netcarambol_menu, localeText[63],localeText[266]);
#endif

    /*
     Carambol come from Game Type
    */
    carambol_menu = menu_new( menu_cb );
    //Points for winning options
    sprintf(str,localeText[213],options_maxp_carambol);
    menu_add_textfield( carambol_menu, str, MENU_ID_MAXP_CARAMBOL, utf8count(localeText[213])-2,KEY_TYPE_NUM,1,100,3,localeText[358]);
    //One Player
    menu_add_entry(carambol_menu, localeText[212], MENU_ID_GAMETYPE_CARAMBOL,localeText[354]);
    //Tournament
    menu_add_entry(carambol_menu, localeText[120], MENU_ID_CARAMBOL_TOURN,localeText[355]);
#ifdef NETWORKING
    //Network
    menu_add_submenu(carambol_menu, localeText[135], netcarambol_menu, 0, NULL,localeText[356]);
#endif
    //Training
    menu_add_entry(carambol_menu, localeText[121], MENU_ID_TRAINING_CARAMBOL,localeText[357]);
    //< back
    menu_add_exit (carambol_menu, localeText[63],localeText[266]);

    /*
     Snooker Network Host come from Snooker Network
    */
#ifdef NETWORKING
    network_hostsnooker_menu = menu_new( menu_cb );
    //P1 Name:
    sprintf(str,localeText[122],player[0].name);
    menu_add_textfield( network_hostsnooker_menu, str, MENU_ID_PLAYER1_NAME, utf8count(localeText[122])-2,KEY_TYPE_MIXED,1,0,15,localeText[344]);
    //P2 Name:
    sprintf(str,localeText[123],player[1].name);
    menu_add_textfield( network_hostsnooker_menu, str, MENU_ID_PLAYER2_NAME, utf8count(localeText[123])-2,KEY_TYPE_MIXED,1,0,15,localeText[345]);
    //Port:
    sprintf(str,localeText[124],options_net_portnum);
    menu_add_textfield( network_hostsnooker_menu, str, MENU_ID_NETWORK_PORTNUM, utf8count(localeText[124])-2,KEY_TYPE_NUM,1024,65535,5,localeText[346]);
    //Start Game
    menu_add_entry (network_hostsnooker_menu, localeText[125], MENU_ID_NETWORK_HOSTS,localeText[347]);
    //< back
    menu_add_exit (network_hostsnooker_menu, localeText[63],localeText[266]);

    /*
     Snooker Network Join come from Snooker Network
    */
    network_joinsnooker_menu = menu_new( menu_cb );
    //Port:
    sprintf(str,localeText[124],options_net_portnum);
    menu_add_textfield( network_joinsnooker_menu, str, MENU_ID_NETWORK_PORTNUM, utf8count(localeText[124])-2,KEY_TYPE_NUM,1024,65535,5,localeText[346]);
    //IP:
    sprintf(str,localeText[126],options_net_hostname);
    menu_add_textfield( network_joinsnooker_menu, str, MENU_ID_NETWORK_IP, 4,KEY_TYPE_NETADDRESS,0,0,15,localeText[348]);
    //Start Game
    menu_add_entry (network_joinsnooker_menu, localeText[125], MENU_ID_NETWORK_JOIN,localeText[349]);
    //< back
    menu_add_exit (network_joinsnooker_menu, localeText[63],localeText[266]);

   /*
     Snooker Network come from Snooker
    */
    netsnooker_menu = menu_new( menu_cb );
    //As Host
    menu_add_submenu(netsnooker_menu,localeText[127], network_hostsnooker_menu, 0, NULL,localeText[350]);
    //Join
    menu_add_submenu(netsnooker_menu, localeText[128], network_joinsnooker_menu, 0, NULL,localeText[351]);
    // Network Speed
    menu_add_submenu (netsnooker_menu, localeText[240], network_speed_menus, 1, localeText[240+options_net_speed],localeText[352]);
    // Network slow compatibilty netcompatible_menu
    menu_add_submenu (netsnooker_menu, localeText[265], netcompatible_menus, 1, localeText[65-options_net_compatible],localeText[353]);
    //< back
    menu_add_exit (netsnooker_menu, localeText[63],localeText[266]);
#endif

    /*
     Snooker come from Game Type
    */
    snooker_menu = menu_new( menu_cb );
    //One Player
    menu_add_entry(snooker_menu, localeText[212], MENU_ID_GAMETYPE_SNOOKER,localeText[354]);
    //Tournament
    menu_add_entry(snooker_menu, localeText[120], MENU_ID_SNOOKER_TOURN,localeText[355]);
#ifdef NETWORKING
    //Network
    menu_add_submenu(snooker_menu, localeText[135], netsnooker_menu, 0, NULL,localeText[356]);
#endif
    //Training
    menu_add_entry(snooker_menu, localeText[121], MENU_ID_TRAINING_SNOOKER,localeText[357]);
    //< back
    menu_add_exit (snooker_menu, localeText[63],localeText[266]);

    /*
     Game type come from Game Menu
    */
    gametype_menu = menu_new( menu_cb );
    //8 ball
    menu_add_submenu(gametype_menu, localeText[93], ball8_menu, 0, NULL,localeText[359]);
    //9 ball
    menu_add_submenu(gametype_menu, localeText[94], ball9_menu, 0, NULL,localeText[360]);
    //carambol
    menu_add_submenu(gametype_menu, localeText[95], carambol_menu, 0, NULL,localeText[361]);
    //snooker
    menu_add_submenu(gametype_menu, localeText[96], snooker_menu, 0, NULL,localeText[362]);
    //< back
    menu_add_exit (gametype_menu, localeText[63],localeText[266]);

    /********************************************************/
    /*
     Game Menu come from Main Menu
    */
    game_menu = menu_new( menu_cb );
    //game type
    menu_add_submenu(game_menu, localeText[99], gametype_menu, 0, NULL,localeText[363]);
    //table size
    //fprintf(stderr,"table menu: %f\n",options_table_size);
    switch((int)(options_table_size/0.3048)) {
      case 6:
      case 7:
        neuwert = 84;
        break;
      case 8:
        neuwert = 85;
        break;
      case 9:
        neuwert = 86;
        break;
      default:
        neuwert = 87;
        break;
      }
    menu_add_submenu(game_menu, localeText[97], tablesize_menu, 1, localeText[neuwert],localeText[364]);
    //Show Control bar
    menu_add_submenu(game_menu, localeText[228], control_menu, 1, localeText[65-options_show_buttons],localeText[365]);
    //jump shots
    menu_add_submenu(game_menu, localeText[216], jump_shot_menu, 1, localeText[65-options_jump_shots],localeText[366]);
    //Mouse shots
    menu_add_submenu(game_menu, localeText[233], mshoot_menu, 1, localeText[235-options_mouseshoot],localeText[367]);
    //Auto free move view
    menu_add_submenu(game_menu, localeText[236], freemove_menu, 1, localeText[65-options_auto_freemove],localeText[368]);
#ifndef TOUCH
    //keypressed behaviour for special keys (MENU_ID_CONTROL_KIND_ON/MENU_ID_CONTROL_KIND_OFF)
    menu_add_submenu(game_menu, localeText[226], keypressed_menu, 1, localeText[65-options_control_kind],localeText[369]);
#endif
    //birdview_ai_menu - shows birdview on AI or Net-Player (on/off)
    menu_add_submenu(game_menu, localeText[227], birdview_ai_menu, 1, localeText[65-options_ai_birdview],localeText[370]);
    //Tournament timelapse
    switch((int)options_tourfast) {
      case 2:
        neuwert = 231;
        break;
      case 5:
        neuwert = 70;
        break;
      case 10:
        neuwert = 71;
        break;
      case 30:
        neuwert = 72;
        break;
      default:
        neuwert = 230;
      }
    menu_add_submenu(game_menu, localeText[229], tourfast_menu, 1, localeText[neuwert],localeText[371]);
    //< back
    menu_add_exit (game_menu, localeText[63],localeText[266]);

    /********************************************************/
    /*
     View Options come from Main Menu
    */
    view_menu = menu_new( menu_cb );
    //red/green stereo
    menu_add_submenu(view_menu, localeText[102], rgstereo_menu, 0, NULL,localeText[372]);
    //lensflare
    menu_add_submenu(view_menu, localeText[103], lensflare_menu, 1, localeText[65-options_lensflare],localeText[373]);
    //ball detail
    switch(options_max_ball_detail) {
      case options_max_ball_detail_LOW:
        neuwert = 69;
        break;
      case options_max_ball_detail_MED:
        neuwert = 70;
        break;
      case options_max_ball_detail_HIGH:
        neuwert = 71;
        break;
      default:
        neuwert = 72;
      }
    menu_add_submenu(view_menu, localeText[104], balldetail_menu, 1, localeText[neuwert],localeText[374]);
    //table theme
    switch(options_table_color) {
      case options_table_color_red:
        neuwert = 89;
        break;
      case options_table_color_blue:
        neuwert = 90;
        break;
      case options_table_color_beige:
        neuwert = 91;
        break;
      case options_table_color_black:
        neuwert = 92;
        break;
      default:
        neuwert = 88;
      }
    menu_add_submenu(view_menu, localeText[108], tabletheme_menu, 1, localeText[neuwert],localeText[375]);
    //room textures on/off
    menu_add_submenu(view_menu, localeText[417], roomtexture_menu, 1, localeText[65-options_deco],localeText[418]);
    //furniture textures on/off
    menu_add_submenu(view_menu, localeText[419], furnituretex_menu, 1, localeText[65-options_furniture],localeText[420]);
    //help line
    menu_add_submenu(view_menu, localeText[98], helpline_menu, 1, localeText[65-vline_on],localeText[376]);
    //ball traces
    menu_add_submenu(view_menu, localeText[109], balltrace_menu, 1, localeText[65-options_balltrace],localeText[377]);
    //Statustext
    menu_add_submenu(view_menu, localeText[214], status_menu, 1, localeText[65-options_status_text],localeText[378]);
    //Avatar
    menu_add_submenu(view_menu, localeText[215], avatar_menu, 1, localeText[65-options_avatar_on],localeText[379]);
#ifndef USE_WIN
    //Default Browser for Manual (only Linux)
    sprintf(str,localeText[19],options_browser);
    menu_add_textfield( view_menu, str, MENU_ID_BROWSER, utf8count(localeText[19])-2,KEY_TYPE_MIXED,1,0,15,localeText[380]);
#endif
    //< back
    menu_add_exit (view_menu, localeText[63],localeText[266]);

    /*
     Anisotrop menu come from Graphic Options
    */
    //Anisothropic filter
    if(options_anisotrop && options_maxanisotrop > 0.0f) {
      anisotrop_menu = menu_new( menu_cb );
      //in case of more options
      for(maxi=0.0;maxi<(options_maxanisotrop+1.0f) && maxi<9.0f;maxi+=1.0f) {
         //Value %01.0f and max = 8
         sprintf(str,localeText[232],maxi);
         menu_add_value_entry(anisotrop_menu, str, MENU_ID_ANISOTROP, maxi,NULL);
      }
      //< back
      menu_add_exit (anisotrop_menu, localeText[63],localeText[266]);
    }

    /********************************************************/
    /*
      Graphic Options come from Main Menu
    */
    g_options_menu = menu_new( menu_cb );
    //Anisothropic filter
    if(options_anisotrop && options_maxanisotrop > 0.0) {
      menu_add_submenu(g_options_menu, localeText[217] , anisotrop_menu, 1, NULL,localeText[381]);
      }
    //Antialiasing
    menu_add_submenu(g_options_menu, localeText[218] , antialias_menu, 1, localeText[65-options_antialiasing],localeText[382]);
#ifndef WETAB
    //Resolution
    menu_add_submenu(g_options_menu, localeText[100], videomode_menu,  1, NULL,localeText[383]);
    //view mode
    menu_add_submenu(g_options_menu, localeText[101], fullscreen_menu, 1, NULL,localeText[384]);
#endif
    //reflections
    menu_add_submenu(g_options_menu, localeText[105], reflection_menu, 0, NULL,localeText[385]);
    //Light Options
    menu_add_submenu(g_options_menu, localeText[219], light_menu, 1, localeText[211-options_positional_light],localeText[386]);
    //Vsync Options
    menu_add_submenu(g_options_menu, localeText[451], vsync_menu, 1, localeText[65-options_vsync],localeText[450]);
    //Back
    menu_add_exit(g_options_menu, localeText[63],localeText[266]);
#ifdef USE_SOUND
    /********************************************************/
    /*
      Soundmenu come from Main Menu
    */
    sound_menu = menu_new( menu_cb );
    //Sound on
    menu_add_entry(sound_menu, localeText[64] , MENU_ID_SOUND_ON,localeText[387]);
    //Sound Off
    menu_add_entry(sound_menu, localeText[65], MENU_ID_SOUND_OFF,localeText[388]);
    //Back
    menu_add_exit(sound_menu, localeText[63],localeText[266]);
    /********************************************************/
    /*
      Musicmenu come from Main Menu
    */
    music_menu = menu_new( menu_cb );
    //Sound on
    menu_add_entry(music_menu, localeText[64] , MENU_ID_MUSIC_ON,localeText[387]);
    //Sound Off
    menu_add_entry(music_menu, localeText[65], MENU_ID_MUSIC_OFF,localeText[388]);
    //Back
    menu_add_exit(music_menu, localeText[63],localeText[266]);
#endif
    /*
      P1 Skill come from Player1
    */
    player1_skill_menu = menu_new( menu_cb );
    //excellent
    menu_add_entry(player1_skill_menu, localeText[112], MENU_ID_PLAYER1_SKILL_EXCEL,localeText[389]);
    //good
    menu_add_entry(player1_skill_menu, localeText[113], MENU_ID_PLAYER1_SKILL_GOOD,localeText[390]);
    //medium
    menu_add_entry(player1_skill_menu, localeText[114], MENU_ID_PLAYER1_SKILL_MEDIUM,localeText[391]);
    //bad
    menu_add_entry(player1_skill_menu, localeText[115], MENU_ID_PLAYER1_SKILL_BAD,localeText[392]);
    //worse
    menu_add_entry(player1_skill_menu, localeText[116], MENU_ID_PLAYER1_SKILL_WORSE,localeText[393]);
    //< back
    menu_add_exit (player1_skill_menu, localeText[63],localeText[266]);

    /*
      P2 Skill come from Player2
    */
    player2_skill_menu = menu_new( menu_cb );
    //excellent
    menu_add_entry(player2_skill_menu, localeText[112], MENU_ID_PLAYER2_SKILL_EXCEL,localeText[389]);
    //good
    menu_add_entry(player2_skill_menu, localeText[113], MENU_ID_PLAYER2_SKILL_GOOD,localeText[390]);
    //medium
    menu_add_entry(player2_skill_menu, localeText[114], MENU_ID_PLAYER2_SKILL_MEDIUM,localeText[391]);
    //bad
    menu_add_entry(player2_skill_menu, localeText[115], MENU_ID_PLAYER2_SKILL_BAD,localeText[392]);
    //worse
    menu_add_entry(player2_skill_menu, localeText[116], MENU_ID_PLAYER2_SKILL_WORSE,localeText[393]);
    //< back;
    menu_add_exit (player2_skill_menu, localeText[63],localeText[266]);

    /*
      P1 Type come from Player1
    */
    player1_type_menu = menu_new( menu_cb );
    //AI
    menu_add_entry(player1_type_menu, localeText[55], MENU_ID_PLAYER1_TYPE_AI,localeText[394]);
    //Human
    menu_add_entry(player1_type_menu, localeText[56], MENU_ID_PLAYER1_TYPE_HUMAN,localeText[395]);
    //< back
    menu_add_exit (player1_type_menu, localeText[63],localeText[266]);

    /*
      P2 Skill come from Player2
    */
    player2_type_menu = menu_new( menu_cb );
    //AI
    menu_add_entry(player2_type_menu, localeText[55], MENU_ID_PLAYER2_TYPE_AI,localeText[394]);
    //Human
    menu_add_entry(player2_type_menu, localeText[56], MENU_ID_PLAYER2_TYPE_HUMAN,localeText[395]);
    //< back
    menu_add_exit (player2_type_menu, localeText[63],localeText[266]);

    /*
      Player1 come from Player Menu
    */
    player1_menu = menu_new( menu_cb );
    //P1 Name:
    sprintf(str,localeText[122],player[0].name);
    menu_add_textfield( player1_menu, str, MENU_ID_PLAYER1_NAME, utf8count(localeText[122])-2,KEY_TYPE_MIXED,1,0,15,localeText[344]);
    //P1 Type
    menu_add_submenu (player1_menu, localeText[146],  player1_type_menu, 1, localeText[56-player[0].is_AI],localeText[396]);
    //P1 Skill
    switch((int)(10*player[0].err)) {
      case 0:
        neuwert = 112;
        break;
      case 1:
        neuwert = 113;
        break;
      case 3:
        neuwert = 114;
        break;
      case 6:
        neuwert = 115;
        break;
      default:
        neuwert = 116;
      }
    menu_add_submenu (player1_menu, localeText[147], player1_skill_menu, 1, localeText[neuwert],localeText[397]);
    //< back
    menu_add_exit (player1_menu, localeText[63],localeText[266]);

    /*
      Player2 come from Player Menu
    */
    player2_menu = menu_new( menu_cb );
    //P2 Name:
    sprintf(str,localeText[123],player[1].name);
    menu_add_textfield( player2_menu, str,  MENU_ID_PLAYER2_NAME, utf8count(localeText[123])-2,KEY_TYPE_MIXED,1,0,15,localeText[345]);
    //"P2 Type
    menu_add_submenu (player2_menu, localeText[117],  player2_type_menu, 1, localeText[56-player[1].is_AI],localeText[396]);
    //P2 Skill
    switch((int)(10*player[1].err)) {
      case 0:
        neuwert = 112;
        break;
      case 1:
        neuwert = 113;
        break;
      case 3:
        neuwert = 114;
        break;
      case 6:
        neuwert = 115;
        break;
      default:
        neuwert = 116;
      }
    menu_add_submenu (player2_menu, localeText[118], player2_skill_menu, 1, localeText[neuwert],localeText[397]);
    //< back
    menu_add_exit (player2_menu, localeText[63],localeText[266]);

    /********************************************************/
    /*
      Player Menu come from Main Menu
    */
    player_menu = menu_new( menu_cb );
    //Player1
    menu_add_submenu(player_menu, localeText[129], player1_menu, 0, NULL,localeText[398]);
    //Player2
    menu_add_submenu(player_menu,localeText[130], player2_menu, 0, NULL,localeText[399]);
    //< back
    menu_add_exit (player_menu, localeText[63],localeText[266]);

    /********************************************************/
    /*
      question to quit the whole program come from Main Menu
    */
    quit_menu = menu_new( menu_cb );
    //YES  out'a here
    menu_add_entry(quit_menu, localeText[110],MENU_ID_MAIN_QUIT,localeText[400]);
    //NO  continue
    menu_add_exit(quit_menu, localeText[111],localeText[401]);


    /********************************************************/
    /*
      Main menu
    */
    g_main_menu = menu_new( menu_cb );
    //Resume
    menu_add_exit(g_main_menu, localeText[139],localeText[401]);
    //Restart Game
    menu_add_entry(g_main_menu, localeText[220], MENU_ID_RESTART,localeText[402]);
    //Player Menu
    menu_add_submenu(g_main_menu, localeText[221], player_menu, 0, NULL,localeText[403]);
    //Game Options
    menu_add_submenu(g_main_menu, localeText[140], game_menu, 0, NULL,localeText[404]);
    //View Options
    menu_add_submenu(g_main_menu, localeText[222], view_menu, 0, NULL,localeText[405]);
    //Graphic Options
    menu_add_submenu(g_main_menu, localeText[141], g_options_menu, 0, NULL,localeText[406]);
#ifdef USE_SOUND
    //Sound
    menu_add_submenu(g_main_menu, localeText[223], sound_menu, 1, localeText[65-options_use_sound],localeText[407]);
    //Music
    menu_add_submenu(g_main_menu, localeText[422], music_menu, 1, localeText[65-options_use_music],localeText[407]);
#endif
    //Help
    menu_add_entry(g_main_menu, localeText[142], MENU_ID_MAIN_HELP,localeText[408]);
    //Manual
    if(manual_available()) {
       menu_add_entry(g_main_menu, localeText[224], MENU_ID_MANUAL,localeText[409]);
    }
    if(check_xml("history.xml")) {
       menu_add_entry(g_main_menu, localeText[443], MENU_ID_HISTORY,localeText[441]);
    }
    if(check_xml("tournament.xml")) {
       menu_add_entry(g_main_menu, localeText[444], MENU_ID_TOURN_HISTORY,localeText[442]);
    }
    //Quit
    menu_add_submenu(g_main_menu,localeText[143], quit_menu, 0, NULL,localeText[410]);

    g_act_menu = (menuType *)0;
}
