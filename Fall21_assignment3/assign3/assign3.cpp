/*
CSCI 420
Assignment 3 Raytracer

Name: <Your name here>
*/

#include <string.h>
#include "pic.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <limits>
using namespace std;


// For Linux
/*
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
 */
// For Mac
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>


#define MAX_TRIANGLES 2000
#define MAX_SPHERES 10
#define MAX_LIGHTS 10

char *filename = 0;

//different display modes
#define MODE_DISPLAY 1
#define MODE_JPEG 2
int mode = MODE_DISPLAY;

//you may want to make these smaller for debugging purposes
#define WIDTH 640
#define HEIGHT 480
#define ASPECT_RATIO 640.0/480.0

//the field of view of the camera
#define fov 60.0
#define PI 3.1415926535
const double EPSILON = 1e-6;
#define ALPHA tan((fov/2.0) * (PI/180.0))

unsigned char buffer[HEIGHT][WIDTH][3];

struct Point
{
    double x;
    double y;
    double z;
    Point() { x = y = z = 0.0f; }
    Point(double x, double y, double z) : x(x), y(y), z(z) {}
    Point add(Point p)
    {
        Point newPoint;
        newPoint.x = x + p.x;
        newPoint.y = y + p.y;
        newPoint.z = z + p.z;
        return newPoint;
    }
    Point operator+ (Point p) {
        return Point(x + p.x, y + p.y, z + p.z);
    }
    Point substract(Point p)
    {
        Point newPoint;
        newPoint.x = x - p.x;
        newPoint.y = y - p.y;
        newPoint.z = z - p.z;
        return newPoint;
    }
    Point operator- (Point p) {
        return Point(x - p.x, y - p.y, z - p.z);
    }
    Point operator- () {
        return Point(-x, -y, -z);
    }
    Point dot(double p)
    {
        Point newPoint;
        newPoint.x = x * p;
        newPoint.y = y * p;
        newPoint.z = z * p;
        return newPoint;
    }
    double dot(Point other)
    {
        return (x * other.x + y * other.y + z * other.z);
    }
    Point operator* (double p)
    {
        return Point(x * p, y * p, z * p);
    }
    double operator* (Point other)
    {
        return (x * other.x + y * other.y + z * other.z);
    }
    Point multi(Point other)
    {
        return Point(x * other.x, y * other.y, z * other.z);
    }
    Point cross(Point p)
    {
        Point s;
        s.x = y * p.z - z * p.y;
        s.y = z * p.x - x * p.z;
        s.z = x * p.y - y * p.x;
        return s;
    }
    Point cross(Point a, Point b)
    {

        double x = a.y * b.z - a.z * b.y;
        double y = a.z * b.x - a.x * b.z;
        double z = a.x * b.y - a.y * b.x;
        return Point(x, y, z);
    }
    Point& inverse()
    {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }
    double get_length()
    {
        return sqrt(x * x + y * y + z * z);
    }
    Point& normalize()
    {
        if (get_length() != 0) {
            x /= get_length();
            y /= get_length();
            z /= get_length();
        }
        return *this;
    }

};

/*
 * Ray class
 */
struct Ray {
    Point origin;
    Point direction;
    Ray(Point origin, Point dir) : origin(origin), direction(dir) {}
    Point get_position(double t)
    {
        return origin + direction * t;
    }
};

struct Vertex {
    Point position;
    Point color_diffuse;
    Point color_specular;
    Point normal;
    double shininess;
};

typedef struct _Triangle {
    struct Vertex v[3];
    Point getValue(int vertex) {
        return v[vertex].position;
    }
} Triangle;

typedef struct _Sphere {
    Point position;
    Point color_diffuse;
    Point color_specular;
    double shininess;
    double radius;
} Sphere;

typedef struct _Light {
    Point position;
    Point color;
} Light;

Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
Point ambient_light;

int num_triangles = 0;
int num_spheres = 0;
int num_lights = 0;

/*
 * convert degree to radius
 */
double convert_rad(double angle) {
    return angle / 180.0 * PI;
}

void plot_pixel_display(int x, int y, unsigned char r, unsigned char g, unsigned char b);

void plot_pixel_jpeg(int x, int y, unsigned char r, unsigned char g, unsigned char b);

void plot_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);

/*
 * test ray-sphere intersection
 * return true if intersects
 */
bool sphere_intersection(Ray ray, double &t, int &index) {
    t = numeric_limits<double>::max();
    index = -1;

    // test all spheres
    // find the closest one
    for (int i = 0; i < num_spheres; i++) {
        Sphere sphere = spheres[i];
        Point center = sphere.position;
        Point d = ray.direction;
        Point o = ray.origin;
        double r = sphere.radius;
        // Point oc = ray.origin - center;
        // a = xd^2 + yd^2 + zd^2 (always be 1, because direction is normalized)
        double a = 1;
        // b = 2(xd(x0-xc) + yd(y0-yc) + zd(z0-zc))
        double b = 2 * (d.x * (o.x - center.x) + d.y * (o.y - center.y) + d.z * (o.z - center.z));
        // double b = 2 * oc.dot(ray.direction);
        // c = (x0-xc)^2 + (y0-yc)^2 + (z0-zc)^2 - r^2
        double c = pow(o.x - center.x, 2) + pow(o.y - center.y, 2) + pow(o.z - center.z, 2) - pow(r, 2);
        // double c = oc.dot(oc) - pow(r, 2);
        
        // in square root part: b^2-4ac
        double in_sqrt = b * b - 4 * a * c;
        if (in_sqrt < 0) {
            continue;
        }

        // two roots
        double t1 = (-b + sqrt(in_sqrt)) / 2.0;
        double t2 = (-b - sqrt(in_sqrt)) / 2.0;
        double _t = min(t1, t2);
        // 0 < _t < t
        if (_t > EPSILON && _t < t) {
            t = _t;
            index = i;
        }
    }
    return index != -1;
}

/*
 * test reay-triangle intersection
 * return true if intersects
 */
bool triangle_intersection(Ray ray, double& t, int& index) {
    t = numeric_limits<double>::max();
    index = -1;

    // test all triangles
    // find the closest one using Möller–Trumbore intersection algorithm
    for (int i = 0; i < num_triangles; i++) {
        Triangle triangle = triangles[i];
        Point edge1 = triangle.v[1].position - triangle.v[0].position;
        Point edge2 = triangle.v[2].position - triangle.v[0].position;
        double a, f, u, v;
        Point h, s, q;
        h = ray.direction.cross(edge2);
        a = edge1.dot(h);
        // a != 0
        if (a > -EPSILON && a < EPSILON) {
            continue;
        }
        f = 1.0 / a;
        s = ray.origin - triangle.v[0].position;
        u = f * s.dot(h);
        if (u < 0.0 || u > 1.0) {
            continue;
        }
        q = s.cross(edge1);
        v = f * ray.direction.dot(q);
        if (v < 0.0 || u + v > 1.0) {
            continue;
        }
        double _t = f * edge2.dot(q);
        if (_t > EPSILON && _t < t) {
            t = _t;
            index = i;
        }
    }
    return index != -1;
}

/*
 * calculate color ratio in a triangle
 */
Point get_barycentric_coordinates(Triangle triangle, Point p) {
    Point a, b, c;
    a = triangle.v[0].position;
    b = triangle.v[1].position;
    c = triangle.v[2].position;
    Point ab = b - a;
    Point ac = c - a;
    double area_abc = ab.cross(ac).get_length() / 2.0;

    Point pa = a - p;
    Point pb = b - p;
    Point pc = c - p;

    double area_pbc = pb.cross(pc).get_length() / 2.0;
    double area_pca = pc.cross(pa).get_length() / 2.0;
    double area_pab = pa.cross(pb).get_length() / 2.0;

    return Point(area_pbc / area_abc, area_pca / area_abc, area_pab / area_abc);
}

/*
 * check if a position can be hit by light
 */
bool hit_by_light(Light light, Point position) {
    Point direction = (light.position - position).normalize();
    Ray ray = Ray(position, direction);
    // r(t) = o + t*d
    // use t to record hit point with objects on ray
    double t_s, t_t; // t for hit sphere and t for hit triangle
    int index_s, index_t; // index for hit sphere and index for hit triangle

    // find closest sephere
    bool hit_sphere = sphere_intersection(ray, t_s, index_s);
    // find closest triangle
    bool hit_triangle = triangle_intersection(ray, t_t, index_t);

    // if hit nothing, the light is not blocked
    if (!hit_sphere && !hit_triangle) {
        return true;
    }

    return false;
}

Point get_reflect_direction(Point in, Point normal) {
    return (in - normal * 2 * normal.dot(in)).normalize();
}

/*
 * calculate phongshading
 */
Point get_phong_shading_color(Vertex hit_point, Light light) {
    Point diffuse, specular;
    Point light_direction = (light.position - hit_point.position).normalize();
    double diff = max(light_direction * hit_point.normal, 0.0);
    diffuse = light.color.multi(hit_point.color_diffuse * diff);

    // camera is at origin
    Point viewDir = (-hit_point.position).normalize();
    Point reflectDir = get_reflect_direction(-light_direction, hit_point.normal);
    double spec = pow(max(viewDir.dot(reflectDir),0.0), hit_point.shininess);
    specular =  light.color.multi(hit_point.color_specular * spec);
    
    return diffuse + specular;
}

void clamp(Point& color) {
    if (color.x > 1.0)
        color.x = 1.0;
    if (color.y > 1.0)
        color.y = 1.0;
    if (color.z > 1.0)
        color.z = 1.0;
}


/*
 * Create ray
 * Origin point is (0,0,0), direction if pixel at (x,y), camera direction is -z
 * Cite: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays
 */
Ray create_ray(int x, int y) {
    double pixel_NDC_x = (x+0.5)/WIDTH;
    double pixel_NDC_y = (y+0.5)/HEIGHT;

    double pixel_screen_x = 2 * pixel_NDC_x -1; //Bohan said to subtract 1 for proper bounds
    double pixel_screen_y = 2 * pixel_NDC_y -1; //Bohan said to subtract 1 for proper bounds

    double pixel_camera_x = pixel_screen_x * ASPECT_RATIO * ALPHA;
    double pixel_camera_y = pixel_screen_y * ALPHA;

    return Ray(Point(0, 0, 0), Point(pixel_camera_x, pixel_camera_y, -1.0).normalize());
}

/*
 * Ray tracing
 * get color at pixel (x, y)
 * returns rgb value
 */
Point ray_tracing(int x, int y) {
    // generate ray
    Ray ray = create_ray(x, y);
    Point color = ambient_light;

    // r(t) = o + t*d
    // use t to record hit point with objects on ray
    double t_s, t_t; // t for hit sphere and t for hit triangle
    int index_s, index_t; // index for hit sphere and index for hit triangle

    // find closest sephere
    bool hit_sphere = sphere_intersection(ray, t_s, index_s);
    // find closest triangle
    bool hit_triangle = triangle_intersection(ray, t_t, index_t);
    // cout << index_s << " " << index_t << endl;
    if (!hit_triangle && !hit_sphere) {
        // return backgroud color
        return color;
    }

    Vertex hit_point;
    // find sphere is closer or triangle is closer
    // closet one has smaller t
    if ((hit_sphere && !hit_triangle) || (hit_sphere && hit_triangle && t_s < t_t)) {
        // hit sphere first
        Point hit_position = ray.get_position(t_s);
        hit_point.position = hit_position;
        hit_point.color_diffuse = spheres[index_s].color_diffuse;
        hit_point.color_specular = spheres[index_s].color_specular;
        hit_point.shininess = spheres[index_s].shininess;
        hit_point.normal = (hit_position - spheres[index_s].position).normalize();
    } else {
        // hit triangle first
        Point hit_position = ray.get_position(t_t);
        // calculate color ratio
        // ratio for a, b, c vertices
        Point ratio = get_barycentric_coordinates(triangles[index_t], hit_position);
        // p = x * a + y * b + z * c
        hit_point.position = hit_position;
        Vertex a, b, c;
        a = triangles[index_t].v[0];
        b = triangles[index_t].v[1];
        c = triangles[index_t].v[2];
        hit_point.color_diffuse = a.color_diffuse * ratio.x + b.color_diffuse * ratio.y + c.color_diffuse * ratio.z;
        hit_point.color_specular = a.color_specular * ratio.x + b.color_specular * ratio.y + c.color_specular * ratio.z;
        hit_point.shininess = a.shininess * ratio.x + b.shininess * ratio.y + c.shininess * ratio.z;
        hit_point.normal = (a.normal * ratio.x + b.normal * ratio.y + c.normal * ratio.z).normalize();
    }

    // for each light source, fire shadow ray
    // for each unblocked shadow ray, evaluate local Phone model for that light,
    // and add the result to pixel color
    color = Point(0, 0, 0);
    for (int i = 0; i < num_lights; i++) {
        if (hit_by_light(lights[i], hit_point.position)) {
            color = color + get_phong_shading_color(hit_point, lights[i]);
        }
    }

    // clamp color to range [0, 1]
    clamp(color);
    return color;
}

//MODIFY THIS FUNCTION
void draw_scene() {
    unsigned int x, y;
    int step = 1;
    //simple output
    for (x = 0; x < WIDTH; x += step) {
        glPointSize(2.0);
        glBegin(GL_POINTS);
        for (y = 0; y < HEIGHT; y += step) {
            // get color at this pixel
            Point color = ray_tracing(x, y);
            plot_pixel(x, y, color.x * 255.0, color.y * 255.0, color.z * 255.0);
        }
        glEnd();
        glFlush();
    }
    printf("Done!\n");
    fflush(stdout);
}

void plot_pixel_display(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    glColor3f(((double) r) / 256.f, ((double) g) / 256.f, ((double) b) / 256.f);
    glVertex2i(x, y);
}

void plot_pixel_jpeg(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    buffer[HEIGHT - y - 1][x][0] = r;
    buffer[HEIGHT - y - 1][x][1] = g;
    buffer[HEIGHT - y - 1][x][2] = b;
}

void plot_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    plot_pixel_display(x, y, r, g, b);
    if (mode == MODE_JPEG)
        plot_pixel_jpeg(x, y, r, g, b);
}

void save_jpg() {
    Pic *in = NULL;

    in = pic_alloc(640, 480, 3, NULL);
    printf("Saving JPEG file: %s\n", filename);

    memcpy(in->pix, buffer, 3 * WIDTH * HEIGHT);
    if (jpeg_write(filename, in))
        printf("File saved Successfully\n");
    else
        printf("Error in Saving\n");

    pic_free(in);

}

void parse_check(char *expected, char *found) {
    if (strcasecmp(expected, found)) {
        char error[100];
        printf("Expected '%s ' found '%s '\n", expected, found);
        printf("Parse error, abnormal abortion\n");
        exit(0);
    }

}

void parse_doubles(FILE *file, char *check, Point &p) {
    char str[100];
    fscanf(file, "%s", str);
    parse_check(check, str);
    fscanf(file, "%lf %lf %lf", &p.x, &p.y, &p.z);
    printf("%s %lf %lf %lf\n", check, p.x, p.y, p.z);
}

void parse_rad(FILE *file, double *r) {
    char str[100];
    fscanf(file, "%s", str);
    parse_check("rad:", str);
    fscanf(file, "%lf", r);
    printf("rad: %f\n", *r);
}

void parse_shi(FILE *file, double *shi) {
    char s[100];
    fscanf(file, "%s", s);
    parse_check("shi:", s);
    fscanf(file, "%lf", shi);
    printf("shi: %f\n", *shi);
}

int loadScene(char *argv) {
    FILE *file = fopen(argv, "r");
    int number_of_objects;
    char type[50];
    int i;
    Triangle t;
    Sphere s;
    Light l;
    fscanf(file, "%i", &number_of_objects);

    printf("number of objects: %i\n", number_of_objects);
    char str[200];

    parse_doubles(file, "amb:", ambient_light);

    for (i = 0; i < number_of_objects; i++) {
        fscanf(file, "%s\n", type);
        printf("%s\n", type);
        if (strcasecmp(type, "triangle") == 0) {

            printf("found triangle\n");
            int j;

            for (j = 0; j < 3; j++) {
                parse_doubles(file, "pos:", t.v[j].position);
                parse_doubles(file, "nor:", t.v[j].normal);
                parse_doubles(file, "dif:", t.v[j].color_diffuse);
                parse_doubles(file, "spe:", t.v[j].color_specular);
                parse_shi(file, &t.v[j].shininess);
            }

            if (num_triangles == MAX_TRIANGLES) {
                printf("too many triangles, you should increase MAX_TRIANGLES!\n");
                exit(0);
            }
            triangles[num_triangles++] = t;
        } else if (strcasecmp(type, "sphere") == 0) {
            printf("found sphere\n");

            parse_doubles(file, "pos:", s.position);
            parse_rad(file, &s.radius);
            parse_doubles(file, "dif:", s.color_diffuse);
            parse_doubles(file, "spe:", s.color_specular);
            parse_shi(file, &s.shininess);

            if (num_spheres == MAX_SPHERES) {
                printf("too many spheres, you should increase MAX_SPHERES!\n");
                exit(0);
            }
            spheres[num_spheres++] = s;
        } else if (strcasecmp(type, "light") == 0) {
            printf("found light\n");
            parse_doubles(file, "pos:", l.position);
            parse_doubles(file, "col:", l.color);

            if (num_lights == MAX_LIGHTS) {
                printf("too many lights, you should increase MAX_LIGHTS!\n");
                exit(0);
            }
            lights[num_lights++] = l;
        } else {
            printf("unknown type in scene description:\n%s\n", type);
            exit(0);
        }
    }
    return 0;
}

void display() {

}

void init() {
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, WIDTH, 0, HEIGHT, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void idle() {
    //hack to make it only draw once
    static int once = 0;
    if (!once) {
        draw_scene();
        if (mode == MODE_JPEG)
            save_jpg();
    }
    once = 1;
}

int main(int argc, char **argv) {
    if (argc < 2 || argc > 3) {
        printf("usage: %s <scenefile> [jpegname]\n", argv[0]);
        exit(0);
    }
    if (argc == 3) {
        mode = MODE_JPEG;
        filename = argv[2];
    } else if (argc == 2)
        mode = MODE_DISPLAY;

    glutInit(&argc, argv);
    loadScene(argv[1]);

    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WIDTH, HEIGHT);
    int window = glutCreateWindow("Ray Tracer");
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    init();
    glutMainLoop();
}
