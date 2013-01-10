#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <stdlib.h>

/* collision.c */
//gcc -o collision collision.c -lglut

#define VOL 150
#define NP 7

struct square {
        int x;  // x position
        int y;  // y position
        int w;  // size
        int v;  // vertical velocity
        int vd; // vertical direction (1 or -1)
        int h;  // horizontal velocity
        int hd; // horizontal direction (1 or -1)
} *piec;

void scene() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0f, 1.0f, 0.0f);
    glRecti(piec[0].x,piec[0].y,piec[0].x+piec[0].w,piec[0].y-piec[0].w);
    glColor3f(0.0f, 0.5f, 0.0f);
    glRecti(piec[1].x,piec[1].y,piec[1].x+piec[1].w,piec[1].y-piec[1].w);
    glColor3f(0.0f, 0.0f, 0.5f);
    glRecti(piec[2].x,piec[2].y,piec[2].x+piec[2].w,piec[2].y-piec[2].w);
    glColor3f(1.0f, 1.0f, 0.0f);
    glRecti(piec[3].x,piec[3].y,piec[3].x+piec[3].w,piec[3].y-piec[3].w);
    glColor3f(0.5f, 0.0f, 0.0f);
    glRecti(piec[4].x,piec[4].y,piec[4].x+piec[4].w,piec[4].y-piec[4].w);
    glColor3f(0.0f, 0.0f, 0.0f);
    glRecti(piec[5].x,piec[5].y,piec[5].x+piec[5].w,piec[5].y-piec[5].w);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRecti(piec[6].x,piec[6].y,piec[6].x+piec[6].w,piec[6].y-piec[6].w);
    glutSwapBuffers();
}

void setup(float R, float G, float B, GLdouble P) {
    GLdouble N=0-P;
    glClearColor(R,G,B,1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(N,P,N,P,N,P);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Vcollision (int P) {
    int i;
    for (i=0;i<NP;i++) {
        if (i==P) continue;
        if ((piec[P].y==piec[i].y-piec[i].w) && (piec[P].x<=piec[i].x+piec[i].w) &&
            (piec[P].x+piec[P].w>=piec[i].x)) {piec[P].vd=-1;
            piec[P].y-=1;
            if (piec[i].vd==-1) piec[i].vd=1;
            if (piec[P].v>=piec[i].v) piec[i].v--;
            else {piec[P].v++;piec[i].v++;}}
        else if ((piec[P].y-piec[P].w==piec[i].y) && (piec[P].x<=piec[i].x+piec[i].w) &&
            (piec[P].x+piec[P].w>=piec[i].x)) {piec[P].vd=1;
            piec[P].y+=1;
            if (piec[i].vd==1) piec[i].vd=-1;
            if (piec[P].v>=piec[i].v) piec[i].v--;
            else {piec[P].v++;piec[i].v++;}}
    }
}

void Hcollision (int P) {
    int i;
    for (i=0;i<NP;i++) {
        if (i==P) continue;
        if ((piec[P].x==piec[i].x+piec[i].w) && (piec[P].y>=piec[i].y-piec[i].w) &&
            (piec[P].y-piec[P].w<=piec[i].y)) {piec[P].hd=1;
            piec[P].x+=1;
            if (piec[i].hd==1) piec[i].hd=-1;
            if (piec[P].h>=piec[i].h) piec[i].h--;
            else {piec[P].h++;piec[i].h++;}}
        else if ((piec[P].x+piec[P].w==piec[i].x) && (piec[P].y>=piec[i].y-piec[i].w) &&
            (piec[P].y-piec[P].w<=piec[i].y)) {piec[P].hd=-1;
            piec[P].x-=1;
            if (piec[i].hd==-1) piec[i].hd=1;
            if (piec[P].h>=piec[i].h) piec[i].h--;
            else {piec[P].h++;piec[i].h++;}}
    }
}

void move(int T) {
    int rendintrv, i;
    for (rendintrv=0; rendintrv<15; rendintrv++) {
        if (T==1000001) T=1;
        for (i=0; i<NP; i++) {
            if ((piec[i].v>0) && (T%piec[i].v==0)) {piec[i].y+=piec[i].vd;
                if (piec[i].y-piec[i].w<=-VOL) {piec[i].vd=1;
                    piec[i].y=-VOL+piec[i].w+1; piec[i].v--;}
                else if (piec[i].y>=VOL) {piec[i].vd=-1;
                    piec[i].y=VOL-1; piec[i].v--;}
                else Vcollision(i);
            }
            if ((piec[i].h>0) && (T%piec[i].h==0)) {piec[i].x+=piec[i].hd;
                if (piec[i].x+piec[i].w>=VOL) {piec[i].hd=-1;
                    piec[i].x=VOL-piec[i].w-1; piec[i].h--;}
                else if (piec[i].x<=-VOL) {piec[i].hd=1;
                    piec[i].x=-VOL+1; piec[i].h--;}
                else Hcollision(i);
            }
            if (piec[i].v<=1) piec[i].v=2;       // min-max speeds
            else if (piec[i].v>30) piec[i].v=50;
            if (piec[i].h<=1) piec[i].h=2;
            else if (piec[i].h>30) piec[i].h=50;
        }
        T++;
    }
    scene();
    glutTimerFunc(1,move,T);
}


int main(int argc, char *argv[]) {
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
        glutInitWindowSize(900,900);
    glutCreateWindow("openGL test");
    glutDisplayFunc(scene);
    glutTimerFunc(1500,move,1);

    piec=malloc(NP*sizeof(*piec));
    piec[0].x=-100;piec[0].y=0; piec[0].w=30; piec[0].v=8;piec[0].vd=1; piec[0].h=4;piec[0].hd=1;
    piec[1].x=10;piec[1].y=50; piec[1].w=18; piec[1].v=4;piec[1].vd=-1; piec[1].h=8;piec[1].hd=-1;
    piec[2].x=50;piec[2].y=0; piec[2].w=14; piec[2].v=8;piec[2].vd=-1; piec[2].h=8;piec[2].hd=1;
    piec[3].x=75;piec[3].y=90; piec[3].w=22; piec[3].v=6;piec[3].vd=1; piec[3].h=6;piec[3].hd=-1;
    piec[4].x=-50;piec[4].y=0; piec[4].w=60; piec[4].v=5;piec[4].vd=-1; piec[4].h=5;piec[4].hd=1;
    piec[5].x=100;piec[5].y=-60; piec[5].w=45; piec[5].v=3;piec[5].vd=-1; piec[5].h=6;piec[5].hd=1;
    piec[6].x=-120;piec[6].y=-100; piec[6].w=27; piec[6].v=7;piec[6].vd=-1; piec[6].h=4;piec[6].hd=1;
    setup(1.0,0.0,1.0,VOL);

    glutMainLoop();
    return 0;
}
