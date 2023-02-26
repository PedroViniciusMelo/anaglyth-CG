#include <cstdlib>
#include <cmath>
#include <GL/glut.h>
#include <curses.h>
#include "util/settings.h"
#include "Classes/Model.cpp"

/*
   Anaglyphs implementados em OpenGL
   Adiciona as duas imagens no buffer de acumulação
*/


int tela_cheia = FALSE;
int tipo_btn_mouse = -1;
float velocidade_de_rotacao = 0.5;     /* Velocidade de rotação dos objetos      */
double angulo_de_rotacao = 1.0;         /* Incremento do ângulo de rotação da câmera */
CAMERA camera;
XYZ origem = {0.0, 0.0, 0.0};


int main(int argc,char **argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ACCUM | GLUT_RGB | GLUT_DEPTH);

    //Configurações da tela
    glutCreateWindow("Anaglyph");
    camera.largura_tela = 400;
    camera.altura_tela = 300;
    glutReshapeWindow(camera.largura_tela, camera.altura_tela);
    if (tela_cheia)
        glutFullScreen();

    //Callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(processar_reajuste_janela);
    glutVisibilityFunc(visualizacao_da_janela);
    glutKeyboardFunc(processar_teclado);
    glutSpecialFunc(processar_direcionais);
    glutMouseFunc(processar_mouse);
    glutMotionFunc(processar_movimento_mouse);

    //Cena
    configurar_ambiente();
    camera_origem();

    glutMainLoop();
    return(0);
}


void configurar_ambiente()
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);
    glDisable(GL_DITHER);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0);
    glPointSize(1.0);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glFrontFace(GL_CW);
    glClearColor(0.0,0.0,0.0,0.0);
    glClearAccum(0.0,0.0,0.0,0.0);   /* The default */
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
}

void display()
{
    XYZ r;
    double escala,radiandos,metade_distancia,ndfl;
    double esquerda,direita,cima,inferior,proximidade,distancia=10000;

    /* Faz recorte da imagem */
    proximidade = camera.distancia_focal / 5;

    /* Cálculo do vetor direito */
    CROSSPROD(camera.vd,camera.vu,r);
    normalizar(&r);
    r.x *= camera.separacao_olhos / 2.0;
    r.y *= camera.separacao_olhos / 2.0;
    r.z *= camera.separacao_olhos / 2.0;

    escala  = camera.largura_tela / (double)camera.altura_tela;
    radiandos = DTOR * camera.abertura / 2;
    metade_distancia     = proximidade * tan(radiandos);
    ndfl    = proximidade / camera.distancia_focal;

    /* Seta o buffer para writing e reading */
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);

    /* Resetando buffer para garantir que o filtro será aplicado corretamente */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_ACCUM_BUFFER_BIT); /* Not strictly necessary */

    glViewport(0, 0, camera.largura_tela, camera.altura_tela);

    /* Filtro do olho esquerdo */
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);

    /* Cálculos da matriz de projeção */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    esquerda  = - escala * metade_distancia + 0.5 * camera.separacao_olhos * ndfl;
    direita = escala * metade_distancia + 0.5 * camera.separacao_olhos * ndfl;
    cima    =   metade_distancia;
    inferior = - metade_distancia;
    glFrustum(esquerda, direita, inferior, cima, proximidade, distancia);

    /* Renderiza o modelo */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.vp.x - r.x,camera.vp.y - r.y,camera.vp.z - r.z,
              camera.vp.x - r.x + camera.vd.x,
              camera.vp.y - r.y + camera.vd.y,
              camera.vp.z - r.z + camera.vd.z,
              camera.vu.x,camera.vu.y,camera.vu.z);
    renderizar_cena();
    glFlush();
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

    /* Adiciona ao buffer de acumulação */
    glAccum(GL_LOAD,1.0);

    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Cálculos da matriz de projeção */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    esquerda  = - escala * metade_distancia - 0.5 * camera.separacao_olhos * ndfl;
    direita = escala * metade_distancia - 0.5 * camera.separacao_olhos * ndfl;
    cima    =   metade_distancia;
    inferior = - metade_distancia;
    glFrustum(esquerda, direita, inferior, cima, proximidade, distancia);

    /* Filtro do olho direito */
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    glColorMask(GL_FALSE,GL_FALSE,GL_TRUE,GL_TRUE);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.vp.x + r.x,camera.vp.y + r.y,camera.vp.z + r.z,
              camera.vp.x + r.x + camera.vd.x,
              camera.vp.y + r.y + camera.vd.y,
              camera.vp.z + r.z + camera.vd.z,
              camera.vu.x,camera.vu.y,camera.vu.z);
    renderizar_cena();
    glFlush();
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

    /* Adiciona ao buffer de acumulação e retorna a imagem esperada */
    glAccum(GL_ACCUM,1.0);
    glAccum(GL_RETURN,1.0);

    glutSwapBuffers();
}

/*
   Renderiza a cena escolhida
*/

auto malha = new Model("Modelos/Bone.obj");
void renderizar_cena()
{
    static double angulo_de_rotacao = 0.0;

    glPushMatrix();
    glRotatef((float) angulo_de_rotacao, 0.0, 1.0, 0.0);
    glScalef(5, 5, 5);
    malha->draw();
    glPopMatrix();

    angulo_de_rotacao += velocidade_de_rotacao;
    iluminacao();
}

/*
   Configuração da iluminação
*/
void iluminacao()
{
    glEnable(GL_LIGHT0);

    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
}

/*
   Processa as entradas de teclado
*/
void processar_teclado(unsigned char key, int x, int y)
{
    switch (key) {
        case ESC:                            /* Sair */
        case 'Q':
        case 'q':
            exit(0);
        case 'h':                           /* Recentralizar câmera     */
        case 'H':
            camera_origem();
            break;
        case 'i':                           /* Mover camera para cima */
        case 'I':
            translatar_camera(0, 1);
            break;
        case 'k':                           /* Mover camera para baixo */
        case 'K':
            translatar_camera(0, -1);
            break;
        case 'j':                           /* Mover camera para esquerda */
        case 'J':
            translatar_camera(-1, 0);
            break;
        case 'l':                           /* Mover camera para direita */
        case 'L':
            translatar_camera(1, 0);
            break;
        default:
            break;
    }
}

/*
   Processa entrada de direcionais, gira a câmera
*/
void processar_direcionais(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_LEFT:
            girar_camera(-1, 0, 0);
            break;
        case GLUT_KEY_RIGHT:
            girar_camera(1, 0, 0);
            break;
        case GLUT_KEY_UP:
            girar_camera(0, 1, 0);
            break;
        case GLUT_KEY_DOWN:
            girar_camera(0, -1, 0);
            break;
        default:
            break;
    }
}

/*
   Girar em (ix,iy) ou girar em (iz) a câmera sobre o ponto foco.
   ix,iy,iz são sinalizadores, 0 não faz nada, +- 1 gira em direções opostas
*/
void girar_camera(int ix, int iy, int iz)
{
    XYZ vp,vu,vd;
    XYZ right;
    XYZ newvp,newr;
    double radius,dd,radians;
    double dx,dy,dz;

    vu = camera.vu;
    normalizar(&vu);
    vp = camera.vp;
    vd = camera.vd;
    normalizar(&vd);
    CROSSPROD(vd,vu,right);
    normalizar(&right);
    radians = angulo_de_rotacao * PI / 180.0;

    /* Faz a rolagem */
    if (iz != 0) {
        camera.vu.x += iz * right.x * radians;
        camera.vu.y += iz * right.y * radians;
        camera.vu.z += iz * right.z * radians;
        normalizar(&camera.vu);
        return;
    }

    /* Cálculo da distância do ponto de rotação */
    dx = camera.vp.x - camera.pr.x;
    dy = camera.vp.y - camera.pr.y;
    dz = camera.vp.z - camera.pr.z;
    radius = sqrt(dx*dx + dy*dy + dz*dz);

    /* DEtermina o novo ponto de vista */
    dd = radius * radians;
    newvp.x = vp.x + dd * ix * right.x + dd * iy * vu.x - camera.pr.x;
    newvp.y = vp.y + dd * ix * right.y + dd * iy * vu.y - camera.pr.y;
    newvp.z = vp.z + dd * ix * right.z + dd * iy * vu.z - camera.pr.z;
    normalizar(&newvp);
    camera.vp.x = camera.pr.x + radius * newvp.x;
    camera.vp.y = camera.pr.y + radius * newvp.y;
    camera.vp.z = camera.pr.z + radius * newvp.z;

    /* Determina o novo vetor direito */
    newr.x = camera.vp.x + right.x - camera.pr.x;
    newr.y = camera.vp.y + right.y - camera.pr.y;
    newr.z = camera.vp.z + right.z - camera.pr.z;
    normalizar(&newr);
    newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
    newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
    newr.z = camera.pr.z + radius * newr.z - camera.vp.z;

    camera.vd.x = camera.pr.x - camera.vp.x;
    camera.vd.y = camera.pr.y - camera.vp.y;
    camera.vd.z = camera.pr.z - camera.vp.z;
    normalizar(&camera.vd);

    /* Determine o novo vetor up */
    CROSSPROD(newr,camera.vd,camera.vu);
    normalizar(&camera.vu);
}

/*
   Translata o ponto de visão da câmera
   Em resposta às teclas i,j,k,l
   Move também o local de rotação da câmera
*/
void translatar_camera(int ix, int iy)
{
    XYZ vu,vd;
    XYZ right;

    double delta;

    vu = camera.vu;
    normalizar(&vu);
    vd = camera.vd;
    normalizar(&vd);
    CROSSPROD(vd,vu,right);
    normalizar(&right);
    delta = angulo_de_rotacao * camera.distancia_focal / 90.0;

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

/*
   Determina o tipo de botão pressionado do mouse
*/
void processar_mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            tipo_btn_mouse = GLUT_LEFT_BUTTON;
        } else if (button == GLUT_MIDDLE_BUTTON) {
            tipo_btn_mouse = GLUT_MIDDLE_BUTTON;
        }
    }
}

/*
   Função geralmente usada para o redimensionamento da janela
*/
void visualizacao_da_janela(int visible)
{
    if (visible == GLUT_VISIBLE)
        timer(0);
    else
        ;
}

/*
   Função geralmente usada para o redimensionamento da janela
*/
void timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(30, timer, 0);
}

/*
   Função de reajuste da janela
*/
void processar_reajuste_janela(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,(GLsizei)w,(GLsizei)h);
    camera.largura_tela = w;
    camera.altura_tela = h;
}


/*
   Reposiciona a câmera para a origem
*/
void camera_origem()
{
    camera.abertura = 60;
    camera.pr = origem;

    camera.vd.x = 1;
    camera.vd.y = 0;
    camera.vd.z = 0;

    camera.vu.x = 0;
    camera.vu.y = 1;
    camera.vu.z = 0;

    camera.vp.x = -10;
    camera.vp.y = 0;
    camera.vp.z = 0;

    camera.distancia_focal = 10;
    camera.separacao_olhos = camera.distancia_focal / 30.0;
}


/*
   Processa o movimento do mouse
*/
void processar_movimento_mouse(int x, int y)
{
    static int xlast=-1,ylast=-1;
    int dx,dy;

    dx = x - xlast;
    dy = y - ylast;
    if (dx < 0)      dx = -1;
    else if (dx > 0) dx =  1;
    if (dy < 0)      dy = -1;
    else if (dy > 0) dy =  1;

    if (tipo_btn_mouse == GLUT_LEFT_BUTTON)
        girar_camera(-dx, dy, 0);
    else if (tipo_btn_mouse == GLUT_MIDDLE_BUTTON)
        girar_camera(0, 0, dx);

    xlast = x;
    ylast = y;
}

/*
   Normaliza um vetor
*/
void normalizar(XYZ *p)
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


