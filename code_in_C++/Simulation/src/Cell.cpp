#include "Cell.h"
#include <execinfo.h>
#include <unistd.h>
namespace SimEco
{
	Climate** Cell::cell_climates;
	//unsigned int Cell::totalCells = 0;

	Cell::Cell(){
		//speciesInside = NULL;
		//numSpecies = 0;
		//speciesInside = (Specie **)malloc(sizeof(Specie *) * Cell::MaxCapacity);
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

	// A continuous value of suitability, é a função que ja fizemos em GPU
	/*Cell::getLocalSuitabilities(){

	}    */

} // SimEco