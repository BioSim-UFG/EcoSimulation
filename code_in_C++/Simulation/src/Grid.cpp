#include "Grid.h"

using namespace std;

namespace SimEco{

    Grid::Grid(){}


    Grid::~Grid(){}

    void Grid::addSpecies(Specie* sp, uint sp_num){
        int i = 0;

        while(i != sp_num || species.size() <= NUM_TOTAL_SPECIES ){
            // o método back() retornava o endereço do ultimo elemento, então bastava derreferenciar o tal, ou passar o endereço de sp[i],
            //ou seja, *species.back() = sp[i];   ou     species.back() = &sp[i];  , respectivamente
            *species.back() = sp[i];
            i++;
        }
        

    }

}