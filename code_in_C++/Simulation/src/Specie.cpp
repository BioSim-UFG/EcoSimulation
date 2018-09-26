#include "Specie.h"
#include "color.h"

namespace SimEco{

	/*	niche: array de NUM_ENV_VARS elementos, onde cada um tem o valor minimun e maximum
		dispCapacity: capacidade de dispersão da espécie
		celIdx: índice da célula que está espécie está inicialmente	
	*/
	uint Specie::_nSpecies =0;

    Specie::Specie(array<NicheValue, NUM_ENV_VARS> niche, Dispersion dispCapacity, uint cellIdx){
    	//celulaQueTo = nullptr;
		_nSpecies++;
		_name = _nSpecies;
    	dispCap = dispCapacity;
    	curSize = 0.0f;
    	growth = 0.0f;
		celulas_Idx = (uint *) malloc(sizeof(uint) * 1);
		celulas_Idx[0] = cellIdx;
		celulas_IdxSize = 1;

		copy(niche.begin(), niche.end(), this->niche.begin());
    }

	Specie::Specie(array<NicheValue, NUM_ENV_VARS> niche, Dispersion dispCapacity, uint cellIdxs[], uint cellIdxsSize){
		_nSpecies++;
		_name = _nSpecies;
		dispCap = dispCapacity;
		curSize = 0.0f;
		growth = 0.0f;
		celulas_Idx = (uint *)malloc(sizeof(uint) * cellIdxsSize);
		celulas_IdxSize = cellIdxsSize;
		
		for(int i=0; i<celulas_IdxSize; i++){
			celulas_Idx[i] = cellIdxs[i];
		}

		copy(niche.begin(), niche.end(), this->niche.begin());
	}

	//retorna o índice da posição da celula no vetor de células ocupadas, ou
	long Specie::occupingCell(uint cellIdx){
		for(uint i=0; i< celulas_IdxSize; i++){
			if(celulas_Idx[i] == cellIdx)
				return i;
		}
		return -1;
	}

	Specie::Specie(){
		celulas_Idx = NULL;
		celulas_IdxSize = 0;
		
	}

	Specie::~Specie(){
		//printf(YEL("destrutor de Specie - end. %p\n"),this);
		free(celulas_Idx);
		celulas_Idx = NULL;
	}
}