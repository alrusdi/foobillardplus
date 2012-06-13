/* font.c
**
**    create pixmaps from text using freetype
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
#ifdef USE_WIN
   #include <windows.h>
#endif
#include "options.h"
#include "sys_stuff.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
 #include <OpenGL/gl.h>
 #include <OpenGL/glu.h>
 typedef GLvoid (*_GLUfuncptr)(GLvoid);
#else
 #include <GL/gl.h>
 #include <GL/glu.h>
#endif
#include <regex.h>

#include "vmath.h"
#ifdef _MSC_VER
typedef unsigned __int8  uint8_t;
typedef unsigned __int32 uint32_t;
#else
#include <stdint.h>
#endif

#define ASCII_IN_TABLE 1

#define divisor 64000.0

/*
  The first 128 entries are tuples of 4 bit values. The lower bits
  are a mask that when xor'd with a byte removes the leading utf-8
  bits. The upper bits are a character class number. The remaining
  160 entries are a minimal deterministic finite automaton. It has
  10 states and each state has 13 character class transitions, and 
  3 unused transitions for padding reasons. When the automaton en-
  ters state zero, it has found a complete valid utf-8 code point;
  if it enters state one then the input sequence is not utf-8. The
  start state is state nine. Note the mixture of octal and decimal
  for stylistic reasons. The ASCII_IN_TABLE macro makes the array
  bigger and the code simpler--but not necessarily faster--if set.
*/

static const uint8_t utf8d[] = {

#if ASCII_IN_TABLE
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
#endif

  070,070,070,070,070,070,070,070,070,070,070,070,070,070,070,070,
  050,050,050,050,050,050,050,050,050,050,050,050,050,050,050,050,
  030,030,030,030,030,030,030,030,030,030,030,030,030,030,030,030,
  030,030,030,030,030,030,030,030,030,030,030,030,030,030,030,030,
  204,204,188,188,188,188,188,188,188,188,188,188,188,188,188,188,
  188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,
  174,158,158,158,158,158,158,158,158,158,158,158,158,142,126,126,
  111, 95, 95, 95, 79,207,207,207,207,207,207,207,207,207,207,207,

  0,1,1,1,8,7,6,4,5,4,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1,1,1,
  1,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,4,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,8,7,6,4,5,4,3,2,1,1,1,1,

};

static int init_me=1;
static FT_Library    library;   /* handle to library     */

#define VERTEXCOUNTING 9000

static GLfloat VertexData[VERTEXCOUNTING];  // Place for ?? vertices (how much really needed?)
static GLshort NormalData[VERTEXCOUNTING];  // Place for ?? vertices (how much really needed?)
static GLsizei VertexCounter;     // how much vertices to draw
static GLuint  VertexIndex;       // Index to next value of VertexData
static GLenum  glType;            // hold the type of vertices

struct TessDataVec{
    OGL_ES_DOUBLE d[3];
    int is_start;
    struct TessDataVec * next;
};

struct TessData {
    int points_per_spline;
    struct GLUtesselator * tobj;
    struct TessDataVec * v;
    FT_Vector from;
    int first_call;
};

/***********************************************************************/

struct TessData * new_tessdata(void)
{
    struct TessData * tessdata;
    tessdata=malloc(sizeof(struct TessData));
    tessdata->first_call=1;
    tessdata->v=NULL;
    tessdata->tobj=gluNewTess();
    tessdata->points_per_spline=6;
    return(tessdata);
}

/***********************************************************************/

void free_tessdata(struct TessData * tessdata)
{
    struct TessDataVec * v;
    struct TessDataVec * vn;

    v=tessdata->v;
    while(v!=NULL){
        vn=v->next; free(v); v=vn;
    }

    gluDeleteTess(tessdata->tobj);

    free( tessdata );
}

/***********************************************************************/

/*
  Count the "real" chars in an utf8string
*/

size_t utf8count( const char *string )
{

unsigned char lb;
int n;
size_t realcount = 0;

for ( n = 0; string[n]!=0; n++ ) {
  lb = string[n];
  realcount++;
  if (( lb & 0x80 ) == 0 ) {         // lead bit is zero, must be a single ascii
    //fprintf(stderr,"1 octet\n" );
  } else if (( lb & 0xE0 ) == 0xC0 ) { // 110x xxxx
    //fprintf(stderr,"2 octets\n" );
    if(string[n+1] == 0) {
      //fprintf(stderr,"Error in 1 Octet. End of String reached\n");
    } else {
      n++;
    }
  } else if (( lb & 0xF0 ) == 0xE0 ) { // 1110 xxxx
    //fprintf(stderr,"3 octets\n" );
    if(string[n+1] == 0 || string[n+2] == 0) {
      //fprintf(stderr,"Error in 2 Octet. End of String reached\n");
    } else {
      n+=2;
    }
  } else if (( lb & 0xF8 ) == 0xF0 ) { // 1111 0xxx
    //fprintf(stderr,"4 octets\n" );
    if(string[n+1] == 0 || string[n+2] == 0 || string[n+3] == 0) {
      //fprintf(stderr,"Error in 3 Octet. End of String reached\n");
    } else {
      n+=3;
    }
  } else {
    //fprintf(stderr,"Unrecognized lead byte (%02x)\n", lb );
  }
}

//fprintf(stderr,"Output length with strlen: %i\n",strlen(string));
//fprintf(stderr"The real count is: %i\n",realcount);
//fprintf(stderr,"String is: %s\n",string);
return realcount;
}

/***********************************************************************/

/*
  retrieve the information for a REAL index of possibly utf8 decoded strings
  returns the index of the char for ttf handling or 0 on error. The second Parameter has
  the new next index# for the string of parameter 1
  For better error handling inside the old program code, there is no error fired. Only success


  Char. number range  |        UTF-8 octet sequence
     (hexadecimal)    |              (binary)
  --------------------+---------------------------------------------
  0000 0000-0000 007F | 0xxxxxxx
  0000 0080-0000 07FF | 110xxxxx 10xxxxxx
  0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
  0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

*/

FT_ULong decode(uint8_t* s, FT_ULong *newindex) {
  uint8_t data, byte, stat = 9;
  uint32_t unic = 0;
  *newindex=0;

  while ((byte = *s++)) {

#if ASCII_IN_TABLE
    data = utf8d[ byte ];
    stat = utf8d[ 256 + (stat << 4) + (data >> 4) ];
    byte = (byte ^ (uint8_t)(data << 4));
#else
    if (byte < 0x80) {
      stat = utf8d[ 128 + (stat << 4) ];
    } else {
      data = utf8d[ byte ];
      stat = utf8d[ 128 + (stat << 4) + (data >> 4) ];
      byte = (byte ^ (uint8_t)(data << 4));
    }
#endif

    unic = (unic << 6) | byte;

    if (!stat) {
      // unic is now a proper code point, we just print it out.
      //printf("U+%04X\n", unic);
      //printf("%i ",unic);
      return (FT_ULong)unic;
      unic = 0;
    }
    if (stat == 1) {
      // the byte is not allowed here; the state would have to
      // be reset to continue meaningful reading of the string
      return (FT_ULong)0;
    }
  *newindex+=1;
  }
return (FT_ULong)0;
}

/***********************************************************************/

//FT_ULong getrealindex(FT_ULong index, char *string, FT_ULong *newindex) {
// ###TODO###
//}

/***********************************************************************/

void my_draw_bitmap( char * src, int w1, int h1, int x0, int y0, char * dst , int w )
{
    int x,y;
    for(y=0;y<h1;y++) {
      for(x=0;x<w1;x++) {
          dst[(y+y0)*w+x+x0]+=src[y*w1+x];
      }
    }
}

/***********************************************************************/

void getStringPixmapFT(char *str, char *fontname, int font_height, char ** data, int * dwidth, int * dheight, int * width, int * height)
/* data containes the pixmap */
{
    FT_Face       face;      /* handle to face object */
    int           pen_x,pen_y,i,w,h,j,error,w1,h1;
    FT_ULong      realindex, newindex, n;

    w1=0; h1=0;
    //.. initialise library ..
    if(init_me){
        error = FT_Init_FreeType( &library );
        if ( error ) {
#ifdef USE_WIN
            MessageBox(0,"FT_Init_FreeType error","Foobillard++ Error",MB_OK);
#else
            fprintf(stderr,"FT_Init_FreeType error\n");
#endif
            sys_exit(1);
        }
        init_me=0;
    }
//.. create face object ..
    error = FT_New_Face( library, fontname, 0, &face );
    if ( error == FT_Err_Unknown_File_Format ){
#ifdef USE_WIN
        MessageBox(0,"The font file could be opened and read, but it appears that its font format is unsupported","Foobillard++ Error",MB_OK);
#else
        fprintf(stderr,"The font file could be opened and read, but it appears that its font format is unsupported\n");
#endif
        sys_exit(1);
    } else if ( error ) {
#ifdef USE_WIN
        MessageBox(0,"Another error code means that the font file could not e opened or read, or simply that it is broken","Foobillard++ Error",MB_OK);
#else
        fprintf(stderr,"Another error code means that the font file could not e opened or read, or simply that it is broken\n");
#endif
        sys_exit(1);
    } /* else {
        fprintf(stderr,"FT_New_Face OK!\n");
    } */
    //.. set character size ..

    error = FT_Set_Pixel_Sizes(face,   /* handle to face object   */
                               0,      /* pixel_width             */
                               font_height );   /* pixel_height   */
    pen_x = 0;
    pen_y = 0;
    
//    fprintf(stderr,"getStringPixmapFT: num_glyphs=%d\n",face->num_glyphs);
    w=0;
    h=font_height;
    for(i=0;i<2;i++){
        if (i==1){
            for(w1=w,w=8;w<w1;w*=2);
            for(h1=h,h=8;h<h1;h*=2);
//            fprintf(stderr,"getStringPixmapFT: allocing  w=%d h=%d\n",w,h);
            (*data)=malloc(w*h);
            for(j=0;j<w*h;j++) {
               (*data)[j]=0;
            }
        }
        for ( n = 0; str[n]!=0; n++ ) {
            FT_UInt  glyph_index;

            if(!(realindex = decode((uint8_t *)&str[n],&newindex))) {
              realindex = str[n]; //ugly, but in function
            }
            n += newindex;
            // retrieve glyph index from character code
            glyph_index = FT_Get_Char_Index( face, realindex );

            // load glyph image into the slot (erase previous one)
            error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
            if (error) {
#ifdef USE_WIN
                MessageBox(0,"FT_Load_Glyph error","Foobillard++ Error",MB_OK);
#else
                fprintf(stderr,"FT_Load_Glyph:error#%X\n",error);
#endif
                sys_exit(1);
            }

            // convert to an anti-aliased bitmap
            error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
            if (error) {
#ifdef USE_WIN
                MessageBox(0,"FT_Render_Glyph error","Foobillard++ Error",MB_OK);
#else
                fprintf(stderr,"FT_Render_Glyph:error#%X\n",error);
#endif
                sys_exit(1);
            }
             
            if(i!=0){
                // now, draw to our target surface
                my_draw_bitmap( (char *)face->glyph->bitmap.buffer,
                                face->glyph->bitmap.width, face->glyph->bitmap.rows,
                                pen_x + face->glyph->bitmap_left,
                                pen_y + font_height*face->ascender/(face->ascender-face->descender) - face->glyph->bitmap_top,
                                *data , w);
                pen_x += (face->glyph->advance.x >> 6);
            } else {
//                fprintf(stderr,"getStringPixmapFT: w=%d h=%d\n",w,h);
                w += (face->glyph->advance.x >> 6);
//                fprintf(stderr,"getStringPixmapFT: w=%d h=%d\n",w,h);
            }
        }
    }

    error = FT_Done_Face(face);
    if ( error ) {
#ifdef USE_WIN
        MessageBox(0,"FT_Done_Face error","Foobillard++ Error",MB_OK);
#else
        fprintf(stderr,"FT_Done_Face error# %d\n",error);
#endif
        sys_exit(1);
    }
    //fprintf(stderr,"FT_Done_FreeType ready\n");

    *dwidth=w;
    *dheight=h;
    if( width  != NULL ) *width  = w1;
    if( height != NULL ) *height = h1;
}

/***********************************************************************
 *           Write one vertex to the float vertex-array                *
 ***********************************************************************/

#ifdef USE_WIN	//RB
  void APIENTRY my_Vertex_cb(void * data)
#else
  void my_Vertex_cb(void * data)
#endif
{
    OGL_ES_DOUBLE * d;
    d=(OGL_ES_DOUBLE *)data;
    // works with normal OpenGL with type overcast from double to float here
    NormalData[VertexCounter] = 0;
    VertexData[VertexIndex++] = d[0];
    NormalData[VertexCounter] = 0;
    VertexData[VertexIndex++] = d[1];
    NormalData[VertexCounter] = -1;
    VertexData[VertexIndex++] = d[2];
    VertexCounter++;
}

/***********************************************************************
 *   Error function for the glu tessalation functions (only fprintf)   *
 ***********************************************************************/

#ifdef USE_WIN
  void APIENTRY my_error(GLenum errorCode)
#else
  void my_error(GLenum errorCode)
#endif
{
   const GLubyte *estring;

   estring = gluErrorString(errorCode);
   fprintf(stderr, "Tessellation Error: %s\n", estring);
}

/***********************************************************************
 *        Entry point for tessalation glbegin for vertex arrays        *
 ***********************************************************************/

#ifdef USE_WIN
  void APIENTRY my_begin(GLenum type)
#else
  void my_begin(GLenum type)
#endif
{
  VertexCounter = 0; // set the counter of drawn vertices to 0 = start
  VertexIndex = 0;   // set the Index also to 0 = start
  glType = type;     // the vertex type
}

/***********************************************************************
 *                     write the vertices to screen                    *
 ***********************************************************************/

#ifdef USE_WIN
  void APIENTRY my_end(void)
#else
  void my_end(void)
#endif
{

  if(VertexCounter && VertexIndex) {
     // do something only, if vertices are set
     glEnableClientState(GL_NORMAL_ARRAY);
     glEnableClientState(GL_VERTEX_ARRAY);
     glVertexPointer(3,GL_FLOAT,0,VertexData);
     glNormalPointer(GL_SHORT,0,NormalData);
     glPushMatrix();
     glDrawArrays(glType,0,VertexCounter);
     glPopMatrix();
  } else {
     fprintf(stderr,"Unknown error in call to wrote tessalation object to screen\n");
  }
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
}

/***********************************************************************/

void tess_add_point( VMvect v, struct TessData * data, int is_start )
{
    struct TessDataVec * tdv;
    struct TessDataVec * tdv_p = NULL;

    tdv=data->v;
    while(tdv!=NULL){
        tdv_p=tdv;
        tdv=tdv->next;
    }

    tdv=malloc(sizeof(struct TessDataVec));
    tdv->d[0]=v.x/divisor;
    tdv->d[1]=v.y/divisor;
    tdv->d[2]=v.z/divisor;
    tdv->is_start=is_start;
    tdv->next=NULL;

    if(data->v == NULL){
        data->v=tdv;
    }

    if(tdv_p != NULL){
        tdv_p->next=tdv;
    }

    gluTessVertex(data->tobj, tdv->d, tdv->d);
}

/***********************************************************************/

int cb_tess_move_to( FT_Vector * to, void * user )
{
    //    int * first_call = (int *) user;
    VMvect v;
    struct TessData * data = (struct TessData *) user;

    if( ! data->first_call ){
        gluTessEndContour(data->tobj);
    } else {
        data->first_call=0;
    }
    gluTessBeginContour(data->tobj);

    v=vec_xyz( to->x, to->y, 0.0 );
    tess_add_point(v, data, 1);

    data->from=*to;
    return 0;
}

/***********************************************************************/

int cb_tess_line_to( FT_Vector * to, void * user )
{
    VMvect v;
    struct TessData * data = (struct TessData *) user;

    v=vec_xyz( to->x, to->y, 0.0 );
    tess_add_point(v, data, 0);

    data->from=*to;
    return 0;
}

/***********************************************************************/

VMvect conic_spline_point( VMvect vi, VMvect vf, VMvect vc, VMfloat t )
{
    VMvect v1, v2, v;
    v1=vec_add(vi,vec_scale(vec_diff(vc,vi),t));
    v2=vec_add(vc,vec_scale(vec_diff(vf,vc),t));
    v =vec_add(v1,vec_scale(vec_diff(v2,v1),t));
    return v;
}

/***********************************************************************/

VMvect cubic_spline_point( VMvect vi, VMvect vc1, VMvect vc2, VMfloat t )
{
    VMvect v1, v2, v3, v4, v5, v;
    v1=vec_add(vi, vec_scale(vec_diff(vc1,vi ),t));
    v2=vec_add(vc1,vec_scale(vec_diff(vc2,vc1),t));
    v3=vec_add(vc2,vec_scale(vec_diff(v1 ,vc2),t));
    v4=vec_add(v1, vec_scale(vec_diff(v2 ,v1 ),t));
    v5=vec_add(v2, vec_scale(vec_diff(v3 ,v2 ),t));
    v =vec_add(v4, vec_scale(vec_diff(v5 ,v4 ),t));
    return v;
}

/***********************************************************************/

int cb_tess_conic_to( FT_Vector * ctrl, FT_Vector * to, void * user )
{
    VMvect vi,vf,vc, v;
    VMfloat t, dt;
    struct TessData * data = (struct TessData *) user;

    dt=1.0/data->points_per_spline;

    vi=vec_xyz( data->from.x, data->from.y, 0.0 );
    vf=vec_xyz( to->x, to->y, 0.0 );
    vc=vec_xyz( ctrl->x, ctrl->y, 0.0 );

    for(t=0.0;t<1.0+dt/2.0;t+=dt){
        v=conic_spline_point(vi, vf, vc, t);
        tess_add_point(v, data, 0);
    }

    data->from=*to;
    return 0;
}

/***********************************************************************/

int cb_tess_cubic_to( FT_Vector * ctrl1, FT_Vector * ctrl2, FT_Vector * to, void * user )
{
    VMvect vi,vc1,vc2, v;
    VMfloat t, dt;
    struct TessData * data = (struct TessData *) user;

    dt=1.0/data->points_per_spline;

    vi=vec_xyz( data->from.x, data->from.y, 0.0 );
    vc1=vec_xyz( ctrl1->x, ctrl1->y, 0.0 );
    vc2=vec_xyz( ctrl2->x, ctrl2->y, 0.0 );

    for(t=0.0;t<1.0+dt/2.0;t+=dt){
        v=cubic_spline_point(vi, vc1, vc2, t);
        tess_add_point(v, data, 0);
    }

    data->from=*to;
    return 0;
}

/***********************************************************************/

void makeGLGeometryFT(FT_GlyphSlot glyph, VMfloat depth)
{
    FT_Outline *outline;
    FT_Outline_Funcs funcs;
    struct TessData * tessdata = new_tessdata();
    struct TessDataVec * tdv   = NULL;
    struct TessDataVec * tdv_p = NULL;
    struct TessDataVec * tdv_n = NULL;
    struct TessDataVec * tdv_s = NULL;
    VMvect n2,d2;
    GLfloat NormalData[VERTEXCOUNTING];  // Place for VERTEXCOUNTING normales (how much really needed?)
    int i,j,k;

    outline=&(glyph->outline);

    gluTessNormal(tessdata->tobj, 0.0, 0.0, -1.0 );
    gluTessCallback(tessdata->tobj, GLU_TESS_BEGIN, (_GLUfuncptr)my_begin);
    gluTessCallback(tessdata->tobj, GLU_TESS_VERTEX, (_GLUfuncptr)my_Vertex_cb);
    gluTessCallback(tessdata->tobj, GLU_TESS_END, (_GLUfuncptr)my_end);
    gluTessCallback(tessdata->tobj, GLU_TESS_ERROR, (_GLUfuncptr)my_error);

    funcs.move_to  = (FT_Outline_MoveToFunc)cb_tess_move_to;
    funcs.line_to  = (FT_Outline_LineToFunc)cb_tess_line_to;
    funcs.conic_to = (FT_Outline_ConicToFunc)cb_tess_conic_to;
    funcs.cubic_to = (FT_Outline_CubicToFunc)cb_tess_cubic_to;
    funcs.shift    = 0;
    funcs.delta    = 0;
    gluTessBeginPolygon(tessdata->tobj, NULL);
    FT_Outline_Decompose( outline, &funcs, tessdata );
    gluTessEndPolygon(tessdata->tobj);

    gluTessNormal(tessdata->tobj, 0.0, 0.0, 1.0 );
    glPushMatrix();
    glTranslatef(0,0,-depth);
    gluTessBeginPolygon(tessdata->tobj, NULL);
    FT_Outline_Decompose( outline, &funcs, tessdata );
    gluTessEndPolygon(tessdata->tobj);
    glPopMatrix();

    if(depth!=0.0){
      tdv=tessdata->v;
        if(tdv!=NULL){
           tdv_s=tdv;
           tdv_n=tdv->next;
             if(tdv_n==NULL || tdv_n->is_start) {
               tdv_n=tdv_s;
             }
        }
      while( tdv!=NULL ){
        i = 0;
        j = 0;
        k = 0;
        tdv_s=tdv;
        tdv_p=tdv;
        while( tdv_p->next!=NULL && tdv_p->next->is_start==0 ){ tdv_p=tdv_p->next; }
           do {
             d2.x = tdv_n->d[0]-tdv->d[0];
             d2.y = tdv_n->d[1]-tdv->d[1];
             n2=vec_unit( vec_xyz(-d2.y,  d2.x, 0.0) );
             NormalData[i++] = n2.x; NormalData[i++] = n2.y; NormalData[i++] = n2.z;
             VertexData[j++] = tdv->d[0]; VertexData[j++] = tdv->d[1]; VertexData[j++] = 0.0;
             NormalData[i++] = n2.x; NormalData[i++] = n2.y; NormalData[i++] = n2.z;
             VertexData[j++] = tdv->d[0]; VertexData[j++] = tdv->d[1]; VertexData[j++] = -depth;
             NormalData[i++] = n2.x; NormalData[i++] = n2.y; NormalData[i++] = n2.z;
             VertexData[j++] = tdv_n->d[0]; VertexData[j++] = tdv_n->d[1]; VertexData[j++] = -depth;
             NormalData[i++] = n2.x; NormalData[i++] = n2.y; NormalData[i++] = n2.z;
             VertexData[j++] = tdv_n->d[0]; VertexData[j++] = tdv_n->d[1]; VertexData[j++] = 0.0;
             k+=4;
             tdv_p=tdv;
             tdv=tdv->next;
             if(tdv!=NULL){ 
               tdv_n=tdv->next;
                 if(tdv_n==NULL || tdv_n->is_start) {
                    tdv_n=tdv_s;
                 }
             }
           } while( tdv!=NULL && tdv->is_start==0 );
          glEnableClientState(GL_NORMAL_ARRAY);
          glEnableClientState(GL_VERTEX_ARRAY);
          glNormalPointer(GL_FLOAT, 0, NormalData);
          glVertexPointer(3, GL_FLOAT, 0, VertexData);
          glPushMatrix();
          glDrawArrays(GL_QUADS,0,k-4);
          glPopMatrix();
          glDisableClientState(GL_NORMAL_ARRAY);
          glDisableClientState(GL_VERTEX_ARRAY);
          //fprintf(stderr,"Indices = %i\n",k);
        //glEnd();
      }
    }
    glTranslatef(glyph->advance.x/divisor,0,0);
    free_tessdata(tessdata);
}

/***********************************************************************/

GLuint getStringGLListFT (char *str, char *fontname, VMfloat font_height, float depth, VMfloat * width, VMfloat * height)
/* data containes the pixmap */
{
    GLuint rval;
    FT_Face face;      /* handle to face object */
    int error;
    FT_ULong n,realindex,newindex;
    FT_UInt glyph_index;

    //.. initialise library ..
    if(init_me){
        error = FT_Init_FreeType( &library );
        if ( error ) {
#ifdef USE_WIN
            MessageBox(0,"FT_Init_Freetype error","Foobillard++ Error",MB_OK);
#else
            fprintf(stderr,"FT_Init_FreeType error\n");
#endif
            sys_exit(1);
        }
        init_me=0;
    }
    error = FT_New_Face( library, fontname, 0, &face );

    if ( error == FT_Err_Unknown_File_Format ){
#ifdef USE_WIN
        MessageBox(0,"The font file could be opened and read, but it appears that its font format is unsupported","Foobillard++ Error",MB_OK);
#else
        fprintf(stderr,"The font file could be opened and read, but it appears that its font format is unsupported\n");
#endif
        sys_exit(1);
    } else if ( error ) {
#ifdef USE_WIN
        MessageBox(0,"Another error code means that the font file could not be opened or read, or simply that it is broken","Foobillard++ Error",MB_OK);
#else
        fprintf(stderr,"Another error code means that the font file could not be opened or read, or simply that it is broken\n");
#endif
        sys_exit(1);
    }
    //.. set character size ..
    error = FT_Set_Char_Size(
                             face,    /* handle to face object           */
                             0,       /* char_width in 1/64th of points  */
                             font_height*divisor,   /* char_height in 1/64th of points */
                             72*72/64,      /* horizontal device resolution    */
                             72*72/64 );    /* vertical device resolution      */
    rval = glGenLists( 1 );

    glNewList( rval,  GL_COMPILE );
    glPushMatrix();

    if (width!=NULL)  *width=0;
    if (height!=NULL) *height=font_height;

    for ( n = 0; str[n]!=0; n++ ){
        if(!(realindex = decode((uint8_t *)&str[n],&newindex))) {
           realindex = str[n]; //ugly, but in function
           }
        n += newindex;

        // retrieve glyph index from character code
        glyph_index = FT_Get_Char_Index( face, realindex );

        // load glyph image into the slot (erase previous one)
        error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
        if (error) { fprintf(stderr,"FT_Load_Glyph:error#%X\n",error); exit(1); }
        makeGLGeometryFT(face->glyph, depth);
        if (width!=NULL) (*width) += (VMfloat)(face->glyph->advance.x)/divisor;
    }

    error = FT_Done_Face(face);
    if ( error ) {
#ifdef USE_WIN
        MessageBox(0,"FT_Done_Face error","Foobillard++ Error",MB_OK);
#else
        fprintf(stderr,"FT_Done_Face error# %d\n",error);
#endif
        sys_exit(1);
    }
    //fprintf(stderr,"FT_Done_FreeType ready\n");
    glPopMatrix();
    glEndList();

    return rval;
}
