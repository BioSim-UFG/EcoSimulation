#include "SimTypes.h"
#include "Cell.h"
#include "Grid.h"
#include "Simulation.h"
#include "Specie.h"

#include "color.h"


#include <iostream>
#include <chrono>

using namespace SimEco;


void carrega_founders(const char *founders_input, Specie founders[]);


//no futuro podemos usar argumentos (argv) para passar o nome do arquivo de founders
int main(int argc,char const *argv[]){
	if(argc < 3){
		printf(RED("Número de argumentos inválido,") GRN("formato correto: ./<Executavel> <nome_da_simulacao> <numero_de_passos>\n"));
		exit(4);
	}
	
	Simulation *simulacao;
	Grid *grid;
	//Cell *celulas;	/* vetor de celulas */
	Specie *founders = new Specie[NUM_FOUNDERS]; /*vetor de classes */
	//Specie *founders = (Specie *)malloc(sizeof(Specie) * NUM_FOUNDERS);

	auto start_clock = chrono::high_resolution_clock::now();


	/*******INICIALIZANDO ESPECIES FOUNDERS*********/
	
	printf(GRN("Lendo especies Founders ...")); fflush(stdout);
	carrega_founders("../../input/SpecieData.txt" , founders);
	printf(BOLD(LGTGRN("OK!\n"))); fflush(stdout);

	/*****************INICIALIZANDO GRID *****************/

		/***** lendo Serie Climatica das Celulas *****/

	int numero_de_timeSteps = atoi(argv[2])+1; // 50 tempos + tempo zero
	Cell::cell_climates = new Climate *[numero_de_timeSteps];
	for(size_t i=0; i<numero_de_timeSteps; i++)
		Cell::cell_climates[i] = new Climate[MAX_CELLS];

	printf(GRN("Lendo serie Climatica ...")); fflush(stdout);
	int celulas_lidasClima = Grid::setCellsClimate("../../output/DummyHex2566 - Output - MinTemp.txt", 		//climas de todas as celulas em todos os tempos
												   "../../output/DummyHex2566 - Output - MaxTemp.txt",
												   "../../output/DummyHex2566 - Output - MinPPTN.txt",
												   "../../output/DummyHex2566 - Output - MaxPPTN.txt",
												   "../../output/DummyHex2566 - Output - NPP.txt", numero_de_timeSteps);
	printf(BOLD(LGTGRN("OK!\n"))); fflush(stdout);


		/***** lendo matriz de adjacencia de Conectividade das Celulas *****/

	printf(GRN("Lendo conectividade das celulas ..."));	fflush(stdout);		//CONSOME muita memória temporariamente (200mb)
	int celulas_lidasConnec = Grid::setCellsConnectivity("../../input/DummyHex2566 - Connectances - Geo.Single.Zip.Stream", 
													"../../input/DummyHex2566 - Connectances - Topo.Single.Zip.Stream", 
													"../../input/DummyHex2566 - Connectances - Rivers.Single.Zip.Stream");
	printf(BOLD(LGTGRN("\tOK!\n")));	fflush(stdout);



	//verificando se o numero de celulas é igual/batem, se não, pega o menor ( exclui as celulas extras)
	u_int num_cells = min(celulas_lidasClima, celulas_lidasConnec);
	if(celulas_lidasClima != celulas_lidasConnec){
		printf(BOLD(LGTYEL("AVISO! numero de celulas lidas em Serie Climatica e conectividade diferem. Utilizando o menor.")));

		printf(" novo numero de celulas = %u\n", num_cells);
		//remove as celulas extras
		/*for(int i=num_cells-1; i < celulas_lidasClima-1; i++){
			delete &Grid::cells[i];
		}*/

		u_int i=Grid::matrixSize-1;
		while (Grid::indexMatrix[i].i > num_cells-1)	//acha o índice  da ultima linha da matriz, que é menor que num_cells
			i--;
		
		if(i != Grid::matrixSize-1){
			i+=1;
			Grid::connectivityMatrix = (Connectivity *)realloc(Grid::connectivityMatrix, i * sizeof(Connectivity));
			Grid::indexMatrix = (MatIdx_2D *)(Connectivity *)realloc(Grid::indexMatrix, i * sizeof(MatIdx_2D));
			Grid::matrixSize = i;
		}
	}

	
	/**************************INICIANDO SIMULAÇÃO*******************************/
	grid = new Grid(num_cells, NUM_FOUNDERS);

	//coloca os founders em suas celulas
	grid->setFounders(founders, NUM_FOUNDERS);

	//printf("\n * * * * * * * * * * * * * * * * * * * * * * *\n\n"); fflush(stdout);
	printf(LGTGRN(BOLD("Iniciando Simulação \n"))); fflush(stdout);
	
	cout<<BLU("\tCalculando tempo ZERO\n"); fflush(stdout);
	simulacao = new Simulation(*grid, argv[1]);		//calcula o tempo -1 (tempo ZERO)


	/** agora que toda a pré configuração está pronta, vamos rodar mesmo a simulação **/

	simulacao->run(numero_de_timeSteps);	//roda a simulação para N timeSteps -1(tempo zero)



	/***************************FIM DA SIMULAÇÃO*********************************/

	// finaliza contagem de tempo
	auto finish_clock = chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish_clock - start_clock;

	cout<<"\nTempo total: "<< LGTYEL( << elapsed.count() <<" s\n");


	delete[] founders;
	delete[] Cell::cell_climates;
	delete grid;
	delete simulacao;
	free(Grid::connectivityMatrix );
	free(Grid::indexMatrix);

	return 0;
}




void carrega_founders(const char *founders_input, Specie founders[]){
	Dispersion dispersionCapacity;
	array<NicheValue, NUM_ENV_VARS> niche;
	uint cellIdx;

	FILE *src = fopen(founders_input, "r");
	if (src == NULL){
		perror(RED("Erro ao abrir SpecieData.txt"));
		exit(1);    
	}

	int i;
	fscanf(src, "%*[^\n]\n");  //pula primeira linha
	for(i=0; i<NUM_FOUNDERS; i++){
		if(feof(src)) break;
		//lê valores do nicho e de capacidade de dispersão
		fscanf(src, "%f %f %f %f", &niche[0].minimum, &niche[0].maximum, &niche[1].minimum, &niche[1].maximum);
		fscanf(src, "%f %f %f", &dispersionCapacity.Geo, &dispersionCapacity.Topo, &dispersionCapacity.River);
		fscanf(src, "%u", &cellIdx);
		fscanf(src, "\n");

		founders[i] = *new Specie(niche, dispersionCapacity, cellIdx);
	}

	if(i < NUM_FOUNDERS){
		printf(LGTYEL(BOLD("\n\tATENÇÃO, numero de founders em %s insuficiente\n")), founders_input);
		printf(LGTYEL(BOLD("\tReplicando founders para tamanho necessário.\n\t")));
		int num_lidos = i;
		for(i; i<NUM_FOUNDERS; i++){
			founders[i] = *new Specie(founders[i%num_lidos]);
		}
	}	

	fclose(src);
}

