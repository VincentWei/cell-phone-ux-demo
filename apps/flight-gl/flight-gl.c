///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
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
    #include "./shader/letmefly.vert.h"
    #include "./shader/letmefly.frag.h"
#endif

static GLuint vs = 0;
static GLuint fs = 0;
static GLuint prog = 0;

static float modelView[16];
static float projection[16];

#define TERRAIN_W       256
#define TERRAIN_H       256
#define TERRAIN_S       65535
#define HEIGHT_MNT      450
#define LENGHT_X_MNT    62
#define LENGHT_Y_MNT    62
#define STEP_X_MNT      96.0
#define STEP_Y_MNT      96.0
static GLfloat terrain_vertex[TERRAIN_W * TERRAIN_H];
static GLfloat terrain_color[TERRAIN_W * TERRAIN_H][3];

static int width    = ACTIVITY_W;
static int height   = ACTIVITY_H;

#define OBS_START_X     992.0
#define OBS_START_Y     103.0

static float obs[3] = { OBS_START_X, HEIGHT_MNT * 1.3, OBS_START_Y};
static float dir[3], v1[2], v2[2];
static float v = 900.0;
static float alpha = 75.0;
static float beta = 90.0;

static float rolling = 0.0;
static float rolling_angle = 0.0;
static float sideslip_angle = 0.0;

#define TSCALE  4
#define FOV     85

static struct timeval T_obs    = {0, 0};
static struct timeval T_frame  = {0, 0};
static GLint frames     = 0;

void letmefly_reshape(int w, int h);
void letmefly_init(void);
void letmefly_run(void);
void letmefly_draw(void);
void letmefly_deinit(void);
void letmefly_rolling(float d_angle);
void letmefly_sideslip(float d_angle);

#if OPENGL_API_USE_ES2
#else
void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
        GLfloat centerx, GLfloat centery, GLfloat centerz,
        GLfloat upx, GLfloat upy, GLfloat upz)
{
    GLfloat m[16];
    GLfloat x[3], y[3], z[3];
    GLfloat mag;

    /* Make rotation matrix */

    /* Z vector */
    z[0] = eyex - centerx;
    z[1] = eyey - centery;
    z[2] = eyez - centerz;
    mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
    if (mag) {   /* mpichler, 19950515 */
        z[0] /= mag;
        z[1] /= mag;
        z[2] /= mag;
    }

    /* Y vector */
    y[0] = upx;
    y[1] = upy;
    y[2] = upz;

    /* X vector = Y cross Z */
    x[0] = y[1] * z[2] - y[2] * z[1];
    x[1] = -y[0] * z[2] + y[2] * z[0];
    x[2] = y[0] * z[1] - y[1] * z[0];

    /* Recompute Y = Z cross X */
    y[0] = z[1] * x[2] - z[2] * x[1];
    y[1] = -z[0] * x[2] + z[2] * x[0];
    y[2] = z[0] * x[1] - z[1] * x[0];

    /* mpichler, 19950515 */
    /* cross product gives area of parallelogram, which is < 1.0 for
     * * non-perpendicular unit-length vectors; so normalize x, y here
     * */

    mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    if (mag) {
        x[0] /= mag;
        x[1] /= mag;
        x[2] /= mag;
    }

    mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
    if (mag) {
        y[0] /= mag;
        y[1] /= mag;
        y[2] /= mag;
    }

#define M(row,col) m[col*4+row]
    M(0, 0) = x[0];
    M(0, 1) = x[1];
    M(0, 2) = x[2];
    M(0, 3) = 0.0;
    M(1, 0) = y[0];
    M(1, 1) = y[1];
    M(1, 2) = y[2];
    M(1, 3) = 0.0;
    M(2, 0) = z[0];
    M(2, 1) = z[1];
    M(2, 2) = z[2];
    M(2, 3) = 0.0;
    M(3, 0) = 0.0;
    M(3, 1) = 0.0;
    M(3, 2) = 0.0;
    M(3, 3) = 1.0;
#undef M
    glMultMatrixf(m);

    /* Translate Eye to Origin */
    glTranslatef(-eyex, -eyey, -eyez);

}

void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
    GLfloat xmin, xmax, ymin, ymax;

    ymax = zNear * tan(fovy * M_PI / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}
#endif

static unsigned int elasped_time(struct timeval *t0)
{
    unsigned int sec, usec, time;
    struct timeval t1;

    gettimeofday(&t1, NULL);

    sec = t1.tv_sec - t0->tv_sec;
    if (t1.tv_usec >= t0->tv_usec) {
        usec = t1.tv_usec - t0->tv_usec;
    } else {
        usec = t1.tv_usec + 1000000 - t0->tv_usec;
        --sec;
    }
    time = sec*1000 + ((double)usec) / 1000;

    t0->tv_sec = t1.tv_sec, t0->tv_usec = t1.tv_usec;

    return time;
}

static void calc_pos_obs(void)
{
    float alpha1, alpha2;
    double dt = elasped_time(&T_obs) / 1000.0;

    alpha += sideslip_angle;
    dir[0] = sin(alpha * M_PI / 180.0);
    dir[2] = cos(alpha * M_PI / 180.0) * sin(beta * M_PI / 180.0);
    dir[1] = cos(beta * M_PI / 180.0);

    if (dir[0] < 1.0e-5 && dir[0] > -1.0e-5)
        dir[0] = 0;
    if (dir[1] < 1.0e-5 && dir[1] > -1.0e-5)
        dir[1] = 0;
    if (dir[2] < 1.0e-5 && dir[2] > -1.0e-5)
        dir[2] = 0;

    alpha1 = alpha + FOV / 2.0;
    v1[0] = sin(alpha1 * M_PI / 180.0);
    v1[1] = cos(alpha1 * M_PI / 180.0);

    alpha2 = alpha - FOV / 2.0;
    v2[0] = sin(alpha2 * M_PI / 180.0);
    v2[1] = cos(alpha2 * M_PI / 180.0);

    obs[0] += v * dir[0] * dt; 
    obs[1] += v * dir[1] * dt; 
    obs[2] += v * dir[2] * dt; 

    if (obs[1] < 0.0)
        obs[1] = 0.0;
}

static int clip_strip(float y, float *s, float *e)
{
    float x1, x2, t1, t2, tmp;

    if (v1[1] == 0.0) {
        t1 = 0.0;
        x1 = -HUGE_VAL;
    }
    else {
        t1 = y / v1[1];
        x1 = t1 * v1[0];
    }

    if (v2[1] == 0.0) {
        t2 = 0.0;
        x2 = HUGE_VAL;
    }
    else {
        t2 = y / v2[1];
        x2 = t2 * v2[0];
    }

    if (((x1 < -(LENGHT_X_MNT * STEP_X_MNT) / 2) && (t2 <= 0.0)) ||
            ((t1 <= 0.0) && (x2 > (LENGHT_X_MNT * STEP_X_MNT) / 2)) ||
            ((t1 < 0.0) && (t2 < 0.0)))
        return 0;

    if ((t1 == 0.0) && (t2 == 0.0)) {
        if ((v1[0] < 0.0) && (v1[1] > 0.0) && (v2[0] < 0.0) && (v2[1] < 0.0)) {
            *s = -(LENGHT_X_MNT * STEP_X_MNT) / 2;
            *e = STEP_X_MNT;
            return 1;
        }
        else {
            if ((v1[0] > 0.0) && (v1[1] < 0.0) && (v2[0] > 0.0) && (v2[1] > 0.0)) {
                *s = -STEP_X_MNT;
                *e = (LENGHT_X_MNT * STEP_X_MNT) / 2;
                return 1;
            }
            else
                return 0;
        }
    }
    else {
        if (t2 < 0.0) {
            if (x1 < 0.0)
                x2 = -(LENGHT_X_MNT * STEP_X_MNT) / 2;
            else
                x2 = (LENGHT_X_MNT * STEP_X_MNT) / 2;
        }

        if (t1 < 0.0) {
            if (x2 < 0.0)
                x1 = -(LENGHT_X_MNT * STEP_X_MNT) / 2;
            else
                x1 = (LENGHT_X_MNT * STEP_X_MNT) / 2;
        }
    }

    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    x1 -= STEP_X_MNT;
    if (x1 < -(LENGHT_X_MNT * STEP_X_MNT) / 2)
        x1 = -(LENGHT_X_MNT * STEP_X_MNT) / 2;

    x2 += STEP_X_MNT;
    if (x2 > (LENGHT_X_MNT * STEP_X_MNT) / 2)
        x2 = (LENGHT_X_MNT * STEP_X_MNT) / 2;

    *s = ((int) (x1 / STEP_X_MNT)) * STEP_X_MNT;
    *e = ((int) (x2 / STEP_X_MNT)) * STEP_X_MNT;

    return 1;
}

static void draw_terrain(void)
{
    static GLfloat vertex[128*3];
    static GLfloat color[128*4];
    static GLfloat texCoord[128*2];
    static float mvp[16];

    int h, i, idx, ox, oy, length;
    float j, k, s, e;
    long mnt;

    calc_pos_obs();
    ox = (int) (obs[0] / STEP_X_MNT);
    oy = (int) (obs[2] / STEP_Y_MNT);
    mnt = ((ox * TSCALE) & 255) + ((oy * TSCALE) & 255) * 256;

#if OPENGL_API_USE_ES2
    matIdentity(porjection);
    matPerspective(porjection,
            50.0, (GLfloat)width / (GLfloat)height,
            LENGHT_X_MNT * STEP_Y_MNT * 0.01,
            LENGHT_X_MNT * STEP_Y_MNT * 0.7);

    matIdentity(modelView);
    matLookAt(modelView,
            obs[0], obs[1], obs[2],
            obs[0] + dir[0], obs[1] + dir[1], obs[2] + dir[2],
            0.0, 1.0, 0.0);

    matTranslate(modelView, (float) ox * STEP_X_MNT, 0, (float) oy * STEP_Y_MNT);
    matMult (mvp, projection, modelView);
#else
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
            50.0, (GLfloat)width / (GLfloat)height,
            LENGHT_X_MNT * STEP_Y_MNT * 0.01,
            LENGHT_X_MNT * STEP_Y_MNT * 0.7);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    rolling+=rolling_angle;
    glRotatef(rolling, 0.0, 0.0, 1.0);
    gluLookAt( obs[0], obs[1], obs[2],
            obs[0] + dir[0], obs[1] + dir[1], obs[2] + dir[2],
            0.0, 1.0, 0.0);
    glTranslatef((float) ox *STEP_X_MNT, 0, (float) oy *STEP_Y_MNT);
#endif

    for (h = 0, k = -(LENGHT_Y_MNT * STEP_Y_MNT) / 2; h < LENGHT_Y_MNT;
            k += STEP_Y_MNT, h++) {
        if (!clip_strip(k, &s, &e))
            continue;

        length = 0;
        for (i = (int) (LENGHT_X_MNT / 2 + s / STEP_X_MNT), j = s; j <= e;
                j += STEP_X_MNT, i++) {

            idx = (i * TSCALE + h * 256 * TSCALE + mnt) & 65535;
            color[length * 4] = terrain_color[idx][0];
            color[length * 4 + 1] = terrain_color[idx][1];
            color[length * 4 + 2] = terrain_color[idx][2];
            color[length * 4 + 3] = 1.0f;
            vertex[length * 3] = j;
            vertex[length * 3 + 1] = terrain_vertex[idx];
            vertex[length * 3 + 2] = k;
            texCoord[length * 2] = (ox + i) / 8.0;
            texCoord[length * 2 + 1] = (ox + h) / 8.0;
            ++length;

            idx = (i * TSCALE + h * 256 * TSCALE + 256 * TSCALE + mnt) & 65535;
            color[length * 4] = terrain_color[idx][0];
            color[length * 4 + 1] = terrain_color[idx][1];
            color[length * 4 + 2] = terrain_color[idx][2];
            color[length * 4 + 3] = 1.0f;
            vertex[length * 3] = j;
            vertex[length * 3 + 1] = terrain_vertex[idx];
            vertex[length * 3 + 2] = k + STEP_Y_MNT;
            texCoord[length * 2] = (ox + i) / 8.0;
            texCoord[length * 2 + 1] = (ox + h + 1) / 8.0;
            ++length;
        }

        if (length > 0) {
#if OPENGL_API_USE_ES2
            glVertexAttribPointer(VERTEX_POS_INDEX, 3, GL_FLOAT, GL_FALSE, 0, vertex);
            glEnableVertexAttribArray(VERTEX_POS_INDEX);
            glVertexAttribPointer(VERTEX_COLOR_INDEX, 4, GL_FLOAT, GL_FALSE, 0, color);
            glEnableVertexAttreibArray(VERTEX_COLOR_INDEX);
            glVertexAttribPointer(VERTEX_TEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, texCoord);
            glEnableVertexAttreibArray(VERTEX_TEX_INDEX);

            glUniformMatrix4fv(glGetUniformLocation(prog, "mvp"), 1, GL_FALSE, mvp);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, length);
#else
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            glVertexPointer(3, GL_FLOAT, 0, vertex);
            glColorPointer(4, GL_FLOAT, 0, color);
            glTexCoordPointer(2, GL_FLOAT, 0, texCoord);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, length);

            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
        }
        //printf("length %d\n", length);
    }
}

static void draw_fighter(void)
{
#define boxW    100
#define boxH    100
    static GLfloat vertex[][2] = {
        {-boxW/4, -boxH/8},
        {-boxW/32, -boxH/16},
        {-boxW/32, boxH/32},
        {boxW/32, boxH/32},
        {boxW/32, -boxH/16},
        {boxW/4, -boxH/8},
    };
    static GLfloat color[][4] = {
        {0.0, 1.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 1.0},
        {0.0, 0.0, 1.0, 1.0},
        {0.0, 0.0, 1.0, 1.0},
        {1.0, 0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
    };

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0.0, width, 0.0, height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(width/2, 50.0, 0.0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertex);
    glColorPointer(4, GL_FLOAT, 0, color);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

static void calc_color(GLfloat h, GLfloat c[3])
{
    GLfloat fact;
    GLfloat color[4][3] = {
        {1.0, 1.0, 1.0},
        {0.0, 0.8, 0.0},
        {1.0, 1.0, 0.3},
        {0.0, 0.0, 0.8}
    };

    h = h * (1.0 / 255.0);

    if (h >= 0.9) {
        c[0] = color[0][0];
        c[1] = color[0][1];
        c[2] = color[0][2];
        return;
    }

    if ((h < 0.9) && (h >= 0.7)) {
        fact = (h - 0.7) * 5.0;
        c[0] = fact * color[0][0] + (1.0 - fact) * color[1][0];
        c[1] = fact * color[0][1] + (1.0 - fact) * color[1][1];
        c[2] = fact * color[0][2] + (1.0 - fact) * color[1][2];
        return;
    }

    if ((h < 0.7) && (h >= 0.6)) {
        fact = (h - 0.6) * 10.0;
        c[0] = fact * color[1][0] + (1.0 - fact) * color[2][0];
        c[1] = fact * color[1][1] + (1.0 - fact) * color[2][1];
        c[2] = fact * color[1][2] + (1.0 - fact) * color[2][2];
        return;
    }

    if ((h < 0.6) && (h >= 0.5)) {
        fact = (h - 0.5) * 10.0;
        c[0] = fact * color[2][0] + (1.0 - fact) * color[3][0];
        c[1] = fact * color[2][1] + (1.0 - fact) * color[3][1];
        c[2] = fact * color[2][2] + (1.0 - fact) * color[3][2];
        return;
    }

    c[0] = color[3][0];
    c[1] = color[3][1];
    c[2] = color[3][2];

}

static void load_terrain(void)
{
    GLubyte buffer[256 * 256], pic[256 * 256];
    FILE *FilePic;
    int i, tmp;
    size_t result;

    if ((FilePic = fopen("./res/flight-gl/terrain.dat", "r")) == NULL) {
        fprintf(stderr, "Error loading terrain.dat\n");
        exit(-1);
    }
    result = fread(buffer, 256 * 256, 1, FilePic);
    assert(result == 1);
    fclose(FilePic);

    for (i = 0; i < (256 * 256); i++) {
        terrain_vertex[i] = (buffer[i] * (HEIGHT_MNT / 255.0f));
        calc_color((GLfloat)buffer[i], terrain_color[i]);
        tmp = (((int) buffer[i]) + 96);
        pic[i] = (tmp > 255) ? 255 : tmp;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameterf(GL_TEXTURE_2D,GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (GLvoid*)(&pic[0]));

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

static void load_shader(void)
{
#if OPENGL_API_USE_ES2
    vs = glCreateShader(GL_VERTEX_SHADER);
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    prog = glCreateProgram();

    glShadeBinary(1, &vs, (GLenum)0, letmeflyVert, sizeof(GLint)*letmeflyVertLength);
    glShadeBinary(1, &fs, (GLenum)0, letmeflyFrag, sizeof(GLint)*letmeflyFragLength);

    glAttachShader(prog, vs);
    glAttachShader(prog, fs);

    glBindAttribLocation(prog, VERTEX_POS_INDEX, "a_position");
    glBindAttribLocation(prog, VERTEX_COLOR_INDEX, "a_color");
    glBindAttribLocation(prog, VERTEX_TEX_INDEX, "a_texCoord");

    glLinkProgram(prog);
    glUseProgram(prog);
    if(glGetError() == GL_INVALID_OPERATION)
        printf("shader error \n");
#endif
}

static void print_frame_rate(void)
{
    GLfloat seconds;
    GLfloat fps;
    struct timeval t;
    GLint dt;
    t.tv_sec = T_frame.tv_sec;
    t.tv_usec = T_frame.tv_usec;
    dt = elasped_time(&t);
    frames++;
    if (dt >= 2000) {
        seconds = dt / 1000.0;
        fps = frames / seconds;
//        printf("Frame rate: %f\n", fps);
        frames = 0;
        T_frame.tv_sec = t.tv_sec;
        T_frame.tv_usec = t.tv_usec;
    }
}

void letmefly_reshape(int w, int h)
{
    width = w;
    height = h;
    glViewport (0, 0, (GLint)width, (GLint)height);
}

void letmefly_init(void)
{
    GLfloat color[] = {0.6, 0.7, 0.7, 1.0};

    glClearColor(color[0], color[1], color[2], color[3]);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
#if OPENGL_API_USE_ES2
#else
    glShadeModel(GL_SMOOTH);
    glEnable(GL_FOG);
    glFogx(GL_FOG_MODE, GL_EXP2);
    glFogfv(GL_FOG_COLOR, color);
    glFogf(GL_FOG_DENSITY, 0.0007);
#endif
    glDisable(GL_BLEND);

    load_terrain();
    load_shader();
}

void letmefly_run(void)
{
    gettimeofday(&T_obs, NULL);
    gettimeofday(&T_frame, NULL);
}

void letmefly_draw(void)
{

#if OPENGL_API_USE_ES2
#else
    glShadeModel(GL_SMOOTH);
    glEnable(GL_FOG);
#endif
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_terrain();
    draw_fighter();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
#if OPENGL_API_USE_ES2
#else
    glDisable(GL_FOG);
    glShadeModel(GL_FLAT);
#endif    

    glFlush();

    print_frame_rate();
}

void letmefly_deinit(void)
{
#if OPENGL_API_USE_ES2
    glDetachShader(prog, vs);
    glDetachShader(prog, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteProgram(prog);
#endif
}

void letmefly_rolling(float d_angle)
{
    rolling_angle = d_angle;
}

void letmefly_sideslip(float d_angle)
{
    sideslip_angle = d_angle;
}

#endif /* GPU_GLES1 */

