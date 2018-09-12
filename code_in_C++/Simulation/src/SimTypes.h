#ifndef SIMTYPES_H
#define SIMTYPES_H

#include <array>
#include <vector>

#define NUM_ENV_VARS 2


using namespace std;

namespace SimEco
{

    class Cell;
    class Grid;
    class Specie;
    class Simulation;

    typedef struct{
        float maximum;          // Annual maxima per cell, in a given time, for a given variable
        float minimum;          // Annual minima per cell, in a given time, for a given variable
    }EnvValue, NicheValue;

    //EnvValue se refere a celula, e NicheValue à especie
    typedef struct{
        //usar array se for tamanho fixo, e vector se for dinamico
        array<EnvValue, NUM_ENV_VARS> envValues;    // Temperature and precipitation -> max and min for both
        float *NPP; //melhor ser um float, ao inves de float*, não? ( float = 32 bis e ponteiro = 64bits)
    }Climate;

    typedef struct{
        vector<float> Geo;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on geographic distance
        vector<float> Topo;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on topographic heterogeneity
        vector<float> Rivers;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on river network
    }Connectivity;

}

#endif