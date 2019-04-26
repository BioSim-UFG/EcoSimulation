#ifndef SPECIE_H
#define SPECIE_H

#include "SimTypes.h"
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace SimEco{
    

    class Specie{
      public:
        static uint _nSpecies;
        uint _name;
        
        array<NicheValue, NUM_ENV_VARS> niche;        //niche of the specie

        double totalPopulation;          //number of members/population;    Mas pra que float???  10.5 animais?
        //unordered_map<uint, float> cellsPopulation;
        vector<float> cellsPopulation;      //cellsPopulation[i] = população daquela espécie na célula de id 'i'
        //unordered_set<uint> occupiedCells;  //lista de celulas ocupadas por essa espécie ( O(1))

        float growth;           //taxa de crescimento

        //Capacity of dispersion over geographic distance, river barriers and topographic heterogeneity
        Dispersion dispCap;
        constexpr static float dispThreshold = 0.1500f;     //minimum viable dispersion capacity
        constexpr static float fitnessThreshold = 0.02f;    //minimum viable specie fitness in a cell
        constexpr static float popThreshold = 380.0f;    //minimum specie population necessary for non extinction

        
        float reachability(const Connectivity &destCellConn );

        void addCellPop(uint cellIdx, float population); //soma 'population' à população da célula indicada por cellIdx, caso a celula ainda nao foi ocupada, ela passa a ser ocupada
        void setCellPop(uint cellIdx, float population); //sobrescreve a população da celula, ocupando-a se a mesma não o estiver.
        float getCellPop(uint cellIdx);     //retorna a população da célula, retorna um valor <= 0.0f se ela não estiver ocupada
        float eraseCellPop(uint);     //remove a população dessa espécie naquela célula

        void getTotalPop();

        //criação de uma espécie numa única célula
        Specie(const array<NicheValue, NUM_ENV_VARS> &niche, const Dispersion &dispCapacity, uint CellIdx, float specieGrowth);
        //criação de uma especie já espalhada por várias células
        Specie(const array<NicheValue, NUM_ENV_VARS> &niche, const Dispersion &dispCapacity, float cellsPop[], float specieGrowth);

        //construtor copy
        //Specie(const Specie &src);
        Specie();
        ~Specie();
        
    };


    inline void Specie::addCellPop(uint cellIdx, float population){
		this->cellsPopulation[cellIdx] += population;
		this->totalPopulation +=population;
	}
	inline void Specie::setCellPop(uint cellIdx, float population){
		totalPopulation -= cellsPopulation[cellIdx];
		cellsPopulation[cellIdx] = population;
		totalPopulation += population;
	}

	inline float Specie::getCellPop(uint cellIdx){
        return cellsPopulation[cellIdx];
    }
	inline float Specie::eraseCellPop(uint cellIdx){
        auto removed = cellsPopulation[cellIdx];
        totalPopulation-=removed;
        cellsPopulation[cellIdx] = 0.0f;
        return removed;
	}

    inline float Specie::reachability(const Connectivity &cellsConn){
        float result;
        result = max(cellsConn.Geo * this->dispCap.Geo, cellsConn.Topo * this->dispCap.Topo);
        result = max(result, cellsConn.River * this->dispCap.River);
        return result;
    }
}

#endif