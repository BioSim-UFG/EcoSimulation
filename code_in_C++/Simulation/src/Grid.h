#ifndef GRID_H
#define GRID_H

#include "SimTypes.h"
#include "Cell.h"
#include <utility>

#define NUM_TOTAL_CELLS 2566
#define NUM_TOTAL_SPECIES 1


using namespace std;

namespace SimEco
{
    class Grid{

        array<Specie *, NUM_TOTAL_SPECIES> species;
        
        array<Cell, NUM_TOTAL_CELLS> cells;

        Connectivity *ConnectivityMatrix;   //matriz esparça compactada ( ver CUsparse)
        pair<size_t, size_t> *indexMatrix;
        size_t matrixSize;

 
     public:

        Grid();
        Grid(Cell cells[], size_t cellSize);
        ~Grid();    

        void addSpecies(Specie sp[], size_t sp_num);

        void addCell(const Cell &novaCelula);
        void addCells(const array<Cell, NUM_TOTAL_CELLS> &novasCelulas);    //pega o vector novasCelulas e copia/passa os elementos para a Grid

        void setCells(Cell celulas[], size_t size);    //seta as celulas
        void setCellsConnectivity(Connectivity *adjMatrix, size_t size);    //passa a matriz de adjacencia, e lá dentro compacta ela

        static void load_CellsClimate(FILE *minTemp_src, FILE *maxTemp_src, FILE *minPptn_src, FILE *maxPptn_src, FILE *NPP_src,
                                size_t timeSteps);

        // A continuous value of suitability (from N species at one cell) --> é a função que ja fizemos em GPU, por isso precisa dos dados do nicho das especies
        void getCellSuitabilities(array<NicheValue, NUM_TOTAL_SPECIES> &niches);
        // A continuous value of suitability from one specie at N cells
        void getSpecieSuitabilities(array<EnvValue, NUM_TOTAL_CELLS> &climas);
    };
}

#endif