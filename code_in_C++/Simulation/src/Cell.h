#include <vector>
#include <array>

#define NUM_ENV_VARS 2
#define NUM_TOTAL_CELLS 20

using namespace std;

namespace SimEco
{
    class Cell;

    typedef struct{
        float maximum;          // Annual maxima per cell, in a given time, for a given variable
        //float center;            // Midpoint between max and min
        float minimum;          // Annual minima per cell, in a given time, for a given variable
    }EnvValue, NicheValue;

    //EnvValue se refere a celula, e NicheValue à especie
    typedef struct{
        //usar array se for tamanho fixo, e vector se for dinamico
        array<EnvValue, NUM_ENV_VARS> envValues;    // Temperature and precipitation -> max and min for both
        float *NPP; //melhor ser um float, ao inves de float*, não? ( float = 32 bis e ponteiro = 64bits)
    }Climate;

    typedef struct{
        vector<float> Geo;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on geographic distance
        vector<float> Topo;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on topographic heterogeneity
        vector<float> Rivers;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on river network
    }Connectivity;


    class Cell{
        //variavel da classe, que indica o total de celulas criadas
        static unsigned int totalCells=0;


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

        // A continuous value of suitability --> é a função que ja fizemos em GPU, por isso precisa dos dados do nicho das especies
        void getCellSuitabilities(array<NicheValue> niches);

        //EnvValue é da celula, e NicheValue é da especie( nicho da especie)
        bool IsLocalTolerance(EnvValue LocalEnv, NicheValue Niche);                 // -> mas isso já nao é calculado na suitability ?

        //PARA QUE ESSA COISA?????????????????????????????? :D
        //void getGridClim(int timeStep, pPaleoClimate paleoClimate , pGrid grid , pSngVector envVec, pSngVector NPPVec ); 
    };


    

} // SimEco
