#include "Specie.h"

namespace SimEco{

    Specie::Specie(array<NicheValue, NUM_ENV_VARS> niche, Dispersion dispCapacity, uint cellIdx){
    	//celulaQueTo = nullptr;
    	dispCap = dispCapacity;
    	curSize = 0.0f;
    	growth = 0.0f;
		celula_Idx = (uint *) malloc(sizeof(uint) * 1);
		celula_Idx[0] = cellIdx;

		copy(niche.begin(), niche.end(), this->niche.begin());
    }
	Specie::Specie(){}
    Specie::~Specie(){}
}