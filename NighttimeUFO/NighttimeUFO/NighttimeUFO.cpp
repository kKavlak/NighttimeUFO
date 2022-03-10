#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 500

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

bool timer = 1; // timer flag

bool clickControl = 1, drawf = 1; // flags for fire rate and clicks

int click = 0; // counter for clicks

double realAngle; // determining the angle between UFO and Rocket

#define ENEMY_SPEED 3
#define PLAYER_SPEED 10

typedef struct {
    vec_t pos;
    vec_t vel;
    double angle;
} rocket_t;

rocket_t R = { { 0, 0 }, 45 };
// structure for Rocket 

typedef struct {
    float r, g, b;
} color_t;
// structure for Color

typedef struct {
    vec_t   pos;
    color_t color;
    vec_t   vel;
} ufo_t;
// structure for UFO

typedef struct {
    vec_t pos;
    vec_t N;
} vertex_t;
// vertex structure

ufo_t ufo[1] =
{
   { {-300, 130 }, { 1, 1, 0 }, { 6,  0} }
};
// array for UFOs (there is only one UFO) 

color_t mulColor(float k, color_t c) {
    color_t tmp = { k * c.r, k * c.g, k * c.b };
    return tmp;
}

color_t addColor(color_t c1, color_t c2) {
    color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
    return tmp;
}
// functions for color & lighting

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.

void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

double distanceImpact(double d) {
    return (-1.0 / 350.0) * d + 1.0;
}


color_t calculateColor(ufo_t source, vertex_t v) {
    vec_t L = subV(source.pos, v.pos);
    vec_t uL = unitV(L);
    float factor = dotP(uL, v.N) * distanceImpact(magV(L));
    return mulColor(factor, source.color);
}


void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}


void drawUFO(int x, int y)
// Function for drawing the Linear moving UFO
{
    glColor3f(0.5, 0.5, 0.5); // Gray color
    glBegin(GL_POLYGON);
    glVertex2f(x - 18, y + 27);
    glVertex2f(x + 18, y + 27);
    glVertex2f(x + 63, y);
    glVertex2f(x + 27, y - 9);
    glVertex2f(x + 18, y - 27);
    glVertex2f(x - 18, y - 27);
    glVertex2f(x - 27, y - 9);
    glVertex2f(x - 63, y);
    glEnd();

    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - 18, y + 27);
    glVertex2f(x + 18, y + 27);
    glVertex2f(x + 27, y + 9);
    glVertex2f(x + 63, y);
    glVertex2f(x + 27, y - 9);
    glVertex2f(x + 18, y - 27);
    glVertex2f(x - 18, y - 27);
    glVertex2f(x - 27, y - 9);
    glVertex2f(x - 63, y);
    glVertex2f(x - 27, y + 9);
    glEnd();

    glColor4f(1, 1, 0, 0.5);
    glBegin(GL_POLYGON);
    glVertex2f(x - 18, y - 27);
    glVertex2f(x - 90, y - 600);
    glVertex2f(x + 90, y - 600);
    glVertex2f(x + 18, y - 27);
    glEnd();
    // Drawing the UFO

}

void drawRocket(rocket_t R)
{ // function for drawing player object (rocket)
    int x, y;
    float c, s;

    x = R.pos.x;
    y = R.pos.y;
    c = cos(R.angle * D2R);
    s = sin(R.angle * D2R);

    glColor3f(1, 1, 1); // White color
    glLineWidth(6);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x + 50 * s, y + 50 * c);
    glEnd();
    glLineWidth(1);
}

//
// To display onto window using OpenGL commands
//
void display() {
    //
    // clear window to black
    //
    glClearColor(0, 0, 0.1, 0); // Dark night sky
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(ufo[0].color.r, ufo[0].color.g, ufo[0].color.b);
    drawUFO(ufo[0].pos.x, ufo[0].pos.y);
    //Drawing the UFO

    for (int x = -400; x <= 400; x++) {
        // for loop to make lighting effects
        vertex_t P = { { x, -100 }, { 0, 1 } };

        color_t res = { 0, 0, 0 };
        res = addColor(res, calculateColor(ufo[0], P));
        // Form different colors for different lighting conditions
        glBegin(GL_LINES);
        glColor3f(res.r, res.g, res.b);
        glVertex2f(x, -100 + x / 10 - 60);
        // An increment is made to -100 to form a slope
        glColor3f(0.1, 0, 0);
        // Earth plane has a color gradient (reddish brown)
        glVertex2f(x, -250);
        glEnd();
    }

    glColor3f(1, 1, 1);
    circle(-winWidth / 2 + 50, winHeight / 2 - 50, 10);
    glColor3f(0, 0, 0.1);
    circle(-winWidth / 2 + 55, winHeight / 2 - 47, 6);
    // Moon in the background

    if (click > 0 && drawf == 1)
        drawRocket(R);
    // draw player object when clicked

    glutSwapBuffers();
}

//
// key function for ASCII characters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    case GLUT_KEY_F2: timer = !timer; // F2 key pauses the animation
        break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.
    if (button == GLUT_LEFT_BUTTON && stat == GLUT_UP)
    {
        click++; // click counter incremented
        if (clickControl == 1) {
            // if firing is available (only one player object allowed on screen)
            R.pos.x = x - winWidth / 2;
            R.pos.y = winHeight / 2 - y;
            // determine where the player object is going to be
            drawf = 1;
            // draw the object
            clickControl = 0;
            // firing is unavailable until the first object disappears 
        }
    }
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here

    if (timer == 1) {
        // when the timer is active
        for (int i = 0; i < 1; i++)
            ufo[i].pos = addV(ufo[i].pos, ufo[i].vel);
        // UFO movement

        if (ufo->pos.x >= winWidth)
            ufo->pos.x = -600; // UFO comes out the other side of the screen


        R.vel = mulV(PLAYER_SPEED, unitV(subV(ufo->pos, R.pos)));
        R.pos = addV(R.pos, R.vel);
        // Player object chases the UFO

        realAngle = angleBetween2V(ufo->pos, R.pos);
        // Determine the angle between player object and UFO
        if (R.angle < realAngle)
        {
            if (R.pos.x > ufo->pos.x)
                R.angle--;
            else
                R.angle++;
            // Change the angle and direction of player object
        }

        if (R.pos.y >= 120 && fabs(R.pos.x - ufo->pos.x) < 63)
        {
            drawf = 0;
            R.angle = 0;
            // Player object disappears when it touches the UFO
        }

        if (R.pos.x > winWidth / 2)
        {
            drawf = 0;
            R.angle = 0;
            // Player object disappears when it exits the screen
        }

        if (drawf == 0)
            clickControl = 1;
        // Firing is available again after the player object has disappeared 
    }

    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}

#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("Nighttime UFO");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}