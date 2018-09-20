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


int main(int argc,char const *argv[]){
    Simulation *simulacao;
    Grid *grid;

    Specie *founders = (Specie *)malloc(sizeof(Specie) * NUM_TOTAL_SPECIES); /*vetor de classes */
    FILE *historicoClimas_input;
    FILE *cellsConectivity_input;
    FILE *founders_input;

    /*  *************IMPORTANTE**************
        ~~~~~NOTA: O que percebi:
        - o Arquivo 'DummyHex2566 - Coords and CLim.txt'  contém as coordenadas e variaveis ( temp e precp) do tempo zero
            então podemos usar isso para obter as coordenadas (explicação do pq nao usar as variaveis a seguir).

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

    //falta carregar conectividade das celulas e serie temporal das celulas


    grid = new Grid();
    grid->addSpecies(founders, NUM_TOTAL_SPECIES);


    simulacao = new Simulation(*grid, founders);
    
    
    //cout<<"sizeof(Cell): "<<sizeof(Cell)<<" bytes\n";


    printf(LGTGRN("Simulação Criada\n"));
    

    return 0;
}

