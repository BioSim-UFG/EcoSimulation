#ifndef SIMTYPES_H
#define SIMTYPES_H

//#include <vector>
#include <array>

#define NUM_ENV_VARS 2

#if defined(_WIN32) || defined(_WIN64)
typedef unsigned uint;
typedef unsigned u_int;

#endif


using namespace std;

namespace SimEco
{

	class Cell;
	class Grid;
	class Specie;
	class Simulation;


/*******tipos usados nas celulas e especies*******/

	typedef struct{
		float minimum;          // Annual minima per cell, in a given time, for a given variable
		float maximum;          // Annual maxima per cell, in a given time, for a given variable
	}EnvValue, NicheValue;

	//EnvValue se refere a celula, e NicheValue à especie
	typedef struct{
		array<EnvValue, NUM_ENV_VARS> envValues;    // Temperature and precipitation -> min and max for both
		float NPP;
		float K;		//Const value generated based on NPP to determine maximum resource availability in cell
	}Climate;

	enum climVar{Temp=0, Pptn=1};

	typedef struct{
		float Geo;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on geographic distance
		float Topo;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on topographic heterogeneity
		float River;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on river network
	}Connectivity, Dispersion;

/****tipos usados na grid*********/
	typedef struct{
		int i;
		int j;
	}MatIdx_2D;

/***tipoes usados na simulação******/
	typedef struct { 
    	float x, y; 
	} vec_t;

	typedef struct { 
    	vec_t * const v; 
    	int len;

		inline float area(){
			float area = 0.0f, d;
			int j, i;

			if (len > 2) {
				j = len - 1;
				for (i = 0; (i < len); i++) {
					d = (v[j].x + v[i].x);
					area = area + d * (v[j].y - v[i].y);
					j = i;
				}
				area = -area * 0.5f;
			}
			return area;
		}
	}poly_t;
}

#endif