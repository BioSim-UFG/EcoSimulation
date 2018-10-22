#ifndef HELPER_H
#define HELPER_H

#include "color.h"
#include "Grid.h"
#include "Specie.h"
#include "SimTypes.h"

#include <iostream>
#include <chrono>
#include <unordered_map>

#define N_INPUT 2


namespace SimEco{

	enum class Exceptions {fileException=1, decompStrException, memRealocException, argException, configurationException=5};
	
	int intException(enum Exceptions ex);
	void testArgs(int argc,char const *argv[]);

	//##########Fase de testes####

	inline void recordSpecieTxtFile(const char *path, int timeStep, Specie &sp);
	inline void recordSpecieFile(const char *path, int timeStep, Specie &sp);

	void recordTimeStepFiles(const char *path, int timeStep, Grid g, const char *simName);


	//Timer class
	class Clock
	{   
		//e se lançar mais de um timer? por exemplo, um timer pra execução toda, e outro só pra parte que calcula um time step?
		//por isso alterei pra classe suportar mais de umm timer ao mesmo tempo
		// ....
		//vi q na vdd era só criar outro objeto timer, então retirei essa feature
		private:

		typedef struct {
			std::chrono::high_resolution_clock::time_point startClock;
			std::chrono::high_resolution_clock::time_point pausedClock;
			bool isPaused;
			//std::chrono::high_resolution_clock::period elapsedPause;
			//std::chrono::high_resolution_clock::time_point last;
		}Clock_count;

		//std::unordered_map<int, Clock_count> timers;
		Clock_count timer;

		public:
		//std::chrono::high_resolution_clock::time_point start;
		//std::chrono::high_resolution_clock::time_point end;
		//std::chrono::duration<double> elapsed;

		void Start();

		//void timerStart(int id);

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