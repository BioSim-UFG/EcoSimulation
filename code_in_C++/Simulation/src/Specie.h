#ifndef SPECIE_H
#define SPECIE_H

#include "SimTypes.h"

namespace SimEco{
    

    class Specie{
        array<NicheValue, NUM_ENV_VARS> niche;        //niche of the specie
        //int curCellIdx;         //current Cell Index
        Cell *celulaQueTo;        //pointer to current Cell
        float curSize;          //number of members/population;    Mas pra que float???  10.5 animais?
        float growth;           //taxa de crescimento
        float dispTx            //taxa de dispersão

        //dispersion Capacities
        float dispCapGeo;  //Capacity of dispersion over geographic distance
        float dispCapRiver; //Capacity of dispersion over river barriers
        float dispCapTopo; //Capacity of dispersion over topographic heterogeneity

      public:
        Specie();
        ~Specie();
    
    };

    /* FOUNDER PRA Q ?????????????????
    class Founder: Specie{
        int seedCell;             //Starting cell of the founder

    }
    */

}

#endif