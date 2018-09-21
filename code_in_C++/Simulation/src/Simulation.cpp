#include "Simulation.h"
#include "color.h"
#include <iostream>
#include <cmath>
#include <memory>
namespace SimEco{
	Simulation::Simulation(Grid &grid, Specie founders[], uint foundersPosition[], uint founders_size): _grid(grid), founders(founders), foundersSize(founders_size){
		
		//aqui faz o trabalho de preparação da simulação, usando a(s) especie(s) fundadora(s)

		//coloca os founders em suas celulas
		grid.putSpecies(founders, foundersPosition, NUM_TOTAL_SPECIES);

		cout<<BLU("\tCalculando tempo ZERO\n");
		for(uint i=0; i<founders_size; i++){
			processFounder_timeZero(founders[i]);
		}

	}

	void Simulation::processFounder_timeZero(Specie founder){
		//aqui chama calcFitness
		float *fitness = calcSpecieFitness(founder, 0);

		//agora, usando os fitness, espalhar o founder pela grid


	}

	float* Simulation::calcSpecieFitness(Specie &specie, uint timeStep){
		float *fitness = new float[_grid.cellsSize];

		float StdAreaNoOverlap, StdSimBetweenCenters;
		float MidTol;
		float MinTempTol, MaxTempTol, MinPrecpTol, MaxPrecpTol;
		float MinTempEnv, MaxTempEnv, MinPrecpEnv, MaxPrecpEnv;
		float MidEnv;
		float LocFitness;

		vec_t NichePtns[nSteps + 3]; //pontos do poligono do nicho ( da especie ) ( struct com float x e y)
		poly_t ClipedNichePoly = {NichePtns, nSteps + 3};

		MinTempTol = specie.niche[0].minimum;
		MaxTempTol = specie.niche[0].maximum;
		MinPrecpTol = specie.niche[1].minimum;
		MaxPrecpTol = specie.niche[1].maximum;

		Climate *climates = Cell::getTimeClimates(timeStep);

		//calcula o fitness dessa especia pra cada celula da grid
		for(uint cellIdx=0; cellIdx < Grid::cellsSize; cellIdx++){
			MinTempEnv = climates[cellIdx].envValues[0].minimum;
			MaxTempEnv = climates[cellIdx].envValues[0].maximum;
			MinPrecpEnv = climates[cellIdx].envValues[1].minimum;
			MaxPrecpEnv = climates[cellIdx].envValues[1].maximum;

			// Does the species tolerate the local environment?
			if ((MinTempEnv < MinTempTol) || (MaxTempEnv > MaxTempTol) || (MinPrecpEnv < MinPrecpTol) || (MaxPrecpEnv > MaxPrecpTol)){
				LocFitness = 0.0f;
			}
			// Yes, it tolerates, lets calculate the fitness
			else {
			//Primeira variavel de ambiente
				MidTol = ((MaxTempTol + MinTempTol) / 2.0f);
				if( (MaxTempTol - MinTempTol) < 1E-2 ){
					StdAreaNoOverlap = 1.0f;
					MidEnv = 0.0f;
				}else{
					MidEnv = ((MaxTempEnv + MinTempEnv) / 2.0f);

					//cria poligono da Tolerancia já clipado aqui
					NicheCurve(MinTempTol, MaxTempTol, MinTempEnv, MaxTempEnv, ClipedNichePoly);
					StdAreaNoOverlap = 1.0f - ClipedNichePoly.area();
				}
				StdSimBetweenCenters = 1.0f - (fabsf(MidEnv - MidTol) / ((MaxTempTol - MinTempTol)/2.0f));
				// Local fitness, given the first environmental variable
				LocFitness = (StdAreaNoOverlap * StdSimBetweenCenters);


			//Segunda variavel de ambiente
				MidTol = ((MaxPrecpTol + MinPrecpTol) / 2.0f);
				if( (MaxPrecpTol - MinPrecpTol) < 1E-2 ){
					StdAreaNoOverlap = 1.0f;
					MidEnv = 0.0f;
				}else{
					MidEnv = ((MaxPrecpEnv + MinPrecpEnv) / 2.0f);

					//cria poligono da Tolerancia já clipado aqui
					NicheCurve(MinPrecpTol, MaxPrecpTol, MinPrecpEnv, MaxPrecpEnv, ClipedNichePoly);
					StdAreaNoOverlap = 1 - ClipedNichePoly.area();
				}
				StdSimBetweenCenters = 1 - (fabsf(MidEnv - MidTol) / ((MaxPrecpTol - MinPrecpTol)/2.0f));
				// Local fitness, given both environmental variables
				LocFitness = LocFitness * (StdAreaNoOverlap * StdSimBetweenCenters);
			}

			// store fitness value of 'espIndex' especie  for 'cellIdx' cell
			fitness[cellIdx] = LocFitness;
			//if(LocFitness !=0 )printf("LocFit-%.8f  CELL- %d \n",LocFitness,cellIdx);
		}

		return fitness;
	}

	void Simulation::NicheCurve(const float MinTol, const float MaxTol, const float MinEnv, const float MaxEnv, poly_t &nichePoly){
		// nichePoly must be nSteps+3 long
		float erfX, erfY;
		float PhiNum;
		float PhiDen1, PhiDen2;
	
		// Read input data
		const float mi = ((MaxTol + MinTol) / 2.0f);
		const float sigma = (MaxTol - mi) / 2.0;
		const float &a =     MinTol;
		const float &b =     MaxTol;
	
		float x;// = MaxTol;
		const float MinimumMax = MaxTol < MaxEnv? MaxTol:MaxEnv;
		const float MaximumMin = MinTol > MinEnv? MinTol:MinEnv;
		float p, Tmp;
		//float Step = ((b-a) / nSteps);
		const float Step((MinimumMax - MaximumMin) / nSteps);

		x = MinimumMax;
		nichePoly.v[0].x = x;
		nichePoly.v[0].y = 0.0f;
	
		//printf("MaximumMin=%f MinimumMax=%f\t Step=%f\n",MaximumMin,MinimumMax,Step );
		//printf("vertice %d -> x-%f   y-%f\n",0,NichePoly->v[0].x, NichePoly->v[0].y );
		for(int i = 0; i <= nSteps; i++){
			// https://en.wikipedia.org/wiki/Truncated_normal_distribution
			Tmp = (x - mi) / sigma;
			PhiNum = (1.0f/sqrtf(2.0f*pi))*expf((-0.5f)*(Tmp*Tmp));
			//PhiNum = (rsqrt (2.0f*pi))*expf((-0.5f)*(Tmp*Tmp));

			// Error function of (x1)
			erfX = ((b-mi) / sigma) / sqrtf(2.0f);
			Tmp = fabsf(erfX);

			//aqui escolher entre qual dos dois usar
			//erfY = 1.0f-(1.0f/powf(1.0f+(ErfA*Tmp)+(ErfB*(Tmp*Tmp))+(ErfC*powf(Tmp,3.0f))+(ErfD*powf(Tmp,4.0f)),4.0f));
			erfY=erff(Tmp); 

			if(erfX < 0.0f)
				erfY = -1.0f * erfY;
			
			PhiDen1 = (1.0f+erfY) / 2.0f;
			// Error function of (x2)
			erfX = ((a-mi) / sigma) / sqrtf(2.0f);
			Tmp = fabs(erfX);

			//aqui escolher entre qual dos dois usar
			//erfY = 1.0f-(1.0f/powf(1.0f+(ErfA*Tmp)+(ErfB*(Tmp*Tmp))+(ErfC*powf(Tmp,3.0f))+(ErfD*powf(Tmp,4.0f)),4.0f));
			erfY=erff(Tmp);

			if(erfX < 0.0f)
				erfY = -1.0f * erfY;

			PhiDen2 = (1.0f+erfY) / 2.0f;
			p = (PhiNum / (sigma * (PhiDen1 - PhiDen2)));

			nichePoly.v[i+1].x = x;
			nichePoly.v[i+1].y = p;
			//printf("vertice %d -> x-%f   y-%f\n",i+1,NichePoly->v[i+1].x, NichePoly->v[i+1].y );
			x = x - Step;

			//printf("erff(Tmp) = %f", erfY);
		}

		nichePoly.v[nSteps+2].x = nichePoly.v[nSteps+1].x;
		nichePoly.v[nSteps+2].y = 0.0f;
		//printf("vertice %d -> x-%f   y-%f\n\n",nSteps+2,NichePoly->v[nSteps+2].x, NichePoly->v[nSteps+2].y );
	}

}