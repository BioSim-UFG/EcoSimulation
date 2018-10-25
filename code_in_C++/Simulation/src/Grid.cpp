#include "Grid.h"
#include "Cell.h"
#include "Specie.h"
#include "Helper.h"
#include <string.h>
//#include <execinfo.h>

#include "decompressData.h"

#define VECTOR_BLOCK_SIZE 20


using namespace std;

namespace SimEco{

	Connectivity *Grid::connectivityMatrix = NULL;
	MatIdx_2D *Grid::indexMatrix = NULL;
	unordered_map<uint, uint> Grid::indexMap;
	u_int Grid::matrixSize = 0;

	//Cell *Grid::cells;
	int Grid::cellsSize = 0;

	Grid::Grid(uint num_cells, uint num_species){
		//cells = new Cell[num_cells];
		cellsSize = num_cells;
		//species = (Specie *)malloc( sizeof(Specie) * num_species );
		//species = new Specie[num_species];
		//speciesSize = num_species;
		//species.resize(num_species);
	}

	Grid::~Grid(){
		//printf(YEL("destrutor de Grid - end. %p\n"), this);
		//delete[] species;
		//species = NULL;
	}

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

	/*
	void Grid::setFounders(Specie sp[], size_t sp_size){
		int i = 0;
		//Cell &celula = this->cells[positions[i]];
		if(sp_size > speciesSize){
			//species = (Specie *)realloc(species, sizeof(Specie) * sp_size);
			delete[] species;
			species = new Specie[sp_size];
		}
		speciesSize = sp_size;

		while(i < sp_size){
			//celula.speciesInside[celula.numSpecies+i] = &sp[i];	//coloca o ponteiro da especie dentro do array 'speciesInside' da celula
			species[i] = sp[i];
			i++;
		}
		//celula.numSpecies += sp_size;
	}
	*/

	//lê a serie climatica das celulas, e retorna o número de celulas lidas
	int Grid::setCellsClimate(const char *minTemp_src, const char *maxTemp_src, const char *minPptn_src, const char *maxPptn_src, const char *NPP_src,
							 size_t timeSteps)
	{
		/*
		FILE *minTemp_arq = fopen(minTemp_src, "r");
		FILE *maxTemp_arq = fopen(maxTemp_src, "r");
		FILE *minPptn_arq = fopen(minPptn_src, "r");
		FILE *maxPptn_arq = fopen(maxPptn_src, "r");
		FILE *NPP_arq = fopen(NPP_src, "r");

		if (minTemp_arq == NULL){ printf(RED("Erro ao abrir %s\n"), minTemp_src); 	exit( intException(Exceptions::fileException) ); }
		if (maxTemp_arq == NULL){ printf(RED("Erro ao abrir %s\n"), maxTemp_src); 	exit( intException(Exceptions::fileException) ); }
		if (minPptn_arq == NULL){ printf(RED("Erro ao abrir %s\n"), minPptn_src); 	exit( intException(Exceptions::fileException) ); }
		if (maxPptn_arq == NULL){ printf(RED("Erro ao abrir %s\n"), maxPptn_src); 	exit( intException(Exceptions::fileException) ); }
		if (NPP_arq == NULL){ printf(RED("Erro ao abrir %s\n"), NPP_src); 		exit( intException(Exceptions::fileException) ); }

		long int i, j;

		fscanf(minTemp_arq, "%*[^\n]\n"); //ignora a primeira linha
		fscanf(maxTemp_arq, "%*[^\n]\n"); //ignora a primeira linha
		fscanf(minPptn_arq, "%*[^\n]\n"); //ignora a primeira linha
		fscanf(maxPptn_arq, "%*[^\n]\n"); //ignora a primeira linha
		fscanf(NPP_arq, "%*[^\n]\n");     //ignora a primeira linha

		for (i = 0; i < Configuration::MAX_CELLS; i++){
			
			if (feof(minTemp_arq) || feof(maxTemp_arq) || feof(minPptn_arq) || feof(maxPptn_arq) || feof(NPP_arq) )
				break;

			for (j = timeSteps - 1; j >= 0; j--){
				fscanf(minTemp_arq, "%f", &(Cell::cell_climates[j][i].envValues[climVar::Temp].minimum));
				fscanf(maxTemp_arq, "%f", &(Cell::cell_climates[j][i].envValues[climVar::Temp].maximum));
				fscanf(minPptn_arq, "%f", &(Cell::cell_climates[j][i].envValues[climVar::Pptn].minimum));
				fscanf(maxPptn_arq, "%f", &(Cell::cell_climates[j][i].envValues[climVar::Pptn].maximum));
				fscanf(NPP_arq, "%f", &(Cell::cell_climates[j][i].NPP));
			}
			fscanf(minTemp_arq, "%*[^\n]\n");
			fscanf(maxTemp_arq, "%*[^\n]\n");
			fscanf(minPptn_arq, "%*[^\n]\n");
			fscanf(maxPptn_arq, "%*[^\n]\n");
			fscanf(NPP_arq, "%*[^\n]\n");
		}
		*/
		
		FILE *minTemp_arq = fopen(minTemp_src, "rb");
		FILE *maxTemp_arq = fopen(maxTemp_src, "rb");
		FILE *minPptn_arq = fopen(minPptn_src, "rb");
		FILE *maxPptn_arq = fopen(maxPptn_src, "rb");
		FILE *NPP_arq = fopen(NPP_src, "rb");

		if (minTemp_arq == NULL){ printf(RED("Erro ao abrir %s\n"), minTemp_src); 	exit( intException(Exceptions::fileException) ); }
		if (maxTemp_arq == NULL){ printf(RED("Erro ao abrir %s\n"), maxTemp_src); 	exit( intException(Exceptions::fileException) ); }
		if (minPptn_arq == NULL){ printf(RED("Erro ao abrir %s\n"), minPptn_src); 	exit( intException(Exceptions::fileException) ); }
		if (maxPptn_arq == NULL){ printf(RED("Erro ao abrir %s\n"), maxPptn_src); 	exit( intException(Exceptions::fileException) ); }
		if (NPP_arq == NULL){ printf(RED("Erro ao abrir %s\n"), NPP_src); 		exit( intException(Exceptions::fileException) ); }

		int i, j;
		int nTimeSteps, nCells, temp;

		//lê a quantidade de timeSteps(linhas) que a(s) stream(s) têm
		bool errorFlag=false;
		fread(&nTimeSteps, sizeof(int), 1, minTemp_arq);
		fread(&temp, sizeof(int), 1, maxTemp_arq);		if(temp!=nTimeSteps) errorFlag = true;
		fread(&temp, sizeof(int), 1, minPptn_arq);		if(temp!=nTimeSteps) errorFlag = true;
		fread(&temp, sizeof(int), 1, maxPptn_arq);		if(temp!=nTimeSteps) errorFlag = true;
		fread(&temp, sizeof(int), 1, NPP_arq);			if(temp!=nTimeSteps) errorFlag = true;
		
		if(errorFlag){
			printf(RED("quantidade de timeSteps diferem entre arquivos de clima das Células"));
			exit(intException(Exceptions::configurationException));
		}

		//pega o numero de timeSteps minimo, entre o obtido no arquivo stream e o passado como argumento da função.
		nTimeSteps = min((size_t)nTimeSteps, timeSteps);

		for (i = 0; i < nTimeSteps; i++){
			
			if (feof(minTemp_arq) || feof(maxTemp_arq) || feof(minPptn_arq) || feof(maxPptn_arq) || feof(NPP_arq) )
				break;

			fread(&nCells, sizeof(int), 1, minTemp_arq);
			fread(&temp, sizeof(int), 1, maxTemp_arq);		if(temp!=nCells) errorFlag = true;
			fread(&temp, sizeof(int), 1, minPptn_arq);		if(temp!=nCells) errorFlag = true;
			fread(&temp, sizeof(int), 1, maxPptn_arq);		if(temp!=nCells) errorFlag = true;
			fread(&temp, sizeof(int), 1, NPP_arq);			if(temp!=nCells) errorFlag = true;
		
			if(errorFlag){
				printf(RED("quantidade de celulas no timeStep %d diferem entre arquivos de clima das Células"), i);
				exit(intException(Exceptions::configurationException));
			}

			for (j = 0; j < nCells; j++){
				fread(&(Cell::cell_climates[i][j].envValues[climVar::Temp].minimum), sizeof(float), 1, minTemp_arq);
				fread(&(Cell::cell_climates[i][j].envValues[climVar::Temp].maximum), sizeof(float), 1, maxTemp_arq);
				fread(&(Cell::cell_climates[i][j].envValues[climVar::Pptn].minimum), sizeof(float), 1, minPptn_arq);
				fread(&(Cell::cell_climates[i][j].envValues[climVar::Pptn].maximum), sizeof(float), 1, maxPptn_arq);
				fread(&(Cell::cell_climates[i][j].NPP), sizeof(float), 1, NPP_arq);

				Cell::cell_climates[i][j].K = Cell::calcK(Cell::cell_climates[i][j].NPP);

				//forçando a mandar commit
			}

		}
		
		fclose(minTemp_arq);
		fclose(maxTemp_arq);
		fclose(minPptn_arq);
		fclose(maxPptn_arq);
		fclose(NPP_arq);

		//return i;
		return nCells;
	}

	//Lê dos arquivos comprimidos de conectividade das celulas.
	/* Consome MUITA memória (~200mb por enquanto), mas apenas momentaneamente, depois a libera;*/
	int Grid::setCellsConnectivity(const char *geoConectivity_src, const char *topoConectivity_src, const char *riversConectivity_src){
		byte *geobuffer = NULL, *topobuffer = NULL, *riversbuffer = NULL;
		byte *geobuffer_start, *topobuffer_start, *riversbuffer_start;	//ponteiros para o inicio do buffer
		size_t geoSize = 0, topoSize = 0, riversSize = 0;

		u_int compressedMat_size = 0;

		FILE *geo_arq = fopen(geoConectivity_src,"rb");
		FILE *topo_arq = fopen(topoConectivity_src,"rb");
		FILE *rivers_arq = fopen(riversConectivity_src,"rb");

		if(geo_arq == NULL){ printf(RED("Erro ao abrir arquivo %s\n"), geoConectivity_src); 		exit( intException(Exceptions::fileException) );}
		if(topo_arq == NULL){ printf(RED("Erro ao abrir arquivo %s\n"), topoConectivity_src); 		exit( intException(Exceptions::fileException) );}
		if(rivers_arq == NULL){ printf(RED("Erro ao abrir arquivo %s\n"), riversConectivity_src); 	exit( intException(Exceptions::fileException) );}

		printf("\n");
		printf(BLU("\tDescomprimindo Streams")); fflush(stdout);
		int decompressRet1, decompressRet2, decompressRet3;

		printf(SetBOLD SetForeWHT "\t   [");		//imprimindo barra de progresso
		decompressRet1 = my_decompress(geo_arq, &geobuffer, &geoSize);			printf("#################"); fflush(stdout);
		decompressRet2 = my_decompress(topo_arq, &topobuffer, &topoSize);		printf("#################"); fflush(stdout);
		decompressRet3 = my_decompress(rivers_arq, &riversbuffer, &riversSize);	printf("################]\n" RESETTEXT); fflush(stdout);
		
		if (decompressRet1 != Z_OK || decompressRet2 != Z_OK || decompressRet3 != Z_OK){
			printf(RED("\tErro ao descomprimir streams!"));
			fclose(geo_arq); fclose(topo_arq); fclose(rivers_arq);
			exit( intException(Exceptions::decompStrException) );
		}

		fclose(geo_arq);
		fclose(topo_arq);
		fclose(rivers_arq);
		geobuffer_start = geobuffer;
		topobuffer_start = topobuffer;
		riversbuffer_start = riversbuffer;




		/********termina a descompenssão, e começa a consumir os buffers (ja descomprimidos)*********/

		int num_cells = ((int*)geobuffer)[0];
		if( num_cells != ((int*)topobuffer)[0] || num_cells != ((int*)riversbuffer)[0]){
			printf(BOLD(LGTYEL("AVISO! numero de células nos arquivos de conectividade diferem. Utilizando o menor")));
			num_cells = min(((int *)topobuffer)[0], num_cells);
			num_cells = min(((int *)riversbuffer)[0], num_cells);
			printf("Numero de células: %i", num_cells);
		}
		geobuffer += sizeof(int);       //pula/descarta o primeiro inteiro do buffer
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

		int block_size = (num_cells*2);		//tamanho do bloco (quantidade de elementos) que será realocado quando necessário
		int blocksAllocated = 0;


		printf(BLU("\tCompactando matriz esparsa ")); fflush(stdout);
		for(int i=0; i<num_cells; i++){
			//lê/descarta int que possui número de colunas
			geobuffer += sizeof(int); topobuffer += sizeof(int); riversbuffer += sizeof(int);

			float *geoConn = (float*)geobuffer;		//"converte" o array de bytes para um array de floats
			float *topoConn = (float*)topobuffer;
			float *riversConn = (float*)riversbuffer;

			for(int j=0; j<num_cells; j++){
				//se a conectividade de todos os tipos forem despreziveis
				if(geoConn[j] < connThreshold && topoConn[j] < connThreshold && riversConn[j] < connThreshold)
					continue;
				
				// se o numero de elementos alocados for insuficiente, aloca mais um bloco
				while( blocksAllocated*block_size <= compressedMat_size){
					blocksAllocated++;
					connectivityMatrix = (Connectivity *)realloc(connectivityMatrix, (blocksAllocated * block_size) * sizeof(Connectivity));
					indexMatrix = (MatIdx_2D *)realloc(indexMatrix, (blocksAllocated * block_size) * sizeof(MatIdx_2D));
					if (connectivityMatrix == NULL || indexMatrix == NULL){
						perror(RED("Erro ao realocar memoria!"));
						exit( intException(Exceptions::memRealocException));
					}
				}
				
				connectivityMatrix[compressedMat_size] = {geoConn[j], topoConn[j], riversConn[j]};
				indexMatrix[compressedMat_size] = {i, j};

				if( indexMap.count( (uint)i) == 0 ){
					indexMap.insert({(uint)i, (uint)compressedMat_size});
					//printf("mapa: %u ---> %u", i, indexMap.at(i));fflush(stdout);
				}

				compressedMat_size++;
			}

			geobuffer+=sizeof(float)*num_cells; topobuffer+=sizeof(float)*num_cells; riversbuffer+=sizeof(float)*num_cells;


			//imprimindo barra de progresso
			printf(BLU("\r\tCompactando matriz esparsa "));
			printf(SetForeWHT SetBOLD);
			printf("[");
			for(int k=0; k<50;k++){
				if( ((double)k/50)  <= (double)i/num_cells ) printf("#");
				else printf(" ");
			}
			printf("]");
			printf(RESETTEXT);
			fflush(stdout);
		}

		free(geobuffer_start);
		free(topobuffer_start);
		free(riversbuffer_start);

		//realloca matriz compactada para tamanho certo/preciso
		connectivityMatrix = (Connectivity *)realloc(connectivityMatrix, compressedMat_size * sizeof(Connectivity));
		indexMatrix = (MatIdx_2D *)realloc(indexMatrix, compressedMat_size * sizeof(MatIdx_2D));

		Grid::matrixSize = compressedMat_size;


		//printf(CYN("\n\tnumero de entradas no map = %lu"), indexMap.size() );
		printf(CYN("\n\t\t\t\t    %i elementos removidos, tamanho final=%i\n"), (num_cells * num_cells) - Grid::matrixSize, Grid::matrixSize);
		fflush(stdout);
		return num_cells;
	}

}