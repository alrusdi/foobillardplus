/* ball.c
**
**    code for creating the GL-ball display-lists
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include "billard.h"
#include "billmove.h"
#include "ball.h"
#include "png_loader.h"
#include "options.h"
#include "font.h"
#include "vmath.h"
#include "png_loader.h"

#ifdef __MINGW32__
	extern void ( APIENTRY * glActiveTextureARB)( GLenum );
#endif

static char * balltexdata[22];
static GLuint balltexbind[22];
static GLuint sphere_ballbind;
static int    balltexw,balltexh;
static GLuint shadowtexbind;
static int    depth;

static MATH_ALIGN16 GLfloat col_null [4] = {0.0, 0.0, 0.0, 0.0}; /* Tron Game Mode */
static MATH_ALIGN16 GLfloat col_spec [4] = {0.0, 0.0, 0.0, 1.0}; /* dont need any specular because of reflections */
static MATH_ALIGN16 GLfloat col_refl [4] = {1.0, 1.0, 1.0, 0.28};
static MATH_ALIGN16 GLfloat col_refl3[4] = {1.0, 1.0, 1.0, 0.60};   /* for rendered cubemap */
static MATH_ALIGN16 GLfloat col_diff [4] = {0.7, 0.7, 0.7, 1.0};
static MATH_ALIGN16 GLfloat col_diff3[4] = {0.69, 0.69, 0.69, 1.0};
static MATH_ALIGN16 GLfloat col_amb  [4] = {0.2, 0.2, 0.2, 1.0};
static MATH_ALIGN16 GLfloat col_amb3 [4] = {0.31, 0.31, 0.31, 1.0}; /* for simple reflections */
static MATH_ALIGN16 GLfloat col_shad [4] = {0.5, 0.0, 0.0, 0.0};

enum BallSet { BALLSET_POOL, BALLSET_CARAMBOL, BALLSET_SNOOKER, BALLSET_NONE };
enum BallSet g_ballset=BALLSET_NONE;

/*
 Functions
*/

/***********************************************************************/

void normalize(GLfloat *v) /*FOLD00*/
{
    GLfloat d;
    d = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
    v[0] /= d;
    v[1] /= d;
    v[2] /= d;
}

/***********************************************************************/

void rescale(GLfloat *v, GLfloat r)
{
    GLfloat d;
    d = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2])/r;
    v[0] /= d;
    v[1] /= d;
    v[2] /= d;
}

/***********************************************************************/

static int in_array( ElemArray * array, GLfloat * v, GLfloat tex_x, GLfloat tex_y )
{
    int i,j,k;
    for(i=0;i<array->vnr;i++){
    	j=i*3;
    	k=i*2;
        if( fabs(v[0]-array->vert[j])<1.0E-6 &&
            fabs(v[1]-array->vert[j+1])<1.0E-6 &&
            fabs(v[2]-array->vert[j+2])<1.0E-6 &&
            fabs(tex_x-array->tex[k])<1.0E-6 &&
            fabs(tex_y-array->tex[k+1])<1.0E-6 ){
            return i;
        }
    }
    for(i=0;i<array->vnr;i++){
    	j=i*3;
        if( fabs(v[0]-array->vert[j])<1.0E-6 &&
            fabs(v[1]-array->vert[j+1])<1.0E-6 &&
            fabs(v[2]-array->vert[j+2])<1.0E-6 ){
            return -2;
        }
    }

    return -1;
}

/***********************************************************************/

void createvertex( GLfloat * v, GLfloat * n, GLfloat tex_x, GLfloat tex_y, ElemArray * array )
{
    static int count;
    int pos,i,k;
    if ( array == NULL ){
        //fprintf(stderr,"creating vertex (non-array)\n");
        glTexCoord2f( tex_x, tex_y );
        glNormal3fv(n);
        glVertex3fv(v);
    } else {
        if(array->indnr==0) {
        	count=0;
        }
        pos = in_array( array,v, tex_x, tex_y );
        if( pos>-1 ){
            array->index[array->indnr] = pos;
            array->indnr++;
        } else {
            //fprintf(stderr,"creating vertex\n");
        	i=array->vnr*3;
        	k=array->vnr*2;
            array->vert[i]=v[0];
            array->vert[i+1]=v[1];
            array->vert[i+2]=v[2];
            if( pos!=-1 ){
                count++;
                //fprintf(stderr,"count=%d\n",count);
            }
            array->norm[i]=n[0];
            array->norm[i+1]=n[1];
            array->norm[i+2]=n[2];
            array->tex[k]=tex_x;
            array->tex[k+1]=tex_y;
            array->index[array->indnr] = array->vnr;
            array->vnr++;
            array->indnr++;
            //fprintf(stderr,"createvertex: array->vnr=%d\n",array->vnr);
        }
    }
}

/***********************************************************************/

void ball_subdivide_nonrec(GLfloat *v1, GLfloat *v2, GLfloat *v3, int depth, GLfloat r, ElemArray *array)
/* if array=0 just make vertices */
{
    GLfloat v12[3], v13[3], v[3];
    GLfloat n[3];
    GLint i,j,k,x,y;
    VMfloat xf,yf,vl;
    int otherside;
    VMfloat xt,yt;
    int subdiv;

    if(depth==1) depth=0;
    if(!(depth&1)){ /* even */
        subdiv=1<<(depth/2);
    }else{          /* odd */
        subdiv=(1<<((depth-3)/2))*3;
    }


    for (i = 0; i < 3; i++) {
        v12[i] = v2[i]-v1[i];
        v13[i] = v3[i]-v1[i];
    }
    otherside = ( v1[2]+v2[2]+v3[2]>0.0 );
    for( i=0; i<subdiv; i++ ){ /* make tristrips */
        if(array==NULL){
            glBegin(GL_TRIANGLE_STRIP);
        } else {
            array->prim_size[array->num_prim]=2*(subdiv-i)+1;
            (array->num_prim)++;
        }
        for(j=0;j<2*(subdiv-i)+1;j++){
            if(!(j&1)){ /* even */
                x=j/2;     y=i;
            } else {    /* odd */
                x=(j-1)/2; y=i+1;
            }
            xf=(VMfloat)x/(VMfloat)subdiv;
            yf=(VMfloat)y/(VMfloat)subdiv;
            vl=0.0;
            for(k=0;k<3;k++){
                v[k]=v1[k]+v13[k]*xf+v12[k]*yf;
                vl+=v[k]*v[k];
            }
            vl=sqrt(vl);
            for(k=0;k<3;k++){
                n[k]=v[k]/vl;
                v[k]=n[k]*r;
            }
            xt=n[0];
            yt=n[1];
            createvertex( v, n, 0.5-0.5*(otherside?xt:-xt), 0.5+0.5*yt, array);
        }
        if(array==NULL){
            glEnd();
        }
    }
//    fprintf(stderr,"back again all (depth=%d)\n",depth);
}

/***********************************************************************/

ElemArray * create_ball_icosa_array( VMfloat r, int ddepth, int id )
{
#define X .525731112119133606
#define Z .850650808352039932

    ElemArray * array;
    int i,j,pnr,fnr,actind;
    VMfloat volume, scale_ratio;
    VMvect v1,v2,v3;

    static GLfloat vdata[12][3] = {
        {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
        {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
        {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
    };

    static GLint tindices[20][3] = {
        {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
        {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
        {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
        {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11}
    };

    static int init = 0;

    if(!init){
        init=1;
         for (i = 0; i < 12; i++) {
             rescale(vdata[i],r);
         }
    }

    if(ddepth%2==0){
        fnr = 20*(1<<(ddepth/2*2));
        pnr = 10*((1<<(ddepth/2*2))-1) + 12;
    } else if(ddepth!=1) {
        fnr = 20*(1<<((ddepth-3)/2*2));
        pnr = 10*((1<<((ddepth-3)/2*2))-1) + 12;
        pnr += fnr * 4;
        fnr *= 9;
    } else if(ddepth==1) {
        fnr = 20*(1<<(ddepth/2*2));
        pnr = 10*((1<<(ddepth/2*2))-1) + 12;
        pnr += fnr;
        fnr *= 3;
    }
    //fprintf(stderr,"pnr-ideal=%d\n",pnr);
    pnr+=(int)(sqrt(pnr)+0.5)*2;  /* could (still?) get narrow (theoret. *sqrt(PI) instead of *2) */
    //fprintf(stderr,"pnr-nachher=%d\n",pnr);

    array = (ElemArray *)malloc( sizeof(ElemArray) );
    array->vert  = (GLfloat *)malloc( pnr*3*sizeof(GLfloat) );
    array->norm  = (GLfloat *)malloc( pnr*3*sizeof(GLfloat) );
    array->tex   = (GLfloat *)malloc( pnr*2*sizeof(GLfloat) );
    array->reftex = (GLfloat *)malloc( pnr*2*sizeof(GLfloat) );
    array->index = (int *)    malloc( fnr*3*sizeof(int)     );
    array->num_prim = 0;

    array->indnr=0;
    array->vnr=0;

    //fprintf(stderr,"fnr*3=%d\n",fnr*3);
    //fprintf(stderr,"pnr=%d\n",pnr);

    for (i = 0; i < 20; i++) {
        ball_subdivide_nonrec(
                           &vdata[tindices[i][0]][0],
                           &vdata[tindices[i][1]][0],
                           &vdata[tindices[i][2]][0],
                           ddepth, r, array
                          );
    }

    //fprintf(stderr,"array->indnr=%d\n",array->indnr);
    //fprintf(stderr,"array->vnr=%d\n",array->vnr);
    //fprintf(stderr,"create_ball_icosa_array: rescaling vertices\n");
    volume=0.0;
    actind=0;
        for(i=0;i<array->num_prim;i++){
            for(j=0;j<array->prim_size[i]-2;j++){

                v1.x = array->vert[array->index[actind+j+0]*3+0];
                v1.y = array->vert[array->index[actind+j+0]*3+1];
                v1.z = array->vert[array->index[actind+j+0]*3+2];

                v2.x = array->vert[array->index[actind+j+1]*3+0];
                v2.y = array->vert[array->index[actind+j+1]*3+1];
                v2.z = array->vert[array->index[actind+j+1]*3+2];

                v3.x = array->vert[array->index[actind+j+2]*3+0];
                v3.y = array->vert[array->index[actind+j+2]*3+1];
                v3.z = array->vert[array->index[actind+j+2]*3+2];

                if(!(j&1)){
                    volume+=tri_vol_xy( v1, v2, v3 );
                } else {
                    volume-=tri_vol_xy( v1, v2, v3 );
                }
            }
            actind+=array->prim_size[i];
        }
    //fprintf(stderr,"create_ball_icosa_array: volume=%f\n",volume);

    scale_ratio=pow((4.0/3.0*r*r*r*M_PI)/fabs(volume),1.0/3.0);
    //fprintf(stderr,"create_ball_icosa_array: scale_ratioe=%f\n",scale_ratio);
    for(i=0;i<array->vnr*3;i++){
        array->vert[i] = array->vert[i]*scale_ratio;
    }
    //fprintf(stderr,"create_ball_icosa_array: compiling list\n");
    glNewList(id, GL_COMPILE);
      glPushMatrix();
        /* call array */
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_NORMAL_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glVertexPointer( 3, GL_FLOAT, 0, array->vert );
        glNormalPointer( GL_FLOAT, 0, array->norm );
        glTexCoordPointer( 2, GL_FLOAT, 0, array->tex );

        actind=0;
        for(i=0;i<array->num_prim;i++){
            glDrawElements( GL_TRIANGLE_STRIP, array->prim_size[i], GL_UNSIGNED_INT, &(array->index[actind]) );
            actind+=array->prim_size[i];
        }
    glPopMatrix();
    glEndList();
//    fprintf(stderr,"array compiled %d, NE=%d, %d\n",glGetError(),GL_NO_ERROR, array);
    return array;
}

/***********************************************************************/

void free_elem_array( ElemArray * array )
{
    free( array->vert );
    free( array->norm );
    free( array->tex );
    free( array->reftex );
    free( array->index );
}

/***********************************************************************/

void draw_ball( BallType * ball, myvec cam_pos, GLfloat cam_FOV, int win_width)
/* expects to be on table center */
{
    float cnear, geomfact;
    static int glballlist[]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    static GLfloat ballmatr[]={
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    int i,detail;
    static int maxdetail=-1;
    static ElemArray * array[11];

    if( maxdetail != options_max_ball_detail ){
        for(i=0;i<=maxdetail;i++){
            if(glballlist[i]!=-1) glDeleteLists(glballlist[i],1);
            glballlist[i]=-1;
            for(i=0;i<=maxdetail;i++){
                free_elem_array( array[i] );
                array[i]=(ElemArray *)0;
            }
        }
        maxdetail = options_max_ball_detail;
    }

    if( glballlist[0]==-1 ){
        for(i=0;i<=maxdetail;i++){
            glballlist[i] = glGenLists(1);
            array[i] = create_ball_icosa_array( BALL_D/2.0, i, glballlist[i] );
        }
    }

    glPushMatrix();

    glTranslatef( ball->r.x, ball->r.y, ball->r.z );

    ballmatr[ 0]=ball->b[0].x;
    ballmatr[ 1]=ball->b[0].y;
    ballmatr[ 2]=ball->b[0].z;

    ballmatr[ 4]=ball->b[1].x;
    ballmatr[ 5]=ball->b[1].y;
    ballmatr[ 6]=ball->b[1].z;

    ballmatr[ 8]=ball->b[2].x;
    ballmatr[ 9]=ball->b[2].y;
    ballmatr[10]=ball->b[2].z;

    glMultMatrixf( ballmatr );

    geomfact=40.0/cam_FOV*(VMfloat)win_width/800;
    cnear = options_ball_detail_nearmax*geomfact;

    /* 1.0 instead of 2.0 because angles get smaller when smaller detail */
    detail=maxdetail-1.0*log(vec_abs(vec_diff(cam_pos,ball->r))/cnear)/0.693147f; //quicker then log(2.0)
    //original: detail=maxdetail-1.0*log(vec_abs(vec_diff(cam_pos,ball->r))/cnear)/log(2.0);

    if( detail>maxdetail ) {
    	detail=maxdetail;
    } else if( detail<0 ) {
    	detail=0;
    }

    if(options_glassballs) {
      /* Glass balls */
      glEnable (GL_BLEND); //only needing, if other balls are drawn (e.g. glass-balls)
      glMaterialfv(GL_FRONT, GL_DIFFUSE, col_null);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_COLOR); // glass balls)
      // End glass balls
    }

    glCallList(glballlist[detail]);

    if(options_glassballs) {
     glPolygonMode(GL_BACK,GL_LINE);
    }

    glPopMatrix();
}

/***********************************************************************/

void my_copy_area_1_3( char * src, int w1, int wc, int hc, int x0, int y0, char * dst , int w)
{
    int x,y,i;
    for(y=0;y<hc;y++){
        for(x=0;x<wc;x++){
            for(i=0;i<3;i++){
                if( src[y*w1+x]!=(char)0xFF )
                    dst[((y+y0)*w+x+x0)*3+i]=src[y*w1+x];
            }
        }
    }
}

/***********************************************************************/

void create_balltex( int nr, int * w, int * h, int * depth, char ** data)
{
    int i, x, y ,width, height, dw, dh;
    int bg_r, bg_g, bg_b;
    int fg_r, fg_g, fg_b;
    VMfloat r_hole=60.0;
    VMfloat r,d,dx,dy,fact;
    VMfloat sx=0.0;
    VMfloat sy=0.0;
    VMfloat sum=0.0;
    VMfloat sx2=0.0;
    VMfloat sy2=0.0;
    char str[80];
    char * nrdata;
    int xmin, ymin;
    int xmax=0;
    int ymax=0;

    if(nr<=9) r_hole=50;
    if(nr==0) r_hole=16;

    options_col_ball=options_col_ball_pool;
    fg_r = ((options_col_ball[nr])>>16) & 0xFF;
    fg_g = ((options_col_ball[nr])>> 8) & 0xFF;
    fg_b = ((options_col_ball[nr])>> 0) & 0xFF;

    if(nr==0){ bg_r=0x00; bg_g=0x00; bg_b=0x00; }
    else     { bg_r=0xFF; bg_g=0xFF; bg_b=0xFF; }

    *w=256;
    *h=256;
    *depth=24;
    *data=malloc((*w)*(*h)*3);

    /* bg color */
    for(y=0;y<*h;y++){
        for(x=0;x<*w;x++){
            (*data)[(*w*y+x)*3+0]=bg_r;
            (*data)[(*w*y+x)*3+1]=bg_g;
            (*data)[(*w*y+x)*3+2]=bg_b;
        }
    }

    /* ball color (strip for half) */
    for(y=0;y<*h;y++){
        for(x=0;x<*w;x++){
            if( nr<9 || (y>=64 && y<=192) ){
                (*data)[(*w*y+x)*3+0]=fg_r;
                (*data)[(*w*y+x)*3+1]=fg_g;
                (*data)[(*w*y+x)*3+2]=fg_b;
            }
        }
    }

    /* number circle */
    for(y=0;y<*h;y++){
        for(x=0;x<*w;x++){
            dx=x-(*w)/2;
            dy=y-(*h)/2;
            r=sqrt(dx*dx+dy*dy);
            if(r<r_hole+0.5){
                if(r>r_hole-0.5)
                    fact=r-r_hole+0.5;
                else
                    fact=0.0;
                (*data)[(*w*y+x)*3+0]=(VMfloat)fg_r*fact+(VMfloat)bg_r*(1.0-fact);
                (*data)[(*w*y+x)*3+1]=(VMfloat)fg_g*fact+(VMfloat)bg_g*(1.0-fact);
                (*data)[(*w*y+x)*3+2]=(VMfloat)fg_b*fact+(VMfloat)bg_b*(1.0-fact);
            }
        }
    }

    /* number */
    if(nr!=0){
        sprintf(str,"%d",nr);
        getStringPixmapFT(str, options_ball_fontname, 75, &nrdata, &dw, &dh, &width, &height);
        xmin=dw;
        ymin=dh;
        for(i=0;i<dw*dh;i++){
            if(nrdata[i]==(char)0xFF){
                if( (i%dw)<xmin ) xmin=(i%dw);
                if( (i%dw)>xmax ) xmax=(i%dw);
                if( (i/dw)<ymin ) ymin=(i/dw);
                if( (i/dw)>ymax ) ymax=(i/dw);
            }
            sx+=(VMfloat)(i%dw)*(VMfloat)((unsigned char)nrdata[i]);
            sy+=(VMfloat)(i/dw)*(VMfloat)((unsigned char)nrdata[i]);
            sum+=(VMfloat)((unsigned char)nrdata[i]);
//            fprintf(stderr,"%d",(unsigned char)nrdata[i]);
            nrdata[i]=~nrdata[i];
        }
        if(nr==6 || nr==9){
            for(y=ymax+5;y<ymax+10;y++){
                for(x=xmin+6;x<xmax-6;x++){
                    nrdata[y*dw+x]=0x00;
                }
            }
            ymax=ymax+10;
        }
        sx/=sum;
        sy/=sum;
        sx2=(VMfloat)(xmin+xmax)/2.0;
        sy2=(VMfloat)(ymin+ymax)/2.0;
        sx=(sx+sx2)/2.0;
        sy=(sy+sy2)/2.0;
        //fprintf(stderr,"sx=%f, sy=%f\n");
        my_copy_area_1_3( nrdata, dw, width, height, (*w)/2-(sx+0.5), (*h)/2-(sy+0.5), *data, *w);
    }

    if( options_rgstereo_on ){
        /* graying out texture */
        for(i=0;i<(*h)*(*w);i++){
            d=((unsigned char)(*data)[i*3+0]+
               (unsigned char)(*data)[i*3+1]+
               (unsigned char)(*data)[i*3+2])/3.0;
            (*data)[i*3+0]=d;
            (*data)[i*3+1]=d;
            (*data)[i*3+2]=d;
        }
    }

}

/***********************************************************************/

void create_balltex_snooker( int nr, int * w, int * h, int * depth, char ** data)
{
    int i,x,y;
    int bg_r=0x00;
    int bg_g=0x00;
    int bg_b=0x00;
    int fg_r, fg_g, fg_b;
    VMfloat r_hole=16;
    VMfloat r,d,dx,dy,fact;

    if(nr>8) nr=1;

    options_col_ball=options_col_ball_snooker;
    fg_r = ((options_col_ball[nr])>>16) & 0xFF;
    fg_g = ((options_col_ball[nr])>> 8) & 0xFF;
    fg_b = ((options_col_ball[nr])>> 0) & 0xFF;

    *w=256;
    *h=256;
    *depth=24;
    *data=malloc((*w)*(*h)*3);

    /* ball color */
    for(y=0;y<*h;y++){
        for(x=0;x<*w;x++){
            (*data)[(*w*y+x)*3+0]=fg_r;
            (*data)[(*w*y+x)*3+1]=fg_g;
            (*data)[(*w*y+x)*3+2]=fg_b;
        }
    }

    /* dot */
    for(y=0;y<*h;y++){
        for(x=0;x<*w;x++){
            dx=x-(*w)/2;
            dy=y-(*h)/2;
            r=sqrt(dx*dx+dy*dy);
            if(r<r_hole+0.5){
                if(r>r_hole-0.5) {
                    fact=r-r_hole+0.5;
                } else {
                    fact=0.0;
                }
                (*data)[(*w*y+x)*3+0]=(VMfloat)fg_r*fact+(VMfloat)bg_r*(1.0-fact);
                (*data)[(*w*y+x)*3+1]=(VMfloat)fg_g*fact+(VMfloat)bg_g*(1.0-fact);
                (*data)[(*w*y+x)*3+2]=(VMfloat)fg_b*fact+(VMfloat)bg_b*(1.0-fact);
            }
        }
    }

    if( options_rgstereo_on ){
        /* graying out texture */
        for(i=0;i<(*h)*(*w);i++){
            d=((unsigned char)(*data)[i*3+0]+
               (unsigned char)(*data)[i*3+1]+
               (unsigned char)(*data)[i*3+2])/3.0;
            (*data)[i*3+0]=d;
            (*data)[i*3+1]=d;
            (*data)[i*3+2]=d;
        }
    }

}

/***********************************************************************/

void create_balltex_carambol( int nr, int * w, int * h, int * depth, char ** data)
{
    int i,x,y;
    int bg_r=0x00;
    int bg_g=0x00;
    int bg_b=0x00;
    int fg_r, fg_g, fg_b;
    VMfloat r_hole=16;
    VMfloat r,d,dx,dy,fact;

    options_col_ball=options_col_ball_carambol;
    fg_r = ((options_col_ball[nr])>>16) & 0xFF;
    fg_g = ((options_col_ball[nr])>> 8) & 0xFF;
    fg_b = ((options_col_ball[nr])>> 0) & 0xFF;

    *w=256;
    *h=256;
    *depth=24;
    *data=malloc((*w)*(*h)*3);

    /* ball color */
    for(y=0;y<*h;y++){
        for(x=0;x<*w;x++){
            (*data)[(*w*y+x)*3+0]=fg_r;
            (*data)[(*w*y+x)*3+1]=fg_g;
            (*data)[(*w*y+x)*3+2]=fg_b;
        }
    }

    /* dot */
    for(y=0;y<*h;y++){
        for(x=0;x<*w;x++){
            dx=x-(*w)/2;
            dy=y-(*h)/2;
            r=sqrt(dx*dx+dy*dy);
            if(r<r_hole+0.5){
                if(r>r_hole-0.5)
                    fact=r-r_hole+0.5;
                else
                    fact=0.0;
                (*data)[(*w*y+x)*3+0]=(VMfloat)fg_r*fact+(VMfloat)bg_r*(1.0-fact);
                (*data)[(*w*y+x)*3+1]=(VMfloat)fg_g*fact+(VMfloat)bg_g*(1.0-fact);
                (*data)[(*w*y+x)*3+2]=(VMfloat)fg_b*fact+(VMfloat)bg_b*(1.0-fact);
            }
        }
    }

    if( options_rgstereo_on ){
        /* graying out texture */
        for(i=0;i<(*h)*(*w);i++){
            d=((unsigned char)(*data)[i*3+0]+
               (unsigned char)(*data)[i*3+1]+
               (unsigned char)(*data)[i*3+2])/3.0;
            (*data)[i*3+0]=d;
            (*data)[i*3+1]=d;
            (*data)[i*3+2]=d;
        }
    }

}

/***********************************************************************/

void free_pooltexbinds( void )
{
    int i;
    for(i=0;i<16;i++){
        glDeleteTextures(1,&balltexbind[i]);
    }
    g_ballset=BALLSET_NONE;
}

/***********************************************************************/

void free_caramboltexbinds( void )
{
    int i;
    for(i=0;i<3;i++){
        glDeleteTextures(1,&balltexbind[i]);
    }
    g_ballset=BALLSET_NONE;
}

/***********************************************************************/

void free_snookertexbinds( void )
{
    int i;
    for(i=0;i<8;i++){
        glDeleteTextures(1,&balltexbind[i]);
    }
    g_ballset=BALLSET_NONE;
}

/***********************************************************************/

void create_pooltex_binds( void )
{
    int i;

    g_ballset=BALLSET_POOL;
    for(i=0;i<16;i++){
        glGenTextures(1,&balltexbind[i]);
        create_balltex(i,&balltexw,&balltexh,&depth,&balltexdata[i]);
        glBindTexture(GL_TEXTURE_2D,balltexbind[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options_tex_min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options_tex_mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, balltexw, balltexh, GL_RGB,
                          GL_UNSIGNED_BYTE, balltexdata[i]);
        if(options_anisotrop && options_value_anisotrop > 0.0) {
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, options_value_anisotrop);
        }
        free(balltexdata[i]);
        //fprintf(stderr,"balltexbind[%d]=%d\n",i,balltexbind[i]);
    }
}

/***********************************************************************/

void create_caramboltex_binds( void )
{
    int i;

    g_ballset=BALLSET_CARAMBOL;
    for(i=0;i<3;i++){
        glGenTextures(1,&balltexbind[i]);
        create_balltex_carambol(i,&balltexw,&balltexh,&depth,&balltexdata[i]);
        glBindTexture(GL_TEXTURE_2D,balltexbind[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options_tex_min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options_tex_mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, balltexw, balltexh, GL_RGB,
                          GL_UNSIGNED_BYTE, balltexdata[i]);
        if(options_anisotrop && options_value_anisotrop > 0.0) {
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, options_value_anisotrop);
        }
        free(balltexdata[i]);
        //fprintf(stderr,"balltexbind[%d]=%d\n",i,balltexbind[i]);
    }
}

/***********************************************************************/

void create_snookertex_binds( void )
{
    int i;

    g_ballset=BALLSET_SNOOKER;
    for(i=0;i<8;i++){
        glGenTextures(1,&balltexbind[i]);
        create_balltex_snooker(i,&balltexw,&balltexh,&depth,&balltexdata[i]);
        glBindTexture(GL_TEXTURE_2D,balltexbind[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options_tex_min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options_tex_mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, balltexw, balltexh, GL_RGB,
                          GL_UNSIGNED_BYTE, balltexdata[i]);
        if(options_anisotrop && options_value_anisotrop > 0.0) {
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, options_value_anisotrop);
        }
        free(balltexdata[i]);
        //fprintf(stderr,"balltexbind[%d]=%d\n",i,balltexbind[i]);
    }
    for(i=8;i<22;i++){
        balltexbind[i]=balltexbind[1];
        //fprintf(stderr,"balltexbind[%d]=%d\n",i,balltexbind[i]);
    }
}

/***********************************************************************/

enum BallSet ballset_for_gametype(enum gameType g)
{
            switch(g){
            case GAME_8BALL:    return(BALLSET_POOL);     break;
            case GAME_9BALL:    return(BALLSET_POOL);     break;
            case GAME_CARAMBOL: return(BALLSET_CARAMBOL); break;
            case GAME_SNOOKER:  return(BALLSET_SNOOKER);  break;
            default:            return(BALLSET_NONE);
            }
}

/***********************************************************************/

void create_texbinds( BallsType *balls )
{
    switch(g_ballset){
    case BALLSET_POOL:      free_pooltexbinds();     break;
    case BALLSET_CARAMBOL:  free_caramboltexbinds(); break;
    case BALLSET_SNOOKER:   free_snookertexbinds();  break;
    case BALLSET_NONE:      break;
    }
    switch(balls->gametype){
    case GAME_8BALL:    create_pooltex_binds();     break;
    case GAME_9BALL:    create_pooltex_binds();     break;
    case GAME_CARAMBOL: create_caramboltex_binds(); break;
    case GAME_SNOOKER:  create_snookertex_binds();  break;
    }
}

/***********************************************************************/

void draw_balls( BallsType balls, myvec cam_pos, GLfloat cam_FOV, int win_width, int spheretexbind, VMvect * lightpos, int lightnr, unsigned int * cuberef_binds )
{
    static int init = 0;
    static int sphere1_id= -1;            // sphere blending glcompile-id
    static int sphere2_id= -1;            // sphere blending glcompile-id (standard-blend)
    static int cuberef_id = -1;           // cuberef glcompile-id
    static int light_id = -1;             // light glquad glcompile-id
    static int shadow_id = -1;            // shadow glcompile-id
    static int cuberef1_id = -1;          // cuberef1 glcompile-id
    static int blended_id = -1;           // blended glcompile-id
    static int drawball1_id = -1;         // draw ball glcompile-id
    static int drawball2_id = -1;         // draw ball glcompile-id

#define SH_SZ 0.040
#define SH_FACT 0.32

    int i,j;
    VMfloat fact;
    VMvect v,vn;
    GLfloat stretch_matrix[16]={0.0}; //init all to zero
    GLfloat mat[16],x,y,w;
    GLfloat texmat[16];
    GLfloat dummy;

    stretch_matrix[10]=1.0; // stretch for shadows here is 1.0 every time for every ball
    stretch_matrix[15]=57.15E-3/BALL_D; //dto. 1.0f if macro BALL_D is unchanged

    col_shad[3]=1.0; //every time the same for shadow color [3]
    glDisable(GL_CULL_FACE);
    if( !init ){
        create_png_texbind("shadow2.png", &shadowtexbind, 1, GL_LUMINANCE);
        create_png_texbind("sphere_ball.png", &sphere_ballbind, 3, GL_RGB);
        create_texbinds(&balls);

 	    cuberef_id = glGenLists(1);
 	    glNewList(cuberef_id, GL_COMPILE);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_GEN_R);
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_CUBE_MAP_ARB);
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_TEXTURE_2D);
        glEndList();

 	    light_id = glGenLists(1);
 	    glNewList(light_id, GL_COMPILE);
        glBegin(GL_QUADS);
        glNormal3s( 0,0,1 );
        glTexCoord2s(0,1);
        glVertex3f( -SH_SZ, +SH_SZ, 0.0 );
        glTexCoord2s(1,1);
        glVertex3f( +SH_SZ, +SH_SZ, 0.0 );
        glTexCoord2s(1,0);
        glVertex3f( +SH_SZ, -SH_SZ, 0.0 );
        glTexCoord2s(0,0);
        glVertex3f( -SH_SZ, -SH_SZ, 0.0 );
        glEnd();
        glPopMatrix();
        glEndList();

 	    shadow_id = glGenLists(1);
 	    glNewList(shadow_id, GL_COMPILE);
 	    glMaterialfv(GL_FRONT, GL_AMBIENT,   col_amb );
        glMaterialfv(GL_FRONT, GL_SPECULAR,  col_spec);
        glMaterialf (GL_FRONT, GL_SHININESS, 0.0     );
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDepthMask (GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc (GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,shadowtexbind);
        glEndList();

 	    cuberef1_id = glGenLists(1);
 	    glNewList(cuberef1_id, GL_COMPILE);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_R);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
        glEndList();

 	    blended_id = glGenLists(1);
 	    glNewList(blended_id, GL_COMPILE);
 	    glDepthMask (GL_FALSE);
        glEnable(GL_BLEND);
        glPolygonOffset( 0.0, -2.0 );
        glEnable( GL_POLYGON_OFFSET_FILL );
        glEndList();

 	    drawball1_id = glGenLists(1);
 	    glNewList(drawball1_id, GL_COMPILE);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   col_diff3);
        glMaterialfv(GL_FRONT, GL_AMBIENT,   col_amb3 );
        glMaterialfv(GL_FRONT, GL_SPECULAR,  col_spec );
        glMaterialf (GL_FRONT, GL_SHININESS, 0.0      );
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glEndList();

 	    drawball2_id = glGenLists(1);
 	    glNewList(drawball2_id, GL_COMPILE);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   col_diff);
        glMaterialfv(GL_FRONT, GL_AMBIENT,   col_amb );
        glMaterialfv(GL_FRONT, GL_SPECULAR,  col_spec);
        glMaterialf (GL_FRONT, GL_SHININESS, 0.0     );
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glEndList();

 	    sphere1_id = glGenLists(1);
 	    glNewList(sphere1_id, GL_COMPILE);
 	      glBindTexture(GL_TEXTURE_2D,spheretexbind);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glEndList();

      sphere2_id = glGenLists(1);
   	  glNewList(sphere2_id, GL_COMPILE);
   	    glBindTexture(GL_TEXTURE_2D,sphere_ballbind);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
      glEndList();

        init=1;
    }

    /* whole ball culling */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glGetFloatv(GL_MODELVIEW_MATRIX,mat);
#define BORDER_SHIFT 2.0
    glTranslatef(0,0,-BORDER_SHIFT*balls.ball[0].d); /* half FOV must be >= asin(1/BORDER_SHIFT) */
    glMultMatrixf(mat);
    glGetFloatv(GL_PROJECTION_MATRIX,mat);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    for(i=0;i<balls.nr;i++) if(balls.ball[i].in_game){
        x = mat[0]*balls.ball[i].r.x +
            mat[4]*balls.ball[i].r.y +
            mat[8]*balls.ball[i].r.z +
            mat[12];
        y = mat[1]*balls.ball[i].r.x +
            mat[5]*balls.ball[i].r.y +
            mat[9]*balls.ball[i].r.z +
            mat[13];
        w = mat[3]*balls.ball[i].r.x +
            mat[7]*balls.ball[i].r.y +
            mat[11]*balls.ball[i].r.z +
            mat[15];
        balls.ball[i].in_fov = ( x>-w && x<w && y>-w && y<w );
        }

    if(g_ballset!=ballset_for_gametype(balls.gametype)){
        create_texbinds(&balls);
    }

    //fprintf(stderr,"max_ARB=%d\n",i);

    glEnable(GL_TEXTURE_2D);

    /* draw balls */
    if( options_cuberef ) {
        //fprintf(stderr,"drawball1_id %i\n",drawball1_id);
        glCallList(drawball1_id);
    } else {
        //fprintf(stderr,"drawball2_id %i\n",drawball2_id);
        glCallList(drawball2_id);
    }

    /* draw balls */
        for(i=0;i<balls.nr;i++) {
          if(balls.ball[i].in_game && balls.ball[i].in_fov){
            glBindTexture(GL_TEXTURE_2D,balltexbind[balls.ball[i].nr]);
            draw_ball(&balls.ball[i],cam_pos,cam_FOV,win_width);
          }
        }

    /* draw extra blended ball-reflections */
    if(!(options_cuberef && cuberef_binds==0) ){
        //fprintf(stderr,"blended_id %i\n",blended_id);
        glCallList(blended_id);
        if(options_cuberef){
            glBlendFunc (GL_ONE_MINUS_DST_COLOR, GL_ONE);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, col_refl3);
        } else {
            glBlendFunc (GL_SRC_ALPHA, GL_ONE);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, col_refl);
        }
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        if( options_cuberef && cuberef_binds!=0 ) {
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            //fprintf(stderr,"cuberef1_id %i\n",cuberef1_id);
            glCallList(cuberef1_id);
            glGetFloatv(GL_MODELVIEW_MATRIX, texmat);
            /* maybe set this per ball to camera direction */
            texmat[12]=0.0;
            texmat[13]=0.0;
            texmat[14]=0.0;
            /* transpose */
            dummy=texmat[1]; texmat[1]=texmat[4]; texmat[4]=dummy;
            dummy=texmat[2]; texmat[2]=texmat[8]; texmat[8]=dummy;
            dummy=texmat[6]; texmat[6]=texmat[9]; texmat[9]=dummy;
            glMatrixMode(GL_TEXTURE);
            glLoadMatrixf(texmat);
            glMatrixMode(GL_MODELVIEW);
        } else {
        	   if(options_ball_sphere) {
               glCallList(sphere1_id);
        	   } else {
        	   	  glCallList(sphere2_id);
        	   }
        }
        for(i=0;i<balls.nr;i++) {
        	if(balls.ball[i].in_game && balls.ball[i].in_fov) {
            if( options_cuberef && cuberef_binds!=0 ){
                glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cuberef_binds[i]);
            }
            draw_ball(&balls.ball[i],cam_pos,cam_FOV,win_width);
         }
        }
        if( options_cuberef && cuberef_binds!=0 ){
   	        //fprintf(stderr,"cuberef_id %i\n",cuberef_id);
   	        glCallList(cuberef_id);
        } else {
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            glDisable(GL_BLEND);
        }
        glDisable( GL_POLYGON_OFFSET_FILL );
        glDepthMask (GL_TRUE);
    }

   /* draw shadows */
    //fprintf(stderr,"shadow_id %i\n",cuberef_id);
    glCallList(shadow_id);
    for(i=0;i<balls.nr;i++) {
     if(balls.ball[i].in_game && balls.ball[i].in_fov) {
      for(j=0;j<lightnr;j++) {
        v=vec_diff(balls.ball[i].r,vec_xyz(lightpos[j].x,lightpos[j].y,0.0));
        fact=1.0+vec_abs(v)*SH_FACT;
        col_shad[0]=0.5-0.3*vec_abs(v);
        if(col_shad[0]<0.0) {
        	col_shad[0]=0.0;
        }
        col_shad[1]=col_shad[0];
        col_shad[2]=col_shad[0];
        //col_shad[3] is always the same and at the function start declared
        glMaterialfv(GL_FRONT, GL_DIFFUSE, col_shad);
        glColor3i(0,0,0);   /* against shadow flicker bug */
        if( v.x==0.0 && v.y==0.0 && v.z==0.0 ){
            v=vec_xyz(1,0,0);
        } else {
            v=vec_unit(v);
        }
        vn=vec_xyz(-v.y,v.x,v.z);
        //all other in the stretch_matrix are well defined at the top of the function
        stretch_matrix[ 0]=vn.x*vn.x+fact*v.x*v.x;
        stretch_matrix[ 5]=vn.y*vn.y+fact*v.y*v.y;
        stretch_matrix[ 4]=vn.x*vn.y+fact*v.x*v.y;
        stretch_matrix[ 8]=vn.x*vn.z+fact*v.x*v.z;
        stretch_matrix[ 9]=vn.y*vn.z+fact*v.y*v.z;
        stretch_matrix[ 1]=vn.x*vn.y+fact*v.x*v.y;
        stretch_matrix[ 2]=vn.x*vn.z+fact*v.x*v.z;
        stretch_matrix[ 6]=vn.y*vn.z+fact*v.y*v.z;
        glPushMatrix();
        glTranslatef( lightpos[j].x+(balls.ball[i].r.x-lightpos[j].x)*1.025,
                      lightpos[j].y+(balls.ball[i].r.y-lightpos[j].y)*1.025,
                      -balls.ball[i].d/2.02 );
        glMultMatrixf(stretch_matrix);
        //fprintf(stderr,"light_id %i\n",light_id);
        glCallList(light_id);
      }
     }
    }
    glDisable(GL_BLEND);
    glDepthMask (GL_TRUE);
    glEnable(GL_CULL_FACE);
#undef SH_SZ

}

/***********************************************************************
 *                    Draw the ball traces on screen                   *
 ***********************************************************************/

void draw_ballpath( BallType * pball)
{
	int i;
	i = pball->nr;
    glColor3f((VMfloat)(((options_col_ball[i])>>16) & 0xFF)/255.0,
              (VMfloat)(((options_col_ball[i])>> 8) & 0xFF)/255.0,
              (VMfloat)(((options_col_ball[i])>> 0) & 0xFF)/255.0);
    glBegin(GL_LINE_STRIP);
      for(i=0;i<pball->pathcnt;i++){
        glVertex3f(pball->path[i].x,pball->path[i].y,pball->path[i].z);
      }
    glEnd();
}
