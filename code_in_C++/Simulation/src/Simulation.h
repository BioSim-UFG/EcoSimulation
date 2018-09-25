#ifndef SIMULATION_H
#define SIMULATION_H
// Pode ser implementado como um objeto, e a simulação na main executada
// como varios objetos do tipo simulation

#include "SimTypes.h"
#include <math.h>

namespace SimEco{


	class Simulation;

	class Simulation{

	  private:
		Grid &_grid;		//referencia para a grid
		//Specie *founders;	//referencia para array de especies ( fundadoras nesse caso)
		//uint foundersSize;


		constexpr static int nPoints = 5; //precisão do poligono / número de pontos no poligono
		constexpr static float ErfA = 0.278393f;                   // Abramowitz e Stegun approximation to erf
		constexpr static float ErfB = 0.230389f;                   // https://en.wikipedia.org/wiki/Error_function
		constexpr static float ErfC = 0.000972f;
		constexpr static float ErfD = 0.078108f;
		constexpr static double pi = M_PI;

		inline void processFounder_timeZero(Specie &founder);
		//calcula o fitness, e retorna o ponteiro do vetor resultante
		float* calcSpecieFitness(Specie &specie, uint timeStep, float *fitness);
		//cria o poligono (já clipado) da curva do nicho
		void NicheCurve(const float MinTol, const float MaxTol, const float MinEnv, const float MaxEnv, poly_t &NichePoly);

	  public:
		Simulation(Grid &grid);
		void run(int nSteps);
	
	};


}

#endif