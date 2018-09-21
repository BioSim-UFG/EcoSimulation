#ifndef SIMULATION_H
#define SIMULATION_H
// Pode ser implementado como um objeto, e a simulação na main executada
// como varios objetos do tipo simulation

#include "SimTypes.h"
#include "Grid.h"
#include "Specie.h"
#include <math.h>

namespace SimEco{


    class Simulation;

    class Simulation{

      private:
		Grid &_grid;		//referencia para a grid
		Specie *founders;	//referencia para array de especies ( fundadoras nesse caso)
        uint foundersSize;


        constexpr static int nSteps = 5; //precisão do poligono / número de pontos no poligono
        constexpr static float ErfA = 0.278393f;                   // Abramowitz e Stegun approximation to erf
        constexpr static float ErfB = 0.230389f;                   // https://en.wikipedia.org/wiki/Error_function
        constexpr static float ErfC = 0.000972f;
        constexpr static float ErfD = 0.078108f;
        constexpr static double pi = M_PI;

        void processFounder_timeZero(Specie founder);
        //calcula o fitness, e retorna o ponteiro do vetor resultante
        float* calcSpecieFitness(Specie &specie, uint timeStep);
        void NicheCurve(const float MinTol, const float MaxTol, const float MinEnv, const float MaxEnv, poly_t &NichePoly);

      public:
        Simulation(Grid &grid, Specie *founders, uint foundersPosition[], uint founders_size);

        
    
    };


}

#endif