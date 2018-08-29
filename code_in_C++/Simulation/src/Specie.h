

namespace EcoSim{

    class Specie;
    class Founder;
    

    class Specie{
        array<NicheValue niche, NUM_ENV_VARS> niche;        //starting niche of the founder
        //int curCellIdx;         //current Cell Index
        Cell *celulaQueTo;        //pointer to current Cell
        float curSize;          //number of members/population;    Mas pra que float???  10.5 animais?
        float growth;           //taxa de crescimento


        //dispersion Capacities
        float dispCapGeo;  //Capacity of dispersion over geographic distance
        float dispCapRiver; //Capacity of dispersion over river barriers
        float dispCapTopo; //Capacity of dispersion over topographic heterogeneity

        Specie()
        ~Specie();
    
    }

    /* FOUNDER PRA Q ?????????????????
    class Founder: Specie{
        int seedCell;             //Starting cell of the founder

    }
    */

    


}