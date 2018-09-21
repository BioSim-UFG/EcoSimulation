#include "Simulation.h"
#include "color.h"
#include <iostream>

namespace SimEco{
    Simulation::Simulation(Grid &grid, Specie founders[], uint foundersPosition[], uint founders_size): _grid(grid), founders(founders), foundersSize(founders_size){
        
        //aqui faz o trabalho de preparação da simulação, usando a(s) especie(s) fundadora(s)

        //coloca os founders em suas celulas
        grid.putSpecies(founders, foundersPosition, NUM_TOTAL_SPECIES);

        cout<<BLU("\tCalculando tempo ZERO\n");
        for(uint i=0; i<founders_size; i++){
            processFounder_timeZero(founders[i]);
        }

    }

    void Simulation::processFounder_timeZero(Specie founder){
        //aqui chama calcFitness ( função da GPU) e espalha o founder;

    }

    float* Simulation::calcSpecieFitness(Specie &specie){

        float StdAreaNoOverlap, StdSimBetweenCenters;
        float MidTol;
        float MinTempTol, MaxTempTol, MinPrecpTol, MaxPrecpTol;
        float MinTempEnv, MaxTempEnv, MinPrecpEnv, MaxPrecpEnv;
        float MidEnv;
        float LocFitness;

        vec_t NichePtns[nSteps + 3]; //pontos do poligono do nicho ( da especie ) ( struct com float x e y)
        poly_t ClipedNichePoly = {NichePtns, nSteps + 3};

        MinTempTol = specie.niche[0].minimum;
        MaxTempTol = specie.niche[0].maximum;
        MinPrecpTol = specie.niche[1].minimum;
        MaxPrecpTol = specie.niche[1].maximum;

        //calcula o fitness dessa especia pra cada celula da grid
        for(uint cellIdx=0; cellIdx < Grid::cellsSize; cellIdx++){

        }


    }
}