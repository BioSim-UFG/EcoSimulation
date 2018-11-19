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

        float totalPopulation;          //number of members/population;    Mas pra que float???  10.5 animais?
        unordered_map<uint, float> cellsPopulation;
        //uint *celulas_Idx;        //indice das celulas que esta presente
        //uint celulas_IdxSize;
        float growth;           //taxa de crescimento

        //Capacity of dispersion over geographic distance, river barriers and topographic heterogeneity
        Dispersion dispCap;
        constexpr static float dispThreshold = 0.1500f;     //minimum viable dispersion capacity
        constexpr static float fitnessThreshold = 0.02f;    //minimum viable specie fitness in a cell
        constexpr static float popThreshold = 380.0f;    //minimum specie population necessary for non extinction

        
        float reachability(const Connectivity &destCellConn );

        void addCellPop(uint cellIdx, float population); //soma 'population' à população da célula indicada por cellIdx, caso a celula ainda nao foi ocupada, ela passa a ser ocupada
        void setCellPop(uint cellIdx, float population); //sobrescreve a população da celula, ocupando-a se a mesma não o estiver.
        pair<unordered_map<uint, float>::iterator, bool> insertCellPop(uint cellIdx, float population); //insere nova célula no mapa de células ocupadas, apenas se ela não existir no mapa
        float getCellPop(uint cellIdx);     //retorna a população da célula, retorna um valor <= 0.0f se ela não estiver ocupada
        int eraseCellPop(uint);     //remove a célula do mapa de células ocupadas, se ela existir

        void getTotalPop();

        //criação de uma espécie numa única célula
        Specie(const array<NicheValue, NUM_ENV_VARS> &niche, const Dispersion &dispCapacity, uint CellIdx, float specieGrowth);
        //criação de uma especie já espalhada por várias células
        Specie(const array<NicheValue, NUM_ENV_VARS> &niche, const Dispersion &dispCapacity, pair<uint, float> cellsPop[], uint cellPopSize, float specieGrowth);

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
	inline pair<unordered_map<uint, float>::iterator, bool> Specie::insertCellPop(uint cellIdx, float population){
		auto sucess = cellsPopulation.insert({cellIdx, population}); //obs: insert() só adiciona o par {chave,valor}, se nao existir a chave ainda
		if (sucess.second == true)
			totalPopulation += 1.0f; //Só aumenta a população se conseguiu inserir um NOVO elemento no mapa, ou seja, acabou de ocupar a célula
		return sucess;
	}
	inline float Specie::getCellPop(uint cellIdx){
		if (cellsPopulation.find(cellIdx) != cellsPopulation.end())
			return this->cellsPopulation[cellIdx];

		return 0.0f;
	}
	inline int Specie::eraseCellPop(uint cellIdx){
		auto cellIterator = cellsPopulation.find(cellIdx);
		//remove se a celula, se tiver sindo encontrada/se ela existir
		if (cellIterator != cellsPopulation.end()){
			totalPopulation -= cellIterator->second; //diminui a população daquela espécie
			cellsPopulation.erase(cellIterator);
			return 1;
		}

		return 0;
	}

    inline float Specie::reachability(const Connectivity &destCellConn){
        float result;
        result = max(destCellConn.Geo * this->dispCap.Geo, destCellConn.Topo * this->dispCap.Topo);
        result = max(result, destCellConn.River * this->dispCap.River);
        return result;
    }
}

#endif