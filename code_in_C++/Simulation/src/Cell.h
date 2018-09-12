#ifndef CELL_H
#define CELL_H

#include "SimTypes.h"


using namespace std;

namespace SimEco
{

    class Cell{
        //variavel da classe, que indica o total de celulas criadas
        //static unsigned int totalCells;


        public:
        unsigned int id;
        float area;
        int numSpecies = 0;       //quantidade de Especies nessa celula

        /* coloquei um vetor da classe ( nao objeto) para melhor acesso e perfomance,
         e também por ser assim que os dados são acessados e tratados na GPU*/
         
        //Climate climate;        // Environmental variables for each cell
        static Climate* cell_climates;  //aqui vai ser um vetor com todos os dados das celulas
        Connectivity conn;       //cell connectivity with the other cells

        Cell();
        ~Cell();

        /* melhor nao ter uma função que calcula a suitability de apenas uma celula ( falta de paralelismo)
        // A continuous value of suitability
        //Function GetLocalSuitability(LocalEnv: TEnvValues; Niche: TNicheValues): Single;
        */



        //EnvValue é da celula, e NicheValue é da especie( nicho da especie)
        bool IsLocalTolerance(EnvValue LocalEnv, NicheValue Niche);                 // -> mas isso já nao é calculado na suitability ?

        //PARA QUE ESSA COISA?????????????????????????????? :D
        //void getGridClim(int timeStep, pPaleoClimate paleoClimate , pGrid grid , pSngVector envVec, pSngVector NPPVec ); 
    };


    

} // SimEco

#endif