#include "Simulation.h"
#include "Cell.h"
#include "Grid.h"
#include "Specie.h"

#include "color.h"
#include <iostream>
#include <cmath>

namespace SimEco{
	Simulation::Simulation(Grid &grid, const char* name): _grid(grid) , _name(name)/*, founders(founders), foundersSize(founders_size)*/{

		create_Directory();	//cria diretorio de saida/resultados da simulação

		//aqui faz o trabalho de preparação da simulação, usando a(s) especie(s) fundadora(s)
		for(uint i=0; i<_grid.speciesSize; i++){
			processFounder_timeZero(_grid.species[i]);
		}

		recordTimeStepFiles(0);

	}


	/*******************************implementando ainda essa função*********************************/

	//processa o time zero pra uma especie founder especifica
	void Simulation::processFounder_timeZero(Specie &founder){
		/*aqui chama calcFitness e ela retorna um vetor (dinamicamente alocado lá dentro)
		  com os fitness da especie com todas as celulas (consquentemente, vetor de tamanho Grid::cellsSize)*/
		float *fitness = new float[_grid.cellsSize];
		calcSpecieFitness(founder, 0, fitness);

		/*agora, usando os fitness e as conectividades, espalhar o founder pela grid*/
		const MatIdx_2D *idxMat = Grid::indexMatrix;
		uint zipMatPos = Grid::indexMap[founder.celulas_Idx[0]];
		uint lineValue = idxMat[zipMatPos].i;


		//alocação por blocos/Chunks por vez, para evitar alto numero de chamadas ao realloc
		// block_size é arbitrário
		int block_size = (1024 + 512); //tamanho do bloco (quantidade de elementos) que será realocado quando necessário
		int blocksAllocated = 0;

		//enquanto estiver na mesma linha (da matriz compactada), correspondente a linha da matriz de adjacencia)
		while(idxMat[zipMatPos].i == lineValue && zipMatPos < Grid::matrixSize){
			
			//ocupa essa celula também, se o fitness for maior que 0 e não for a própria célula
			if(idxMat[zipMatPos].i != idxMat[zipMatPos].j  && fitness[idxMat[zipMatPos].j] > 0.0f){

				// se o numero de elementos alocados for insuficiente, aloca mais espaço (um bloco)
				while( blocksAllocated*block_size <= founder.celulas_IdxSize){
					blocksAllocated++;
					founder.celulas_Idx = (uint *)realloc(founder.celulas_Idx, sizeof(uint) * (blocksAllocated * block_size));
					if(founder.celulas_Idx == NULL){
						perror(RED("Erro ao realocar memoria"));
						exit(3);
					}
				}

				//adiciona célula na lista de celulas ocupadas (pela especie)
				founder.celulas_Idx[founder.celulas_IdxSize++] = idxMat[zipMatPos].j;
			}

			zipMatPos++;
		}

		founder.celulas_Idx = (uint *)realloc(founder.celulas_Idx, sizeof(uint) * founder.celulas_IdxSize);
		
		delete fitness;
	}

	void Simulation::run(int nSteps){
		
		float *fitness = new float[_grid.cellsSize];

		//timeStep começa em 1, pois deve pular o timeStep Zero (pois ele já foi calculado antes).
		for(int timeStep = 1; timeStep< nSteps; timeStep++){
			printf(BLU("\r\tProcessando timeStep ") "%d/%d", timeStep, nSteps-1);
			fflush(stdout);

			//processa cada timeStep
			for(uint spcIdx=0; spcIdx<_grid.speciesSize; spcIdx++){
				Specie &especie = _grid.species[spcIdx];
				calcSpecieFitness(especie, timeStep, fitness);	//obtem os fitness's da espécie 

				//printf("\nProcessando especie %u", spcIdx); fflush(stdout);
				//processSpecieTimeStep(especie, fitness, timeStep);
				//printf("total celulas ocupadas %u\n", especie.celulas_IdxSize);
			}

		}

		delete fitness;
	}

	void Simulation::processSpecieTimeStep(Specie &specie, float *fitness, int timeStep){

		uint prevCelulas_IdxSize = specie.celulas_IdxSize;
		//ideia de como fazer:

		/*
			fazer um loop que percorre todas as celulas que a espécie já está ocupando, e pra cada iteração
			espalhar a especie a partir daquela célula especifica.
		*/
		//alocação por blocos/Chunks por vez, para evitar alto numero de chamadas ao realloc
		// block_size é arbitrário
		int block_size = (2048 + 1024); //tamanho do bloco (quantidade de elementos) que será realocado quando necessário
		int blocksAllocated = 0;

		for(uint cellIdx = 0; cellIdx < prevCelulas_IdxSize; cellIdx++){
			//printf("\rprocessando celula ja ocupada %u", cellIdx);

			const MatIdx_2D *idxMat = Grid::indexMatrix;
			uint zipMatPos = Grid::indexMap[specie.celulas_Idx[cellIdx]];
			uint lineValue = idxMat[zipMatPos].i;



			//enquanto estiver na mesma linha (da matriz compactada), correspondente a linha da matriz de adjacencia)
			while(idxMat[zipMatPos].i == lineValue && zipMatPos < Grid::matrixSize){
				

				/******DAQUI PRA BAIXA VEM OS PROBLEMAS********/

				//ocupa essa celula também, se o fitness for maior que 0 e não for a própria célula
				if(idxMat[zipMatPos].i != idxMat[zipMatPos].j  && fitness[idxMat[zipMatPos].j] > 0.0f  /*&& celua nao foi ocupada */){

					// se o numero de elementos alocados for insuficiente, aloca mais espaço (um bloco)
					while( blocksAllocated*block_size <= specie.celulas_IdxSize){
						blocksAllocated++;
						specie.celulas_Idx = (uint *)realloc(specie.celulas_Idx, sizeof(uint) * (blocksAllocated * block_size));
						if(specie.celulas_Idx == NULL){
							perror(RED("Erro ao realocar memoria"));
							exit(3);
						}
					}

					//adiciona célula na lista de celulas ocupadas (pela especie)
					specie.celulas_Idx[specie.celulas_IdxSize] = idxMat[zipMatPos].j;
					specie.celulas_IdxSize = min(++specie.celulas_IdxSize, (uint)2566);
				}
				else if( fitness[idxMat[zipMatPos].j] <= 0.0f){
					//remove celula da lista
					//specie.celulas_IdxSize--;
				}

				zipMatPos++;
			}


		}
		specie.celulas_Idx = (uint *)realloc(specie.celulas_Idx, sizeof(uint) * specie.celulas_IdxSize);
		if (specie.celulas_Idx == NULL){
			perror(RED("Erro ao realocar memoria"));
			exit(3);
		}
	}



	//calcula o fitness de uma especie em um determinado timeStep (copiei e adaptei a função que tinhamos pra GPU)
	float* Simulation::calcSpecieFitness(const Specie &specie, uint timeStep, float *fitness){

		float StdAreaNoOverlap=0, StdSimBetweenCenters=0;
		float MidTol=0;
		float MinTempTol=0, MaxTempTol=0, MinPrecpTol=0, MaxPrecpTol=0;
		float MinTempEnv = 0, MaxTempEnv = 0, MinPrecpEnv = 0, MaxPrecpEnv = 0;
		float MidEnv = 0;
		float LocFitness = 0;

		vec_t NichePtns[nPoints + 3]; //pontos do poligono do nicho ( da especie ) ( struct com float x e y)
		poly_t ClipedNichePoly = {NichePtns, nPoints + 3};

		MinTempTol = specie.niche[0].minimum;
		MaxTempTol = specie.niche[0].maximum;
		MinPrecpTol = specie.niche[1].minimum;
		MaxPrecpTol = specie.niche[1].maximum;

		const Climate *climates = Cell::getTimeClimates(timeStep);	//pega os climas das celulas de um timeStep


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

	//cria poligono do nicho já clipado com variaveis do Ambiente ( aka celula) (também copiado e adaptado da GPU)
	void Simulation::NicheCurve(const float MinTol, const float MaxTol, const float MinEnv, const float MaxEnv, poly_t &nichePoly){
		// nichePoly must be nPoints+3 long
		float erfX = 0, erfY = 0;
		float PhiNum = 0;
		float PhiDen1 = 0, PhiDen2 = 0;

		// Read input data
		const float mi = ((MaxTol + MinTol) / 2.0f);
		const float sigma = (MaxTol - mi) / 2.0f;
		const float &a =     MinTol;
		const float &b =     MaxTol;
	
		float x;// = MaxTol;
		const float MinimumMax = MaxTol < MaxEnv? MaxTol:MaxEnv;
		const float MaximumMin = MinTol > MinEnv? MinTol:MinEnv;
		float p, Tmp;
		//float Step = ((b-a) / nPoints);
		const float Step((MinimumMax - MaximumMin) / nPoints);

		x = MinimumMax;
		nichePoly.v[0].x = x;
		nichePoly.v[0].y = 0.0f;
	
		//printf("MaximumMin=%f MinimumMax=%f\t Step=%f\n",MaximumMin,MinimumMax,Step );
		//printf("vertice %d -> x-%f   y-%f\n",0,NichePoly->v[0].x, NichePoly->v[0].y );
		for(int i = 0; i <= nPoints; i++){
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

		nichePoly.v[nPoints+2].x = nichePoly.v[nPoints+1].x;
		nichePoly.v[nPoints+2].y = 0.0f;
		//printf("vertice %d -> x-%f   y-%f\n\n",nPoints+2,NichePoly->v[nPoints+2].x, NichePoly->v[nPoints+2].y );
	}


	/***********************************************************************************/

	//cria o diretorio no caminho ( e o caminho se o tal nao existir ainda), apenas se ele não existir
	inline void Simulation::create_Directory(){
		char pasta[80];
		sprintf(pasta,"mkdir -p Results/%s",_name);
		system(pasta);
	}

	inline void Simulation::recordTimeStepFiles(int timeStep){
		for(size_t i = 0; i < _grid.speciesSize ; i++)
			recordSpecieFile(timeStep, _grid.species[i]);
	}

	inline void Simulation::recordSpecieFile(int timeStep, Specie &sp){
		char fname[80];
		sprintf(fname,"Results/%s/%s_Esp%d_Time%d",_name,_name,sp._name,timeStep);
		FILE *f = fopen(fname,"w");
		if(f == NULL){
			printf(RED("Falha ao abrir o arquivo %s\n"),fname );
			exit(1);
		}

		for (size_t i = 0; i < sp.celulas_IdxSize; i++){
			fprintf(f, "%5.u ", sp.celulas_Idx[i]);
			if ((i + 1) % 7 == 0)
				fprintf(f, "\n");
		}

		fclose(f);
	}
}