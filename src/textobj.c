/* textobj.c
**
**    quad with text as texture using OpenGL
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

#include <stdlib.h>
#ifdef __APPLE__
 #include <OpenGL/gl.h>
 #include <OpenGL/glu.h>
#else
 #include <GL/gl.h>
 #include <GL/glu.h>
#endif
#include <stdio.h>
#include <string.h>
#include "options.h"
#include "font.h"
#include "textobj.h"

/***********************************************************************/

int create_string_quad( char * str, char * fontname, int h, int * quad_id, int * tex_id, VMfloat * quad_w, VMfloat * quad_h )
{
    int    quad_obj;
    GLuint texbind;
    int    texw,texh, width,height;
    char * texdata;
    VMfloat fact;
#define MAX_TEXW 256
#define MAX_TEXH 256

    getStringPixmapFT( str, fontname, h, &texdata, &texw, &texh, &width, &height );
    glGenTextures(1,&texbind);
    glBindTexture(GL_TEXTURE_2D,texbind);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 1, texw, texh, GL_LUMINANCE,
                      GL_UNSIGNED_BYTE, texdata);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options_tex_min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options_tex_mag_filter);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    free(texdata);

    fact=(VMfloat)h;
    quad_obj = glGenLists(1);
    glNewList(quad_obj, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D,texbind);
    GLfloat VertexData[] = {0.0,1.0*fact,0.0,(GLfloat)width/(GLfloat)height*fact,1.0*fact,0.0,(GLfloat)width/(GLfloat)height*fact,0.0,0.0,0.0,0.0,0.0};
    GLfloat TexData[] = {0.0,0.0,(GLfloat)width/(GLfloat)texw,0.0,(GLfloat)width/(GLfloat)texw,(GLfloat)height/(GLfloat)texh,0.0,(GLfloat)height/(GLfloat)texh};
    GLshort NormalData[] = {0,0,1,0,0,1,0,0,1,0,0,1};
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glTexCoordPointer(2,GL_FLOAT, 0, TexData);
    glVertexPointer(3, GL_FLOAT, 0, VertexData);
    glNormalPointer(GL_SHORT, 0, NormalData);
    glPushMatrix();
    glDrawArrays(GL_QUADS,0,4);
    glPopMatrix();
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glEndList();

    *quad_h=fact*1.0;
    *quad_w=(VMfloat)width/(VMfloat)height*fact;

    if(quad_id!=0) *quad_id=quad_obj;
    if(tex_id!=0) *tex_id=texbind;

    return(quad_obj);
}

/***********************************************************************/

textObj * textObj_new(char * str, char * fontname, int height )
{
    textObj * obj;
    obj=malloc(sizeof(textObj));
    obj->height=height;
    obj->depth3D=0.0;  /* for toggling to 3D */
    obj->is_3D=0;
    strcpy(obj->str,str);
    strcpy(obj->fontname,fontname);
    create_string_quad( obj->str, obj->fontname, obj->height, &(obj->quad_id), &(obj->tex_id), &(obj->quad_w), &(obj->quad_h) );
    return obj;
}

/***********************************************************************/

textObj * textObj3D_new(char * str, char * fontname, VMfloat height, VMfloat depth)
{
    textObj * obj;
    obj=malloc(sizeof(textObj));
    obj->height=height;
    obj->depth3D=depth;
    obj->is_3D=1;
    strcpy(obj->str,str);
    strcpy(obj->fontname,fontname);
    obj->obj3D_id=getStringGLListFT( obj->str, obj->fontname, obj->height, obj->depth3D, &(obj->obj3D_w), &(obj->obj3D_h) );
    return obj;
}

/***********************************************************************/

void textObj_setText( textObj * obj, char * str )
{
    if( strcmp(str,obj->str)!=0 ){
        if(! obj->is_3D){
            //fprintf(stderr,"textObj_setText: obj=%p, text=%s\n",obj, str);
            //fprintf(stderr,"textObj_setText 0\n");
            glDeleteLists( obj->quad_id, 1 );
            //fprintf(stderr,"textObj_setText 0.5\n");
            obj->quad_id=0;
            //fprintf(stderr,"obj->tex_id=%d\n",obj->tex_id);
            if( glIsTexture(obj->tex_id) == GL_TRUE )
                glDeleteTextures( 1, (GLuint *)&(obj->tex_id) );
            //fprintf(stderr,"textObj_setText 0.75\n");
            obj->tex_id=0;
            strcpy(obj->str,str);
            //fprintf(stderr,"textObj_setText 1\n");
            create_string_quad( obj->str, obj->fontname, obj->height, &(obj->quad_id), &(obj->tex_id), &(obj->quad_w), &(obj->quad_h) );
            //fprintf(stderr,"textObj_setText 2\n");
        } else {
            glDeleteLists( obj->obj3D_id, 1 );
            obj->obj3D_id=0;
            strcpy(obj->str,str);
            obj->obj3D_id=getStringGLListFT( obj->str, obj->fontname, obj->height, obj->depth3D, &(obj->obj3D_w), &(obj->obj3D_h) );
        }
    }
}

/***********************************************************************/

void textObj_delete_last( textObj * obj )
{
    char str[256];
    int slen;
    strcpy(str,obj->str);
    slen = strlen(str);
    str[slen-1]=0;
    textObj_setText(obj,str);
}

/***********************************************************************/

void textObj_append_char( textObj * obj, int c )
{
    char str[256];
    int slen;
    strcpy(str,obj->str);
    slen = strlen(str);
    str[slen+1]=0;
    str[slen]=c;
    textObj_setText(obj,str);
}

/***********************************************************************/

void textObj_setFont( textObj * obj, char * fontname )
{
    if(! obj->is_3D){
        glDeleteLists( obj->quad_id, 1 );
        obj->quad_id=0;
        glDeleteTextures( 1, (GLuint *)&(obj->tex_id) );
        obj->tex_id=0;
        strcpy(obj->fontname,fontname);
        create_string_quad( obj->str, obj->fontname, obj->height, &(obj->quad_id), &(obj->tex_id), &(obj->quad_w), &(obj->quad_h) );
    } else {
        glDeleteLists( obj->obj3D_id, 1 );
        obj->obj3D_id=0;
        strcpy(obj->fontname,fontname);
        obj->obj3D_id=getStringGLListFT( obj->str, obj->fontname, obj->height, obj->depth3D, &(obj->obj3D_w), &(obj->obj3D_h) );
    }
}

/***********************************************************************/

void textObj_draw( textObj * obj )
{
    if(! obj->is_3D){
        if( obj->quad_id != 0 )
            glCallList( obj->quad_id );
    } else {
        if( obj->obj3D_id != 0 ){
            glCallList( obj->obj3D_id );
        }
    }
}

/***********************************************************************/

void textObj_draw_centered( textObj * obj )
{
    if(! obj->is_3D){
        glPushMatrix();
        glTranslatef( -(VMfloat)(obj->quad_w)/2.0, -(VMfloat)(obj->quad_h)/2.0, 0.0 );
        if( obj->quad_id != 0 )
            glCallList( obj->quad_id );
        glPopMatrix();
    } else {
        glPushMatrix();
        glTranslatef( -(VMfloat)(obj->obj3D_w)/2.0, -(VMfloat)(obj->obj3D_h)/2.0, (VMfloat)(obj->depth3D)/2.0 );
        if( obj->obj3D_id != 0 ){
            glCallList( obj->obj3D_id );
        }
        glPopMatrix();
    }
}

/***********************************************************************/

void textObj_draw_bound( textObj * obj, int hbound, int vbound )
{
    VMfloat x=0.0;
    VMfloat y=0.0;
    if(! obj->is_3D){
        if( hbound == HBOUND_CENTER )  x=-(VMfloat)(obj->quad_w)/2.0;
        if( hbound == HBOUND_RIGHT  )  x=-(VMfloat)(obj->quad_w);
        if( hbound == HBOUND_LEFT   )  x=0.0;
        if( vbound == VBOUND_CENTER )  y=-(VMfloat)(obj->quad_h)/2.0;
        if( vbound == VBOUND_TOP    )  y=-(VMfloat)(obj->quad_h);
        if( vbound == VBOUND_BOTTOM )  y=0.0;
        glPushMatrix();
        glTranslatef( x, y, 0.0 );
        if( obj->quad_id != 0 )
            glCallList( obj->quad_id );
        glPopMatrix();
    } else {
        if( hbound == HBOUND_CENTER )  x=-(VMfloat)(obj->obj3D_w)/2.0;
        if( hbound == HBOUND_RIGHT  )  x=-(VMfloat)(obj->obj3D_w);
        if( hbound == HBOUND_LEFT   )  x=0.0;
        if( vbound == VBOUND_CENTER )  y=-(VMfloat)(obj->obj3D_h)/2.0;
        if( vbound == VBOUND_TOP    )  y=-(VMfloat)(obj->obj3D_h);
        if( vbound == VBOUND_BOTTOM )  y=0.0;
        glPushMatrix();
        glTranslatef( x, y, 0.0 );
        if( obj->obj3D_id != 0 ){
            glCallList( obj->obj3D_id );
        }
        glPopMatrix();
    }
}

/***********************************************************************/

void textObj_delete( textObj * obj )
{
    if(! obj->is_3D){
        glDeleteLists( obj->quad_id, 1 );
        obj->quad_id=0;
        glDeleteTextures( 1, (GLuint *)&(obj->tex_id) );
        obj->tex_id=0;
    } else {
        glDeleteLists( obj->obj3D_id, 1 );
        obj->obj3D_id=0;
    }
}

