#ifndef SIMULATION_H
#define SIMULATION_H
// Pode ser implementado como um objeto, e a simulação na main executada como varios objetos do tiṕo simulation

#include "SimTypes.h"
#include "Grid.h"
#include "Specie.h"

namespace SimEco{


    class Simulation;

    class Simulation{

        Grid &_grid;
        Specie founders[NUM_TOTAL_SPECIES];

      public:
        Simulation(Grid &grid, Specie founders[NUM_TOTAL_SPECIES]);

        
    
    };


}

#endif