#include "Specie.h"
#include "color.h"
#include "Grid.h"

namespace SimEco{

	/*	niche: array de NUM_ENV_VARS elementos, onde cada um tem o valor minimun e maximum
		dispCapacity: capacidade de dispersão da espécie
		celIdx: índice da célula que está espécie está inicialmente	
	*/
	uint Specie::_nSpecies =0;

    Specie::Specie(const array<NicheValue, NUM_ENV_VARS> &niche, const Dispersion &dispCapacity, uint cellIdx, float specieGrowth){
    	//celulaQueTo = nullptr;
		_name = _nSpecies;
		_nSpecies++;

		dispCap = dispCapacity;
		totalPopulation = 0.0;
		growth = specieGrowth;

		cellsPopulation.resize(Grid::cellsSize, 0.0f);
		cellsPopulation[cellIdx]= 1.0f;	// coloca população da celula cellIdx como 1
		totalPopulation+=1.0;

		copy(niche.begin(), niche.end(), this->niche.begin());
    }

	Specie::Specie(const array<NicheValue, NUM_ENV_VARS> &niche, const Dispersion &dispCapacity, float cellsPop[], float specieGrowth){
		_name = _nSpecies;
		_nSpecies++;
		dispCap = dispCapacity;
		totalPopulation = 0.0;
		growth = specieGrowth;

		cellsPopulation.reserve(Grid::cellsSize);
		for(int i=0; i<Grid::cellsSize; i++){
			cellsPopulation.push_back(cellsPop[i]);
			totalPopulation += cellsPop[i];
		}

		copy(niche.begin(), niche.end(), this->niche.begin());
	}

	Specie::Specie(){
		//celulas_Idx = NULL;
		//celulas_IdxSize = 0;
	}

	/*
	Specie::Specie(const SimEco::Specie &src){
		_name = _nSpecies;
		Specie::_nSpecies++;

		//printf("\n\n Name:%d n_species:%d \n\n", Specie::_name, Specie::_nSpecies);

		curSize = src.curSize;
		growth = src.growth;
		dispCap = src.dispCap;
		this->niche = src.niche;
		this->cellsPopulation = src.cellsPopulation;
	}*/

	Specie::~Specie(){
		//printf(YEL("destrutor de Specie - end. %p\n"),this);
		//free(celulas_Idx);
		//celulas_Idx = NULL;
		//delete &cellsPopulation;
	}
}