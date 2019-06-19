#ifndef CELLPOLY_H
#define CELLPOLY_H

#include "Tools.h"
#include "Helper.hpp"

class Cell_HexaPoly
{

public:
    Cell_HexaPoly(Point_t center);

    void setCenter(Point_t center);
    Point_t *get_Vertexes();
    float Area();
    Point_t Center();
    static double Altura(){return altura;}
    static double Raio() { return raio; }
    void draw();

protected:
    static constexpr double raio = 1.0 / 1080.0;
    static constexpr double altura = raio * 1.732050808 / 2.0; // raio * sqrt(3)/2
    Point_t center;
    Point_t vertexes[6];
    float area;
private:

};


#endif