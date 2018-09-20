#include "SimTypes.h"
#include "Cell.h"
#include "Grid.h"
#include "Simulation.h"
#include "Specie.h"
#include "color.h"

#include <iostream>

using namespace SimEco;


void carrega_founders(Specie founders[], FILE *src){
    Dispersion dispersionCapacity;
    array<NicheValue, NUM_ENV_VARS> niche;

    fscanf(src, "%*[^\n]\n");  //pula primeira linha
	for(int i=0; i<NUM_TOTAL_SPECIES; i++){

        //lê valores do nicho e de capacidade de dispersão
        fscanf(src, "%f %f %f %f", &niche[0].minimum, &niche[0].maximum, &niche[1].minimum, &niche[1].maximum);
        fscanf(src, "%f %f %f", &dispersionCapacity.Geo, &dispersionCapacity.Topo, &dispersionCapacity.River);

        founders[i] = *new Specie(dispersionCapacity, niche);
	}
}

void carrega_CellClimate(FILE *minTemp_src, FILE *maxTemp_src, FILE *minPptn_src, FILE *maxPptn_src, FILE *NPP_src, 
						size_t num_cells, size_t timeSteps)
{

	fscanf(minTemp_src, "*[^\n]\n"); //ignora a primeira linha
	fscanf(maxTemp_src, "*[^\n]\n"); //ignora a primeira linha
	fscanf(minPptn_src, "*[^\n]\n"); //ignora a primeira linha
	fscanf(maxPptn_src, "*[^\n]\n"); //ignora a primeira linha
	fscanf(NPP_src, "*[^\n]\n"); //ignora a primeira linha

	for(size_t i=0; i < num_cells; i++){

		for(size_t j=timeSteps-1; j>=0; j--){
			fscanf(minTemp_src, "%f", &(Cell::cell_climates[i][j].envValues[0].minimum) );
		}
    }
}


int main(int argc,char const *argv[]){
    Simulation *simulacao;
    Grid *grid;

    Specie *founders = (Specie *)malloc(sizeof(Specie) * NUM_TOTAL_SPECIES); /*vetor de classes */
    FILE *historicoClimas_input;
    FILE *cellsConectivity_input;
    FILE *founders_input;

    /*  *************IMPORTANTE**************
        ~~~~~NOTA: O que percebi:

        - o  arquivo 'xPLASIM.All3DMats.Single.Zip.Stream' contem as series climaticas temporais, 
        que ao passar por o programa de interpolação, gera os arquivos da pasta output, isto é:
            DummyHex2566 - Output - MaxPPTN.txt, 
            DummyHex2566 - Output - MinPPTN.txt,
            DummyHex2566 - Output - MaxTemp.txt,
            DummyHex2566 - Output - MinTemp.txt,
            DummyHex2566 - Output - NPP.txt

        que poderão ser usadas para as varivaeis climaticas das celulas

    */

    founders_input = fopen("../../input/SpecieData.txt", "r");
    carrega_founders(founders, founders_input);
	fclose(founders_input);

    //falta carregar conectividade das celulas e serie temporal das celulas
	int numero_de_passos = 51; // 50 tempos + tempo zero
	//Cell::cell_climates = new Climate*[numero_de_passos];
	Cell::cell_climates =(Climate**) malloc(sizeof(Climate*) * numero_de_passos);//  new Climate *[numero_de_passos];

	grid = new Grid();
    grid->addSpecies(founders, NUM_TOTAL_SPECIES);


    simulacao = new Simulation(*grid, founders);
    
    
    //cout<<"sizeof(Cell): "<<sizeof(Cell)<<" bytes\n";


    printf(LGTGRN("Simulação Criada\n"));
    

    return 0;
}

