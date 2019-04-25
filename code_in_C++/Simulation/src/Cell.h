#ifndef CELL_H
#define CELL_H

#include <vector>
#include <unordered_set>

#include "SimTypes.h"
#include "Helper.h"

using namespace std;

namespace SimEco
{

    class Cell{

        public:

        // vetor com lista(sets) de ponteiros das espécies que estão presentes na célula
        static vector< unordered_set< Specie* > > speciesPresent; 

        /* coloquei um vetor da classe ( nao objeto) para melhor acesso e perfomance,
         e também por ser assim que os dados são acessados e tratados na GPU*/
         
        /*aqui vai ser um vetor de vetores, onde cada subvetor tem os dados de min/max
		  de um tempo i de todas as celulas, ou seja, 
		  temos uma matriz de N tempos linhas, e K celulas colunas.*/
        static Climate **cell_climates;
        /*A mesma organização/estrutura de cell_climates é aplicada para NPPs*/
        static vector<vector<float>> NPPs;

        static vector<Coord> coordinate;    //coordenada da celula
        static vector<float> area;

        /*K do time step atual (K é varia com o tempo, mas não há dependencia de um K do timeStep anterior)*/
        static vector<float> current_K;     //K = Const value generated based on NPP and area to determine maximum resource availability in cell
        

        //constexpr static uint MaxCapacity = 1;
        //Specie **speciesInside; //vetor de ponteiros
        //u_int numSpecies;    //quantidade de Especies nessa celula

        //retorna referencia para ponteiro do vetor (vetor de climas do tempo indicado) --> http://www.cplusplus.com/forum/general/85183/
        static inline Climate *getTimeClimates(uint timeStepIndex){
            return Cell::cell_climates[timeStepIndex];
        }

        
        //forçando mandar commit
        //static float calcK(float NPP);

        //void addSpecie(Specie *specie);
        //void delSpecie(uint index);
        Cell();
        ~Cell();
        void setMaxCells(unsigned int size);
    };


}

#endif