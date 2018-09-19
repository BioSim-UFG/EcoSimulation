#ifndef SIMULATION_H
#define SIMULATION_H
// Pode ser implementado como um objeto, e a simulação na main executada
// como varios objetos do tipo simulation

#include "SimTypes.h"
#include "Grid.h"
#include "Specie.h"

namespace SimEco{


    class Simulation;

    class Simulation{

		Grid &_grid;		//referencia para a grid
		Specie *founders;	//referencia para array de especies ( fundadoras nesse caso)

      public:
        Simulation(Grid &grid, Specie *founders);

        
    
    };


}

#endif