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

		/*********atributos para matriz de conectividade entre todas as celulas***********/
		static Connectivity *connectivityMatrix;   //matriz de conectividade esparça compactada ( ver CUsparse)
		static MatIdx_2D *indexMatrix;		//fala qual a coordenada (i, j) correponsdente a cada índice da matriz compactada. ex: indexMatrix[idx] == {i,j} (se a matriz for 100% nao esparsa, então idx = (i*NUM_COLUNAS)+j)
		static unordered_map<uint, uint> indexMap;	//mapeia indice de linha (normal) para o indice na matriz compactada
		//troquei int por u_int, pois 50k x 50k dá um valor maior que MAX_INT, mas unsinged int aguenta
		static uint matrixSize;


		constexpr static float connThreshold = 0.1f;



		/********atributos para matriz de vizinhança (fisica diretamente) entre as células************/
		static vector<uint> cellsNeighbors;					//matriz de vizinhança esparça compactada ( ver CUsparse)
		static unordered_map<uint, uint> neighborIndexMap;  /**mapeia indice de linha (normal) para o indice na matriz de vizinhança compactada,
															 * ex: para acessar o primeiro vizinho da celula 73 --> cellsNeighbors[ neighborIndexMap[73] ];
															 * como saber que os vizinhos da celula 73 acabaram? R: sabendo onde os vizinhos da proxima celula começam, ex:
															 * int i = neighborIndexMap[73];
															 * int f = neighborIndexMap[74]; ( se a celula 74 não existir nessa lista, isto é, não possui vizinhos, pegue a seguinte, e assim por diante)
															 * while(i<f){
															 * 		um_dos_vizinhos = cellsNeighbors[i];
															 * 		...
															 * 		i++;
															 * }
															 */

		Grid(uint num_cells, uint num_especies);
		~Grid();    
		void setCells(Cell celulas[], size_t size);    //seta as celulas
		void setCellsConnectivity(Connectivity *adjMatrix, size_t size);    //passa a matriz de adjacencia, e lá dentro compacta ela

		//void setFounders(Specie sp[], size_t sp_num);
		
		void addCell(const Cell &novaCelula);
		//void addCells(const array<Cell, Configuration::MAX_CELLS> &novasCelulas);    //pega o vector novasCelulas e copia/passa os elementos para a Grid


		Cell* getCellat(uint index);

		//lê e armazena a serie climatica das celulas, e retorna o número de celulas lidas;
		static int setCellsClimate(const char *minTemp_src, const char *maxTemp_src, const char *minPptn_src, const char *maxPptn_src, const char *NPP_src,
								size_t timeSteps);

		//lê e armazena as coordenadas, que devem estar normalizadas (entre 0-1), e retorna o numero de celulas lidas;
		static int setCellsCoordinates(const char *long_src, const char *lat_src);

		//lê e armazena a area de todas as células, e retorna o número de celulas lidas;
		static int setCellsArea(const char *area_src);

		//lê e armazena os vizinhos de todas as células;
		static void setCellsNeighbors(const char *vizinhos_src);

		//lê e armazena a conectividade de todas as celulas, e retorna o número de celulas lidas;
		static int setCellsConnectivity(const char *geo_src, const char *topo_src, const char *rivers_src);
	};
}

#endif