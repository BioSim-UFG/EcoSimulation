#ifndef SPECIE_H
#define SPECIE_H

#include "SimTypes.h"
#include <array>

namespace SimEco{
    

    class Specie{
      public:
        array<NicheValue, NUM_ENV_VARS> niche;        //niche of the specie

        //Cell *celulaQueTo;        //pointer to current Cell
        float curSize;          //number of members/population;    Mas pra que float???  10.5 animais?
        float growth;           //taxa de crescimento

        //Capacity of dispersion over geographic distance, river barriers and topographic heterogeneity
        Dispersion dispCap;

        //ideia para achar espécie, um MAP

        Specie(Dispersion dispCapacity, array<NicheValue, NUM_ENV_VARS> niche);
        Specie();
        ~Specie();
    
    };

}

#endif