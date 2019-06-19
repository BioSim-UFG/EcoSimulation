#include "Helper.hpp"
#include <iostream>

//defina para considerar o angulo do globo terrestre (eixo latitude)
//#define NORMALIZE_LATITUDE

using namespace boost::filesystem;

void read_simInfo(string info_file_path, SimInfo_t *info){
	FILE *info_file = fopen(info_file_path.c_str(),"r");

	char atribute_cstr[100];
	char value_cstr[100];

	if(info_file == NULL){
		printf("Não foi possivel abrir o arquivo %s\n", info_file_path.c_str());
		exit(1);
	}

	while(!feof(info_file)){
		fscanf(info_file, "%[^=]s", atribute_cstr); //lê até encontrar '='
		fscanf(info_file,"=");	//descarta o '='
		fscanf(info_file, "%[^\n]s", value_cstr);   //lê até final da linha
		fscanf(info_file,"\n");	//descarta o '\n'

		string atribute = string(atribute_cstr);
		while(atribute.back()==' '){ atribute.pop_back();}	//remove os espaços sobrando

		string value = string(value_cstr);
		while(value.front()==' '){ value.erase(0,1);}	//remove os espaços sobrando
			

		if (atribute.compare("Name")==0){
			info->Name = value;
		}else if(atribute.compare("TimeSteps")==0){
			info->TIMESTEPS = stoi(value);
		}else if(atribute.compare("Num Cells")==0){
			info->NUM_CELLS = stoi(value);
		}else if(atribute.compare("Num Specie Founders")==0){
			info->NUM_FOUNDERS = stoi(value);
		}else if(atribute.compare("Max local specie population")==0){
			info->MaxPopFound = stof(value);
		}else if(atribute.compare("MinTemp Source")==0){
			info->MinTemp_dataSource = value;
		}else if(atribute.compare("MaxTemp Source")==0){
			info->MaxTemp_dataSource = value;
		}else if(atribute.compare("MinPPTN Source")==0){
			info->MinPPTN_dataSource = value;
		}else if(atribute.compare("MaxPPTN Source")==0){
			info->MaxPPTN_dataSource = value;
		}else if(atribute.compare("NPP Source")==0){
			info->NPP_dataSource = value;
		}else if(atribute.compare("Latitude Source")==0){
			info->Lat_dataSource = value;
		}else if(atribute.compare("Longitude Source")==0){
			info->Lon_dataSource = value;
		}else if(atribute.compare("Areas Source")==0){
			info->Areas_dataSource = value;
		}else if(atribute.compare("Neighbors Source")==0){
			info->Neighbors_dataSource = value;
		}
	}
}


void readCoordinates(vector<Coord_t> *coord_v, string latPath, string lonPath){
	int i, n_cells1, n_cells2;
	std::ifstream latFile(latPath, std::ios::binary);
	std::ifstream lonFile(lonPath, std::ios::binary);

	Coord_t coord;

	latFile.read((char*)&n_cells1, sizeof(int));
	lonFile.read((char *)&n_cells2, sizeof(int));
	if(n_cells1 != n_cells2){
		perror("Quantidade de coordenadas de Latitude e Longitude diferem!\n");
		exit(1);
	}

	for(int i=0; i< n_cells1;i++){
		latFile.read((char *)&(coord.lat), sizeof(Coord_t::lat));
		lonFile.read((char *)&(coord.lon), sizeof(Coord_t::lon));
		
		#ifdef NORMALIZE_LATITUDE
		//normalizando o angulo da latitude
		coord.lat = sin(((coord.lat-0.5)*90.0)*3.14159265/180);
		coord.lat = (coord.lat+1)/2;
		#endif
		coord_v->push_back(coord);
	}

	latFile.close();
	lonFile.close();

}

void readAreas(vector<float> *area_v, string areaPath){
	FILE *areaFile = fopen(areaPath.c_str(), "r");
	if(areaFile==NULL){
		perror("Não foi possivel abrir o arquivo de area\n");
		exit(1);
	}

	float area;
	fscanf(areaFile, "%*[^\n]\n");	//descarta a primeira linha

	while(!feof(areaFile)){
		fscanf(areaFile, "%f", &area);
		fscanf(areaFile, "\n");
		area_v->push_back(area);
	}
	fclose(areaFile);
}

void readSimulation_timeSteps(string dir_path, vector<vector<pair<uint,string>>> &timeStep_fileNames){
	path p(dir_path); // p reads clearer than argv[1] in the following code

	try{
		if (exists(p)){ // does p actually exist?
			if (is_regular_file(p)){ // is p a regular file?
				printf("%s is a file\n", p.string().c_str());
			}
			else if (is_directory(p)){ // is p a directory?
				printf("found %s DIR, containing\n", p.string().c_str());
     
				auto dir_it = directory_iterator(p);
				while(dir_it != directory_iterator{}){
					cout << *dir_it << "\tOpening it...\n";
					readTimeStep(dir_it->path(), timeStep_fileNames);
					cout<<"\t\tOk!\n";
					dir_it++;
				}
			}
			else
				printf("%s exists, but is neither a regular file nor a directory\n",p.string().c_str());
		}
		else
			printf("%s does not exist\n",p.string().c_str());
	}
	catch (const filesystem_error &ex){
		printf("%s\n",ex.what() );
	}
}

void readTimeStep(boost::filesystem::path dir_path, vector<vector<pair<uint,string>>> &timeStep_fileNames){
	int timeStep;
	
	if (is_regular_file(dir_path)){ // is p a regular file?
		printf("%s is a file\n", dir_path.string().c_str());
	}else{
		int pos = dir_path.string().find("timeStep");
		int len = string("timeStep").size();
		timeStep = stoi(dir_path.string().substr(pos+len));

		if(timeStep_fileNames.size() < timeStep+1){
			printf("Skipping this folder\n");
			return;
		}

		auto dir_it = directory_iterator(dir_path);
		while(dir_it != directory_iterator{}){
			if(is_regular_file(*dir_it)){
				cout <<"\t\t" <<dir_it->path().filename();

				auto file_name = dir_it->path().filename();
				if(file_name.extension().string().compare(".txt") != 0){
					cout << " -->reading it";

					int s = file_name.string().rfind("Esp");
					int f = file_name.string().rfind("_Time");
					uint specie = stoi(file_name.string().substr(s + 3, f));
					//timeStep_fileDescriptors[timeStep].push_back( {specie, open(dir_it->path().c_str(), O_RDONLY)});	//abre arquivo e armazena o descritor dele
					timeStep_fileNames[timeStep].push_back( {specie, dir_it->path().c_str()} ); //abre arquivo e armazena o descritor dele
				}
				cout<<"\n";
			}

			dir_it++;
		}
		
	}

}