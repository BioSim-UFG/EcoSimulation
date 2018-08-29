#include <vector>
#include <array>

#define ENV_NUM_VARS 4
#define NUM_TOTAL_CELLS 20

using namespace std;

namespace SimEco
{
    class Cell;

    typedef struct{
        float maximum;          // Annual maxima per cell, in a given time, for a given variable
        float center;            // Midpoint between max and min
        float minimum;          // Annual minima per cell, in a given time, for a given variable
    }EnvValue, NicheValue;

    typedef struct{
        //usar array se for tamanho fixo, e vector se for dinamico
        array<EnvValue, ENV_NUM_VARS> envValues;    // Temperature and precipitation
        float *NPP;
    }Climate;



    class Cell{
        public:
        int id;
        
        float area;
        Climate climate;        // Environmental variables for each cell

        vector<float> geoConn;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on geographic distance
        vector<float> geoConn;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on topographic heterogeneity
        vector<float> geoConn;              // Degree of connectivity (1 / distance) between this cell and all other cells, based on river network

    };


    

} // SimEco
