// the headers

#include <stdlib.h>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

GLfloat delta = 1.5f;
GLint axis = 2;
GLboolean stop = false;

/* vertices of cube about the origin */
GLfloat vertices[8][3] =
    {{-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0},
    {1.0, 1.0, -1.0}, {-1.0, 1.0, -1.0}, {-1.0, -1.0, 1.0},
    {1.0, -1.0, 1.0}, {1.0, 1.0, 1.0}, {-1.0, 1.0, 1.0}};

/* colors to be assigned to vertices */
GLfloat colors[8][3] =
    {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0},
    {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0},
    {1.0, 0.0, 1.0}, {1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}};

// called before main loop
void init() 
{
    // set background color
    glClearColor(0.0, 0.0, 0.0, 0.0);   
    // enable depth buffering
    glEnable(GL_DEPTH_TEST); 
    // interpolate colors during rasterization
    glShadeModel(GL_SMOOTH);            
}

void face(int a, int b, int c, int d)
{
    glBegin(GL_POLYGON);
        glColor3fv(colors[a]);
        glVertex3fv(vertices[a]);
        glColor3fv(colors[b]);
        glVertex3fv(vertices[b]);
        glColor3fv(colors[c]);
        glVertex3fv(vertices[c]);
        glColor3fv(colors[d]);
        glVertex3fv(vertices[d]);
    glEnd(); 
}

void colorcube(void)
{
    face(0,3,2,1);
    face(2,3,7,6);
    face(0,4,7,3);
    face(1,2,6,5);
    face(4,5,6,7);
    face(0,1,5,4);
}

GLfloat theta[3] = {0.0, 0.0, 0.0};

void display(void)
{ 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(theta[0], 1.0, 0.0, 0.0);
    glRotatef(theta[1], 0.0, 1.0, 0.0);
    glRotatef(theta[2], 0.0, 0.0, 1.0);
    colorcube(); 
    glutSwapBuffers(); 
}

void spinCube() {
    /* spin the cube delta degrees about selected axis */
    theta[axis] += delta;
    if (theta[axis] > 360.0) theta[axis] -= 360.0;

    /* display result (do not forget this!) */
    glutPostRedisplay();
}

void reshape(int w, int h)
{
    GLfloat aspect = (GLfloat) w / (GLfloat) h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h) /* aspect <= 1 */
        glOrtho(-2.0, 2.0, -2.0/aspect, 2.0/aspect, -10.0, 10.0);
    else /* aspect > 1 */
        glOrtho(-2.0*aspect, 2.0*aspect, -2.0, 2.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
    if (key=='q' || key == 'Q') 
        exit(0);
    if (key==' ')
      stop = !stop;
    if (stop)
        glutIdleFunc(NULL);
    else
        glutIdleFunc(spinCube);

    if (key=='z') 
      axis = 0; 
    if (key=='x') 
      axis = 1; 
    if (key=='c') 
      axis = 2; 

}

void mouse(int btn, int state, int x, int y)
{
   if ((btn==GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) 
      axis = 0; 
   if ((btn==GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN)) 
      axis = 1; 
   if ((btn==GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) 
      axis = 2;
}

// entry point
int main(int argc, char **argv)
{
    
    // initialize GLUT
    glutInit(&argc, argv);
    
    // request double buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    
    // set window size
    glutInitWindowSize(500, 500);
    
    // set window position
    glutInitWindowPosition(0, 0);
    
    // creates a window
    glutCreateWindow("Ahahaha!");

    // initialize states
    init();

    // GLUT callbacks
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    // idle function for constant feedback
    glutIdleFunc(spinCube);

    // start GLUT program
    glutMainLoop();
    return 0;
}
