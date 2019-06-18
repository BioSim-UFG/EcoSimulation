#ifndef VISUAL_HELPER_H
#define VISUAL_HELPER_H
#include <string>

using namespace std;

typedef struct SIM_INFO{
	string Name;
	string SavePath;
	unsigned int NUM_CELLS;
	unsigned int NUM_FOUNDERS;
	unsigned int TIMESTEPS;

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

#endif