#include "SimTypes.h"
#include "Cell.h"
#include "Grid.h"
#include "Simulation.h"
#include "Specie.h"
#include "color.h"

using namespace SimEco;



void carrega_founders(Specie founders[], FILE *src){
    Dispersion dispersionCapacity;
    array<NicheValue, NUM_ENV_VARS> niche;

	for(int i=0; i<NUM_TOTAL_SPECIES; i++){

        //lê valores do nicho e de capacidade de dispersão

        founders[i] = *new Specie(dispersionCapacity, niche);
	}
}


int main(int argc,char const *argv[]){
    Simulation *simulacao;
    Grid *grid;

    Specie *founders; /*vetor de classes */
    FILE *historicoClimas_input;
    FILE *cellsConectivity_input;
    FILE *founders_input;



    carrega_founders(founders, founders_input);
    grid = new Grid();
    simulacao = new Simulation(*grid, founders);
    
    
    


    printf(LGTGRN("Simulação Criada\n"));
    

    return 0;
}

