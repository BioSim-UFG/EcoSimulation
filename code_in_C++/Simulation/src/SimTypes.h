#ifndef SIMTYPES_H
#define SIMTYPES_H

#include <vector>
#include <array>

#define NUM_ENV_VARS 2


using namespace std;

namespace SimEco
{

	class Cell;
	class Grid;
	class Specie;
	class Simulation;

	typedef struct{
		float minimum;          // Annual minima per cell, in a given time, for a given variable
		float maximum;          // Annual maxima per cell, in a given time, for a given variable
	}EnvValue, NicheValue;

	//EnvValue se refere a celula, e NicheValue à especie
	typedef struct{
		array<EnvValue, NUM_ENV_VARS> envValues;    // Temperature and precipitation -> min and max for both
		float NPP;
	}Climate;

	typedef struct{
		int i;
		int j;
	}matIdx_2D;

	enum climVar{Temp=0, Pptn=1};

	typedef struct{
		float Geo;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on geographic distance
		float Topo;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on topographic heterogeneity
		float River;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on river network
	}Connectivity, Dispersion;

}

#endif