#include "Simulation.h"
#include "Cell.h"
#include "Grid.h"
#include "Specie.h"
#include "Helper.h"

#ifndef __NVCC__
#include "cudaErrorCheck.h"
#endif

#include <iostream>
#include <cmath>


#define SIZE 30

namespace SimEco{

    Simulation::Simulation(Grid &grid, const char *name) : _grid(grid), _name(name) /*, founders(founders), foundersSize(founders_size)*/{

    create_SimulationDirectory(_name); //cria diretorio de saida/resultados da simulação

    printf(BLU("\tLendo especies Founders... "));
    fflush(stdout);
    carrega_founders("../../input/SpecieData.txt", grid.species);
    printf(BOLD(LGTBLU("OK!\n")));
    fflush(stdout);

    cout << BLU("\tCalculando tempo ZERO... ");
    fflush(stdout);
    //aqui faz o trabalho de preparação da simulação, usando a(s) especie(s) fundadora(s)
    for (uint i = 0; i < _grid.species.size(); i++){
        processFounder_timeZero(_grid.species[i]);
    }
    cout << BLU("OK\n");
    fflush(stdout);

//grava os resultados do timeStep
//Caso já exista aquela pasta, primeiro remove-la
#ifdef __linux__
    string path = "Results/" + string(_name) + "/timeStep0";
    system(("rm -r Results/" + string(_name)).c_str());
    string comand = "mkdir -p " + path;
#elif _WIN32
    string path = "Results\\" + string(_name) + "\\timeStep0";
    system(("rmdir /s /q Results\\" + string(_name)).c_str());
    string comand = "mkdir " + path;
#endif

    system(comand.c_str());
    recordTimeStepFiles((path).c_str(), 0, _grid, _name);

    //delete[] founders;
}

/*******************************implementando ainda essa função*********************************/

//processa o time zero pra uma especie founder especifica
void Simulation::processFounder_timeZero(Specie &founder){

    calcK(0);

    /*aqui chama calcFitness e ela retorna um vetor (dinamicamente alocado lá dentro)
		  com os fitness da especie com todas as celulas (consquentemente, vetor de tamanho Grid::cellsSize)*/
    float *fitness = new float[_grid.cellsSize];
    calcSpecieFitness(founder, 0, fitness);

    /*agora, usando os fitness e as conectividades, espalhar o founder pela grid*/
    const MatIdx_2D *idxMat = Grid::indexMatrix;
    //uint zipMatPos = Grid::indexMap[founder.celulas_Idx[0]];
    uint zipMatPos = Grid::indexMap[founder.cellsPopulation.begin()->first];
    uint lineValue = idxMat[zipMatPos].i;

    
    //enquanto estiver na mesma linha (da matriz compactada), correspondente a linha da matriz de adjacencia)
    while (idxMat[zipMatPos].i == lineValue && zipMatPos < Grid::matrixSize){

        //ocupa essa celula também, se o fitness for maior que 0 e não for a própria célula
        if (idxMat[zipMatPos].i != idxMat[zipMatPos].j && fitness[idxMat[zipMatPos].j] > 0.0f){

            
            float initialPopulation = Cell::current_K[idxMat[zipMatPos].j] * founder.growth * fitness[idxMat[zipMatPos].j];

            founder.insertCellPop((uint)idxMat[zipMatPos].j, initialPopulation);
            Cell::speciesPresent[(uint)idxMat[zipMatPos].j].insert(&founder);
        }

        zipMatPos++;
    }
    //founder.celulas_Idx = (uint *)realloc(founder.celulas_Idx, sizeof(uint) * founder.celulas_IdxSize);
    delete fitness;
}

void Simulation::run(int nSteps){
    this->_timeSteps = nSteps;

    //cria um subdiretorio para cada timestep
    char dir[50], comand[61];
#ifdef __linux__
    sprintf(dir, "Results/%s/timeStep", _name);
#elif __WIN32
    sprintf(dir, "Results\\%s\\timeStep", _name);
#endif

    int len = strlen(dir);
    for (int i = 0; i < nSteps; i++){
        sprintf(dir + len, "%d", i);
#ifdef __linux__
        sprintf(comand, "mkdir -p %s &", dir);
#elif __WIN32
        sprintf(comand, "mkdir %s &", dir);
#endif
        system(comand);
    }

        //aloca memória para o calculo dos fitness's
        //float *fitness = new float[_grid.cellsSize];
        float *d_fitness;
		float *d_espNiche, *d_cellEnv;
        Climate *d_climates;
        

        Connectivity *d_connectivityMatrix; //matriz esparça compactada ( ver CUsparse)
        MatIdx_2D *d_indexMatrix;
        unordered_map<uint, uint> d_indexMap;

        //CudaSafeCall(cudaMalloc(&d_espNiche, sizeof(NicheValue) * NUM_ENV_VARS * _grid.species.size()));
		//CudaSafeCall(cudaMalloc(&d_cellEnv, sizeof(float) * NUM_ENV_VARS * _grid.cellsSize));
		//CudaSafeCall(cudaMalloc(&d_fitness, sizeof(float) * _grid.species.size() * _grid.cellsSize));

		CudaSafeCall( cudaMalloc(&d_fitness, sizeof(float)* _grid.cellsSize* SIZE));
        CudaSafeCall( cudaMalloc(&d_connectivityMatrix, sizeof(Connectivity) * Grid::matrixSize ));
        CudaSafeCall( cudaMalloc(&d_climates, sizeof(*climates)));

        CudaSafeCall( cudaMalloc(&d_indexMatrix, sizeof(MatIdx_2D)* Grid:matrixSize ));
        CudaSafeCall( cudaMalloc(&d_indexMap, sizeof()) )
		
    //timeStep começa em 1, pois deve pular o timeStep Zero (ele já foi calculado antes).
    for (int timeStep = 1; timeStep < _timeSteps; timeStep++){
        printf(BLU("\r\tProcessando timeStep ") "%d/%d", timeStep, _timeSteps - 1);
        fflush(stdout);

        calcK(timeStep);

        //processa cada timeStep
        //for(uint spcIdx=0; spcIdx<_grid.species.size(); spcIdx++){
        //	Specie &especie = _grid.species[spcIdx];
        //se uma espécie se especializar, ela vai ser adicionada no final, mas ela nao pode ser processada ainda nesse loop
        //auto it_oldEnd = _grid.species.cend();
       
       
       /***** PARALELIZAR AQUI!!!****/

        const Climate *climates = Cell::getTimeClimates(timeStep);
        
        CudaSafeCall( cudaMemcpy(d_climates, climates, sizeof(*climates), cudaMemcpyHostToDevice));

        Paralel_me <<1,SIZE>>(timeStep, d_fitness, _grid.species(), d_climates);
        
        //se a especie for extinta:
        /*if (especie.totalPopulation < Specie::popThreshold)
        {
            it = _grid.species.erase(it); //retorna o novo iterator da posição correspondente ao do elemento removido
            for (auto cell : it->cellsPopulation)
            { //percorre por todas as células que ocupava e a remove de lá
                Cell::speciesPresent[cell.first].erase(&especie);
            }
            //printf("espécie extinta!");
        }
        else
            ++it; //só aumenta o iterator se não remover um elemento do vector
        */
        //++it;
        sprintf(dir + len, "%d", timeStep);
        recordTimeStepFiles(dir, timeStep, _grid, _name);
    }

    //delete fitness;
    cudaFree(d_fitness);
}

__device__ void Paralel_me(int timeStep, float *fitness, Specie *sps, const Climate *climates ){

    int index = blockIdx.x * blockDim.x + threadIdx.x;

    for (auto it = sps.begin()+index; it != sps.cend();it += SIZE){
        auto &especie = *it;

        //CalcSpecieFitness e processSpecieTimeStep não podem ser feitas em paralelo entre elas, mas pode
        //ser paralelizado internamente
        calcSpecieFitness(especie, timeStep, fitness, climates); //obtem os fitness's da espécie

        processSpecieTimeStep(especie, fitness);

        //marking if the specie is to be deleted
        if (especie.totalPopulation < Specie::popThreshold){

        }
    }
}



void Simulation::processSpecieTimeStep(Specie &specie, float *fitness){
    //salva as celulas ocupadas do timeStep anterior
    uint prevCelulas_IdxSize = specie.cellsPopulation.size();
    pair<uint, short> prevCelulas[prevCelulas_IdxSize];
    copy(specie.cellsPopulation.begin(), specie.cellsPopulation.end(), prevCelulas);

    /*fazer um loop que percorre todas as celulas que a espécie já está ocupando, e pra cada iteração
			espalhar a especie a partir daquela célula especifica.
		*/
    const MatIdx_2D *idxMat = Grid::indexMatrix;
    //for(uint cellIdx = 0; cellIdx < prevCelulas_IdxSize; cellIdx++){
    for (auto &cell : prevCelulas){
        //uint zipMatPos = Grid::indexMap[specie.celulas_Idx[cellIdx]];
        uint zipMatPos = Grid::indexMap[cell.first];
        uint lineValue = idxMat[zipMatPos].i;

        //enquanto estiver na mesma linha (da matriz compactada), correspondente a linha da matriz de adjacencia)
        while (idxMat[zipMatPos].i == lineValue && zipMatPos < Grid::matrixSize){

            //ocupa essa celula também, se o fitness for maior que 0 e não estiver ocupada ainda
            if (fitness[idxMat[zipMatPos].j] > Specie::fitnessThreshold && cell.first != idxMat[zipMatPos].j){

                /* USAR unordered_map para indicar quais celulas estão ocupadas, e qual o tamanho da população (dessa especie) dentro da celula
						ou seja, vai mapear o índice da célula ocupada para a população dela.
					*/
                if (specie.reachability(Grid::connectivityMatrix[zipMatPos]) >= Specie::dispThreshold){
                 
                    float Population = cell.second * specie.growth * fitness[idxMat[zipMatPos].j];

                    Population = min(Population, Cell::current_K[idxMat[zipMatPos].j]);

                    specie.insertCellPop((uint)idxMat[zipMatPos].j, Population);
                    Cell::speciesPresent[(uint)idxMat[zipMatPos].j].insert(&specie);
                }
            }
            else if (fitness[idxMat[zipMatPos].j] <= 0.0f){

                specie.eraseCellPop(idxMat[zipMatPos].j);
                Cell::speciesPresent[(uint)idxMat[zipMatPos].j].erase(&specie);
            }

            zipMatPos++;
        }
    }
    //specie.celulas_Idx = (uint *)realloc(specie.celulas_Idx, sizeof(uint) * specie.celulas_IdxSize);
}

//calcula o fitness de uma especie em um determinado timeStep (copiei e adaptei a função que tinhamos pra GPU)
__device__ void Simulation::calcSpecieFitness(const Specie &specie, uint timeStep, float *fitness, const Climate *climates){

    

    float StdAreaNoOverlap = 0, StdSimBetweenCenters = 0;

    const float &MinTempTol = specie.niche[0].minimum;
    const float &MaxTempTol = specie.niche[0].maximum;
    const float &MinPrecpTol = specie.niche[1].minimum;
    const float &MaxPrecpTol = specie.niche[1].maximum;
    //float MinTempEnv = 0, MaxTempEnv = 0, MinPrecpEnv = 0, MaxPrecpEnv = 0;

    float MidTol = 0;
    float MidEnv = 0;
    float LocFitness = 0;

    vec_t NichePtns[nPoints + 3]; //pontos do poligono do nicho ( da especie ) ( struct com float x e y)
    poly_t ClipedNichePoly = {NichePtns, nPoints + 3};

   
   //Idealmente seria um vetor desse para cada thread, mas depende da quantidade de memória disponivel
   // const Climate *climates = Cell::getTimeClimates(timeStep); //pega os climas das celulas de um timeStep

    //calcula o fitness dessa especia pra cada celula da grid
    for (uint cellIdx = 0; cellIdx < Grid::cellsSize; cellIdx++){
        const float &MinTempEnv = climates[cellIdx].envValues[0].minimum;
        const float &MaxTempEnv = climates[cellIdx].envValues[0].maximum;
        const float &MinPrecpEnv = climates[cellIdx].envValues[1].minimum;
        const float &MaxPrecpEnv = climates[cellIdx].envValues[1].maximum;

        // Does the species tolerate the local environment?
        if ((MinTempEnv < MinTempTol) || (MaxTempEnv > MaxTempTol) || (MinPrecpEnv < MinPrecpTol) || (MaxPrecpEnv > MaxPrecpTol)){
            LocFitness = 0.0f;
        }
        // Yes, it tolerates, lets calculate the fitness
        else{
            //Primeira variavel de ambiente
            MidTol = ((MaxTempTol + MinTempTol) / 2.0f);
            if ((MaxTempTol - MinTempTol) < 1E-2){
                StdAreaNoOverlap = 1.0f;
                MidEnv = 0.0f;
            }
            else{
                MidEnv = ((MaxTempEnv + MinTempEnv) / 2.0f);

                //cria poligono da Tolerancia já clipado aqui
                NicheCurve(MinTempTol, MaxTempTol, MinTempEnv, MaxTempEnv, ClipedNichePoly);
                StdAreaNoOverlap = 1.0f - ClipedNichePoly.area();
            }
            StdSimBetweenCenters = 1.0f - (fabsf(MidEnv - MidTol) / ((MaxTempTol - MinTempTol) / 2.0f));
            // Local fitness, given the first environmental variable
            LocFitness = (StdAreaNoOverlap * StdSimBetweenCenters);

            //Segunda variavel de ambiente
            MidTol = ((MaxPrecpTol + MinPrecpTol) / 2.0f);
            if ((MaxPrecpTol - MinPrecpTol) < 1E-2){
                StdAreaNoOverlap = 1.0f;
                MidEnv = 0.0f;
            }
            else{
                MidEnv = ((MaxPrecpEnv + MinPrecpEnv) / 2.0f);

                //cria poligono da Tolerancia já clipado aqui
                NicheCurve(MinPrecpTol, MaxPrecpTol, MinPrecpEnv, MaxPrecpEnv, ClipedNichePoly);
                StdAreaNoOverlap = 1 - ClipedNichePoly.area();
            }
            StdSimBetweenCenters = 1 - (fabsf(MidEnv - MidTol) / ((MaxPrecpTol - MinPrecpTol) / 2.0f));
            // Local fitness, given both environmental variables
            LocFitness = LocFitness * (StdAreaNoOverlap * StdSimBetweenCenters);
        }

        // store fitness value of 'espIndex' especie  for 'cellIdx' cell
        fitness[cellIdx] = LocFitness;
        //if(LocFitness !=0 )printf("LocFit-%.8f  CELL- %d \n",LocFitness,cellIdx);
    }
}

//cria poligono do nicho já clipado com variaveis do Ambiente ( aka celula) (também copiado e adaptado da GPU)
__global__ void Simulation::NicheCurve(const float MinTol, const float MaxTol, const float MinEnv, const float MaxEnv, poly_t &nichePoly){
    // nichePoly must be nPoints+3 long
    float erfX = 0, erfY = 0;
    float PhiNum = 0;
    float PhiDen1 = 0, PhiDen2 = 0;

    // Read input data
    const float mi = ((MaxTol + MinTol) / 2.0f);
    const float sigma = (MaxTol - mi) / 2.0f;
    const float &a = MinTol;
    const float &b = MaxTol;

    float x; // = MaxTol;
    const float &MinimumMax = MaxTol < MaxEnv ? MaxTol : MaxEnv;
    const float &MaximumMin = MinTol > MinEnv ? MinTol : MinEnv;
    float p, Tmp;
    //float Step = ((b-a) / nPoints);
    const float Step = ((MinimumMax - MaximumMin) / nPoints);

    x = MinimumMax;
    nichePoly.v[0].x = x;
    nichePoly.v[0].y = 0.0f;

    //printf("MaximumMin=%f MinimumMax=%f\t Step=%f\n",MaximumMin,MinimumMax,Step );
    //printf("vertice %d -> x-%f   y-%f\n",0,NichePoly->v[0].x, NichePoly->v[0].y );
    for (int i = 0; i <= nPoints; i++){
        // https://en.wikipedia.org/wiki/Truncated_normal_distribution
        Tmp = (x - mi) / sigma;
        PhiNum = (1.0f / sqrtf(2.0f * pi)) * expf((-0.5f) * (Tmp * Tmp));
        //PhiNum = (rsqrt (2.0f*pi))*expf((-0.5f)*(Tmp*Tmp));

        // Error function of (x1)
        erfX = ((b - mi) / sigma) / sqrtf(2.0f);
        Tmp = fabsf(erfX);

        //aqui escolher entre qual dos dois usar
        //erfY = 1.0f-(1.0f/powf(1.0f+(ErfA*Tmp)+(ErfB*(Tmp*Tmp))+(ErfC*powf(Tmp,3.0f))+(ErfD*powf(Tmp,4.0f)),4.0f));
        erfY = erff(Tmp);

        if (erfX < 0.0f)
            erfY = -1.0f * erfY;

        PhiDen1 = (1.0f + erfY) / 2.0f;
        // Error function of (x2)
        erfX = ((a - mi) / sigma) / sqrtf(2.0f);
        Tmp = fabs(erfX);

        //aqui escolher entre qual dos dois usar
        //erfY = 1.0f-(1.0f/powf(1.0f+(ErfA*Tmp)+(ErfB*(Tmp*Tmp))+(ErfC*powf(Tmp,3.0f))+(ErfD*powf(Tmp,4.0f)),4.0f));
        erfY = erff(Tmp);

        if (erfX < 0.0f)
            erfY = -1.0f * erfY;

        PhiDen2 = (1.0f + erfY) / 2.0f;
        p = (PhiNum / (sigma * (PhiDen1 - PhiDen2)));

        nichePoly.v[i + 1].x = x;
        nichePoly.v[i + 1].y = p;
        //printf("vertice %d -> x-%f   y-%f\n",i+1,NichePoly->v[i+1].x, NichePoly->v[i+1].y );
        x = x - Step;

        //printf("erff(Tmp) = %f", erfY);
    }

    nichePoly.v[nPoints + 2].x = nichePoly.v[nPoints + 1].x;
    nichePoly.v[nPoints + 2].y = 0.0f;
    //printf("vertice %d -> x-%f   y-%f\n\n",nPoints+2,NichePoly->v[nPoints+2].x, NichePoly->v[nPoints+2].y );
}

void Simulation::carrega_founders(const char *founders_input, vector<Specie> &founders){
    Dispersion dispersionCapacity;
    array<NicheValue, NUM_ENV_VARS> niche;
    uint cellIdx;
    float specieGrowth;

    FILE *src = fopen(founders_input, "r");
    if (src == NULL){
        perror(RED("Erro ao abrir SpecieData.txt"));
        exit(intException(Exceptions::fileException));
    }

    int i;
    fscanf(src, "%*[^\n]\n"); //pula primeira linha
    for (i = 0; i < Configuration::NUM_FOUNDERS; i++){
        if (feof(src))
            break;
        //lê valores do nicho e de capacidade de dispersão
        fscanf(src, "%f %f %f %f", &niche[0].minimum, &niche[0].maximum, &niche[1].minimum, &niche[1].maximum);
        fscanf(src, "%f %f %f", &dispersionCapacity.Geo, &dispersionCapacity.Topo, &dispersionCapacity.River);
        fscanf(src, "%u", &cellIdx);
        fscanf(src, "%f", &specieGrowth);
        fscanf(src, "\n");

        //founders[i] = *new Specie(niche, dispersionCapacity, cellIdx);
        founders.emplace_back(niche, dispersionCapacity, cellIdx, specieGrowth);
        //printf("geidisp: %f\n", dispersionCapacity.Geo);
    }

    if (i < Configuration::NUM_FOUNDERS){
        printf(LGTYEL(BOLD("\n\tATENÇÃO, numero de founders em %s insuficiente\n")), founders_input);
        printf(LGTYEL(BOLD("\tReplicando founders para tamanho necessário.\n\t")));
        int num_lidos = i;
        for (i; i < Configuration::NUM_FOUNDERS; i++){
            //founders[i] = *new Specie(founders[i % num_lidos]);
            founders.emplace_back(founders[i % num_lidos].niche,
                                  founders[i % num_lidos].dispCap,
                                  founders[i % num_lidos].cellsPopulation.begin()->first,
                                  founders[i % num_lidos].growth);
        }
    }

    fclose(src);
}

void Simulation::calcK(int timestep){
    for (int cellIdx = 0; cellIdx < Configuration::MAX_CELLS; cellIdx++)
    {
        auto NPP = Cell::NPPs[timestep][cellIdx];
        auto area = Cell::area[cellIdx];
        Cell::current_K.at(cellIdx) = (NPP * area) / 50000;
    }
}

} // namespace SimEco