#ifndef TOOLS_H
#define TOOLS_H

#include <GL/glut.h> //inclua a biblioteca glut

typedef struct RGBACOLORd{
    GLubyte r, g, b, a;
    RGBACOLORd(){}
    RGBACOLORd(GLubyte r, GLubyte g, GLubyte b, GLubyte a){
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
}RGBAColord_t;

typedef struct RGBACOLORf{
    GLfloat r, g, b, a;
    RGBACOLORf(){}
    RGBACOLORf(float r, float g, float b, float a){
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
}RGBAColorf_t;

typedef struct{
    GLfloat x, y;    
}Point_t;

Point_t translate(Point_t u, Point_t vetor);
Point_t scale(Point_t u, double sca);
Point_t rotate(Point_t u, double angle);

void translatePolygon(Point_t v[], int size, Point_t vetor);
void scalePolygon(Point_t v[], int size, double sca);
void rotatePolygon(Point_t v[], int size, double angle);


Point_t scaleByPoint(Point_t u, double sca, Point_t center);
Point_t rotateByPoint(Point_t u, double angle, Point_t center);

void scalePolygonByPoint(Point_t v[], int size, double sca, Point_t center);
void rotatePolygonByPoint(Point_t v[], int size, double angle, Point_t center);



#endif