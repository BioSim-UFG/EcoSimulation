#ifndef SPECIE_H
#define SPECIE_H

#include "SimTypes.h"

namespace SimEco{
    

    class Specie{
        array<NicheValue, NUM_ENV_VARS> niche;        //niche of the specie

        Cell *celulaQueTo;        //pointer to current Cell
        float curSize;          //number of members/population;    Mas pra que float???  10.5 animais?
        float growth;           //taxa de crescimento

        //dispersion Capacities
        float dispCapGeo;  //Capacity of dispersion over geographic distance
        float dispCapRiver; //Capacity of dispersion over river barriers
        float dispCapTopo; //Capacity of dispersion over topographic heterogeneity

        //ideia para achar espécia, um MAP

      public:
        Specie();
        ~Specie();
    
    };

}

#endif