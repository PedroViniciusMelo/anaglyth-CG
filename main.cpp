#include <GL/glut.h>
#include "util/declaration.h"
#include "Classes/Model.cpp"

auto malha = new Model("Modelos/Bone.obj");


void createEnvironment(void)
{
    int num[2];

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);
    glDisable(GL_DITHER);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0);
    glPointSize(1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glFrontFace(GL_CW);
    glClearColor(0.0,0.0,0.0,0.0);
    glClearAccum(0.0,0.0,0.0,0.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}


void  createView(void)
{
    int i,j;
    XYZ r;
    double dist, ratio, radians, scale, wd2, ndfl;
    double left, right, top, bottom, near = 0.1, far = 10000;

    near = camera.focal_distance / 5;
    CROSSPROD(camera.vd,camera.vu,r);
    normalizePoints(&r);
    r.x *= camera.eye_separation / 2.0;
    r.y *= camera.eye_separation / 2.0;
    r.z *= camera.eye_separation / 2.0;
    ratio  = camera.screen_width / (double)camera.screen_height;
    radians = DTOR * camera.camera_opening / 2;
    wd2     = near * tan(radians);
    ndfl    = near / camera.focal_distance;
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_ACCUM_BUFFER_BIT);
    glViewport(0,0,camera.screen_width,camera.screen_height);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    switch (lens_type) {
        case RED:
            glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
            break;
        case CYAN_RED:
            glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
            break;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    left  = - ratio * wd2 + 0.5 * camera.eye_separation * ndfl;
    right =   ratio * wd2 + 0.5 * camera.eye_separation * ndfl;
    top    =   wd2;
    bottom = - wd2;
    glFrustum(left, right, bottom, top, near, far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.vp.x - r.x,camera.vp.y - r.y,camera.vp.z - r.z,
              camera.vp.x - r.x + camera.vd.x,
              camera.vp.y - r.y + camera.vd.y,
              camera.vp.z - r.z + camera.vd.z,
              camera.vu.x,camera.vu.y,camera.vu.z);
    createWorld();
    glFlush();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glAccum(GL_LOAD, 1.0);
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    left  = - ratio * wd2 - 0.5 * camera.eye_separation * ndfl;
    right =   ratio * wd2 - 0.5 * camera.eye_separation * ndfl;
    top    =   wd2;
    bottom = - wd2;
    glFrustum(left, right, bottom, top, near, far);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    switch (lens_type) {
        case RED:
            glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
            break;
        case CYAN_RED:
            glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
            break;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.vp.x + r.x,camera.vp.y + r.y,camera.vp.z + r.z,
              camera.vp.x + r.x + camera.vd.x,
              camera.vp.y + r.y + camera.vd.y,
              camera.vp.z + r.z + camera.vd.z,
              camera.vu.x,camera.vu.y,camera.vu.z);
    createWorld();
    glFlush();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glAccum(GL_ACCUM,1.0);
    glAccum(GL_RETURN,1.0);

    glutSwapBuffers();
}
void createWorld(void)
{
    static double rotation_angle = 0.0;

    glPushMatrix();
    glRotatef(rotation_angle, 0.0, 1.0, 0.0);
    renderObject();
    glPopMatrix();
}


void renderObject(void)
{
    static double rotation_angle = 0.0;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPushMatrix();
    glRotatef(rotation_angle, 0.0, 1.0, 0.0);
    glPushMatrix();
    glRotatef(0.0,0.0,0.0,1.0);
    glScalef(5, 5, 5);
    malha->draw();
    glPopMatrix();
    glPopMatrix();
    rotation_angle += rotation_speed;
}

void  handleKeyboard(unsigned char key, int x, int y)
{
    switch (key) {
        /* Move camera up */
        case 'd':
        case 'D':
            translateCamera(0,1);
            break;
            /* Move camera down */
        case 'u':
        case 'U':
            translateCamera(0,-1);
            break;
            /* Move camera left */
        case 'l':
        case 'L':
            translateCamera(-1,0);
            break;
            /* Move camera right */
        case 'r':
        case 'R':
            translateCamera(1,0);
            break;
            /* Get out */
        case ESC:
        case 'Q':
        case 'q':
            exit(0);
            break;
            /* Rotate counterclockwise */
        case '[':
            rotateCamera(0,0,-1);
            break;
            /* Rotate clockwise */
        case ']':
            rotateCamera(0,0,1);
            break;
    }
}


void handleKeyboardArrows(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_LEFT:
            rotateCamera(-1, 0, 0);
            break;
        case GLUT_KEY_RIGHT:
            rotateCamera(1, 0, 0);
            break;
        case GLUT_KEY_UP:
            rotateCamera(0, 1, 0);
            break;
        case GLUT_KEY_DOWN:
            rotateCamera(0, -1, 0);
            break;
    }
}

void rotateCamera(int ix,int iy,int iz)
{
    XYZ vp,vu,vd;
    XYZ right;
    XYZ newvp,newr;
    double radius,dd,radians;
    double dx,dy,dz;

    vu = camera.vu;
    normalizePoints(&vu);
    vp = camera.vp;
    vd = camera.vd;
    normalizePoints(&vd);
    CROSSPROD(vd,vu,right);
    normalizePoints(&right);
    radians = camera_rotation_angle * PI / 180.0;
    if (iz != 0) {
        camera.vu.x += iz * right.x * radians;
        camera.vu.y += iz * right.y * radians;
        camera.vu.z += iz * right.z * radians;
        normalizePoints(&camera.vu);
        return;
    }
    dx = camera.vp.x - camera.pr.x;
    dy = camera.vp.y - camera.pr.y;
    dz = camera.vp.z - camera.pr.z;
    radius = sqrt(dx*dx + dy*dy + dz*dz);
    dd = radius * radians;
    newvp.x = vp.x + dd * ix * right.x + dd * iy * vu.x - camera.pr.x;
    newvp.y = vp.y + dd * ix * right.y + dd * iy * vu.y - camera.pr.y;
    newvp.z = vp.z + dd * ix * right.z + dd * iy * vu.z - camera.pr.z;
    normalizePoints(&newvp);
    camera.vp.x = camera.pr.x + radius * newvp.x;
    camera.vp.y = camera.pr.y + radius * newvp.y;
    camera.vp.z = camera.pr.z + radius * newvp.z;
    newr.x = camera.vp.x + right.x - camera.pr.x;
    newr.y = camera.vp.y + right.y - camera.pr.y;
    newr.z = camera.vp.z + right.z - camera.pr.z;
    normalizePoints(&newr);
    newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
    newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
    newr.z = camera.pr.z + radius * newr.z - camera.vp.z;
    camera.vd.x = camera.pr.x - camera.vp.x;
    camera.vd.y = camera.pr.y - camera.vp.y;
    camera.vd.z = camera.pr.z - camera.vp.z;
    normalizePoints(&camera.vd);
    CROSSPROD(newr,camera.vd,camera.vu);
    normalizePoints(&camera.vu);
}


void translateCamera(int ix,int iy)
{
    XYZ vp, vu, vd;
    XYZ right;
    XYZ newvp, newr;
    double radians, delta;

    vu = camera.vu;
    normalizePoints(&vu);
    vp = camera.vp;
    vd = camera.vd;
    normalizePoints(&vd);
    CROSSPROD(vd,vu,right);
    normalizePoints(&right);
    radians = camera_rotation_angle * PI / 180.0;
    delta = camera_rotation_angle * camera.focal_distance / 90.0;

    camera.vp.x += iy * vu.x * delta;
    camera.vp.y += iy * vu.y * delta;
    camera.vp.z += iy * vu.z * delta;
    camera.pr.x += iy * vu.x * delta;
    camera.pr.y += iy * vu.y * delta;
    camera.pr.z += iy * vu.z * delta;

    camera.vp.x += ix * right.x * delta;
    camera.vp.y += ix * right.y * delta;
    camera.vp.z += ix * right.z * delta;
    camera.pr.x += ix * right.x * delta;
    camera.pr.y += ix * right.y * delta;
    camera.pr.z += ix * right.z * delta;
}


void handleMain(int whichone)
{
    switch (whichone) {
        case 9:
            exit(0);
            break;
    }
}

void handleLenses(int whichone)
{
    lens_type= whichone;
}

void  handleVisibility(int visible)
{
    if (visible == GLUT_VISIBLE)
        handleStopwatch(0);
    else
        ;
}

void  handleStopwatch(int value)
{
    glutPostRedisplay();
    glutTimerFunc(30,  handleStopwatch, 0);
}

void handleRemodel(int w,int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,(GLsizei)w,(GLsizei)h);
    camera.screen_width = w;
    camera.screen_height = h;
}

void updateCameraOrigin(int mode)
{
    camera.camera_opening = 60;
    camera.pr = origin;

    camera.vd.x = 1;
    camera.vd.y = 0;
    camera.vd.z = 0;

    camera.vu.x = 0;
    camera.vu.y = 1;
    camera.vu.z = 0;

    camera.vp.x = -10;
    camera.vp.y = 0;
    camera.vp.z = 0;

    switch (mode) {
        case 0:
        case 2:
        case 4:
            camera.focal_distance = 10;
            break;
        case 1:
            camera.focal_distance = 5;
            break;
        case 3:
            camera.focal_distance = 15;
            break;
    }

    camera.eye_separation = camera.focal_distance / 30.0;
    if (mode == 4)
        camera.eye_separation = 0;
}

void normalizePoints(XYZ *p)
{
    double length;

    length = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
    if (length != 0) {
        p->x /= length;
        p->y /= length;
        p->z /= length;
    } else {
        p->x = 0;
        p->y = 0;
        p->z = 0;
    }
}

/* Main starts here*/

int main(int argc,char **argv)
{
    int main_var, main_lenss;

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ACCUM | GLUT_RGB | GLUT_DEPTH);

    glutCreateWindow("Anaglyphs Cube 3D");
    camera.screen_width = 400;
    camera.screen_height = 300;
    glutReshapeWindow(camera.screen_width, camera.screen_height);

    glutDisplayFunc( createView);
    glutReshapeFunc(handleRemodel);
    glutVisibilityFunc( handleVisibility);
    glutKeyboardFunc( handleKeyboard);
    glutSpecialFunc(  handleKeyboardArrows);

    createEnvironment();
    updateCameraOrigin(0);

    main_lenss = glutCreateMenu(  handleLenses);
    glutAddMenuEntry("RED and Cyan", RED);
    glutAddMenuEntry("Cyan and Red", CYAN_RED);

    main_var = glutCreateMenu(  handleMain);
    glutAddSubMenu("Lens Type", main_lenss);
    glutAddMenuEntry("OUT", 9);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutMainLoop();
    return(0);
}