#include <ctime>
#include <vector>
#include <GL/gl.h>
#include <GL/glut.h>
#include "Classes/Model.cpp"

using namespace std;

static GLfloat spin = 0.0;

void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0); //Faz o set da cor de limpeza
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT); //Glclear especifica o que vai ser limpado, nesse caso o buffer de cores
    glPushMatrix();
    glRotatef(spin, 0.0, 1.0, 1.0);

    auto malha = new Model("Modelos/3DModel.obj");

    glColor3d(1.0, 0.0, 0.0); //Vermelho

    malha->draw();
    glPopMatrix();
    glutSwapBuffers();
}

void spinDisplay() {
    spin = spin + 2.0;
    if (spin > 360.0)
        spin = spin - 360.0;

    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void mouse(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
                glutIdleFunc(spinDisplay);
            break;
        case GLUT_MIDDLE_BUTTON:
            if (state == GLUT_DOWN)
                glutIdleFunc(NULL);
            break;
        default:
            break;
    }
}

/*
 *  Request double buffer display mode.
 *  Register mouse input callback functions
 */
int main(int argc, char **argv) {
     glutInit(&argc, argv);
     glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
     glutInitWindowSize (500, 500);
     glutInitWindowPosition (0, 0);
     glutCreateWindow ("Exercicio");
     init();
     glutDisplayFunc(display);
     glutReshapeFunc(reshape);
     glutMouseFunc(mouse);
     glutMainLoop();
     return 0;
}
