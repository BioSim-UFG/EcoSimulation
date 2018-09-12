#ifndef GRID_H
#define GRID_H

#include "SimTypes.h"

#define NUM_TOTAL_CELLS 20
#define NUM_TOTAL_SPECIES 1


using namespace std;

namespace SimEco
{
    class Grid{

        array<Specie *, NUM_TOTAL_SPECIES> species;
        array<Cell *, NUM_TOTAL_CELLS> cells;        //array de referencias para celulas da grid
       // array<float, (NUM_TOTAL_CELLS * NUM_ENV_VARS)> envVec;  // Store a single vector, with four entries for each grid cell (nGridCells * 4), containing Min/Max SAT, and Min/Max PPTN
        //array<float, NUM_TOTAL_CELLS> NPPVec;                   // Sotre a single vector, with one entry for each grid cell, containing NPP value for each grid cell

        //INSIRA AQUI ESTRUTURA DE DADOS PARA PERCORRER POR TIMESTEP , atualmente é apenas o array de cells acima
        //ideias: lista de adjacencia , matriz de adjacencia e 
 
     public:

        Grid();
        ~Grid();    

        void addSpecies();

        // A continuous value of suitability (from N species at one cell) --> é a função que ja fizemos em GPU, por isso precisa dos dados do nicho das especies
        void getCellSuitabilities(array<NicheValue, NUM_TOTAL_SPECIES> &niches);
        // A continuous value of suitability from one specie at N cells
        void getSpecieSuitabilities(array<EnvValue, NUM_TOTAL_CELLS> &climas);
    };
}

#endif