#include "Tools.h"
#include <math.h>

Point_t translate(Point_t u, Point_t vetor){
	u.x += vetor.x;
	u.y += vetor.y;

	return u;
}

Point_t scale(Point_t u, double sca){
	//printf("%.2lf, %.2lf -> ", u.x, u.y);
	u.x *= sca;
	u.y *= sca;
	//printf("%.2lf, %.2lf\n", u.x, u.y);

	return u;
}

Point_t rotate(Point_t u, double angle){
	double x = cos(angle) * u.x - sin(angle) * u.y;
	double y = sin(angle) * u.x + cos(angle) * u.y;

	u.x = x;
	u.y = y;

	return u;
}


void translatePolygon(Point_t v[], int size, Point_t vetor){
	for (int i = 0; i < size; i++)
		v[i] = translate(v[i], vetor);
}

void scalePolygon(Point_t v[], int size, double sca){
	for(int i=0; i<size; i++)
		v[i] = scale(v[i], sca);
}

void rotatePolygon(Point_t v[], int size, double angle){
	for(int i=0; i<size; i++)
		v[i] = rotate(v[i], angle);
}



Point_t scaleByPoint(Point_t u, double sca, Point_t center){
	Point_t transPoint = {-center.x, -center.y};
	u = translate(u, transPoint);
	u = scale(u, sca);
	u = translate(u, center);
	return u;
}

Point_t rotateByPoint(Point_t u, double angle, Point_t center){
	Point_t transPoint = {-center.x, -center.y};
	u = translate(u, transPoint);
	u = rotate(u, angle);
	u = translate(u, center);
	return u;
}


void scalePolygonByPoint(Point_t v[], int size, double sca, Point_t center){
	for(int i=0; i<size; i++)
		v[i] = scaleByPoint(v[i], sca, center);
}

void rotatePolygonByPoint(Point_t v[], int size, double angle, Point_t center){
	for(int i=0; i<size; i++)
		v[i] = rotateByPoint(v[i], angle, center);
}