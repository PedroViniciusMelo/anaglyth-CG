#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <GL/glut.h>

#define RED  1
#define CYAN_RED  2
#define LEFT  1
#define RIGHT  2
#define DTOR            0.0174532925
#define PI              3.141592653589793238462643
#define CROSSPROD(p1,p2,p3) \
   p3.x = p1.y*p2.z - p1.z*p2.y; \
   p3.y = p1.z*p2.x - p1.x*p2.z; \
   p3.z = p1.x*p2.y - p1.y*p2.x

typedef struct {
   double x,y,z;
} Vertice;

typedef struct {
   Vertice vp;
   Vertice vd;
   Vertice vu;
   Vertice pr;
   double focal_distance;           
   double camera_opening;           
   double eye_separation; 
   int screen_height, screen_width;
} Camera;



float rotation_speed = 0.5;     
double camera_rotation_angle = 1.0;    
Camera camera;
Vertice origin = {0.0, 0.0, 0.0};
int lens_type= CYAN_RED;


