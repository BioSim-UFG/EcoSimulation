#include "Cell.h"

namespace SimEco
{
    //unsigned int Cell::totalCells = 0;

    Cell::Cell(){
        //id = totalCells++;        //adicionar inicialização
    }
    Cell::~Cell(){ /*--totalCells;*/ }

    // A continuous value of suitability, é a função que ja fizemos em GPU
    /*Cell::getLocalSuitabilities(){

    }    */

} // SimEco