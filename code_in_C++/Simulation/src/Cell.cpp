#include "Cell.h"


namespace SimEco
{
	Climate** Cell::cell_climates;
    //unsigned int Cell::totalCells = 0;

    Cell::Cell(){
        //id = totalCells++;        //adicionar inicialização
        speciesInside = NULL;
        numSpecies = 0;
    }
    Cell::~Cell(){ /*--totalCells;*/ }

    // A continuous value of suitability, é a função que ja fizemos em GPU
    /*Cell::getLocalSuitabilities(){

    }    */

} // SimEco