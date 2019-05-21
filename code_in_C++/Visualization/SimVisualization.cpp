//#include <GLFW/glfw3.h>

#include <stdio.h>
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
#include "Cell_HexaPoly.hpp"

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


//lista de cores quaisquer
const RGBAColord_t cores[5] = {{42, 170, 172,0}, {211, 196, 167,0}, {165, 104, 191,0}, {19, 40, 15,0}, {210, 222, 217,0}};
int corPos=0;

void nextColor(){
	int rangemin = 0;
	int rangemax = 5;
	corPos = rangemin + ((corPos-rangemin + 1) % ( min<int>(sizeof(cores) / sizeof(RGBAColord_t),rangemax) - rangemin) );
}



int total_timeSteps;
int total_celulas=0;
//vetor de objetos que representam as células na interface gráfica
vector<Cell_HexaPoly> Cells;
vector<RGBAColorf_t> Cells_color_buffer[2];
int active_buffer=0, free_buffer=1;
int curr_timeStep=0;

//registro de populações nas células de cada espécie
//keys: speciePopulations_byTime [time_step] [celula] [specie] = população dessa espécie nesta célula neste determinado timeStep
vector<vector<unordered_map<uint, float>>> Populations_byTime;
float maxPopFound=0.0f;


void swapColorBuffer(){
	int rangemin = 0;
	int rangemax = 2;
	active_buffer = rangemin + ((active_buffer-rangemin + 1) % ( min<int>(sizeof(Cells_color_buffer) / sizeof(RGBAColorf_t),rangemax) - rangemin) );
	free_buffer = rangemin + ((free_buffer - rangemin + 1) % (min<int>(sizeof(Cells_color_buffer) / sizeof(RGBAColorf_t), rangemax) - rangemin));
}

void fillColorBuffer(vector<RGBAColorf_t> &buffer, int timeStep, int specie){
		float density;
	for (int i = 0; i < buffer.size(); i++){
		auto &cell = Populations_byTime[timeStep][i];
		//se existir essa espécie na célula
		if (cell.count(specie)!=0){
			density = Populations_byTime[timeStep][i][specie] / maxPopFound;
		}else{
			density = 0.0f;
		}
		//buffer[i] = RGBAColorf_t(density, 0, 0, 0);	//para fundo preto
		buffer[i] = RGBAColorf_t(1, 1.0f-density, 1.0f-density, 0); //para fundo branco
	}
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

	//celula superior esquerda, cor roxa/lilás
	Cell_HexaPoly cell({0.5f, 0.5f});
	//celula inferior esquerda, cor verde musgo
	Cell_HexaPoly cell2({
			cell.Center().x, 
	(float)(cell.Center().y- 2*cell.altura)});
	//celula meio direita, cor branca (escura?)
	Cell_HexaPoly cell3({
			(float)(cell.Center().x+ (1.5*cell.raio)),
			(float)(cell.Center().y - cell.altura)
	});

	//dada essas relações, qualquer outra posição é espelhamento, e também pode-se apenas usar as coordenadas (lat e lon) como centro


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
		nextColor();
		display();
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


void readCoordinates(vector<Coord_t> *coord_v, string latPath, string lonPath);
void readSimulation_timeSteps(string dir_path);
void readTimeStep(path dir_path);

static string manual_comandos = " W,A,S,D para se mover pelo mapa, \n"
								"'+' e '-' para dar zoom in e zoom out\n"
								"Barra de espaço para mudar cor do mapa\n"
								"(em breve: Barra de espaço para mudar modo de vizualização)\n";

int main(int argc, char **argv){
	if(argc < 4){
		printf("Visualizador gráfico para saída da EcoSim\n");
		printf("Autor: João Gabriel S. Fernandes\n");
		printf("Versão 0.1\n");
		printf("Uso: %s [OPÇÃO...] Sim_dataPrefix Sim_resultName\n\n", argv[0]);
		printf("-t, --total-time-steps : obrigatório\t quantidade de timeSteps\n");
		printf("-dla, --lat-data-path : opcional\t caminho personalizado do arquivo binário de latitude das células\n");
		printf("-dlo, --lon-data-path : opcional\t caminho personalizado do arquivo binário de longitude das células\n");
		printf("-ds, --sim-data-path : opcional\t caminho personalizado do arquivo binário do output da simulação\n");
		printf("-S, --frame-duration : opcional\t duração de cada frame em milissegundos\n");

		//printf("as opções devem vir antes dos argumentos padrões\n");
		cout<<manual_comandos;

		exit(1);
	}

	string simPath = "../Simulation/Results/";
	string latPath = "../../output/";
	string lonPath = "../../output/";
	bool customLat  = false, customLon = false, customSimPath=false;
	bool leu_prefix =false, leu_SimName=false;


	for(int i=1; i<argc;i++){
		string arg(argv[i]);
		if(arg[0]=='-'){
			if(arg.substr(1).find("t")==0){
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
			if(!leu_prefix){
				if(!customLat)
					latPath = latPath+arg+" - Output - Latitude.stream";
				if (!customLon)
					lonPath = lonPath + arg + " - Output - Longitude.stream";
				leu_prefix=true;
			}
			else if(leu_prefix && !leu_SimName){
				simPath = simPath+arg;
				leu_SimName = true;
			}	
		}
	}

	vector<Coord_t> *coords = new vector<Coord_t>();
	Cells.reserve(coords->size());
	readCoordinates(coords, latPath, lonPath);
	for(auto &c: *coords){
		Cells.emplace_back((Point_t){c.lon,c.lat});
	}
	total_celulas=Cells.size();
	Cells_color_buffer[0].resize(total_celulas, {.0f,.0f,.0f,.0f});
	Cells_color_buffer[1].resize(total_celulas, {.0f,.0f,.0f,.0f});
	//deixe nessa ordem, pois readSimulation_timeSteps() precisa saber quantas celulas existem
	Populations_byTime.resize(total_timeSteps+1, vector<unordered_map<uint, float>>(total_celulas));
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

	glutCreateWindow("Mapa simulation");
	init();
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshapeWindow);

	glutIgnoreKeyRepeat(true);
	glutKeyboardFunc(MyKeyboardFunc);
	glutKeyboardUpFunc(MyKeyboardUpFunc);

	glutIdleFunc(idleFunc);

	oldTime=glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();
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
		coord_v->push_back(coord);
	}

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
					ifstream speciePopFile(dir_it->path().string(), std::ios::binary);

					int s = file_name.string().rfind("Esp");
					int f = file_name.string().rfind("_Time");
					uint specie = stoi(file_name.string().substr(s+3,f));

					uint cell;
					float pop;
					while(!speciePopFile.eof()){	
						speciePopFile.read((char*)&cell, sizeof(uint));
						speciePopFile.read((char *)&pop, sizeof(float));
						Populations_byTime[timeStep][cell][specie] = pop;
						maxPopFound = max(maxPopFound, pop);
					}
				}
				cout<<"\n";
			}

			dir_it++;
		}
	}

}