
#include "settings.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <unistd.h>
#include <GL/glut.h>

void handleKeyboardArrows(int key, int, int);
void handleLenses(int);
void handleVisibility(int vis);
void handle_reshape(int, int);
void handleStopwatch(int);
void rotate_camera(int, int, int);
void updateCameraOrigin(int);
void normalize_vector(Vertice *p);
void create_view(void);
void createEnvironment(void);
void render_object(void);