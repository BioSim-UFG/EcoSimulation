/**
 * SimVisualization.cpp
 * Purpose: Visualization of a already exeuted EcoSim simulation
 * 
 * @author João Gabriel Silva Fernandes
 * @version 0.3 20/05/2016
 */


#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include <stdbool.h>
#include <GL/glut.h> //inclua a biblioteca glut

#include <vector>
#include <unordered_map>

#include <string>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <thread>

#include "Tools.h"
#include "Helper.h"
#include "Cell_HexaPoly.hpp"

//defina para considerar o angulo do globo terrestre (eixo latitude)
//#define NORMALIZE_LATITUDE

//pode alterar o SCREEN_HEIGHT e SCREEN_WIDTH, já o resto não garanto...
#define SCREEN_HEIGHT 940
#define SCREEN_WIDTH (940*2)
#define LOWEST_RATIO min<double>(SCREEN_HEIGHT, SCREEN_WIDTH)
#define BIGGEST_RATIO max<double>(SCREEN_HEIGHT, SCREEN_WIDTH)

using namespace std;
using namespace boost::filesystem;

//duração de cada timeStep num frame, em milissegundos
int FRAME_DURATION = 500;

GLint current_width = SCREEN_WIDTH, current_height = SCREEN_HEIGHT;

Point_t total_scale = {1.0f, 1.0f};
Point_t total_translade = {0.0f, 0.0f};



int total_timeSteps;
int total_celulas=0;
//vetor de objetos que representam as células na interface gráfica
vector<Cell_HexaPoly> Cells;
vector<RGBAColorf_t> Cells_color_buffer[2];
int active_buffer=0, free_buffer=1;
int curr_timeStep=0;

//registro de populações nas células de cada espécie
//keys: Populations_byTime [time_step] [celula] [specie] = população dessa espécie nesta célula neste determinado timeStep
vector<vector<vector<float> > > Populations_byTime;
float maxPopFound=0.0f;

//keys: Populations[celula][specie] = população dessa espécie nesta célula neste determinado timeStep
vector<vector<float>> Populations;
//lista de arquivos dentro de cada arquivo, timeStep_fileDescriptors[time_step][pair<specie, file_descriptor>]
vector<vector<pair<uint,int>>> timeStep_fileDescriptors;


void swapColorBuffer(){
	int rangemin = 0;
	int rangemax = 2;
	active_buffer = rangemin + ((active_buffer-rangemin + 1) % ( min<int>(sizeof(Cells_color_buffer) / sizeof(RGBAColorf_t),rangemax) - rangemin) );
	free_buffer = rangemin + ((free_buffer - rangemin + 1) % (min<int>(sizeof(Cells_color_buffer) / sizeof(RGBAColorf_t), rangemax) - rangemin));
}

void fillColorBuffer(vector<RGBAColorf_t> &buffer, int timeStep, int specie){

	for(auto &specie_fd: timeStep_fileDescriptors[timeStep]){
		if(specie_fd.first != specie)
			continue;

		FILE *specie_arq = fdopen(specie_fd.second, "rb");


		uint cellId;
		float pop;
		while (!feof(specie_arq)){
			fread(&cellId, sizeof(uint), 1, specie_arq);
			fread(&pop, sizeof(float), 1, specie_arq);

			/*
			if (Populations[cellId].size() < specie + 1)
				Populations[cellId].resize(specie + 1);
			Populations[cellId].at(specie) = pop;
			maxPopFound = max(maxPopFound, pop);
			*/

			float density = pop / maxPopFound;
			//buffer[cellId] = RGBAColorf_t(density, 0, 0, 0);	//para fundo preto
			buffer[cellId] = RGBAColorf_t(1, 1.0f - density, 1.0f - density, 0); //para fundo branco
		}

	}

	/*
	float density;
	for (int i = 0; i < buffer.size(); i++){
		auto &cell = Populations[i];
		if (cell.size() > specie)
			density = cell.at(specie) / maxPopFound;
		else
			density = 0.0f;
		//buffer[i] = RGBAColorf_t(density, 0, 0, 0);	//para fundo preto
		buffer[i] = RGBAColorf_t(1, 1.0f-density, 1.0f-density, 0); //para fundo branco
	}
	*/
}


void ApplyInput(int deltaTime);
void display();



void init(){
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.015, 0.228, 0.85, 0);
	glutSwapBuffers();

	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_LINEAR);
	glEnable(GL_MULTISAMPLE_ARB);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);

	gluOrtho2D(0, 1.0 * SCREEN_WIDTH/LOWEST_RATIO, 0, 1.0 * SCREEN_HEIGHT/LOWEST_RATIO);
}

void reshapeWindow(int width, int height){
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//levando em conta que os dados de latitude estão entre 0-1
	gluOrtho2D(0, (double)width / LOWEST_RATIO, 0, (double)height / LOWEST_RATIO);
	current_width = width;
	current_height = height;

	display();
}



void display(){

	glClearColor(0.115, 0.328, 0.85, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	//ratio of screen reshape
	double width_ratio = current_width / (double)SCREEN_WIDTH;
	double height_ratio = current_height / (double)	SCREEN_HEIGHT;

	glPushMatrix();

	//escala pelo centro da tela
	Point_t ortho_center = {
		(SCREEN_WIDTH / LOWEST_RATIO) / 2,
		(SCREEN_HEIGHT / LOWEST_RATIO) / 2
	};


	glTranslatef( ortho_center.x*width_ratio,  ortho_center.y*height_ratio, 0);
	glScalef(total_scale.x, total_scale.y, 1.0f);
	glTranslatef(-ortho_center.x*width_ratio, -ortho_center.y*height_ratio, 0);

	glTranslatef(total_translade.x, total_translade.y, 0.0f);

	//printf("altura = %f\n", Cell_HexaPoly::Altura());
	for(int i=0; i<Cells.size();i++){
		//glColor3ub(cores[corPos].r, cores[corPos].g, cores[corPos].b);
		//nextColor();
		glColor3fv(&Cells_color_buffer[active_buffer][i].r);
		Cells.at(i).draw();
		//printf("|  drawing at point %.4f-%.4f  ", Cells[i].Center().x, Cells[i].Center().y);
	}
	//printf("\n");
	glPopMatrix();



	//para desenhar o "+" no centro
	glPushMatrix();

	glColor3f(1.0f,0,0);
	glTranslated((width_ratio-1) * ortho_center.x, (height_ratio-1)*ortho_center.y, 0);

	glBegin(GL_LINES);
	glVertex2f(ortho_center.x-0.025f , ortho_center.y);
	glVertex2f(ortho_center.x+0.025f , ortho_center.y);
	glVertex2f(ortho_center.x , ortho_center.y-0.025f);
	glVertex2f(ortho_center.x , ortho_center.y+0.025f);
	glEnd();

	glPopMatrix();

	//troca o buffer para evitar 'flickering' (tremedeira)
	glutSwapBuffers();
}

int oldTime=0;
int countToNextFrame=0;
void idleFunc(){
	static bool haveToLoadNextCellColors = true;
	static thread *fillCell_colorBuffer_thrd;

	int newTime = glutGet(GLUT_ELAPSED_TIME);
	int delta = newTime - oldTime;
	oldTime = newTime;
	ApplyInput(delta);

	countToNextFrame+=delta;


	//seta as proxima novas cores;
	if(haveToLoadNextCellColors){
		fillCell_colorBuffer_thrd = new std::thread(fillColorBuffer, std::ref(Cells_color_buffer[free_buffer]), curr_timeStep, 0);
		haveToLoadNextCellColors=false;
	}

	//se for hora, pinta as celulas com nova cor
	if(countToNextFrame >= FRAME_DURATION && curr_timeStep<total_timeSteps){
		countToNextFrame=0;

		fillCell_colorBuffer_thrd->join();
		fillCell_colorBuffer_thrd->~thread();
		haveToLoadNextCellColors=true;
		swapColorBuffer();
		printf("new Frame of timeStep %d!\n", curr_timeStep);

		display();
		curr_timeStep++;
		if(curr_timeStep == total_timeSteps)
			haveToLoadNextCellColors = false;
	}
}








/**********************Trecho de codigo responsável pelo input*************************/

//velocidade em unidade por milisegundo
#define velocity 0.05f
#define scaVel 0.001f
#define transVel 0.001f
Point_t vetor_l={-velocity,0}, vetor_r={velocity,0}, vetor_d={0,-velocity}, vetor_u={0,velocity};
Point_t vetor = {0.0f, 0.0f};

bool keystates[256] = {0};	//lista de estados das teclas, true para apertada, e false para solta

void MyKeyboardFunc(unsigned char Key, int x, int y){
	keystates[Key] = true;
	if(Key == ' '){
		//MUDA O MODO de visualização, AGORA do Q N SEI ( qual especie ver, ou densidade de todas espécies juntas, ou variabilidade genetica, etc)
		
	}	
}

void MyKeyboardUpFunc(unsigned char Key, int x, int y){
	keystates[Key] = false;
}



void ApplyInput(int deltaTime){
	bool callDisplay = false;

	if(keystates['+']){
		total_scale.x+= total_scale.x*scaVel*deltaTime;
		total_scale.y+= total_scale.y*scaVel*deltaTime;
		if(total_scale.x > 450)	//treshold de zoomIn
			total_scale.x = total_scale.y = 450;

		callDisplay = true;
	}
	if(keystates['-']){
		total_scale.x-= total_scale.x*scaVel*deltaTime;
		total_scale.y-= total_scale.y*scaVel*deltaTime;
		if(total_scale.x <=1e-3)	//treshold de zoomOut
			total_scale.x = total_scale.y = 1e-3;
		
		callDisplay=true;
	}

	if(keystates['d']){   total_translade.x -= transVel*deltaTime/total_scale.x; callDisplay=true;}
	if(keystates['a']){   total_translade.x += transVel*deltaTime/total_scale.x; callDisplay=true;}
	if(keystates['s']){   total_translade.y += transVel*deltaTime/total_scale.y; callDisplay=true;}
	if(keystates['w']){   total_translade.y -= transVel*deltaTime/total_scale.y; callDisplay=true;}


	if(callDisplay)
		display();
}

SimInfo_t processArgs(int argc, char **argv, string *simPath, string *latPath, string *lonPath);
void read_simInfo(string info_file_path, SimInfo_t *info);
void readCoordinates(vector<Coord_t> *coord_v, string latPath, string lonPath);
void readSimulation_timeSteps(string dir_path);
void readTimeStep(path dir_path);

static string manual_comandos = " W,A,S,D para se mover pelo mapa, \n"
								"'+' e '-' para dar zoom in e zoom out\n"
								"Barra de espaço para mudar cor do mapa\n"
								"(em breve: Barra de espaço para mudar modo de vizualização)\n";

int main(int argc, char **argv){
	if(argc < 2){
		printf("Visualizador gráfico para saída da EcoSim\n");
		printf("Autor: João Gabriel S. Fernandes\n");
		printf("Versão 0.3\n");
		printf("Uso: %s [OPÇÃO...] sim_info_file\n\n", argv[0]);
		printf("-t, --total-time-steps : opcional\t quantidade de timeSteps\n");
		printf("-ds, --sim-data-path : opcional\t caminho personalizado do arquivo binário do output da simulação\n");
		printf("-S, --frame-duration : opcional\t duração de cada frame em milissegundos\n");
		
		printf("\nsim_info_file: arquivo texto de informações, gerado pela simulação\n");

		//printf("as opções devem vir antes dos argumentos padrões\n");
		cout<<manual_comandos;

		exit(1);
	}

	string simPath = "../Simulation/Results/";
	string latPath = "../../output/";
	string lonPath = "../../output/";

	auto simInfo = processArgs(argc, argv, &simPath, &latPath, &lonPath);	




	total_celulas=simInfo.NUM_CELLS;
	total_timeSteps=simInfo.TIMESTEPS;

	Cells.reserve(total_celulas);
	vector<Coord_t> *coords = new vector<Coord_t>();
	coords->reserve(total_celulas);
	readCoordinates(coords, simInfo.Lat_dataSource, simInfo.Lon_dataSource);
	for(auto &c: *coords){
		Cells.emplace_back((Point_t){c.lon,c.lat});
	}
	Cells_color_buffer[0].resize(total_celulas, {1.0f,1.0f,1.0f,.0f});
	Cells_color_buffer[1].resize(total_celulas, {1.0f,1.0f,1.0f,.0f});

	//Populations_byTime.resize(total_timeSteps+1, vector<vector<float>>(total_celulas,vector<float>()));

	Populations.resize(total_celulas);
	timeStep_fileDescriptors.resize(total_timeSteps);
	readSimulation_timeSteps(simPath);

	cout << manual_comandos;


	printf("Files loaded!\n\nPress Enter to start\ns");
	getc(stdin);

	/*****************************CONFIGURAÇÕES OPENGL*******************************/

	argc=1;
	glutInit(&argc, argv);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(10, 50);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_DEPTH);

	int windowId = glutCreateWindow("Mapa simulation");
	init();
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshapeWindow);

	glutIgnoreKeyRepeat(true);
	glutKeyboardFunc(MyKeyboardFunc);
	glutKeyboardUpFunc(MyKeyboardUpFunc);

	glutIdleFunc(idleFunc);

	oldTime=glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();


	//fechando os arquivos
	for(auto &ts: timeStep_fileDescriptors){
		for(auto &fd: ts){
			close(fd.second);
		}
	}

}


SimInfo_t processArgs(int argc, char **argv, string *simPath, string *latPath, string *lonPath){

	bool customTimeSteps = false, customSimPath = false;
	SimInfo_t info;

	for(int i=1; i<argc;i++){
		string arg(argv[i]);
		if(arg[0]=='-'){
			if(arg.substr(1).find("t")==0){
				customTimeSteps=true;
				if(arg.size()>2)	//se o valor do argumento '-t' está logo em seguida (sem espaço)
					total_timeSteps = stoi(arg.substr(2));
				else{
					if(++i > argc){
						printf("Faltando valor de -t ou --total-time-steps");
						exit(1);
					}
					arg = string(argv[i]);	//se tiver espaço depois de '-t', pega o proximo argumento
					total_timeSteps = stoi(arg);
				}
			}
			else if(arg.substr(1).find("-total-time-steps")==0){
				customTimeSteps=true;
				if(arg.size()>18)	//se o valor do argumento '--total-time-steps' está logo em seguida (sem espaço)
					total_timeSteps = stoi(arg.substr(18));
				else{
					//se tiver espaço depois de '-t', pega o proximo argumento
					if(++i > argc){
						printf("Faltando valor de -t ou --total-time-steps");
						exit(1);
					}
					arg = string(argv[i]);
					total_timeSteps = stoi(arg);
				}
			}

			if(arg.substr(1).find("S")==0){
				if(arg.size()>2)	//se o valor do argumento '-t' está logo em seguida (sem espaço)
					FRAME_DURATION = stoi(arg.substr(2));
				else{
					if(++i > argc){
						printf("Faltando valor de -S");
						exit(1);
					}
					arg = string(argv[i]);	//se tiver espaço depois de '-t', pega o proximo argumento
					FRAME_DURATION = stoi(arg);
				}
			}

		}
		else{
			read_simInfo((*simPath) +arg.substr(0,arg.rfind("_info.txt"))+"/"+ arg, &info);
			*latPath = info.Lat_dataSource;
			*lonPath = info.Lon_dataSource;
			*simPath = *simPath +info.Name;
		}
	}

	if(!customTimeSteps)
		total_timeSteps = info.TIMESTEPS;
	return info;
}


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
			maxPopFound = stof(value);
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
	ifstream latFile(latPath, std::ios::binary);
	ifstream lonFile(lonPath, std::ios::binary);

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


void readSimulation_timeSteps(string dir_path){
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
					readTimeStep(dir_it->path());
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

void readTimeStep(path dir_path){
	int timeStep;
	
	if (is_regular_file(dir_path)){ // is p a regular file?
		printf("%s is a file\n", dir_path.string().c_str());
	}else{
		int pos = dir_path.string().find("timeStep");
		int len = string("timeStep").size();
		timeStep = stoi(dir_path.string().substr(pos+len));

		if(total_timeSteps < timeStep+1){
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
					timeStep_fileDescriptors[timeStep].push_back( {specie, open(dir_it->path().c_str(), O_RDONLY)});	//abre arquivo e armazena o descritor dele

					/*
					ifstream speciePopFile(dir_it->path().string(), std::ios::binary);

					int s = file_name.string().rfind("Esp");
					int f = file_name.string().rfind("_Time");
					uint specie = stoi(file_name.string().substr(s+3,f));

					uint cell;
					float pop;
					while(!speciePopFile.eof()){	
						speciePopFile.read((char*)&cell, sizeof(uint));
						speciePopFile.read((char *)&pop, sizeof(float));
						if(Populations_byTime[timeStep][cell].size() < specie-1)
							Populations_byTime[timeStep][cell].resize(specie+1);
						Populations_byTime[timeStep][cell].at(specie) = pop;
						maxPopFound = max(maxPopFound, pop);
					}

					speciePopFile.close();
					*/
				}
				cout<<"\n";
			}

			dir_it++;
		}
		
	}

}