#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <GL/glut.h>

typedef struct {
    double x,y,z;
} XYZ;

#define CROSSPROD(p1,p2,p3) \
   p3.x = p1.y*p2.z - p1.z*p2.y; \
   p3.y = p1.z*p2.x - p1.x*p2.z; \
   p3.z = p1.x*p2.y - p1.y*p2.x

typedef struct {
    XYZ vp;                      /* Vetor posição*/
    XYZ vd;                      /* Vetor direção*/
    XYZ vu;                      /* Vetor up*/
    XYZ pr;                      /* Ponto de rotação*/
    double distancia_focal;      /* Distância focal entre vd*/
    double abertura;             /* Camera abertura */
    double separacao_olhos;      /* Separação dos olhos*/
    int altura_tela,largura_tela;
} CAMERA;

void display();
void configurar_ambiente();
void renderizar_cena();
void iluminacao();
void processar_teclado(unsigned char key, int x, int y);
void processar_direcionais(int key, int x, int y);
void processar_mouse(int, int, int, int);
void visualizacao_da_janela(int vis);
void processar_reajuste_janela(int, int);
void processar_movimento_mouse(int, int);
void timer(int);
void girar_camera(int, int, int);
void translatar_camera(int, int);
void camera_origem();
void normalizar(XYZ *p);

#define DTOR            0.0174532925
#define PI              3.141592653589793238462643
#define ESC 27