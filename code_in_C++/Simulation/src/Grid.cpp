#include "Grid.h"
#include "Specie.h"
#include <string.h>
#include <execinfo.h>

#include "decompressData.h"
#include "color.h"

using namespace std;

namespace SimEco{

	Connectivity *Grid::connectivityMatrix = NULL;
	MatIdx_2D *Grid::indexMatrix = NULL;
	map<uint, uint> Grid::mapLine_to_Compact_Idx;
	u_int Grid::matrixSize = 0;

	//Cell *Grid::cells;
	int Grid::cellsSize = 0;

	Grid::Grid(uint num_cells, uint num_species){
		//cells = new Cell[num_cells];
		cellsSize = num_cells;
		species = (Specie *)malloc( sizeof(Specie) * num_species );
		speciesSize = num_species;
	}

	Grid::~Grid(){
		//for(auto &i: species) free(i);
	}

	/*void Grid::setCells(Cell *cells, size_t size){
		memcpy(Grid::cells, cells, sizeof(Cell)*size);
		cellsSize = size;
	}*/

	//recebe e compacta matriz de adjacencia		//depois atualizar e deixar como o load_cellsConnectivy()
	void Grid::setCellsConnectivity(Connectivity *adjMatrix, size_t size){
		u_int pos=0;

		for(int i=0; i<size; i++){
			for(int j=0; j<size; j++){
				//temos um problema: o valor de distancia geologica pode ser descartável, mas de outra não
				if (adjMatrix[i*size + j].Geo < connThreshold && adjMatrix[i*size + j].River < connThreshold && adjMatrix[i*size + j].Topo < connThreshold)
					continue;

				connectivityMatrix[pos] = adjMatrix[i * size + j];
				indexMatrix[pos] = {i, j};
				pos++;
			}
		}
		matrixSize = pos;
	}

	/*Cell* Grid::getCellat(uint index){
		return index<cellsSize ? &cells[index] : nullptr;
	}*/

	void Grid::setFounders(Specie sp[], size_t sp_size){
		int i = 0;
		//Cell &celula = this->cells[positions[i]];
		if(sp_size > speciesSize){
			species = (Specie *)realloc(species, sizeof(Specie) * sp_size);
			speciesSize = sp_size;
		}

		while(i<sp_size){
			//celula.speciesInside[celula.numSpecies+i] = &sp[i];	//coloca o ponteiro da especie dentro do array 'speciesInside' da celula
			species[i] = sp[i];
			i++;
		}
		//celula.numSpecies += sp_size;
	}

	int Grid::setCellsClimate(const char *minTemp_src, const char *maxTemp_src, const char *minPptn_src, const char *maxPptn_src, const char *NPP_src,
							 size_t timeSteps)
	{

		FILE *minTemp_arq = fopen(minTemp_src, "r");
		FILE *maxTemp_arq = fopen(maxTemp_src, "r");
		FILE *minPptn_arq = fopen(minPptn_src, "r");
		FILE *maxPptn_arq = fopen(maxPptn_src, "r");
		FILE *NPP_arq = fopen(NPP_src, "r");

		if (minTemp_arq == NULL){ printf(RED("Erro ao abrir %s\n"), minTemp_src); 	exit(1); }
		if (maxTemp_arq == NULL){ printf(RED("Erro ao abrir %s\n"), maxTemp_src); 	exit(1); }
		if (minPptn_arq == NULL){ printf(RED("Erro ao abrir %s\n"), minPptn_src); 	exit(1); }
		if (maxPptn_arq == NULL){ printf(RED("Erro ao abrir %s\n"), maxPptn_src); 	exit(1); }
		if (NPP_arq == NULL){ printf(RED("Erro ao abrir %s\n"), NPP_src); 		exit(1); }

		int i, j;

		fscanf(minTemp_arq, "%*[^\n]\n"); //ignora a primeira linha
		fscanf(maxTemp_arq, "%*[^\n]\n"); //ignora a primeira linha
		fscanf(minPptn_arq, "%*[^\n]\n"); //ignora a primeira linha
		fscanf(maxPptn_arq, "%*[^\n]\n"); //ignora a primeira linha
		fscanf(NPP_arq, "%*[^\n]\n");     //ignora a primeira linha

		for (i = 0; i < MAX_CELLS; i++){

			for (j = timeSteps - 1; j >= 0; j--){
				fscanf(minTemp_arq, "%f", &(Cell::cell_climates[i][j].envValues[climVar::Temp].minimum));
				fscanf(maxTemp_arq, "%f", &(Cell::cell_climates[i][j].envValues[climVar::Temp].maximum));
				fscanf(minPptn_arq, "%f", &(Cell::cell_climates[i][j].envValues[climVar::Pptn].minimum));
				fscanf(maxPptn_arq, "%f", &(Cell::cell_climates[i][j].envValues[climVar::Pptn].maximum));
				fscanf(NPP_arq, "%f", &(Cell::cell_climates[i][j].NPP));
			}
			fscanf(minTemp_arq, "%*[^\n]\n");
			fscanf(maxTemp_arq, "%*[^\n]\n");
			fscanf(minPptn_arq, "%*[^\n]\n");
			fscanf(maxPptn_arq, "%*[^\n]\n");
			fscanf(NPP_arq, "%*[^\n]\n");

			if (feof(minTemp_arq) || feof(maxTemp_arq) || feof(minPptn_arq) || feof(maxPptn_arq) || feof(NPP_arq) )
				break;
			
		}

		fclose(minTemp_arq);
		fclose(maxTemp_arq);
		fclose(minPptn_arq);
		fclose(maxPptn_arq);
		fclose(NPP_arq);

		return i;
	}

	int Grid::setCellsConnectivity(const char *geoConectivity_src, const char *topoConectivity_src, const char *riversConectivity_src){
		byte *geobuffer = NULL, *topobuffer = NULL, *riversbuffer = NULL;
		size_t geoSize = 0, topoSize = 0, riversSize = 0;

		u_int compressedMat_size = 0;

		FILE *geo_arq = fopen(geoConectivity_src,"rb");
		FILE *topo_arq = fopen(topoConectivity_src,"rb");
		FILE *rivers_arq = fopen(riversConectivity_src,"rb");

		if(geo_arq == NULL){ printf(RED("Erro ao abrir arquivo %s\n"), geoConectivity_src); 		exit(1);}
		if(topo_arq == NULL){ printf(RED("Erro ao abrir arquivo %s\n"), topoConectivity_src); 		exit(1);}
		if(rivers_arq == NULL){ printf(RED("Erro ao abrir arquivo %s\n"), riversConectivity_src); 	exit(1);}

		printf("\n");
		printf(BLU("\tDescomprimindo Streams\n"));
		if( my_decompress(geo_arq, &geobuffer, &geoSize) != Z_OK ||
			my_decompress(topo_arq, &topobuffer, &topoSize) != Z_OK ||
			my_decompress(rivers_arq, &riversbuffer, &riversSize) != Z_OK)
		{
			printf(RED("\tErro ao descomprimir streams!"));
			fclose(geo_arq); fclose(topo_arq); fclose(rivers_arq);
			exit(2);
		}
		fclose(geo_arq);
		fclose(topo_arq);
		fclose(rivers_arq);

		int num_cells = ((int*)geobuffer)[0];
		if( num_cells != ((int*)topobuffer)[0] || num_cells != ((int*)riversbuffer)[0]){
			printf(BOLD(LGTYEL("AVISO! numero de células nos arquivos de conectividade diferem. Utilizando o menor")));
			num_cells = min(((int *)topobuffer)[0], num_cells);
			num_cells = min(((int *)riversbuffer)[0], num_cells);
			printf("Numero de células: %i", num_cells);
		}
		geobuffer += sizeof(int);       
		topobuffer += sizeof(int);
		riversbuffer += sizeof(int);


		/* EXPLICAÇÃO:
			Aqui eu aloco um bloco, de tamanho N, de elementos para a matriz compactada,
			então, caso os elementos já alocados não sejam suficientes (numero de elementos < tamanho atual da matriz compactada),
			é alocado um novo bloco de N elementos, e continua-se a criar a matriz compactada.
			E assim se repete caso necessaŕio ( caso precise de mais elementos)

			No final usa-se o realloc para deixar a memoria alocada igual ao tamanho da matriz, isto é, deixamos alocados apenas 
			o necessário.
		*/	

		int block_size = (num_cells/5);		//tamanho do bloco (quantidade de elementos) que será realocado quando necessário
		int blocksAllocated = 0;

		printf(BLU("\tCompactando matriz esparsa ")); fflush(stdout);
		for(int i=0; i<num_cells; i++){
			//lê/descarta int que possui  número de colunas
			geobuffer += sizeof(int); topobuffer += sizeof(int); riversbuffer += sizeof(int);
			float *geoConn = (float*)geobuffer;
			float *topoConn = (float*)topobuffer;
			float *riversConn = (float*)riversbuffer;

			for(int j=0; j<num_cells; j++){
				//se a conectividade de todos os tipos forem despreziveis
				if(geoConn[j] < connThreshold && topoConn[j] < connThreshold && riversConn[j] < connThreshold)
					continue;
				
				// se o numero de elementos alocados for insuficiente, realoca mais um bloco
				if( blocksAllocated*block_size <= compressedMat_size){
					blocksAllocated++;
					connectivityMatrix = (Connectivity *)realloc(connectivityMatrix, (blocksAllocated * block_size) * sizeof(Connectivity));
					indexMatrix = (MatIdx_2D *)realloc(indexMatrix, (blocksAllocated * block_size) * sizeof(MatIdx_2D));
				}

				connectivityMatrix[compressedMat_size] = {geoConn[j], topoConn[j], riversConn[j]};
				indexMatrix[compressedMat_size] = {i, j};
				mapLine_to_Compact_Idx.insert({(uint)i, (uint)compressedMat_size});
				compressedMat_size++;
			}

			geobuffer+=sizeof(float)*num_cells; topobuffer+=sizeof(float)*num_cells; riversbuffer+=sizeof(float)*num_cells;
		}

		//realloca matriz compactada para tamanho certo/preciso
		connectivityMatrix = (Connectivity *)realloc(connectivityMatrix, compressedMat_size * sizeof(Connectivity));
		indexMatrix = (MatIdx_2D *)realloc(indexMatrix, compressedMat_size * sizeof(MatIdx_2D));

		Grid::matrixSize = compressedMat_size;

		printf(" %i elementos removidos, tamanho final=%i\n", (num_cells * num_cells) - Grid::matrixSize, Grid::matrixSize);
		fflush(stdout);
		return num_cells;
	}

}