//#include <GLFW/glfw3.h>

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <GL/glut.h> //inclua a biblioteca glut

#include <vector>

#include "Tools.h"

#define SCREEN_WIDTH 1280/2
#define SCREEN_HEIGHT 960/2

using namespace std;

//lista de cores quaisquer
const RGBAiColor_t cores[5] = {{42, 170, 172,0}, {211, 196, 167,0}, {165, 104, 191,0}, {19, 40, 15,0}, {210, 222, 217,0}};
int corPos=0;

void nextColor(){
	corPos = (corPos + 1) % (sizeof(cores) / sizeof(RGBAiColor_t));
}





typedef struct{
	float lat;
	float lon;
}Coord_t;

class Cell_HexaPoly{

public:
	Cell_HexaPoly(Point_t center) : center(center)
	{
		vertexes[0].x = center.x-raio;		vertexes[0].y = center.y;
		vertexes[1].x = center.x-raio/2;	vertexes[1].y = center.y + altura;
		vertexes[2].x = center.x+raio/2;	vertexes[2].y = center.y + altura;
		vertexes[3].x = center.x+raio;		vertexes[3].y = center.y;
		vertexes[4].x = center.x+raio/2;	vertexes[4].y = center.y - altura;
		vertexes[5].x = center.x-raio/2;	vertexes[5].y = center.y - altura;
	}

	Point_t* get_Vertexes(){
		return vertexes;
	}
	Point_t Center(){
		return center;
	}

	void draw(){
		auto &v=vertexes;
		glBegin(GL_POLYGON);
		glVertex2f(v[0].x, v[0].y);
		glVertex2f(v[1].x, v[1].y);
		glVertex2f(v[2].x, v[2].y);
		glVertex2f(v[3].x, v[3].y);
		glVertex2f(v[4].x, v[4].y);
		glVertex2f(v[5].x, v[5].y);
		glEnd();
	}

	static constexpr double raio = 1.0/10.0;
	static constexpr double altura = raio * 1.732050808 / 2.0;	// raio * sqrt(3)/2
protected:
	Point_t center;
	Point_t vertexes[6];

private:
};



void init(){
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);

	//levando em conta que os dados de latitude estão entre 0-1

	if(SCREEN_WIDTH > SCREEN_HEIGHT){
		double d = (double)SCREEN_HEIGHT/(double)SCREEN_WIDTH;
		gluOrtho2D(0, 1.0, 0, 1.0*d);
	}else{
		double d = (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT;
		gluOrtho2D(0, 1.0*d, 0, 1.0);
	}
}

void display(){

	glClear(GL_COLOR_BUFFER_BIT);
	

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

	//dada essas relações, qualquer outra posição é espelhamento, e também pode-se apenas usar a latitude como centro
	
	glPushMatrix();

	glColor3ub(cores[corPos].r, cores[corPos].g, cores[corPos].b);
	cell.draw();

	nextColor();
	glColor3ub(cores[corPos].r, cores[corPos].g, cores[corPos].b);
	cell2.draw();

	nextColor();
	glColor3ub(cores[corPos].r, cores[corPos].g, cores[corPos].b);
	cell3.draw();


	glPopMatrix();

	//ponto central e raio da cincunferencia
	glFlush();
}

void reshapeWindow(int width, int height){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, width, height);

	//levando em conta que os dados de latitude estão entre 0-1

	if(width > height){
		double d = (double)height/(double)width;
		gluOrtho2D(0, 1.0, 0, 1.0*d);
	}else{
		double d = (double)width / (double)height;
		gluOrtho2D(0, 1.0*d, 0, 1.0);
	}

	display();
}




/**********************Trecho de codigo responsável pelo input*************************/

//velocidade em unidade por milisegundo
#define velocity 0.5f
#define scaVel 0.001f
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

void ApplyInput(){
	if(keystates['q']){
		
	}
}


int oldTime;

int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(10, 50);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glutCreateWindow("Mapa simulation");
	init();
	oldTime=glutGet(GLUT_ELAPSED_TIME);
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshapeWindow);

	glutIgnoreKeyRepeat(true);
	glutKeyboardFunc(MyKeyboardFunc);
	glutKeyboardUpFunc(MyKeyboardUpFunc);

	glutIdleFunc(ApplyInput);

	glutMainLoop();
}