/* bumpref.c
**
**    bummp-reflection-mapping using
**    NVIDIA vertex-shaders register-combiners and texture-programs
**    Copyright (C) 2002  Florian Berger
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

#ifdef __APPLE__
 #include <OpenGL/gl.h>
 #include <OpenGL/glu.h>
 #include <OpenGL/glext.h>
#else
 #include <GL/gl.h>
 #include <GL/glu.h>
 #include <GL/glext.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bumpref.h"
#include "png_loader.h"
#include "options.h"
#include "vmath.h"

#ifndef NO_NV_BUMPREF  /* force not to use bumpref */

#ifdef GL_NV_register_combiners
#ifdef GL_NV_texture_shader
#ifdef GL_NV_vertex_program

#define USE_NV_BUMPREF

#endif /*GL_NV_vertex_program*/
#endif /*GL_NV_texture_shader*/
#endif /*GL_NV_register_combiners*/

#endif /* NO_NV_BUMPREF */

// ### TODO ### Nvidia HILO texture remarked inside this file (old stuff - remove HILO).......

/***********************************************************************/
/*               Generate the cube maps for the edges                  */
/*   This function is ARB compatible and used for Edges Bumpmapping    */
/***********************************************************************/

int bumpref_create_cubemap( char * posx_name,
                           char * posy_name,
                           char * posz_name,
                           char * negx_name,
                           char * negy_name,
                           char * negz_name )
{
#ifdef USE_NV_BUMPREF
    int i, target;
    unsigned int cube_tex_bind;
    unsigned char * data;

            fprintf(stderr,"setting environment cube map\n");

            glActiveTextureARB(GL_TEXTURE2_ARB);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            glGenTextures( 1 , &cube_tex_bind );
            glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cube_tex_bind);
            glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, options_tex_min_filter);
            glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, options_tex_mag_filter);
            glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            for(i=0;i<6;i++){
                int w,h,depth;
                fprintf(stderr,"setting cube map #%d\n",i);
                switch(i){
                case 0: load_png(posx_name, &w, &h, &depth, (char **)&data); break;
                case 1: load_png(posy_name, &w, &h, &depth, (char **)&data); break;
                case 2: load_png(posz_name, &w, &h, &depth, (char **)&data); break;
                case 3: load_png(negx_name, &w, &h, &depth, (char **)&data); break;
                case 4: load_png(negy_name, &w, &h, &depth, (char **)&data); break;
                case 5: load_png(negz_name, &w, &h, &depth, (char **)&data); break;
                }
                switch(i){
                case 0: target=GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB; break;
                case 1: target=GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB; break;
                case 2: target=GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB; break;
                case 3: target=GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB; break;
                case 4: target=GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB; break;
                case 5: target=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB; break;
                }
                fprintf(stderr,"rendering mipmaps for #%d\n",i);
                gluBuild2DMipmaps(target, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, data);
                fprintf(stderr,"freeing data #%d\n",i);
                free(data);
            }

            return(cube_tex_bind);
#else
            return(0);
#endif
}

/***********************************************************************/
// This function is ARB compatible but only used for Nvidia HILO
// have a look what happens, if it used as normal function - later
// ### TODO ### remove the function ?

/* void gaussian_blur( int w, int h, VMfloat * ddata, int d )
{
    int xi, yi, xi2, yi2, dx, dy, i;
    VMfloat * wdata;
    VMfloat gauss_sum, gauss;

    wdata = malloc(sizeof(VMfloat)*w*h);

    for(i=0;i<w*h;i++){
        wdata[i]=ddata[i];
    }

    for(yi=0;yi<h;yi++){
        for(xi=0;xi<w;xi++){
            ddata[yi*w+xi]=0.0;
            gauss_sum=0.0;
            for(dy=-d;dy<=d;dy++){
                for(dx=-d;dx<=d;dx++){
                    gauss=exp(-2.0*(VMfloat)(dx*dx+dy*dy)/(VMfloat)(d*d));
                    xi2=xi+dx; if(xi2<0)xi2+=w; if(xi2>=w)xi2-=w;
                    yi2=yi+dy; if(yi2<0)yi2+=h; if(yi2>=h)yi2-=h;
                    ddata[yi*w+xi]+=gauss*wdata[yi2*w+xi2];
                    gauss_sum+=gauss;
                }
            }
            ddata[yi*w+xi]/=gauss_sum;
        }
    }

    free(wdata);
} */

/***********************************************************************/
/*                         normalize bump data                         */
/*                   This function is ARB compatible                   */
/***********************************************************************/
void bump2normal(int w, int h, char * data, VMfloat strength)
/* data has to be rgb */
{
    int xi, yi, i, xi_n, yi_n;
    VMfloat sd, sd_xn, sd_yn, dzx, dzy, dx, dy;
    unsigned char * udata;
    VMfloat * ddata;
    VMvect n;

    udata = (unsigned char *)data;
    ddata = malloc(sizeof(VMfloat)*w*h);

    for(i=0;i<w*h;i++){
        ddata[i]=udata[i*3];
    }

    for(yi=0;yi<h;yi++){
        for(xi=0;xi<w;xi++){

            i=(yi*w+xi)*3;

            xi_n = (xi+1<w)?xi+1:0;
            yi_n = (yi+1<h)?yi+1:0;

            sd     = ddata[yi*w+xi];
            sd_xn  = ddata[yi*w+xi_n];
            sd_yn  = ddata[yi_n*w+xi];

            dzx = (sd_xn-sd)*strength;
            dzy = (sd_yn-sd)*strength;

            dx=1.0;
            dy=1.0;

            n=vec_scale(vec_unit(vec_cross(vec_xyz(dx,0,dzx),vec_xyz(0,-dy,dzy))),-1.0);

            udata[i+2]=0x80+127.4*1.0*n.z;
            udata[i+1]=0x80+127.4*1.0*n.y;
            udata[i+0]=0x80+127.4*1.0*n.x;
        }
    }
    free(ddata);
}

/***********************************************************************/
/*                normalize bump data for Nvidia HILO                  */
/*    This function is ARB compatible but only used for Nvidia HILO    */
/***********************************************************************/

/* data has to be rgb */
/* signed short * bump2normal_HILO(int w, int h, char * data, VMfloat strength)
{
    int xi, yi, i, xi_n, yi_n;
    VMfloat sd, sd_xn, sd_yn, sd_xyn, dzx, dzy, dx, dy;
    unsigned char * udata;
    signed short * sdata;
    VMfloat * ddata;
    VMvect n;

    udata = (unsigned char *)data;
    ddata = malloc(sizeof(VMfloat)*w*h);
    sdata = malloc(sizeof(signed short)*w*h*2);

    for(i=0;i<w*h;i++){
        ddata[i]=udata[i*3];
    }
    gaussian_blur( w,h, ddata, 3 );

    for(yi=0;yi<h;yi++){
        for(xi=0;xi<w;xi++){

            i=(yi*w+xi)*3;

            xi_n = (xi+1<w)?xi+1:0;
            yi_n = (yi+1<h)?yi+1:0;

            sd     = ddata[yi*w+xi];
            sd_xn  = ddata[yi*w+xi_n];
            sd_yn  = ddata[yi_n*w+xi];
            sd_xyn = ddata[yi_n*w+xi_n];

            dzx = (sd_xn-sd)*strength;
            dzy = (sd_yn-sd)*strength;

            dx=1.0;
            dy=1.0;

            n=vec_scale(vec_unit(vec_cross(vec_xyz(dx,0,dzx),vec_xyz(0,-dy,dzy))),-1.0);

            sdata[(yi*w+xi)*2+1]=32767.4*n.y;
            sdata[(yi*w+xi)*2+0]=32767.4*n.x;
        }
    }
    free(ddata);

    return(sdata);
} */

/***********************************************************************/
/*                      returns texture-bind-id                        */
/*                   This function is ARB compatible                   */
/*              Nvidia HILO components are gone - farewell.            */
/***********************************************************************/

int bumpref_create_bumpmap( char * map_name, VMfloat strength)
{
#ifdef USE_NV_BUMPREF
    unsigned int texbind;
    int w, h, depth;
    char * data;
    load_png(map_name, &w, &h, &depth, (char **)&data);
    bump2normal(w, h, data, strength);
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1 , &texbind );
    glBindTexture(GL_TEXTURE_2D,texbind);
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options_tex_min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options_tex_mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, w, h, GL_RGB, GL_UNSIGNED_BYTE, data);
    glDisable(GL_TEXTURE_2D);
    return(texbind);
#else
    return(0);
#endif
}

/***********************************************************************/
/*                 Setup the bumpref for the edges                     */
/***********************************************************************/


BumpRefType bumpref_setup_vp_ts_rc(char * map_name, VMfloat strength, char * posx_name, char * posy_name, char * posz_name, char * negx_name, char * negy_name, char * negz_name, VMfloat zoffs)
/* dont call this 1st time inside a GL display list */
/* doesnt have any effect when called 1st */
{
    BumpRefType bumpref;
#ifdef USE_NV_BUMPREF

        bumpref.bumpref_list = glGenLists(1);
        glNewList(bumpref.bumpref_list, GL_COMPILE);

        /* texture shader */
        glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
        /* glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_DOT3_RGB_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB);
        glClientActiveTextureARB(GL_TEXTURE2_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_DOT3_RGB_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB); */
        glClientActiveTextureARB(GL_TEXTURE3_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_DOT3_RGB_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB);
        glEndList();

        bumpref.normalmap_bind = bumpref_create_bumpmap( map_name, strength);
        bumpref.cubemap_bind   = bumpref_create_cubemap( posx_name, posy_name, posz_name, negx_name, negy_name, negz_name );
        bumpref.init=1;

    return bumpref ;
#else  /* ifdef USE_NV_BUMPREF */
    memset(&bumpref,'\0',sizeof(bumpref));
    return bumpref;  
#endif
}

/***********************************************************************/
/*                    Make the bumpmapping visible                     */
/***********************************************************************/
void bumpref_use(BumpRefType * bumpref)
{
#ifdef USE_NV_BUMPREF
    if(bumpref->init){
    glEnable(GL_TEXTURE_2D);
        glCallList(bumpref->bumpref_list);

        /* normalmap to tex0 */
        glActiveTextureARB(GL_TEXTURE0_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, bumpref->normalmap_bind);

        /* cubemap to tex3 */
        glActiveTextureARB(GL_TEXTURE3_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_DOT3_RGB_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, bumpref->cubemap_bind);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_2D);
    }


#endif /* ifdef USE_NV_BUMPREF */
}

/***********************************************************************/
/*            Restore the state before bumpmapping (edges)             */
/*                   This function is Nvidia stuff                     */
/***********************************************************************/

void bumpref_restore(void)
{
#ifdef USE_NV_BUMPREF
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    glEnable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE3_ARB);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
		  //Return to standard modulate texenv
		  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#endif
}

/***********************************************************************/
/*                 Setup the bumpref for the wood                      */
/*                   This function is Nvidia stuff                     */
/***********************************************************************/

BumpRefType bump_setup_vp_rc( char * map_name, VMfloat strength, int texgen )
{
    BumpRefType bumpref;
#ifdef USE_NV_BUMPREF

    static char * vert_prog_str=
            "!!VP1.0 \n"
//            #
//            # c[0-3]  = modelview projection (composite) matrix
//            #           TrackMatrixNV(GL_VERTEX_PROGRAM_NV, 0, GL_MODELVIEW_PROJECTION_NV, GL_IDENTITY_NV);
//            # c[4-7]  = modelview transpose
//            #           TrackMatrixNV(GL_VERTEX_PROGRAM_NV, 0, GL_MODELVIEW, GL_INVERSE_TRANSPOSE_NV);
//            # c[8].z  = offset for correct z-buffering
//            # c[8].x  = 0.0 to avoid "ADD R0.z, R0.z, -R0.z"
//            # c[9]  = light pos
//            # c[10].x  = 0.5   /* for unsigend mapping */
//            #
//            # generates light and half vectors into col0 and col1
//            #
            /* R2 = mormal vector n */
            "MOV   R2, v[NRML];                      \n"
            /* R0 = xy tangent vector n_xy */
            "MOV   R0.x, -R2.y;                      \n"
            "MOV   R0.y,  R2.x;                      \n"
            "MOV   R0.z,  c[8].x;                    \n"   /* R0.z=0 */
            /* normalize */
            "MOV   R1, R0;                           \n"
            "DP3   R0.w, R1, R1;                     \n"
            "RSQ   R0.w, R0.w;                       \n"
            "MUL   R0.xyz, R1, R0.w;                 \n"
            /* R1 = other tangent vector n_t = n x n_xy */
            "MUL   R1, R2.zxyw, R0.yzxw;             \n"
            "MAD   R1, R2.yzxw, R0.zxyw, -R1;        \n"
            /* R3=lightdir=dist(light,pos) */
            "ADD   R3, c[9], -v[OPOS];               \n"
            /* normalize */
            "MOV   R4, R3;                           \n"
            "DP3   R3.w, R4, R4;                     \n"
            "RSQ   R3.w, R3.w;                       \n"
            "MUL   R3.xyz, R4, R3.w;                 \n"
            /* R4=camdir=dist(cam,pos) */
            "ADD   R4, c[7], -v[OPOS];               \n"
            /* normalize */
            "MOV   R5, R4;                           \n"
            "DP3   R4.w, R5, R5;                     \n"
            "RSQ   R4.w, R4.w;                       \n"
            "MUL   R4.xyz, R5, R4.w;                 \n"
            /* R4=halfdir=add(light,cam) */
            "ADD   R4, R4, R3;               \n"
            /* normalize */
            "MOV   R5, R4;                           \n"
            "DP3   R4.w, R5, R5;                     \n"
            "RSQ   R4.w, R4.w;                       \n"
            "MUL   R4.xyz, R5, R4.w;                 \n"
            /* col0 = lightdir in basis R0 R1 R2 and in unsigned notation */
            "DP3   R5.x, R0, R3;                \n"
            "DP3   R5.y, R1, R3;                \n"
            "DP3   R5.z, R2, R3;                \n"
            "MAD   o[COL0].xyz, c[10].x, R5, c[10].x;    \n"
            /* col1 = halfdir  in basis R0 R1 R2 */
            "DP3   R5.x, R0, R4;                \n"
            "DP3   R5.y, R1, R4;                \n"
            "DP3   R5.z, R2, R4;                \n"
            "DP3   o[COL1].x, R0, R4;                \n"
            "DP3   o[COL1].y, R1, R4;                \n"
            "DP3   o[COL1].z, R2, R4;                \n"
            "MAD   o[COL1].xyz, c[10].x, R5, c[10].x;    \n"
            /* dont forget tex units */
            "MOV   o[TEX0], v[TEX0];                 \n"
            "MOV   o[TEX1], v[TEX0];                 \n" /* copy tex0 coords to tex1 coords */
            "MOV   o[TEX2], v[TEX2];                 \n"
            "MOV   o[TEX3], v[TEX3];                 \n"
            /* apply modelview + projection */
            "DP4   o[HPOS].x, c[0], v[OPOS];         \n"
            "DP4   o[HPOS].y, c[1], v[OPOS];         \n"
            "DP4   R1.z,      c[2], v[OPOS];         \n"
            "DP4   o[HPOS].w, c[3], v[OPOS];         \n"
            "ADD   o[HPOS].z, R1.z, -c[8].z;         \n"
            "END                                     \n";

//    glGenProgramsNV(1, (GLuint *)&bumpref.vert_prog_bind);
//    glBindProgramNV(GL_VERTEX_PROGRAM_NV, (GLuint)bumpref.vert_prog_bind);
//    glLoadProgramNV(GL_VERTEX_PROGRAM_NV, (GLuint)bumpref.vert_prog_bind, strlen(vert_prog_str), vert_prog_str);

    bumpref.bumpref_list = glGenLists(1);
    glNewList(bumpref.bumpref_list, GL_COMPILE);

    /*vertex program*/
//    glTrackMatrixNV(GL_VERTEX_PROGRAM_NV, 0, GL_MODELVIEW_PROJECTION_NV, GL_IDENTITY_NV);
//    glTrackMatrixNV(GL_VERTEX_PROGRAM_NV, 4, GL_MODELVIEW, GL_INVERSE_TRANSPOSE_NV /*GL_IDENTITY_NV*/);
//    glProgramParameter4fNV(GL_VERTEX_PROGRAM_NV, 8, 0.0, 1.0, 0.000 /*no z-offset*/, 1.0);
//    glProgramParameter4fNV(GL_VERTEX_PROGRAM_NV, 10, 0.5, 0.0, 0., 0.0);      /* const 0.5 */
//    glBindProgramNV(GL_VERTEX_PROGRAM_NV, bumpref.vert_prog_bind);

    /* register combiners */
    // const0 = (.5, .5, .5, 1);
    // const1 = (.5, .3, .1, 1);
    glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV,8);
    // combiner stage 0
    // spare0 = expand(col0) . expand(tex1);
    // spare1 = expand(col1) . expand(tex1);
    glCombinerOutputNV(GL_COMBINER0_NV,
                     GL_RGB,
                     GL_SPARE0_NV,
                     GL_SPARE1_NV,
                     GL_DISCARD_NV /* discard */,
                     GL_NONE /* scale */,
                     GL_NONE /* bias */,
                     GL_TRUE /* dotprod ab */,
                     GL_TRUE /* dotprod cd */,
                     GL_FALSE /* sum discarded anyway */);
    glCombinerInputNV(GL_COMBINER0_NV,
                    GL_RGB,
                    GL_VARIABLE_A_NV,
                    GL_PRIMARY_COLOR_NV/* input */,
                    GL_EXPAND_NORMAL_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER0_NV,
                    GL_RGB,
                    GL_VARIABLE_B_NV,
                    GL_PRIMARY_COLOR_NV/* input */,
                    GL_EXPAND_NORMAL_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER0_NV,
                    GL_RGB,
                    GL_VARIABLE_C_NV,
                    GL_SECONDARY_COLOR_NV/* input */,
                    GL_EXPAND_NORMAL_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER0_NV,
                    GL_RGB,
                    GL_VARIABLE_D_NV,
                    GL_SECONDARY_COLOR_NV/* input */,
                    GL_EXPAND_NORMAL_NV,
                    GL_RGB /* component usage */);

    // combiner stage 1
    glCombinerOutputNV(GL_COMBINER1_NV,
                     GL_RGB,
                     GL_DISCARD_NV,
                     GL_DISCARD_NV,
                     GL_PRIMARY_COLOR_NV /* sum */,
                     GL_NONE /* scale */,
                     GL_NONE /* bias */,
                     GL_FALSE /* dotprod ab */,

                     GL_FALSE /* dotprod cd */,
                     GL_FALSE /* sum discarded anyway */);
    glCombinerInputNV(GL_COMBINER1_NV,
                    GL_RGB,
                    GL_VARIABLE_A_NV,
                    GL_PRIMARY_COLOR_NV/* input */,
                    GL_EXPAND_NORMAL_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER1_NV,
                    GL_RGB,
                    GL_VARIABLE_B_NV,
                    GL_ZERO/* input */,
                    GL_UNSIGNED_INVERT_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER1_NV,
                    GL_RGB,
                    GL_VARIABLE_C_NV,
                    GL_PRIMARY_COLOR_NV/* input */,
                    GL_HALF_BIAS_NORMAL_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER1_NV,
                    GL_RGB,
                    GL_VARIABLE_D_NV,
                    GL_SPARE0_NV/* input */,
                    GL_UNSIGNED_INVERT_NV,
                    GL_RGB /* component usage */);

    // combiner stage 2
    glCombinerOutputNV(GL_COMBINER2_NV,
                     GL_RGB,
                     GL_DISCARD_NV,
                     GL_DISCARD_NV,
                     GL_SECONDARY_COLOR_NV /* sum */,
                     GL_NONE /* scale */,
                     GL_NONE /* bias */,
                     GL_FALSE /* dotprod ab */,
                     GL_FALSE /* dotprod cd */,
                     GL_FALSE /* sum discarded anyway */);
    glCombinerInputNV(GL_COMBINER2_NV,
                    GL_RGB,
                    GL_VARIABLE_A_NV,
                    GL_SECONDARY_COLOR_NV/* input */,
                    GL_EXPAND_NORMAL_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER2_NV,
                    GL_RGB,
                    GL_VARIABLE_B_NV,
                    GL_ZERO/* input */,
                    GL_UNSIGNED_INVERT_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER2_NV,
                    GL_RGB,
                    GL_VARIABLE_C_NV,
                    GL_SECONDARY_COLOR_NV/* input */,
                    GL_HALF_BIAS_NORMAL_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER2_NV,
                    GL_RGB,
                    GL_VARIABLE_D_NV,
                    GL_SPARE1_NV/* input */,
                    GL_UNSIGNED_INVERT_NV,
                    GL_RGB /* component usage */);

    // combiner stage 3
    // spare0 = expand(col0) . expand(tex1);
    // spare1 = expand(col1) . expand(tex1);
    glCombinerOutputNV(GL_COMBINER3_NV,
                     GL_RGB,
                     GL_SPARE0_NV,
                     GL_SPARE1_NV,
                     GL_DISCARD_NV /* discard */,
                     GL_NONE /* scale */,
                     GL_NONE /* bias */,
                     GL_TRUE /* dotprod ab */,
                     GL_TRUE /* dotprod cd */,
                     GL_FALSE /* sum discarded anyway */);
    glCombinerInputNV(GL_COMBINER3_NV,
                    GL_RGB,
                    GL_VARIABLE_A_NV,
                    GL_PRIMARY_COLOR_NV/* input */,
                    GL_SIGNED_IDENTITY_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER3_NV,
                    GL_RGB,
                    GL_VARIABLE_B_NV,
                    GL_TEXTURE1_ARB/* input */,
                    GL_EXPAND_NORMAL_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER3_NV,
                    GL_RGB,
                    GL_VARIABLE_C_NV,
                    GL_SECONDARY_COLOR_NV/* input */,
                    GL_SIGNED_IDENTITY_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER3_NV,
                    GL_RGB,
                    GL_VARIABLE_D_NV,
                    GL_TEXTURE1_ARB/* input */,
                    GL_EXPAND_NORMAL_NV,
                    GL_RGB /* component usage */);

    // combiner stage 4
    // spare1 = unsigned(spare1) * unsigned(spare1);
    glCombinerOutputNV(GL_COMBINER4_NV,
                     GL_RGB,
                     GL_SPARE1_NV,
                     GL_DISCARD_NV,/* discard 2nd prod */
                     GL_DISCARD_NV /* discard sum */,
                     GL_NONE /* scale */,
                     GL_NONE /* bias */,
                     GL_FALSE /* dotprod ab */,
                     GL_TRUE /* discarded anyway */,
                     GL_FALSE /* sum discarded anyway */);
    glCombinerInputNV(GL_COMBINER4_NV,
                    GL_RGB,
                    GL_VARIABLE_A_NV,
                    GL_SPARE1_NV/* input */,
                    GL_UNSIGNED_IDENTITY_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER4_NV,
                    GL_RGB,
                    GL_VARIABLE_B_NV,
                    GL_SPARE1_NV/* input */,
                    GL_UNSIGNED_IDENTITY_NV,
                    GL_RGB /* component usage */);
    // combiner stage 5
    // spare1 = unsigned(spare1) * unsigned(spare1);
    glCombinerOutputNV(GL_COMBINER5_NV,
                     GL_RGB,
                     GL_SPARE1_NV,
                     GL_DISCARD_NV,/* discard 2nd prod */
                     GL_DISCARD_NV /* discard sum */,
                     GL_NONE /* scale */,
                     GL_NONE /* bias */,
                     GL_FALSE /* dotprod ab */,
                     GL_TRUE /* discarded anyway */,
                     GL_FALSE /* sum discarded anyway */);
    glCombinerInputNV(GL_COMBINER5_NV,
                    GL_RGB,
                    GL_VARIABLE_A_NV,
                    GL_SPARE1_NV /* input */,
                    GL_UNSIGNED_IDENTITY_NV,
                    GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER5_NV,
                    GL_RGB,
                    GL_VARIABLE_B_NV,
                    GL_SPARE1_NV /* input */,
                    GL_UNSIGNED_IDENTITY_NV,
                    GL_RGB /* component usage */);
    // combiner stage 6
    // spare1 = unsigned(spare1) * unsigned(spare1);
    glCombinerOutputNV(GL_COMBINER6_NV,
                     GL_RGB,
                     GL_SPARE1_NV,
                     GL_DISCARD_NV,/* discard 2nd prod */
                     GL_DISCARD_NV /* discard sum */,
                     GL_NONE /* scale */,
                     GL_NONE /* bias */,
                     GL_FALSE /* dotprod ab */,
                     GL_TRUE /* discarded anyway */,
                     GL_FALSE /* sum discarded anyway */);
    glCombinerInputNV(GL_COMBINER6_NV,
                    GL_RGB,
                    GL_VARIABLE_A_NV,
                    GL_SPARE1_NV /* input */,
                    GL_UNSIGNED_IDENTITY_NV,
                      GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER6_NV,
                    GL_RGB,
                    GL_VARIABLE_B_NV,
                    GL_SPARE1_NV /* input */,
                    GL_UNSIGNED_IDENTITY_NV,
                      GL_RGB /* component usage */);
    // combiner stage 7
    // spare1 = unsigned(spare1) * unsigned(spare1);
    glCombinerOutputNV(GL_COMBINER7_NV,
                     GL_RGB,
                     GL_SPARE1_NV,
                     GL_DISCARD_NV,/* discard 2nd prod */
                     GL_DISCARD_NV /* discard sum */,
                     GL_NONE /* scale */,
                     GL_NONE /* bias */,
                     GL_FALSE /* dotprod ab */,
                     GL_TRUE /* discarded anyway */,
                     GL_FALSE /* sum discarded anyway */);
    glCombinerInputNV(GL_COMBINER7_NV,
                    GL_RGB,
                    GL_VARIABLE_A_NV,
                    GL_SPARE1_NV /* input */,
                    GL_UNSIGNED_IDENTITY_NV,
                      GL_RGB /* component usage */);
    glCombinerInputNV(GL_COMBINER7_NV,
                    GL_RGB,
                    GL_VARIABLE_B_NV,
                    GL_SPARE1_NV /* input */,
                    GL_UNSIGNED_IDENTITY_NV,
                      GL_RGB /* component usage */);
    //  final_product = const0 * spare1;
    glFinalCombinerInputNV(GL_VARIABLE_E_NV,
                           GL_SPARE1_NV,
                           GL_UNSIGNED_IDENTITY_NV,
                           GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_F_NV,
                           GL_CONSTANT_COLOR0_NV,
                           GL_UNSIGNED_IDENTITY_NV,
                           GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_D_NV,
                           GL_E_TIMES_F_NV,
//                           GL_ZERO,  /* dont use specular */
                           GL_UNSIGNED_IDENTITY_NV,
                           GL_RGB);

    // out.rgb = spare0 * const1 + final_product;
    glFinalCombinerInputNV(GL_VARIABLE_A_NV,
                           GL_SPARE0_NV,
                           GL_UNSIGNED_IDENTITY_NV,
                           GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_B_NV,
                           GL_CONSTANT_COLOR1_NV,
                           GL_UNSIGNED_IDENTITY_NV,
                           GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_C_NV,
                           GL_ZERO,
                           GL_UNSIGNED_IDENTITY_NV,
                           GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_G_NV,
                           GL_CONSTANT_COLOR0_NV,
                           GL_UNSIGNED_IDENTITY_NV,
                           GL_ALPHA);
    glEndList();

    bumpref.normalmap_bind = bumpref_create_bumpmap( map_name, strength);

    /* default values */
    bumpref.bump_light[0] = 0.0;
    bumpref.bump_light[1] = 0.0;
    bumpref.bump_light[2] = 0.0;

    bumpref.bump_diff_col[0] = 0.5;
    bumpref.bump_diff_col[1] = 0.0;
    bumpref.bump_diff_col[2] = 0.0;
    bumpref.bump_diff_col[3] = 1.0;

    bumpref.bump_spec_col[0] = 0.8;
    bumpref.bump_spec_col[1] = 0.8;
    bumpref.bump_spec_col[2] = 0.8;
    bumpref.bump_spec_col[3] = 1.0;

    bumpref.init=1;

    return(bumpref);
#else  /* ifdef USE_NV_BUMPREF */
    memset(&bumpref,'\0',sizeof(bumpref));
    return bumpref;
#endif
}

/***********************************************************************/
/*                 Setup the light for the edges/woods                 */
/*                   This function is Nvidia stuff                     */
/***********************************************************************/

void bump_set_light( BumpRefType * bumpref, VMfloat x, VMfloat y, VMfloat z )
{
#ifdef USE_NV_BUMPREF
    bumpref->bump_light[0]=x;
    bumpref->bump_light[1]=y;
    bumpref->bump_light[2]=z;
    glProgramParameter4fNV(GL_VERTEX_PROGRAM_NV, 9, bumpref->bump_light[0], bumpref->bump_light[1], bumpref->bump_light[2], 1.0);  /* some light pos */
#endif
}

/***********************************************************************/
/*                 Setup the diff for the edges/woods                  */
/*                   This function is Nvidia stuff                     */
/***********************************************************************/

void bump_set_diff( BumpRefType * bumpref, VMfloat r, VMfloat g, VMfloat b )
{
#ifdef USE_NV_BUMPREF
    bumpref->bump_diff_col[0]=r;
    bumpref->bump_diff_col[1]=g;
    bumpref->bump_diff_col[2]=b;
    bumpref->bump_diff_col[3]=1.0;
    glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, bumpref->bump_diff_col);
#endif
}

/***********************************************************************/
/*                 Setup the spec for the edges/woods                  */
/*                   This function is Nvidia stuff                     */
/***********************************************************************/

void bump_set_spec( BumpRefType * bumpref, VMfloat r, VMfloat g, VMfloat b )
{
#ifdef USE_NV_BUMPREF
    bumpref->bump_spec_col[0]=r;
    bumpref->bump_spec_col[1]=g;
    bumpref->bump_spec_col[2]=b;
    bumpref->bump_spec_col[3]=1.0;
    glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, bumpref->bump_spec_col);
#endif
}

/***********************************************************************/
/*             Make the bumpmappings visible for the wood              */
/*                   This function is Nvidia stuff                     */
/***********************************************************************/

void bump_use( BumpRefType * bumpref )
{
#ifdef USE_NV_BUMPREF

    glCallList(bumpref->bumpref_list);

    glEnable(GL_REGISTER_COMBINERS_NV);
    glDisable(GL_TEXTURE_SHADER_NV);
    glEnable(GL_VERTEX_PROGRAM_NV);

    glCombinerParameterfvNV( GL_CONSTANT_COLOR0_NV, bumpref->bump_spec_col );
    glCombinerParameterfvNV( GL_CONSTANT_COLOR1_NV, bumpref->bump_diff_col );

    //fprintf(stderr,"light=%f %f %f\n", bumpref->bump_light[0], bumpref->bump_light[1], bumpref->bump_light[2]);
    glProgramParameter4fNV(GL_VERTEX_PROGRAM_NV, 9, bumpref->bump_light[0], bumpref->bump_light[1], bumpref->bump_light[2], 1.0);  /* some light pos */

    /* normalmap to tex1 */
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, bumpref->normalmap_bind);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glPolygonOffset( 0.0, -2.0 );
    glEnable( GL_POLYGON_OFFSET_FILL );
#endif
}

/***********************************************************************/
/*             Restore the state before bumpmapping (wood)             */
/*                   This function is Nvidia stuff                     */
/***********************************************************************/

void bump_restore(void)
{
#ifdef USE_NV_BUMPREF
    glDisable(GL_REGISTER_COMBINERS_NV);
    glDisable(GL_TEXTURE_SHADER_NV);
    glDisable(GL_VERTEX_PROGRAM_NV);

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable( GL_POLYGON_OFFSET_FILL );
#endif
}
