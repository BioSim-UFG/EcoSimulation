#include "Grid.h"

using namespace std;

namespace SimEco{

    Grid::Grid(){
    }


    Grid::~Grid(){
    }

    Grid::addSpecies(Specie* sp, uint sp_num){
        int i = 0;

        while(i != sp_num || species.size() <= NUM_TOTAL_SPECIES ){
            species.back() = sp[i]; 
            i++;
        }
        

    }

}