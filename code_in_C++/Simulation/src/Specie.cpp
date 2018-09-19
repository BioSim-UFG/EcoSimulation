#include "Specie.h"

namespace SimEco{

    Specie::Specie(Dispersion dispCapacity, array<NicheValue, NUM_ENV_VARS> niche){
    	//celulaQueTo = nullptr;
    	dispCap = dispCapacity;

    	curSize = 0.0f;
    	growth = 0.0f;

		copy(niche.begin(), niche.end(), this->niche.begin());

    }
    Specie::~Specie(){}
}