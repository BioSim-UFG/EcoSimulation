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
	Configuration SimConf;
	SimConf.Configure();

	printf(UNDL("Simulation name") ": %-16.16s  ", SimConf.NAME);
	printf(UNDL("TimeSteps") ": %u\n", SimConf.TIMESTEPS);
	printf(UNDL("Num Cells") ": %-20u    ", SimConf.MAX_CELLS);
	printf(UNDL("Num Founders") ": %u\n", SimConf.NUM_FOUNDERS);
	printf(UNDL("Save Path") ": %s\n", SimConf.SAVEPATH);

	SimConf.MinTemp_dataSource = string("../../output/WHHexGrid600 - Output - MinTemp.stream");
	SimConf.MaxTemp_dataSource = string("../../output/WHHexGrid600 - Output - MaxTemp.stream");
	SimConf.MinPPTN_dataSource = string("../../output/WHHexGrid600 - Output - MinPPTN.stream");
	SimConf.MaxPPTN_dataSource = string("../../output/WHHexGrid600 - Output - MaxPPTN.stream");
	SimConf.NPP_dataSource = string("../../output/WHHexGrid600 - Output - NPP.stream");

	SimConf.Lon_dataSource = "../../output/WHHexGrid600 - Output - Longitude.stream";
	SimConf.Lat_dataSource = "../../output/WHHexGrid600 - Output - Latitude.stream";

	SimConf.Areas_dataSource = "../../input/WHHexGrid600 - Area.txt";
	SimConf.Neighbors_dataSource = "../../input/WHHexGrid600 - GraphNeighbors.txt";

	

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
	int celulas_lidasClima = Grid::setCellsClimate(SimConf.MinTemp_dataSource.c_str(), SimConf.MaxTemp_dataSource.c_str(),
												   SimConf.MinPPTN_dataSource.c_str(), SimConf.MaxPPTN_dataSource.c_str(),
												   SimConf.NPP_dataSource.c_str(), numero_de_timeSteps); //climas de todas as celulas em todos os tempos
	printf(BOLD(LGTGRN("OK!\n"))); fflush(stdout);


	/***** lendo arquivos de coordenadas das celulas**********/
	printf(GRN("Lendo Coordenadas das celulas... "));	fflush(stdout);
	int celulas_lidasCoord = Grid::setCellsCoordinates( SimConf.Lon_dataSource.c_str(), SimConf.Lat_dataSource.c_str() );
	printf(BOLD(LGTGRN("\tOK!\n")));	fflush(stdout);


	/***** lendo arquivo da area das celulas**********/
	printf(GRN("Lendo Area das celulas... "));	fflush(stdout);
	int celulas_lidasArea = Grid::setCellsArea(SimConf.Areas_dataSource.c_str());
	printf(BOLD(LGTGRN("\tOK!\n")));	fflush(stdout);


	/***** lendo arquivo de vizinhança entre as celulas**********/
	printf(GRN("Lendo vizinhas diretas entre celulas... ")); fflush(stdout);
	Grid::setCellsNeighbors(SimConf.Neighbors_dataSource.c_str());
	printf(BOLD(LGTGRN("\tOK!\n")));	fflush(stdout);


	/***** lendo matriz de adjacencia de Conectividade das Celulas *****/
	/*
	printf(GRN("Lendo conectividade das celulas... "));	fflush(stdout);		//CONSOME muita memória temporariamente (200mb)
	int celulas_lidasConnec = Grid::setCellsConnectivity("../../input/WHHexGrid600 - Connectances - Geo.Single.Zip.Stream", 
													"../../input/WHHexGrid600 - Connectances - Topo.Single.Zip.Stream", 
													"../../input/WHHexGrid600 - Connectances - Rivers.Single.Zip.Stream");
	printf(BOLD(LGTGRN("\tOK!\n")));	fflush(stdout);
	*/



	//verificando se o numero de celulas é igual/batem, se não, pega o menor numero e exclui as celulas extras
	u_int num_cells = min(min(celulas_lidasClima, celulas_lidasCoord), celulas_lidasArea);
	if(num_cells!=celulas_lidasClima || num_cells != celulas_lidasCoord || num_cells!=celulas_lidasArea){
		printf(BOLD(LGTYEL("AVISO! numero de celulas lidas em Serie Climatica, coordenadas  e Areas diferem. Utilizando o menor.")));

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

	//feenableexcept(FE_ALL_EXCEPT & ~FE_INEXACT); // Enable all floating point exceptions but FE_INEXACT

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

	cout << "\nGravando info da simulação\n";
	recordSimulationInfo(*simulacao);

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
