#include <GL/glut.h>
#include "util/declaration.h"
#include "Classes/Model.cpp"

auto malha = new Model("Modelos/Bone.obj");


void createEnvironment()
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);
    glDisable(GL_DITHER);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
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

void setup_left(){
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_ACCUM_BUFFER_BIT);
    glViewport(0,0,camera.screen_width,camera.screen_height);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    if(lens_type == RED){
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
    }else if (lens_type == CYAN_RED){
        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);

    }
}

void setup_view(int face, double &left, double &right, double &top, double &bottom, double &near, double &far, double escala_tela, double metade_largura, double ndfl){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(face == LEFT){
        left  = - escala_tela * metade_largura + 0.5 * camera.eye_separation * ndfl;
        right = escala_tela * metade_largura + 0.5 * camera.eye_separation * ndfl;
    }else{
        left  = - escala_tela * metade_largura - 0.5 * camera.eye_separation * ndfl;
        right = escala_tela * metade_largura - 0.5 * camera.eye_separation * ndfl;
    }
    top    =   metade_largura;
    bottom = - metade_largura;
    glFrustum(left, right, bottom, top, near, far);

}

void setup_right(){
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glAccum(GL_LOAD, 1.0);
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    if(lens_type == RED){
        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
    }else if (lens_type == CYAN_RED){
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);

    }
}

void draw_scene(int face, Vertice r){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if(face == LEFT){
        gluLookAt(camera.vp.x - r.x,camera.vp.y - r.y,camera.vp.z - r.z,
                  camera.vp.x - r.x + camera.vd.x,
                  camera.vp.y - r.y + camera.vd.y,
                  camera.vp.z - r.z + camera.vd.z,
                  camera.vu.x,camera.vu.y,camera.vu.z);
    }else{
        gluLookAt(camera.vp.x + r.x,camera.vp.y + r.y,camera.vp.z + r.z,
                  camera.vp.x + r.x + camera.vd.x,
                  camera.vp.y + r.y + camera.vd.y,
                  camera.vp.z + r.z + camera.vd.z,
                  camera.vu.x,camera.vu.y,camera.vu.z);
    }

    render_object();
    glFlush();
}


void  create_view()
{
    Vertice r;
    double ndfl;
    double left, right, top, bottom, far = 10000;
    double escala_tela  = camera.screen_width / (double)camera.screen_height;
    double abertura_camera = DTOR * camera.camera_opening / 2;
    double proximidade = camera.focal_distance / 5;
    double metade_largura = proximidade * tan(abertura_camera);

    CROSSPROD(camera.vd,camera.vu,r);
    normalize_vector(&r);
    r.x *= camera.eye_separation / 2.0;
    r.y *= camera.eye_separation / 2.0;
    r.z *= camera.eye_separation / 2.0;
    ndfl    = proximidade / camera.focal_distance;

    setup_left();
    setup_view(LEFT, left, right, top, bottom, proximidade, far, escala_tela, metade_largura, ndfl);
    draw_scene(LEFT, r);

    setup_right();
    setup_view(RIGHT, left, right, top, bottom, proximidade, far, escala_tela, metade_largura, ndfl);
    draw_scene(RIGHT, r);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glAccum(GL_ACCUM,1.0);
    glAccum(GL_RETURN,1.0);

    glutSwapBuffers();
}

void render_object()
{
    static double rotation_angle = 0.0f;

    glPushMatrix();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPushMatrix();
    glRotatef((float) rotation_angle, 0.0, 1.0, 0.0);
    glPushMatrix();
    glRotatef(0.0,0.0,0.0,1.0);
    glScalef(5, 5, 5);
    malha->draw();
    rotation_angle += rotation_speed;

    glPopMatrix();
}


void handleKeyboardArrows(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_LEFT:
            rotate_camera(-1, 0, 0);
            break;
        case GLUT_KEY_RIGHT:
            rotate_camera(1, 0, 0);
            break;
        case GLUT_KEY_UP:
            rotate_camera(0, 1, 0);
            break;
        case GLUT_KEY_DOWN:
            rotate_camera(0, -1, 0);
            break;
        default:
            break;
    }
}

void rotate_camera(int ix, int iy, int iz)
{
    Vertice vp,vu,vd;
    Vertice right;
    Vertice newvp,newr;
    double radius,dd,radians;
    double dx,dy,dz;

    vu = camera.vu;
    normalize_vector(&vu);
    vp = camera.vp;
    vd = camera.vd;
    normalize_vector(&vd);
    CROSSPROD(vd,vu,right);
    normalize_vector(&right);
    radians = camera_rotation_angle * PI / 180.0;
    if (iz != 0) {
        camera.vu.x += iz * right.x * radians;
        camera.vu.y += iz * right.y * radians;
        camera.vu.z += iz * right.z * radians;
        normalize_vector(&camera.vu);
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
    normalize_vector(&newvp);
    camera.vp.x = camera.pr.x + radius * newvp.x;
    camera.vp.y = camera.pr.y + radius * newvp.y;
    camera.vp.z = camera.pr.z + radius * newvp.z;
    newr.x = camera.vp.x + right.x - camera.pr.x;
    newr.y = camera.vp.y + right.y - camera.pr.y;
    newr.z = camera.vp.z + right.z - camera.pr.z;
    normalize_vector(&newr);
    newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
    newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
    newr.z = camera.pr.z + radius * newr.z - camera.vp.z;
    camera.vd.x = camera.pr.x - camera.vp.x;
    camera.vd.y = camera.pr.y - camera.vp.y;
    camera.vd.z = camera.pr.z - camera.vp.z;
    normalize_vector(&camera.vd);
    CROSSPROD(newr,camera.vd,camera.vu);
    normalize_vector(&camera.vu);
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

void handle_reshape(int w, int h)
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
        default:
            break;
    }

    camera.eye_separation = camera.focal_distance / 30.0;
    if (mode == 4)
        camera.eye_separation = 0;
}

void normalize_vector(Vertice *p)
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
    int main_lenss;

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ACCUM | GLUT_RGB | GLUT_DEPTH);

    glutCreateWindow("Anaglyphs Cube 3D");
    camera.screen_width = 400;
    camera.screen_height = 300;
    glutReshapeWindow(camera.screen_width, camera.screen_height);

    glutDisplayFunc(create_view);
    glutReshapeFunc(handle_reshape);
    glutVisibilityFunc( handleVisibility);
    glutSpecialFunc(  handleKeyboardArrows);

    createEnvironment();
    updateCameraOrigin(0);

    main_lenss = glutCreateMenu(  handleLenses);
    glutAddMenuEntry("RED and Cyan", RED);
    glutAddMenuEntry("Cyan and Red", CYAN_RED);

    glutAddSubMenu("Lens Type", main_lenss);
    glutAddMenuEntry("OUT", 9);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutMainLoop();
    return(0);
}