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

#include "Tools.h"
#include "Cell_HexaPoly.hpp"

#define SCREEN_HEIGHT 940
#define SCREEN_WIDTH 940

using namespace std;
using namespace boost::filesystem;

//lista de cores quaisquer
const RGBAiColor_t cores[5] = {{42, 170, 172,0}, {211, 196, 167,0}, {165, 104, 191,0}, {19, 40, 15,0}, {210, 222, 217,0}};
int corPos=0;

void nextColor(){
	corPos = (corPos + 1) % (sizeof(cores) / sizeof(RGBAiColor_t));
}

GLint current_width = SCREEN_WIDTH, current_height = SCREEN_HEIGHT;

Point_t total_scale = {1.0f, 1.0f};
Point_t total_translade = {0.0f, 0.0f};



//vetor de objetos que representam as células na interface gráfica
vector<Cell_HexaPoly> Cells;

//registro de populações nas células de cada espécie
//keys: speciePopulations_byTime [time_step] [celula] ["specieName"] = população dessa espécie nesta célula neste determinado timeStep
vector<vector<unordered_map<string, float>>> speciePopulations_byTime;



void init(){
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.215, 0.528, 1.0, 0);
	glutSwapBuffers();
	glViewport(0, 0, SCREEN_WIDTH / SCREEN_WIDTH, SCREEN_HEIGHT / SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);

	gluOrtho2D(0, 1.0, 0, 1.0);
}

void display(){

	glClearColor(0.215, 0.528, 1.0, 0);
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



	double width_ratio = current_width / (double)SCREEN_WIDTH - 1;
	double height_ratio = current_height / (double)SCREEN_HEIGHT - 1;

	glPushMatrix();

	//escala pelo centro da tela
	glTranslatef(0.5 + 0.5*width_ratio, 0.5 + 0.5*height_ratio, 0);
	glScalef(total_scale.x, total_scale.y, 1.0f);
	glTranslatef(-0.5 - 0.5f*width_ratio, -0.5 -0.5f*height_ratio, 0);

	glTranslatef(total_translade.x, total_translade.y, 0.0f);

	corPos = 0;
	//printf("altura = %f\n", Cell_HexaPoly::Altura());
	for(int i=0; i<Cells.size();i++){
		glColor3ub(cores[corPos].r, cores[corPos].g, cores[corPos].b);
		nextColor();
		Cells.at(i).draw();
		//printf("|  drawing at point %.4f-%.4f  ", Cells[i].Center().x, Cells[i].Center().y);
	}
	//printf("\n");
	glPopMatrix();



	//para desenhar o "+" no centro
	glPushMatrix();

	glColor3f(1.0f,0,0);
	glTranslated(width_ratio * 0.5f, height_ratio*0.5f, 0);

	glBegin(GL_LINES);
	glVertex2f(0.475f , 0.5f);
	glVertex2f(0.525f, 0.5f);
	glVertex2f(0.5f, 0.475f);
	glVertex2f(0.5f, 0.525f);
	glEnd();

	glPopMatrix();


	//troca o buffer para evitar 'flickering' (tremedeira)
	glutSwapBuffers();
}

void reshapeWindow(int width, int height){
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//levando em conta que os dados de latitude estão entre 0-1
	gluOrtho2D(0, width / (double)SCREEN_WIDTH, 0, height / (double)SCREEN_HEIGHT);

	current_width = width;
	current_height = height;

	display();
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
	if(Key == ' ')
		corPos = (corPos + 1) % (sizeof(cores) / sizeof(RGBAiColor_t));
}

void MyKeyboardUpFunc(unsigned char Key, int x, int y){
	keystates[Key] = false;
	if(!keystates['d'])   vetor.x = 0;
	if(!keystates['a'])   vetor.x = 0;
	if(!keystates['s'])   vetor.y = 0;
	if(!keystates['w'])   vetor.y = 0;
}




int oldTime;
void ApplyInput(){
	bool callDisplay = false;

	int newTime = glutGet(GLUT_ELAPSED_TIME);
	int delta = newTime - oldTime;
	oldTime = newTime;

	if(keystates['+']){
		total_scale.x+= total_scale.x*scaVel*delta;
		total_scale.y+= total_scale.y*scaVel*delta;
		if(total_scale.x > 450)	//treshold de zoomIn
			total_scale.x = total_scale.y = 450;

		callDisplay = true;
	}
	if(keystates['-']){
		total_scale.x-= total_scale.x*scaVel*delta;
		total_scale.y-= total_scale.y*scaVel*delta;
		if(total_scale.x <=1e-3)	//treshold de zoomOut
			total_scale.x = total_scale.y = 1e-3;
		
		callDisplay=true;
	}

	if(keystates['d']){   total_translade.x -= transVel*delta/total_scale.x; callDisplay=true;}
	if(keystates['a']){   total_translade.x += transVel*delta/total_scale.x; callDisplay=true;}
	if(keystates['s']){   total_translade.y += transVel*delta/total_scale.y; callDisplay=true;}
	if(keystates['w']){   total_translade.y -= transVel*delta/total_scale.y; callDisplay=true;}


	if(callDisplay)
		display();
}


void readCoordinates(vector<Coord_t> *coord_v, string latPath, string lonPath);
void readSimulation_timeSteps(string dir_path);



int main(int argc, char **argv){
	if(argc < 3){
		printf("Visualizador gráfico para saída da EcoSim\n");
		printf("Autor: João Gabriel S. Fernandes\n");
		printf("Versão 0.1\n");
		printf("Uso: %s [OPÇÃO...] Sim_dataPrefix Sim_resultName\n\n", argv[0]);
		printf("-dla, --lat-data-path\t caminho personalizado do arquivo binário de latitude das células\n");
		printf("-dlo, --lon-data-path\t caminho personalizado do arquivo binário de longitude das células\n");
		printf("-ds, --sim-data-path\t caminho personalizado do arquivo binário do output da simulação\n");

		printf("as opções devem vir antes dos argumentos padrões\n");

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

	readSimulation_timeSteps(simPath);

	

	argc=1;
	glutInit(&argc, argv);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(10, 50);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutCreateWindow("Mapa simulation");
	init();
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshapeWindow);

	glutIgnoreKeyRepeat(true);
	glutKeyboardFunc(MyKeyboardFunc);
	glutKeyboardUpFunc(MyKeyboardUpFunc);

	glutIdleFunc(ApplyInput);

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
			if (is_regular_file(p)) // is p a regular file?
				printf("%s is a file\n", p.string().c_str());

			else if (is_directory(p)){ // is p a directory?
				printf("found %s dir, containing\n", p.string().c_str());

				auto dir_it = directory_iterator(p);
				while(dir_it != directory_iterator{}){
					cout<< *dir_it++<<'\n';
				}
				/*copy(directory_iterator(p), directory_iterator(),	// directory_iterator::value_type
					 ostream_iterator<directory_entry>(cout, "\n")); // is directory_entry, which is
																	 // converted to a path by the
																	 // path stream inserter
																	 */
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

void readSpeciePopulation_byTime(string dir_path, string specieName){
	
}