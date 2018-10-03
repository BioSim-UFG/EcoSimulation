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
        unordered_map<uint, short> cellsPopulation;
        float curSize;          //number of members/population;    Mas pra que float???  10.5 animais?
        float growth;           //taxa de crescimento

        //Capacity of dispersion over geographic distance, river barriers and topographic heterogeneity
        Dispersion dispCap;
        constexpr static float dispThreshold = 0.1500f;
        constexpr static float fitnessThreshold = 0.02f;

        //ideia para achar espécie, um MAP

        //criação de uma espécie numa única célula
        Specie(const array<NicheValue, NUM_ENV_VARS> &niche, const Dispersion &dispCapacity, uint CellIdx);
        //criação de uma especie já espalhada por várias células
        Specie(const array<NicheValue, NUM_ENV_VARS> &niche, const Dispersion &dispCapacity, uint cellIdxs[], uint cellIdxsSize);
        
        float reachability(const Connectivity &destCellConn );
        //usar um array de bits de comprimento fixo, tamanho = total de células na grid
        
        Specie(const Specie &src);
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