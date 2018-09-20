#ifndef CELL_H
#define CELL_H

#include "SimTypes.h"


using namespace std;

namespace SimEco
{

    class Cell{

        public:
        
        Specie *speciesInside;
        size_t numSpecies = 0;       //quantidade de Especies nessa celula

        /* coloquei um vetor da classe ( nao objeto) para melhor acesso e perfomance,
         e também por ser assim que os dados são acessados e tratados na GPU*/
         
        static Climate* cell_climates;  //aqui vai ser um vetor com todos os dados das celulas de min/max e NPP

        Cell();
        ~Cell();

        //EnvValue é da celula, e NicheValue é da especie( nicho da especie)
        bool IsLocalTolerance(EnvValue LocalEnv, NicheValue Niche);                 // -> mas isso já nao é calculado na suitability ?

};


}

#endif