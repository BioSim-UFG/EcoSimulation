#ifndef SIMULATION_H
#define SIMULATION_H
// Pode ser implementado como um objeto, e a simulação na main executada
// como varios objetos do tipo simulation

#include "SimTypes.h"
#include <math.h>
#include <string.h>

namespace SimEco{


	class Simulation;

	class Simulation{

	  private:
	  	const char *_name;
		Grid &_grid;		//referencia para a grid
		//Specie *founders;	//referencia para array de especies ( fundadoras nesse caso)
		//uint foundersSize;


		constexpr static int nPoints = 5; //precisão do poligono / número de pontos no poligono
		constexpr static float ErfA = 0.278393f;                   // Abramowitz e Stegun approximation to erf
		constexpr static float ErfB = 0.230389f;                   // https://en.wikipedia.org/wiki/Error_function
		constexpr static float ErfC = 0.000972f;
		constexpr static float ErfD = 0.078108f;
		constexpr static double pi = M_PI;

		inline void processFounders_timeZero(Specie &founder);
		//calcula o fitness, e retorna o ponteiro do vetor resultante
		float* calcSpecieFitness(const Specie &specie, uint timeStep, float *fitness);
		//cria o poligono (já clipado) da curva do nicho
		void NicheCurve(const float MinTol, const float MaxTol, const float MinEnv, const float MaxEnv, poly_t &NichePoly);
		//void create_initial_file();
		void create_Directory();

		void createTimeStepFiles(int timeStep);
		void createSpecieFile(int timeStep, Specie &sp);

	  public: 
		Simulation(Grid &grid, const char* name);
		void run(int nSteps);

	};


}

#endif