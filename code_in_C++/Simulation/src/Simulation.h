#ifndef SIMULATION_H
#define SIMULATION_H
// Pode ser implementado como um objeto, e a simulação na main executada
// como varios objetos do tipo simulation

#include "SimTypes.h"
#if defined(_WIN32) || defined(_WIN64)
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <string.h>
#include <vector>

//#include "cuda_runtime.h"

namespace SimEco{


	class Simulation;

	class Simulation{

	   public:

		
		Simulation(Grid & grid, const char *name);
		void carrega_founders(const char *founders_input, vector<Specie> &founders);
		void run(int nSteps);
		uint timeSteps(){return _timeSteps;}
		const Grid& grid(){return _grid;}
		string name(){return string(_name);}

		float maxLocalSpeciePopFound=0.0f;

	  private:
	  	const char *_name;
		uint _timeSteps = 0;
		float _Dtime = 0.005f;	//cada time step == 100  anos -> 1=1000 anos
		Grid &_grid;		//referencia para a grid
		//Specie *founders;	//referencia para array de especies ( fundadoras nesse caso)
		//uint foundersSize;


		constexpr static int nPoints = 12; //precisão do poligono / número de pontos no poligono
		constexpr static float ErfA = 0.278393f;                   // Abramowitz e Stegun approximation to erf
		constexpr static float ErfB = 0.230389f;                   // https://en.wikipedia.org/wiki/Error_function
		constexpr static float ErfC = 0.000972f;
		constexpr static float ErfD = 0.078108f;
		constexpr static double pi = M_PI;

		void processFounder_timeZero(Specie &founder);
		void processFounder_timeZero_noConnectivity(Specie &founder);
		void processSpecieTimeStep(Specie &specie, float *fitness);
		//calcula o fitness, e retorna o ponteiro do vetor resultante
		//void calcSpecieFitness(const Specie &specie, uint timeStep, float *fitness);
		void calcSpecieFitness(const Specie &specie, uint timeStep, float *fitness);
		//cria o poligono (já clipado) da curva do nicho
		void NicheCurve(const float MinTol, const float MaxTol, const float MinEnv, const float MaxEnv, poly_t &NichePoly);
		
	


	};


}

#endif