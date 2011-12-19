/* pngloader.c
**
**    load png-textures
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

#ifndef  _MSC_VER  //RB For only Windows-MSVC
#define COMPILE_PNG_CODE 1
#else
#define COMPILE_PNG_CODE 0
#endif

#if COMPILE_PNG_CODE
	#include <png.h>
#else
#endif
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glu.h>
#include <GL/gl.h>
#ifdef USE_WIN //HS
	#include <GL/glext.h>
 #include <fcntl.h>
 #include <SDL.h>
#endif
#include <string.h>
#include <sys/stat.h>
#include "options.h"
#include "sys_stuff.h"
#include "history.h"

/***********************************************************************
 *                 Loads a png Graphics file into mem                  *
 ***********************************************************************/

int load_png(char * file_name, int * w, int * h, int * depth, char ** data)
{
#if COMPILE_PNG_CODE
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type,
        compression_type, filter_type;
    int rowbytes, channels;
    unsigned int i;
    char * buff;
    char ** row_pointers;

    fp = fopen(file_name, "rb");
    if (!fp){
        fprintf(stderr,"Cannot load required png-file (%s). Terminating!\n",file_name);
        sys_exit(0);
    }

    png_ptr = png_create_read_struct
       (PNG_LIBPNG_VER_STRING, (png_voidp)NULL/*user_error_ptr*/,
        NULL/*user_error_fn*/, NULL/*user_warning_fn*/);
    if (!png_ptr)
        return 0;

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr,
           (png_infopp)NULL, (png_infopp)NULL);
        return 0;
    }

    png_init_io(png_ptr, fp);

    png_read_info(png_ptr, info_ptr);

    png_get_IHDR(png_ptr, info_ptr, &width, &height,
                 &bit_depth, &color_type, &interlace_type,
                 &compression_type, &filter_type);

    channels = png_get_channels(png_ptr, info_ptr);

    rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    buff         = (char *)  malloc(height*rowbytes);
    row_pointers = (char **) malloc(height*sizeof(char *));
    for(i=0;i<height;i++){
        row_pointers[i]=&buff[rowbytes*i];
    }

    png_read_image(png_ptr, (png_bytepp)row_pointers);

    //fprintf(stderr,"load_png: rgstereo=%d\n",options_rgstereo_on);

    if( options_rgstereo_on ){
        //fprintf(stderr,"load_png: graying out texture\n");
        for(i=0;i<height;i++){
            int j,k,d;
            for(j=0;j<rowbytes;j+=channels){
                d=0;
                for(k=0;k<channels;k++) d+=(unsigned char)row_pointers[i][j+k];
                d/=channels;
                for(k=0;k<channels;k++) row_pointers[i][j+k]=d;
            }
        }
    }

    free(row_pointers);
    free(info_ptr);
    free(png_ptr);
    fclose(fp);

    *data  = buff;
    *w     = width;
    *h     = height;
    *depth = channels*bit_depth;

    return 1;
#else
    *data  = 0;
    *w     = 0;
    *h     = 0;
    *depth = 0;
    return 1;
#endif
}

/***********************************************************************
 *             write Screenshot to disc as png or bmp                  *
 *             OS dependent                                            *
 ***********************************************************************/

void Snapshot(int width, int height)
{
    int i;
    char file_name[1024];
    char randomname[200];
#ifdef USE_WIN //on Windows we have to save a bmp-file (problems with png)

    SDL_Surface *temp;
    unsigned char *pixels;

    /* create file */

    get_history(file_name);
    mkdir(file_name);  // every time is not a problem

    sprintf(randomname,"/screen-%i.bmp",rand());
    strcat(file_name,randomname);

	   temp = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
	   if (temp == NULL) return;

	   pixels =(png_byte *)malloc(width * height * 3 * sizeof(png_byte));
	   if (pixels == NULL) {
	       SDL_FreeSurface(temp);
	       return;
	   }

	   glPixelStorei(GL_PACK_ALIGNMENT, 1);
	   glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *)pixels);

	   for (i=0; i<height; i++)
	        memcpy(((char *) temp->pixels) + temp->pitch * i, pixels + 3*width * (height-i-1), width*3);
	   free(pixels);

	   SDL_SaveBMP(temp, file_name);
	   SDL_FreeSurface(temp);
	   return;
#else
#if COMPILE_PNG_CODE
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_colorp palette;
    png_byte *image;
    png_bytep *row_pointers;

    /* create file */
	   get_history(file_name);
#ifdef USE_WIN
    mkdir(file_name);  // every time is not a problem
#else
    mkdir(file_name,0777);  // every time is not a problem
#endif
    sprintf(randomname,"/screen-%i.png",rand());
    strcat(file_name,randomname);

    if((fp = fopen(file_name, "wb"))==NULL) {
       fprintf(stderr,"[write_png_file] File %s could not be opened for writing\n", file_name);
       return;
    }

    if((png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))==NULL) {
       fprintf(stderr,"[write_png_file] png_create_write_struct failed\n");
       fclose(fp);
       remove(file_name);
       return;
    }

    if((info_ptr = png_create_info_struct(png_ptr))==NULL) {
        fprintf(stderr,"[write_png_file] png_create_info_struct failed\n");
        fclose(fp);
        remove(file_name);
#ifdef  png_infopp_NULL
        png_destroy_write_struct(&png_ptr, png_infopp_NULL);
#else
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
#endif
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        remove(file_name);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return;
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof (png_color));
    png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);

    png_write_info(png_ptr, info_ptr);

    png_set_packing(png_ptr);

    if((image = (png_byte *)malloc(width * height * 3 * sizeof(png_byte)))==NULL) {
        fclose(fp);
        remove(file_name);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return;
    }

    if((row_pointers = (png_bytep *)malloc(height * sizeof(png_bytep)))==NULL) {
        fclose(fp);
        remove(file_name);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        free(image);
        image = NULL;
        return;
    }

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *)image);

    for (i = 0; i < height; i++) {
        row_pointers[i] = (png_bytep)image + (height - i) * width * 3;
    }

    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, info_ptr);
    png_free(png_ptr, palette);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    free(row_pointers);
    free(image);
    fclose(fp);

    return;
#endif // COMPILE_PNG_CODE
#endif // USE_WIN
}

/***********************************************************************
 *           Create the texbinds for Graphics for the room             *
 ***********************************************************************/

void create_png_texbind(char *file_name, GLuint *texbind, GLuint component, GLuint format)
{

 int depth,spheretexw,spheretexh;
 char *spheretexdata;

 glGenTextures(1,texbind);
 load_png(file_name,&spheretexw,&spheretexh,&depth,&spheretexdata);
 glBindTexture(GL_TEXTURE_2D,*texbind);
 gluBuild2DMipmaps(GL_TEXTURE_2D, component, spheretexw, spheretexh, format, GL_UNSIGNED_BYTE, spheretexdata);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options_tex_min_filter);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options_tex_mag_filter);
 if(options_anisotrop && options_value_anisotrop > 0.0) {
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, options_value_anisotrop);
 }
 glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
 free(spheretexdata);
}
