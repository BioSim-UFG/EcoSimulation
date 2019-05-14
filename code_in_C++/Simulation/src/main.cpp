#include "SimTypes.h"
#include "Cell.h"
#include "Grid.h"
#include "Simulation.h"
#include "Specie.h"
#include "Helper.h"


#include <iostream>


using namespace SimEco;




int main(int argc,char const *argv[]){
	
	//testArgs(argc,argv);
	Configuration* simConf;
	simConf->Configure();

	printf(UNDL("Simulation name") ": %-16.16s  ", simConf->NAME);
	printf(UNDL("TimeSteps") ": %u\n", simConf->TIMESTEPS);
	printf(UNDL("Num Cells") ": %-20u    ", simConf->MAX_CELLS);
	printf(UNDL("Num Founders") ": %u\n", simConf->NUM_FOUNDERS);
	printf(UNDL("Save Path") ": %s\n", simConf->SAVEPATH);

	Simulation *simulacao;
	Grid *grid;
	Cell *cell;

	
	//Specie *founders = (Specie *)malloc(sizeof(Specie) * NUM_FOUNDERS);
	Clock *timeCounter = new Clock();

	timeCounter->Start();
	
	/*****************INICIALIZANDO GRID *****************/

		/***** lendo Serie Climatica das Celulas *****/

	int numero_de_timeSteps = Configuration::TIMESTEPS+1; // 50 tempos + tempo zero
	Cell::cell_climates = new Climate *[numero_de_timeSteps];
	Cell::NPPs.reserve(numero_de_timeSteps);
	for(size_t i=0; i<numero_de_timeSteps; i++){
		Cell::cell_climates[i] = new Climate[Configuration::MAX_CELLS];
		Cell::NPPs[i].reserve(Configuration::MAX_CELLS);
	}
	Cell::area.reserve(Configuration::MAX_CELLS);
	Cell::current_K.reserve(Configuration::MAX_CELLS);
	Cell::current_K.resize(Configuration::MAX_CELLS);	//aqui ocorre o resize tb pq nos outros o resize é feito em outro lugar, ja aqui não.

	
	
	/***** lendo arquivo da seria climatica das celulas**********/
	printf(GRN("Lendo serie Climatica temporal... ")); fflush(stdout);
	/*int celulas_lidasClima = Grid::setCellsClimate("../../output/WHHexGrid600 - Output - MinTemp.txt", "../../output/WHHexGrid600 - Output - MaxTemp.txt",
												   "../../output/WHHexGrid600 - Output - MinPPTN.txt", "../../output/WHHexGrid600 - Output - MaxPPTN.txt",
												   "../../output/WHHexGrid600 - Output - NPP.txt", numero_de_timeSteps);
	*/
	int celulas_lidasClima = Grid::setCellsClimate("../../output/WHHexGrid600 - Output - MinTemp.stream", //climas de todas as celulas em todos os tempos
												   "../../output/WHHexGrid600 - Output - MaxTemp.stream",
												   "../../output/WHHexGrid600 - Output - MinPPTN.stream",
												   "../../output/WHHexGrid600 - Output - MaxPPTN.stream",
												   "../../output/WHHexGrid600 - Output - NPP.stream", numero_de_timeSteps);
	printf(BOLD(LGTGRN("OK!\n"))); fflush(stdout);


	/***** lendo arquivos de coordenadas das celulas**********/
	printf(GRN("Lendo Coordenadas das celulas... "));	fflush(stdout);
	int celulas_lidasCoord = Grid::setCellsCoordinates("../../output/WHHexGrid600 - Output - Longitude.stream",
													  "../../output/WHHexGrid600 - Output - Latitude.stream");
	printf(BOLD(LGTGRN("\tOK!\n")));	fflush(stdout);


	/***** lendo arquivo da area das celulas**********/
	printf(GRN("Lendo Area das celulas... "));	fflush(stdout);
	int celulas_lidasArea = Grid::setCellsArea("../../input/WHHexGrid600 - Area.txt");
	printf(BOLD(LGTGRN("\tOK!\n")));	fflush(stdout);


	/***** lendo arquivo de vizinhança entre as celulas**********/
	printf(GRN("Lendo vizinhas diretas entre celulas... ")); fflush(stdout);
	Grid::setCellsNeighbors("../../input/WHHexGrid600 - GraphNeighbors.txt");
	printf(BOLD(LGTGRN("\tOK!\n")));	fflush(stdout);


	/***** lendo matriz de adjacencia de Conectividade das Celulas *****/
	printf(GRN("Lendo conectividade das celulas... "));	fflush(stdout);		//CONSOME muita memória temporariamente (200mb)
	int celulas_lidasConnec = Grid::setCellsConnectivity("../../input/WHHexGrid600 - Connectances - Geo.Single.Zip.Stream", 
													"../../input/WHHexGrid600 - Connectances - Topo.Single.Zip.Stream", 
													"../../input/WHHexGrid600 - Connectances - Rivers.Single.Zip.Stream");
	printf(BOLD(LGTGRN("\tOK!\n")));	fflush(stdout);



	//verificando se o numero de celulas é igual/batem, se não, pega o menor numero e exclui as celulas extras
	u_int num_cells = min( min(celulas_lidasClima, celulas_lidasConnec), celulas_lidasArea );
	if(num_cells!=celulas_lidasClima || num_cells != celulas_lidasConnec || num_cells!=celulas_lidasArea){
		printf(BOLD(LGTYEL("AVISO! numero de celulas lidas em Serie Climatica, conectividade diferem e Areas. Utilizando o menor.")));

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
	cell->setMaxCells(num_cells);


	/*******INICIALIZANDO ESPECIES FOUNDERS*********/

	grid = new Grid(num_cells, Configuration::NUM_FOUNDERS);

	//printf("\n * * * * * * * * * * * * * * * * * * * * * * *\n\n"); fflush(stdout);
	printf(LGTGRN(BOLD("Iniciando Simulacao \n"))); fflush(stdout);
	
	simulacao = new Simulation(*grid, Configuration::NAME);		//calcula o tempo -1 (tempo ZERO)



	/** agora que toda a pré configuração está pronta, vamos rodar mesmo a simulação **/

	simulacao->run(numero_de_timeSteps);	//roda a simulação para N timeSteps -1(tempo zero)



	/***************************FIM DA SIMULAÇÃO*********************************/

	// finaliza contagem de tempo
	//timeCounter->timerEnd();

	//cout << "\nTempo total: " << LGTYEL(<< timeCounter->elapsedTime() << " s\n");
	cout << "\nTempo total: " << LGTYEL(<< timeCounter->End() << " s\n");

	//grava log num arquivo


	//delete[] founders;
	delete timeCounter;
	delete[] Cell::cell_climates;
	delete grid;
	delete simulacao;
	free(Grid::connectivityMatrix );
	free(Grid::indexMatrix);

	return 0;
}
