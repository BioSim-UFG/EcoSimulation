#include "Specie.h"
#include "color.h"

namespace SimEco{

	/*	niche: array de NUM_ENV_VARS elementos, onde cada um tem o valor minimun e maximum
		dispCapacity: capacidade de dispersão da espécie
		celIdx: índice da célula que está espécie está inicialmente	
	*/
	uint Specie::_nSpecies =0;

    Specie::Specie(const array<NicheValue, NUM_ENV_VARS> &niche, const Dispersion &dispCapacity, uint cellIdx){
    	//celulaQueTo = nullptr;
		_name = _nSpecies;
		_nSpecies++;

		printf("\n\n Name:%d n_species:%d \n\n", Specie::_name, Specie::_nSpecies);
		fflush(stdout);

		dispCap = dispCapacity;
    	curSize = 0.0f;
    	growth = 0.0f;
		//celulas_Idx = (uint *) malloc(sizeof(uint) * 1);
		//celulas_Idx[0] = cellIdx;
		//celulas_IdxSize = 1;
		cellsPopulation.reserve(1024+512);
		cellsPopulation.insert({cellIdx, 1});	// coloca população da celula cellIdx como 1

		copy(niche.begin(), niche.end(), this->niche.begin());
    }

	Specie::Specie(const array<NicheValue, NUM_ENV_VARS> &niche, const Dispersion &dispCapacity, uint cellIdxs[], uint cellIdxsSize){
		_name = _nSpecies;
		_nSpecies++;

		
		dispCap = dispCapacity;
		curSize = 0.0f;
		growth = 0.0f;
		//celulas_Idx = (uint *)malloc(sizeof(uint) * cellIdxsSize);
		//celulas_IdxSize = cellIdxsSize;
		cellsPopulation.reserve(1024 + 512);

		for(int i=0; i<cellIdxsSize; i++){
			//substituir 1 pela população que seria aqui. (passar objeto da especie original como argumento da função?)
			cellsPopulation.insert( {cellIdxs[i],1 } );
		}

		copy(niche.begin(), niche.end(), this->niche.begin());
	}

	Specie::Specie(){
		//celulas_Idx = NULL;
		//celulas_IdxSize = 0;
	}

	Specie::Specie(const SimEco::Specie &src){
		_name = _nSpecies;
		Specie::_nSpecies++;

		//printf("\n\n Name:%d n_species:%d \n\n", Specie::_name, Specie::_nSpecies);

		curSize = src.curSize;
		growth = src.growth;
		dispCap = src.dispCap;
		this->niche = src.niche;
		this->cellsPopulation = src.cellsPopulation;
	}

	Specie::~Specie(){
		//printf(YEL("destrutor de Specie - end. %p\n"),this);
		//free(celulas_Idx);
		//celulas_Idx = NULL;
		//delete &cellsPopulation;
	}
}