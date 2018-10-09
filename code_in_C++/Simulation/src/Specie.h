#ifndef SPECIE_H
#define SPECIE_H

#include "SimTypes.h"
#include <array>
#include <unordered_map>

namespace SimEco{
    

    class Specie{
      public:
        static uint _nSpecies;
        uint _name;
        
        array<NicheValue, NUM_ENV_VARS> niche;        //niche of the specie

        //uint *celulas_Idx;        //indice das celulas que esta presente
        //uint celulas_IdxSize;
        unordered_map<uint, float> cellsPopulation;
        float totalPopulation;          //number of members/population;    Mas pra que float???  10.5 animais?
        float growth;           //taxa de crescimento

        //Capacity of dispersion over geographic distance, river barriers and topographic heterogeneity
        Dispersion dispCap;
        constexpr static float dispThreshold = 0.1500f;     //minimum viable dispersion capacity
        constexpr static float fitnessThreshold = 0.02f;    //minimum viable specie fitness in a cell
        constexpr static float popThreshold = 380.0f;    //minimum specie population necessary for non extinction

        
        float reachability(const Connectivity &destCellConn );
        
        //criação de uma espécie numa única célula
        Specie(const array<NicheValue, NUM_ENV_VARS> &niche, const Dispersion &dispCapacity, uint CellIdx);
        //criação de uma especie já espalhada por várias células
        Specie(const array<NicheValue, NUM_ENV_VARS> &niche, const Dispersion &dispCapacity, pair<uint, float> cellsPop[], uint cellPopSize);
        
        //construtor copy
        //Specie(const Specie &src);
        Specie();
        ~Specie();
        
    };

    inline float Specie::reachability(const Connectivity &destCellConn){
        float result;
        result = max(destCellConn.Geo * this->dispCap.Geo, destCellConn.Topo * this->dispCap.Topo);
        result = max(result, destCellConn.River * this->dispCap.River);
        return result;
    }
}

#endif