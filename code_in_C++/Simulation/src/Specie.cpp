#include "Specie.h"
#include "color.h"

namespace SimEco{

	/*	niche: array de NUM_ENV_VARS elementos, onde cada um tem o valor minimun e maximum
		dispCapacity: capacidade de dispersão da espécie
		celIdx: índice da célula que está espécie está inicialmente	
	*/
    Specie::Specie(array<NicheValue, NUM_ENV_VARS> niche, Dispersion dispCapacity, uint cellIdx){
    	//celulaQueTo = nullptr;
    	dispCap = dispCapacity;
    	curSize = 0.0f;
    	growth = 0.0f;
		celulas_Idx = (uint *) malloc(sizeof(uint) * 1);
		celulas_Idx[0] = cellIdx;

		copy(niche.begin(), niche.end(), this->niche.begin());
    }

	Specie::Specie(array<NicheValue, NUM_ENV_VARS> niche, Dispersion dispCapacity, uint cellIdxs[], uint cellIdxsSize){
		dispCap = dispCapacity;
		curSize = 0.0f;
		growth = 0.0f;
		celulas_Idx = (uint *)malloc(sizeof(uint) * cellIdxsSize);
		
		for(int i=0; i<celulas_IdxSize; i++){
			celulas_Idx[i] = cellIdxs[i];
		}

		copy(niche.begin(), niche.end(), this->niche.begin());
	}

	Specie::Specie(){
		celulas_Idx = NULL;
		celulas_IdxSize = 0;
	}

	Specie::~Specie(){
		printf(YEL("destrutor de Specie - liberando ptr do vetor celulas_Idx %p\n"),celulas_Idx);
		free(celulas_Idx);
		celulas_Idx = NULL;
	}
}