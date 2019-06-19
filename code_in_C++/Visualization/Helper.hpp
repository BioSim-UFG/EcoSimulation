#ifndef VISUAL_HELPER_H
#define VISUAL_HELPER_H
#include <string>
#include <boost/filesystem.hpp>

using namespace std;

typedef struct{
    float lon;
    float lat;
}Coord_t;

typedef struct SIM_INFO{
	string Name;
	string SavePath;
	unsigned int NUM_CELLS;
	unsigned int NUM_FOUNDERS;
	unsigned int TIMESTEPS;
	float MaxPopFound;

	string MinTemp_dataSource;
	string MaxTemp_dataSource;
	string MinPPTN_dataSource;
	string MaxPPTN_dataSource;
	string NPP_dataSource;

	string Lat_dataSource;
	string Lon_dataSource;

	string Areas_dataSource;

	string Neighbors_dataSource;
}SimInfo_t;


void read_simInfo(string info_file_path, SimInfo_t *info);
void readCoordinates(vector<Coord_t> *coord_v, string latPath, string lonPath);
void readSimulation_timeSteps(string dir_path, vector<vector<pair<uint,string>>> &timeStep_fileNames);
void readTimeStep(boost::filesystem::path dir_path, vector<vector<pair<uint,string>>> &timeStep_fileNames);

#endif