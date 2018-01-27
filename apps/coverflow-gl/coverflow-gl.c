#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <sys/time.h>

#include "global.h"

#ifdef GPU_GLES1

#define OPENGL_API_USE_ES2  0

#include <EGL/egl.h>
#if OPENGL_API_USE_ES2
    #include <GLES2/gl2.h>
    #include <glHelpers.h>
#else
    #include <GLES/gl.h>
#endif

#ifndef M_PI
    #define M_PI (3.14159265)
#endif
#define M_PI_OVER_180 (M_PI/180.0)

#if OPENGL_API_USE_ES2
    #include "./shader/coverflow.vert.h"
    #include "./shader/coverflow.frag.h"
#endif

void coverflow_init(void);
void coverflow_deinit(void);
unsigned int coverflow_elasped_time(struct timeval *tv);
void coverflow_set_texture(unsigned char *image, int w, int h, int bpp, int itemw, int itemh, int n);
void coverflow_set_sub_tex_index(unsigned *indexarray);
void coverflow_add(void);
void coverflow_clear(void);
void coverflow_draw(void);
void coverflow_backing_anim_start_end_value(float *s, float *e);
void coverflow_stepping_anim_start_end_value(float *s, float *e, int direction);
void coverflow_scrolling_anim_start_end_value(float *s, float *e, float vx);
void coverflow_reshape(int w, int h);
bool coverflow_set_eye(float eyex, float eyey, float eyez, bool depthtest);
float coverflow_eyex(void);
int coverflow_hilight(void);

#if OPENGL_API_USE_ES2
#else
static void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
    GLfloat xmin, xmax, ymin, ymax;

    ymax = zNear * tan(fovy * M_PI / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}
#endif

#define TEX_NUM 1
static GLuint textureIds[TEX_NUM];
static GLuint *subTexIndex = NULL;

static int width    = ACTIVITY_W;
static int height   = ACTIVITY_H / 2;
#define ITEM_SPACING    30.0
#define DEXTRAL_ANGLE   70.0

static GLfloat eye[3] = {0.0, -5.0, 239.9};
#define MAX_FRAME_MOVING    10.0
#define MIN_FRAME_MOVING     0.0
static GLfloat frameMoving = 0.0;

typedef struct _ItemInfo {
    GLfloat cx;
    GLfloat cy;
    GLfloat cz;
} ItemInfo;

static ItemInfo itemInfo[500];
static int itemLength = 0;
static int hilight = 0;
static int visibleItemNum = 0;

#define MAX_EYEX() ((itemLength-1)*ITEM_SPACING)

unsigned int coverflow_elasped_time(struct timeval *t0)
{
    unsigned int sec, usec, dtime;
    struct timeval t1;

    gettimeofday(&t1, NULL);

    sec = t1.tv_sec - t0->tv_sec;
    if (t1.tv_usec >= t0->tv_usec) {
        usec = t1.tv_usec - t0->tv_usec;
    } else {
        usec = t1.tv_usec + 1000000 - t0->tv_usec;
        --sec;
    }
    dtime = sec*1000 + ((double)usec) / 1000;

    t0->tv_sec = t1.tv_sec, t0->tv_usec = t1.tv_usec;

    return dtime;
}

void anim_start_end_value(float *s, float *e, int moveTo)
{
    int remainder = moveTo % (int)ITEM_SPACING;
    *s = eye[0];
    *e = moveTo - remainder;
}

inline void coverflow_backing_anim_start_end_value(float *s, float *e)
{
    anim_start_end_value(s, e, eye[0] + ITEM_SPACING / 2);
}

inline void coverflow_stepping_anim_start_end_value(float *s, float *e, int direction)
{
    anim_start_end_value(s, e,
            eye[0] + ((direction > 0) ? -ITEM_SPACING : 3*ITEM_SPACING) / 2
            );
}

inline void coverflow_scrolling_anim_start_end_value(float *s, float *e, float vx)
{
    anim_start_end_value(s, e, eye[0] - vx);
}

void coverflow_add(void)
{
    itemInfo[itemLength].cx = 0.0 + ITEM_SPACING * itemLength;
    itemInfo[itemLength].cy = 0.0;
    itemInfo[itemLength].cz = 0.0;
    ++itemLength;
}

void coverflow_clear(void)
{
    itemLength = 0;
}

#define ITEM_W  36
#define ITEM_H  36
#define ELEMENT_SIZE    4
#define VERTEX_SIZE     2
#define COLOR_SIZE      4
#define TEXCOORD_SIZE   2
#define VBO_NUM         4
enum {
    ELEMENT_INDEX = 0,
    VERTEX_INDEX,
    COLOR_INDEX,
    TEXCOORD_INDEX,
};
static unsigned texcoordStride;
static GLuint vboIds[VBO_NUM];

void initElementVBO(void)
{
    GLubyte elements[][ELEMENT_SIZE] = {
        {0, 4, 5, 1},
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[ELEMENT_INDEX]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            8 * sizeof(GLubyte), elements, GL_STATIC_DRAW);
}

void initVertexVBO(void)
{
    GLfloat vertices[][VERTEX_SIZE] = {
        { ITEM_W,   -ITEM_H},
        {-ITEM_W,   -ITEM_H},
        {-ITEM_W, -3*ITEM_H},
        { ITEM_W, -3*ITEM_H},
        { ITEM_W,    ITEM_H},
        {-ITEM_W,    ITEM_H},
    };

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[VERTEX_INDEX]);
    glBufferData(GL_ARRAY_BUFFER,
            12 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glVertexPointer(VERTEX_SIZE,  GL_FLOAT, 0, (GLvoid*)0);
}

void initColorVBO(void)
{
    GLfloat colors[][COLOR_SIZE] = {
        {1.0, 1.0, 1.0,  0.3},
        {1.0, 1.0, 1.0,  0.3},
        {1.0, 1.0, 1.0, -0.1},
        {1.0, 1.0, 1.0, -0.1},
    };

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[COLOR_INDEX]);
    glBufferData(GL_ARRAY_BUFFER,
            16 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
    glColorPointer(COLOR_SIZE,    GL_FLOAT, 0, (GLvoid*)0);
}

void initTexCoordVBO(int w, int h, int itemw, int itemh, int n)
{
/*
    GLfloat texcoords[][TEXCOORD_SIZE] = {
        {1.0 / 8.0, 1.0 / 2.0},
        {0.0,       1.0 / 2.0},
        {0.0,       0.0,     },
        {1.0 / 8.0, 0.0,     },
        {1.0 / 8.0, 0.0      },
        {0.0,       0.0      },
    };
*/
    int i, k;
    int x, y, nx, ny;
    GLfloat lx, ty, rx, by;
    GLfloat *texcoords;
    texcoordStride = TEXCOORD_SIZE * 6;
    texcoords = (GLfloat*)calloc(texcoordStride * n, sizeof(GLfloat));
    
    nx = w / itemw;
    ny = h / itemh;

    for (i = 0, k = 0; i < texcoordStride * n; i+=texcoordStride, ++k) {
        x  = k % nx;
        y  = k / nx;
        lx = 1.0 / (GLfloat)nx * x;
        rx = 1.0 / (GLfloat)nx * (x + 1);
        ty = 1.0 / (GLfloat)ny * y;
        by = 1.0 / (GLfloat)ny * (y + 1);
//        printf("lx = %f : rx = %f : ty = %f : by = %f\n", lx, rx, ty, by);
        texcoords[i]    = rx, texcoords[i+1]  = by;
        texcoords[i+2]  = lx, texcoords[i+3]  = by;
        texcoords[i+4]  = lx, texcoords[i+5]  = ty;
        texcoords[i+6]  = rx, texcoords[i+7]  = ty;
        texcoords[i+8]  = rx, texcoords[i+9]  = ty;
        texcoords[i+10] = lx, texcoords[i+11] = ty;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[TEXCOORD_INDEX]);
    glBufferData(GL_ARRAY_BUFFER,
            texcoordStride * sizeof(GLfloat) * n, texcoords, GL_STATIC_DRAW);

    free(texcoords);
}

void image_convert_4_4_4_4(unsigned char *src, unsigned char *dst, unsigned size)
{}

void image_convert_5_5_5_1(unsigned char *src, unsigned char *dst, unsigned size)
{}

void image_convert_5_6_5(unsigned char *src, unsigned char *dst, unsigned size)
{
    unsigned i;
    for (i = 0; i < size; i += 4) {
        dst[i]   = src[i+2];
        dst[i+1] = src[i+1];
        dst[i+2] = src[i];
        dst[i+3] = src[i+3];
    }
    return;
}

void coverflow_set_texture(unsigned char *image, int w, int h, int bpp, int itemw, int itemh, int n)
{
    GLint green_bits;
    unsigned char* buff = NULL;
    unsigned size = w * h * bpp;

    glGenTextures(1, &textureIds[0]);
    glBindTexture(GL_TEXTURE_2D, textureIds[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGetIntegerv(GL_GREEN_BITS, &green_bits);
    switch(green_bits) {
        case 4:
            buff = (unsigned char*)malloc(size * sizeof(unsigned char *));
            image_convert_4_4_4_4(image, buff, size);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
                    0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, buff);
            break;
        case 5:
            buff = (unsigned char*)malloc(size * sizeof(unsigned char *));
            image_convert_5_5_5_1(image, buff, size);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
                    0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, buff);
            break;
        case 6:
            buff = (unsigned char*)malloc(size * sizeof(unsigned char *));
            image_convert_5_6_5(image, buff, size);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,
                    0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, buff);
            break;
        case 8:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
                    0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            break;
        default:
            assert(0);
    }

    if (buff) free(buff);

    initTexCoordVBO(w, h, itemw, itemh, n);
}

void coverflow_set_sub_tex_index(unsigned *indexarray)
{
    subTexIndex = indexarray;
}

void draw_item(ItemInfo item, unsigned subtexindex, int distance)
{
    GLfloat mpf;
    GLfloat angle   = 0.0;
    GLfloat x       = item.cx;
    GLfloat y       = item.cy;
    GLfloat z       = item.cz;

    if (distance > 0.0) {
        x += ITEM_SPACING / 2;
        angle += DEXTRAL_ANGLE;
    } else {
        x -= ITEM_SPACING / 2;
        angle -= DEXTRAL_ANGLE;
    }
    if (abs(distance) <= ITEM_SPACING) {
        mpf = 1 - frameMoving * 2 / MAX_FRAME_MOVING;
        if (abs(distance) < ITEM_SPACING / 2) {
            x = item.cx + distance;
            z = mpf * ITEM_SPACING;
            angle = DEXTRAL_ANGLE * distance * 2.0 / ITEM_SPACING;
        } else{
            z = mpf * 2.0 * (ITEM_SPACING - abs(distance));
        }
    }

    glPushMatrix();

    glTranslatef(x, y, z);
    glRotatef(angle, 0.0, -1.0, 0.0);

    // item
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[TEXCOORD_INDEX]);
    glTexCoordPointer(TEXCOORD_SIZE, GL_FLOAT,
            0, (GLvoid*)(subtexindex * texcoordStride * sizeof(GLfloat)));

    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

    // item reflect
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);

    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_COLOR_ARRAY);
    
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);

    glPopMatrix();
}

float coverflow_eyex(void)
{
    return eye[0];
}

bool coverflow_set_eye(float eyex, float eyey, float eyez, bool depthtest)
{
    float d_eye;
    frameMoving = 0.0;
    if (eye[0] != eyex) {
        d_eye = abs(eye[0] - eyex);
        if (depthtest && d_eye > 1.0) {
            d_eye -= 1.0;
            frameMoving = (d_eye > MAX_FRAME_MOVING) ? MAX_FRAME_MOVING : d_eye;
        }

        if (eyex >= 0.0 && eyex <= MAX_EYEX()) {
            eye[0] = eyex;
        } else {
            return false;
        }

        hilight = eye[0] / ITEM_SPACING;
//        printf("eye[0] = %f\n", eye[0]);
    }
    return true;
}

int coverflow_hilight(void)
{
    return hilight;
}

void coverflow_draw(void)
{
    int i, index;
    GLfloat distance;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-eye[0], -eye[1], -eye[2]);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    index = hilight;
    for (i = 0; i < visibleItemNum; ++i) {
        if (i % 2 == 0) {
            index -= i;
        } else {
            index += i;
        }
        if (index >= 0 && index < itemLength) {
            distance = itemInfo[index].cx - eye[0];
            draw_item(itemInfo[index], subTexIndex[index], distance);
//            printf("index = %d\n", index);
        }
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glFlush();
}

void coverflow_reshape(int w, int h)
{
    width = w;
    height = h;
    glViewport (0, 0, (GLint)width, (GLint)height);
    visibleItemNum = width / ITEM_SPACING + 1;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, 500.0);
}

void print_gl_state_value(GLenum pname, char *str)
{
    int value;
    glGetIntegerv(pname, &value);
    printf(str, value);
}

void coverflow_init(void)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glAlphaFunc(GL_GREATER, 0.0);

    glEnable(GL_CULL_FACE);

    glGenBuffers(VBO_NUM, vboIds);
    initElementVBO();
    initVertexVBO();
    initColorVBO();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    print_gl_state_value(GL_MAX_TEXTURE_UNITS, "GL_MAX_TEXTURE_UNITS = %d\n");
    print_gl_state_value(GL_MAX_TEXTURE_SIZE,  "GL_MAX_TEXTURE_SIZE  = %d\n");
}

void coverflow_deinit(void)
{
    glDeleteTextures(TEX_NUM, textureIds);
    glDeleteBuffers(VBO_NUM, vboIds);
    itemLength = 0;
}

#endif /* GPU_GLES1 */
