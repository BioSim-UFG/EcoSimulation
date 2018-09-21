#ifndef SPECIE_H
#define SPECIE_H

#include "SimTypes.h"
#include <array>

namespace SimEco{
    

    class Specie{
      public:
        array<NicheValue, NUM_ENV_VARS> niche;        //niche of the specie

        //NUMCell *celula_ptr;        //pointer to current Cell
        uint *celula_Idx;        //indice das celulas que esta presente
        uint celula_IdxSize;
        float curSize;          //number of members/population;    Mas pra que float???  10.5 animais?
        float growth;           //taxa de crescimento

        //Capacity of dispersion over geographic distance, river barriers and topographic heterogeneity
        Dispersion dispCap;

        //ideia para achar espécie, um MAP

        Specie(array<NicheValue, NUM_ENV_VARS> niche, Dispersion dispCapacity, uint CellIdx);
        Specie();
        ~Specie();
    
    };

}

#endif