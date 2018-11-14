#ifndef GRID_H
#define GRID_H

#include "SimTypes.h"
#include "Helper.h"
#include <unordered_map>
#include <vector>

//#define MAX_CELLS 2566
//#define NUM_FOUNDERS 4


using namespace std;

namespace SimEco
{
	class Grid{


	 public:
			
		//Specie *species;
		vector<Specie> species;

		//uint speciesSize;
		
		//static Cell *cells;
		static int cellsSize;

		static Connectivity *connectivityMatrix;   //matriz esparça compactada ( ver CUsparse)
		static  MatIdx_2D *indexMatrix;
		static unordered_map<uint, uint> indexMap;	//mapeia indice de linha (normal) para o indice na matriz compactada
		//troquei int por u_int, pois 50k x 50k dá um valor maior que MAX_INT, mas unsinged int aguenta
		static uint matrixSize;


		constexpr static float connThreshold = 0.1f;

		Grid(uint num_cells, uint num_especies);
		~Grid();    
		void setCells(Cell celulas[], size_t size);    //seta as celulas
		void setCellsConnectivity(Connectivity *adjMatrix, size_t size);    //passa a matriz de adjacencia, e lá dentro compacta ela

		//void setFounders(Specie sp[], size_t sp_num);
		
		void addCell(const Cell &novaCelula);
		//void addCells(const array<Cell, Configuration::MAX_CELLS> &novasCelulas);    //pega o vector novasCelulas e copia/passa os elementos para a Grid


		Cell* getCellat(uint index);

		//lê a serie climatica das celulas, e retorna o número de celulas lidas
		static int setCellsClimate(const char *minTemp_src, const char *maxTemp_src, const char *minPptn_src, const char *maxPptn_src, const char *NPP_src,
								size_t timeSteps);

		//lê a area de todas as células, e retorna o número de celulas lidas;
		static int setCellsArea(const char *area_src);	

		//lê a conectividade de todas as celulas, e retorna o número de celulas lidas
		static int setCellsConnectivity(const char *geo_src, const char *topo_src, const char *rivers_src);
	};
}

#endif