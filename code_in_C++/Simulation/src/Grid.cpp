#include "Grid.h"
#include "Specie.h"

using namespace std;

namespace SimEco{

    Grid::Grid(){ matrixSize = 0; }

    Grid::Grid(Cell cells[], size_t cellSize){
        matrixSize = 0;
        setCells(cells, cellSize);
    }

    Grid::~Grid(){}

    void Grid::setCells(Cell cells[], size_t size){
        copy(cells, cells + size, this->cells.begin());
    }

    //recebe e compacta matriz de adjacencia
    void Grid::setCellsConnectivity(Connectivity *adjMatrix, size_t size){
        size_t pos=0;

        for(size_t i=0; i<size; i++){
            for(size_t j=0; j<size; j++){
                //temos um problema: o valor de distancia geologica pode ser descartável, mas de outra não
                if (adjMatrix[i * size + j].Geo == 0 && adjMatrix[i * size + j].River==0 && adjMatrix[i * size + j].Topo==0)
                    continue;

                ConnectivityMatrix[pos] = adjMatrix[i * size + j];
                indexMatrix[pos] = {i, j};
                pos++;
            }
        }
        matrixSize = pos;
    }

    void Grid::addSpecies(Specie sp[], size_t sp_num){
        int i = 0;

        while(i != sp_num || species.size() <= NUM_TOTAL_SPECIES ){
            species.back() = &sp[i];
            i++;
        }
    }

}