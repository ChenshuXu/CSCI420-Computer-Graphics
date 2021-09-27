// the headers

#include <stdlib.h>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>


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

// display a frame
void display()
{
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); // reset transformation

    //glRotatef(45,0,0,1);

    // draw a triangle
    glBegin(GL_TRIANGLES);
	//glColor4f(1.0,0.0,0.0,1.0);
        glVertex3f(0.0, 0.0, -0.9);
	//glColor4f(0.0,1.0,0.0,1.0);
        glVertex3f(1.0, 0.0, -0.9);
	//glColor4f(0.0,0.0,1.0,1.0);
        glVertex3f(0.0, 1.0, -0.9);
    glEnd();

    glutSwapBuffers(); // double buffer flush
}

// called every time window is resized to 
// update projection matrix
void reshape(int w, int h)
{
    // setup image size
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // setup camera
    glFrustum(-0.1, 0.1, 
        -float(h)/(10.0*float(w)), 
        float(h)/(10.0*float(w)), 0.03, 1000.0);
    // gluOrtho2D(-2.0, 2.0, 
    //     -2.0*float(h)/(float(w)), 
    //      2.0*float(h)/(float(w)));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keys(unsigned char key, int x, int y) {
    exit(0);
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
    glutKeyboardFunc(keys);

    // start GLUT program
    glutMainLoop();
    return 0;
}
