#include "Grid.h"
#include "Specie.h"

using namespace std;

namespace SimEco{

    Grid::Grid(){ matrixSize = 0; }

    Grid::Grid(Cell cells[], size_t cellSize){
        matrixSize = 0;
        setCells(cells, cellSize);
    }

    Grid::~Grid(){
        for(auto &i: species) free(i);
        
        free(ConnectivityMatrix);
        free(indexMatrix);
    }

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

        while(i <= NUM_TOTAL_SPECIES && i < species.size() ){
            species.back() = &sp[i];
            i++;
        }
    }

    void Grid::load_CellsClimate(FILE *minTemp_src, FILE *maxTemp_src, FILE *minPptn_src, FILE *maxPptn_src, FILE *NPP_src,
                             size_t timeSteps)
    {

        fscanf(minTemp_src, "%*[^\n]\n"); //ignora a primeira linha
        fscanf(maxTemp_src, "%*[^\n]\n"); //ignora a primeira linha
        fscanf(minPptn_src, "%*[^\n]\n"); //ignora a primeira linha
        fscanf(maxPptn_src, "%*[^\n]\n"); //ignora a primeira linha
        fscanf(NPP_src, "%*[^\n]\n");     //ignora a primeira linha

        for (size_t i = 0; i < NUM_TOTAL_CELLS; i++)
        {

            for (int j = timeSteps - 1; j >= 0; j--)
            {
                fscanf(minTemp_src, "%f", &(Cell::cell_climates[i][j].envValues[climVar::Temp].minimum));
                fscanf(maxTemp_src, "%f", &(Cell::cell_climates[i][j].envValues[climVar::Temp].maximum));
                fscanf(minPptn_src, "%f", &(Cell::cell_climates[i][j].envValues[climVar::Pptn].minimum));
                fscanf(maxPptn_src, "%f", &(Cell::cell_climates[i][j].envValues[climVar::Pptn].maximum));
                fscanf(NPP_src, "%f", &(Cell::cell_climates[i][j].NPP));
            }
            fscanf(minTemp_src, "%*[^\n]\n");
            fscanf(maxTemp_src, "%*[^\n]\n");
            fscanf(minPptn_src, "%*[^\n]\n");
            fscanf(maxPptn_src, "%*[^\n]\n");
            fscanf(NPP_src, "%*[^\n]\n");
        }
    }
}