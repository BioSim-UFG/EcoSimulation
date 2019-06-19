#include "Cell_HexaPoly.hpp"

Cell_HexaPoly::Cell_HexaPoly(Point_t center, float area)
{
    setCenter(center);
    setArea(area);
}

void Cell_HexaPoly::setArea(float area){
    this->area = area;
}

void Cell_HexaPoly::setCenter(Point_t center){
    this->center = center;
    vertexes[0].x = center.x - raio;
    vertexes[0].y = center.y;
    vertexes[1].x = center.x - raio / 2;
    vertexes[1].y = center.y + altura;
    vertexes[2].x = center.x + raio / 2;
    vertexes[2].y = center.y + altura;
    vertexes[3].x = center.x + raio;
    vertexes[3].y = center.y;
    vertexes[4].x = center.x + raio / 2;
    vertexes[4].y = center.y - altura;
    vertexes[5].x = center.x - raio / 2;
    vertexes[5].y = center.y - altura;
}

void Cell_HexaPoly::draw(){
    auto &v = vertexes;
    glBegin(GL_POLYGON);
    glVertex2f(v[0].x, v[0].y);
    glVertex2f(v[1].x, v[1].y);
    glVertex2f(v[2].x, v[2].y);
    glVertex2f(v[3].x, v[3].y);
    glVertex2f(v[4].x, v[4].y);
    glVertex2f(v[5].x, v[5].y);
    glEnd();
}

float Cell_HexaPoly::Area(){
    return area;
}

Point_t Cell_HexaPoly::Center(){
    return center;
}

Point_t *Cell_HexaPoly::get_Vertexes(){
    return vertexes;
}
