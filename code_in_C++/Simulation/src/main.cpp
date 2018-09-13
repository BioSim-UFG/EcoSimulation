#include "SimTypes.h"
#include "Cell.h"
#include "Grid.h"
#include "Simulation.h"
#include "Specie.h"
#include "color.h"

using namespace SimEco;

int main(int argc,char const *argv[]){
    Simulation *simulacao;
    Grid *grid;
    Specie *founders;
    FILE *entrada;
    


    grid = new Grid();
    simulacao = new Simulation(*grid, founders);
    
    
    


    printf(LGTGRN("Simulação Criada\n"));
    

    return 0;
}

