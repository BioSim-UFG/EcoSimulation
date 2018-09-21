#include "SimTypes.h"
#include "Cell.h"
#include "Grid.h"
#include "Simulation.h"
#include "Specie.h"

#include "color.h"


#include <iostream>
#include <chrono>

using namespace SimEco;


void carrega_founders(Specie founders[], FILE *src){
	Dispersion dispersionCapacity;
	array<NicheValue, NUM_ENV_VARS> niche;

	fscanf(src, "%*[^\n]\n");  //pula primeira linha
	for(int i=0; i<NUM_TOTAL_SPECIES; i++){
		//lê valores do nicho e de capacidade de dispersão
		fscanf(src, "%f %f %f %f", &niche[0].minimum, &niche[0].maximum, &niche[1].minimum, &niche[1].maximum);
		fscanf(src, "%f %f %f", &dispersionCapacity.Geo, &dispersionCapacity.Topo, &dispersionCapacity.River);
		fscanf(src, "\n");

		founders[i] = *new Specie(dispersionCapacity, niche);
	}
}




int main(int argc,char const *argv[]){
	Simulation *simulacao;
	Grid *grid;
	Specie *founders = (Specie *)malloc(sizeof(Specie) * NUM_TOTAL_SPECIES); /*vetor de classes */

	FILE *minTemp_src, *maxTemp_src, *minPptn_src, *maxPptn_src, *NPP_src;   //climas de todas as celulas em todos os tempos
	FILE *geoConectivity_src, *topoConectivity_src, *riversConectivity_src;
	FILE *founders_input;


	/*******INICIALIZANDO ESPECIES FOUNDERS*********/
	auto start_clock = chrono::high_resolution_clock::now();
	
	founders_input = fopen("../../input/SpecieData.txt", "r");
	if (founders_input == NULL){
		perror(RED("Erro ao abrir SpecieData.txt"));
		exit(1);    
	}

	printf(GRN("Lendo especies Founders ..."));
	carrega_founders(founders, founders_input);
	printf(BOLD(LGTGRN("OK!\n")));
	fclose(founders_input);

	/*****************INICIALIZANDO GRID *****************/

		/**** lendo Serie Climatica das Celulas ****/

	int numero_de_passos = 51; // 50 tempos + tempo zero
	Cell::cell_climates = new Climate*[NUM_TOTAL_CELLS];
	for(size_t i=0; i<NUM_TOTAL_CELLS; i++){
		Cell::cell_climates[i] = new Climate[numero_de_passos];
	}


	minTemp_src = fopen("../../output/DummyHex2566 - Output - MinTemp.txt", "r");
	if(minTemp_src == NULL){
		perror(RED("Erro ao abrir output/DummyHex2566 - Output - MinTemp.txt"));
		exit(1);
	}
	maxTemp_src = fopen("../../output/DummyHex2566 - Output - MaxTemp.txt", "r");
	if(maxTemp_src == NULL){
		perror(RED("Erro ao abrir output/DummyHex2566 - Output - MaxTemp.txt"));
		exit(1);
	}
	minPptn_src = fopen("../../output/DummyHex2566 - Output - MinPPTN.txt", "r");
	if(minPptn_src == NULL){
		perror(RED("Erro ao abrir output/DummyHex2566 - Output - MinPPTN.txt"));
		exit(1);
	}
	maxPptn_src = fopen("../../output/DummyHex2566 - Output - MaxPPTN.txt", "r");
	if(maxPptn_src == NULL){
		perror(RED("Erro ao abrir output/DummyHex2566 - Output - MaxPPTN.txt"));
		exit(1);
	}
	NPP_src = fopen("../../output/DummyHex2566 - Output - NPP.txt", "r");
	if(NPP_src == NULL){
		perror(RED("Erro ao abrir output/DummyHex2566 - Output - NPP.txt"));
		exit(1);
	}

	printf(GRN("Lendo serie Climatica ..."));
	int celulas_lidas1 = Grid::load_CellsClimate(minTemp_src, maxTemp_src, minPptn_src, maxPptn_src, NPP_src, numero_de_passos);
	printf(BOLD(LGTGRN("OK!\n")));
	fclose(minTemp_src);    fclose(maxTemp_src);
	fclose(minPptn_src);    fclose(maxPptn_src);
	fclose(NPP_src);


	
		/**** lendo matriz de adjacencia de Conectividade das Celulas ****/


	geoConectivity_src = fopen("../../input/DummyHex2566 - Connectances - Geo.Single.Zip.Stream", "rb");
	topoConectivity_src = fopen("../../input/DummyHex2566 - Connectances - Topo.Single.Zip.Stream", "rb");
	riversConectivity_src = fopen("../../input/DummyHex2566 - Connectances - Rivers.Single.Zip.Stream", "rb");
	printf(GRN("Lendo conectividade das celulas ..."));	fflush(stdout);
	int celulas_lidas2 = Grid::load_CellsConnectivity(geoConectivity_src, topoConectivity_src, riversConectivity_src);
	printf(BOLD(LGTGRN("OK!\n")));


	if(celulas_lidas1 != celulas_lidas2){
		printf(BOLD(LGTYEL("AVISO! numero de celulas lidas em Serie Climatica e conectividade diferem. Utilizando o menor.")));
		size_t num_cells = min(celulas_lidas1, celulas_lidas2);

		printf(" novo numero de celulas = %lu\n", num_cells);

		//remove as celulas extras
		for(int i=num_cells-1; i < celulas_lidas1-1; i++){
			delete &Grid::cells[i];
		}
		Grid::cellsSize = num_cells;

		u_int i=Grid::matrixSize-1;
		while (Grid::indexMatrix[i].i > num_cells-1)	//acha o índice  da ultima linha da matriz, que é menor que num_cells
			i--;
		
		if(i != Grid::matrixSize-1){
			i+=1;
			Grid::connectivityMatrix = (Connectivity *)realloc(Grid::connectivityMatrix, i * sizeof(Connectivity));
			Grid::indexMatrix = (matIdx_2D *)(Connectivity *)realloc(Grid::indexMatrix, i * sizeof(matIdx_2D));
			Grid::matrixSize = i;
		}
	}


	/*********INICIANDO SIMULAÇÃO***********/

	grid = new Grid();
	grid->addSpecies(founders, NUM_TOTAL_SPECIES);

	simulacao = new Simulation(*grid, founders);
	
	
	//cout<<"sizeof(Cell): "<<sizeof(Cell)<<" bytes\n";


	printf(LGTGRN("Simulação Criada\n"));

	auto finish_clock = chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed = finish_clock - start_clock;


	cout<<"Tempo total: "<<BOLD( WHT( << elapsed.count() <<" s\n"));

	delete[] Cell::cell_climates;
	delete grid;
	delete simulacao;
	//possivel free(founders); --> depende se vai fazer isso na grid; --> descobri q faz, e deu um eero KABULOSO
	free(Grid::connectivityMatrix );
	free(Grid::indexMatrix);

	return 0;
}

