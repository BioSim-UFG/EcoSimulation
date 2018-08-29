#include "Cell.h"
using namespace std;

namespace SimEco
{
    class Grid{
        //talvez tenha que mudar de 'Cell&'' para 'Cell *', por questões de compatibilidade com GPU ( CUDA)
        array<Cell&, NUM_TOTAL_CELLS> cells;        //array de referencias para celulas da grid
        array<float, NUM_TOTAL_CELLS * ENV_NUM_VARS> envVec;        // Store a single vector, with four entries for each grid cell (nGridCells * 4), containing Min/Max SAT, and Min/Max PPTN
        array<float, NUM_TOTAL_CELLS> NPPVec;                       // Sotre a single vector, with one entry for each grid cell, containing NPP value for each grid cell
    };
}