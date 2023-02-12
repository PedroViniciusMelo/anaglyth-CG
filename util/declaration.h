
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <GL/glut.h>

void handleKeyboard(unsigned char key,int x, int y);
void handleKeyboardArrows(int key,int x, int y);
void handleMain(int);
void handleLenses(int);
void handleVisibility(int vis);
void handleRemodel(int,int);
void handleStopwatch(int);
void rotateCamera(int,int,int);
void translateCamera(int,int);
void updateCameraOrigin(int);
void normalizePoints(XYZ *);
void createView(void);
void createEnvironment(void);
void createWorld(void);
void renderObject(void);