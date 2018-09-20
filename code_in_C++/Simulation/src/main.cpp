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
						size_t timeSteps)
{

	fscanf(minTemp_src, "*[^\n]\n"); //ignora a primeira linha
	fscanf(maxTemp_src, "*[^\n]\n"); //ignora a primeira linha
	fscanf(minPptn_src, "*[^\n]\n"); //ignora a primeira linha
	fscanf(maxPptn_src, "*[^\n]\n"); //ignora a primeira linha
	fscanf(NPP_src, "*[^\n]\n"); //ignora a primeira linha

	for(size_t i=0; i < NUM_TOTAL_CELLS; i++){

		for(size_t j=timeSteps-1; j>=0; j--){
			fscanf(minTemp_src, "%f", &(Cell::cell_climates[i][j].envValues[climVar::Temp].minimum) );
            fscanf(maxTemp_src, "%f", &(Cell::cell_climates[i][j].envValues[climVar::Temp].maximum) );
            fscanf(minPptn_src, "%f", &(Cell::cell_climates[i][j].envValues[climVar::Pptn].minimum) );
            fscanf(maxPptn_src, "%f", &(Cell::cell_climates[i][j].envValues[climVar::Pptn].maximum) );
            fscanf(NPP_src, "%f", &(Cell::cell_climates[i][j].NPP) );
		}
        fscanf(minTemp_src, "\n");
        fscanf(maxTemp_src, "\n");
        fscanf(minPptn_src, "\n");
        fscanf(maxPptn_src, "\n");
        fscanf(NPP_src, "\n");
    }
}


int main(int argc,char const *argv[]){
    Simulation *simulacao;
    Grid *grid;

    Specie *founders = (Specie *)malloc(sizeof(Specie) * NUM_TOTAL_SPECIES); /*vetor de classes */
    //FILE *historicoClimas_input;
    FILE *minTemp_src, *maxTemp_src, *minPptn_src, *maxPptn_src, *NPP_src;   //climas de todas as celulas em todos os tempos
    FILE *cellsConectivity_input;
    FILE *founders_input;

    founders_input = fopen("../../input/SpecieData.txt", "r");
    if (founders_input == NULL){
        perror(RED("Nao foi possivel abrir SpecieData.txt"));
        exit(1);    
    }
    carrega_founders(founders, founders_input);
	fclose(founders_input);


    /* Inicializando Células */

	int numero_de_passos = 51; // 50 tempos + tempo zero
	Cell::cell_climates = new Climate*[numero_de_passos];
    for(size_t i=0; i<numero_de_passos; i++){
        Cell::cell_climates[i] = new Climate;
    }

    //falta carregar conectividade das celulas e serie temporal das celulas
    minTemp_src = fopen("../../output/DummyHex2566 - Output - MinTemp.txt", "r");
    maxTemp_src = fopen("../../output/DummyHex2566 - Output - MaxTemp.txt", "r");
    minPptn_src = fopen("../../output/DummyHex2566 - Output - MinPPTN.txt", "r");
    maxPptn_src = fopen("../../output/DummyHex2566 - Output - MaxPPTN.txt", "r");
    NPP_src = fopen("../../output/DummyHex2566 - Output - NPP.txt", "r");

    carrega_CellClimate(minTemp_src, maxTemp_src, minPptn_src ,maxPptn_src, NPP_src, numero_de_passos);

    fclose(minTemp_src);    fclose(maxTemp_src);
	fclose(minPptn_src);    fclose(maxPptn_src);
    fclose(NPP_src);
    
    /*Inicializando Grid */

    grid = new Grid();
    grid->addSpecies(founders, NUM_TOTAL_SPECIES);


    simulacao = new Simulation(*grid, founders);
    
    
    //cout<<"sizeof(Cell): "<<sizeof(Cell)<<" bytes\n";


    printf(LGTGRN("Simulação Criada\n"));
    

    return 0;
}

