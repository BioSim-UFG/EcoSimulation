#include <execinfo.h>
#include <unistd.h>

#include "Cell.h"


namespace SimEco
{
	vector<vector<unsigned int > > Cell::speciesPopulation;
	Climate** Cell::cell_climates;
	//unsigned int Cell::totalCells = 0;

	Cell::Cell(){
		//speciesPopulation.resize(conf.MAX_CELLS);
		//speciesInside = NULL;
		//numSpecies = 0;
		//speciesInside = (Specie **)malloc(sizeof(Specie *) * Cell::MaxCapacity);
	}

	void Cell::setMaxCells(unsigned int size){
		speciesPopulation.resize(size);
	}

	
	//forçando mandar commit

	float Cell::calcK(float NPP){
        float K;
        float area =0.5;
        K = (NPP * area)/ 50000 ;
        return K;
	}
	
	Cell::~Cell(){ /*--totalCells;*/ }
	/*
	void Cell::addSpecie(Specie *specie){
		if(numSpecies >= MaxCapacity){
			printf("Capacidade atingida! func addSpecie()"); fflush(stdout);
			void *array[10];
			size_t size;
			// get void*'s for all entries on the stack
			size = backtrace(array, 10);
			// print out all the frames to stderr
			backtrace_symbols_fd(array, size, STDERR_FILENO);
			throw __throw_out_of_range;
		}
		speciesInside[numSpecies++] = specie;
	}*/



} // SimEco