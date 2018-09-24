#ifndef CELL_H
#define CELL_H

#include "SimTypes.h"


using namespace std;

namespace SimEco
{

    class Cell{

        public:

        /* coloquei um vetor da classe ( nao objeto) para melhor acesso e perfomance,
         e também por ser assim que os dados são acessados e tratados na GPU*/
         
        /*aqui vai ser um vetor de vetores, onde cada subvetor tem os dados de min/max e NPP
		  de um tempo i de todas as celulas, ou seja, 
		  temos uma matriz de N tempos linhas, e K celulas colunas.*/
        static Climate **cell_climates;
        //static const size_t num_cells = NUM_TOTAL_CELLS;

        //constexpr static uint MaxCapacity = 1;
        //Specie **speciesInside; //vetor de ponteiros
        //u_int numSpecies;    //quantidade de Especies nessa celula

        //retorna referencia para ponteiro do vetor (vetor de climas do tempo indicado) --> http://www.cplusplus.com/forum/general/85183/
        static inline Climate *getTimeClimates(uint timeStepIndex){
            return Cell::cell_climates[timeStepIndex];
        }

        //void addSpecie(Specie *specie);
        //void delSpecie(uint index);
        Cell();
        ~Cell();

};


}

#endif