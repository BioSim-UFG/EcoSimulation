#include <vector>
#include <array>

#define ENV_NUM_VARS 2
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

    typedef struct{
        //usar array se for tamanho fixo, e vector se for dinamico
        array<EnvValue, ENV_NUM_VARS> envValues;    // Temperature and precipitation -> max and min for both
        float *NPP; //melhor ser um float, ao inves de float*, não? ( float = 32 bis e ponteiro = 64bits)
    }Climate;



    class Cell{
        //variavel da classe, que indica o total de celulas criadas
        static int totalCells=0;

        public:
        int id;
        
        float area;

        /* coloquei um vetor da classe ( nao objeto) para melhor acesso e perfomance,
         e também por ser assim que os dados são acessados e tratados na GPU
         */
        //Climate climate;        // Environmental variables for each cell
        static Climate* cell_climates;  //aqui vai ser um vetor com todos os dados das celulas

        vector<float> geoConn;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on geographic distance
        vector<float> geoConn;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on topographic heterogeneity
        vector<float> geoConn;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on river network

        Cell();
        ~Cell();

        /* melhor nao ter uma função que calcula a suitability de apenas uma celula ( falta de paralelismo)
        // A continuous value of suitability
        //Function GetLocalSuitability(LocalEnv: TEnvValues; Niche: TNicheValues): Single;
        */

        // A continuous value of suitability, é a função que ja fizemos em GPU
        static void getLocalSuitabilities();
    };


    

} // SimEco
