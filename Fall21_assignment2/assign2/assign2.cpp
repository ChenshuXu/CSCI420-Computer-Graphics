/*
  CSCI 420
  Assignment 2
  Chenshu Xu
 */

#include <stdio.h>
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "pic.h"

#include <vector>
#include <cmath>


/* represents one control point along the spline */
struct point {
    double x;
    double y;
    double z;
};

/* spline struct which contains how many control points, and an array of control points */
struct spline {
    int numControlPoints;
    struct point *points;
};

/* the spline array */
struct spline *g_Splines;

/* total number of splines */
int g_iNumOfSplines;

/*  */
std::vector<point> allPoints;


/*
 * read points from
 */
int loadSplines(char *argv) {
    char *cName = (char *) malloc(128 * sizeof(char));
    FILE *fileList;
    FILE *fileSpline;
    int iType, i = 0, j, iLength;


    /* load the track file */
    fileList = fopen(argv, "r");
    if (fileList == NULL) {
        printf("can't open file\n");
        exit(1);
    }

    /* stores the number of splines in a global variable */
    fscanf(fileList, "%d", &g_iNumOfSplines);

    g_Splines = (struct spline *) malloc(g_iNumOfSplines * sizeof(struct spline));

    /* reads through the spline files */
    for (j = 0; j < g_iNumOfSplines; j++) {
        i = 0;
        fscanf(fileList, "%s", cName);
        fileSpline = fopen(cName, "r");

        if (fileSpline == NULL) {
            printf("can't open file\n");
            exit(1);
        }

        /* gets length for spline file */
        fscanf(fileSpline, "%d %d", &iLength, &iType);

        /* allocate memory for all the points */
        g_Splines[j].points = (struct point *) malloc(iLength * sizeof(struct point));
        g_Splines[j].numControlPoints = iLength;

        /* saves the data to the struct */
        while (fscanf(fileSpline, "%lf %lf %lf",
                      &g_Splines[j].points[i].x,
                      &g_Splines[j].points[i].y,
                      &g_Splines[j].points[i].z) != EOF) {
            i++;
        }
    }

    free(cName);

    return 0;
}

void createSplines() {
    for (int i = 0; i < g_iNumOfSplines; i++) { //create proper # of splines
        for (int j = 0; j < g_Splines[i].numControlPoints - 3; j++) {
            point a = g_Splines[i].points[j];
            point b = g_Splines[i].points[j + 1];
            point c = g_Splines[i].points[j + 2];
            point d = g_Splines[i].points[j + 3];
            /*q(t) = 0.5 *((2 * P1) + -P0 + P2) * t + (2*P0 - 5*P1 + 4*P2 - P3) * t2 + (-P0 + 3*P1- 3*P2 + P3) * t3) (From MVPS.org)*/
            for (double u = 0; u < 1.0; u += 0.005) { //create points at t(0) through t(1)
                //calculate x,y,z components
                point current;
                current.x = 0.5 * ((2 * b.x) + (-a.x + c.x) * u + (2 * a.x - 5 * b.x + 4 * c.x - d.x) * (pow(u,2)) + (-a.x + 3 * b.x - 3 * c.x + d.x) * (pow(u,3)));
                current.y = 0.5 * ((2 * b.y) + (-a.y + c.y) * u + (2 * a.y - 5 * b.y + 4 * c.y - d.y) * (pow(u, 2)) + (-a.y + 3 * b.y - 3 * c.y + d.y) * (pow(u, 3)));
                current.z = 0.5 * ((2 * b.z) + (-a.z + c.z) * u + (2 * a.z - 5 * b.z + 4 * c.z - d.z) * (pow(u, 2)) + (-a.z + 3 * b.z - 3 * c.z + d.z) * (pow(u, 3)));
                //add point to vector of all points
                allPoints.push_back(current);
            }
        }
    }
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: %s <trackfile>\n", argv[0]);
        exit(0);
    }

    // argv[1] should be track.txt file
    loadSplines(argv[1]);
    // create splines
    return 0;
}
