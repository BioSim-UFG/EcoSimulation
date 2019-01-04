#ifndef HELPER_H
#define HELPER_H

#include "Grid.h"
#include "Specie.h"
#include "SimTypes.h"
#include "color.h"

#include <iostream>
#include <chrono>
#include <unordered_map>

#define N_INPUT 2


namespace SimEco{

	enum class Exceptions {fileException=1, decompStrException, memRealocException, argException, configurationException=5};
	
	int intException(enum Exceptions ex);
	void testArgs(int argc,char const *argv[]);

	//##########Fase de testes####

	//cria diretório que guardará as informações da simulação
	void create_SimulationDirectory(const char *simName);

	//guarda informações da espécie, chama recordSpecieTxtFile e recordSpecieFile
	void recordTimeStepFiles(const char *path, int timeStep, Grid g, const char *simName);

	//guarda as informações da espécie em forma de texto
	void recordSpecieTxtFile(const char *path, int timeStep, Specie &sp);
	//guarda as informações da espécie em forma binária
	void recordSpecieFile(const char *path, int timeStep, Specie &sp);



	//Timer class
	class Clock
	{   
		private:
		typedef struct {
			std::chrono::high_resolution_clock::time_point startClock;
			std::chrono::high_resolution_clock::time_point pausedClock;
			bool isPaused;
		}Clock_count;

		Clock_count timer;

		public:

		void Start();
		//pausa o timer
		void Pause();
		//continua o timer
		void Continue();
		//para o timer se nao estiver pausado já. E então retorna o tempo percorrido total pelo timer.
		double End();
		//double elapsedTime();
	};

	class Configuration
	{	public:
		static char NAME[65];
		static char SAVEPATH[265];
		static unsigned int MAX_CELLS;
		static unsigned int NUM_FOUNDERS;
		static unsigned int TIMESTEPS;

		void Configure();
	};

}

#endif