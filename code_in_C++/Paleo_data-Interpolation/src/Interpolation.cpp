#include "PaleoClimate.h"
#include "decompressData.h"
#include "color.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <time.h>

#include <fenv.h>

#ifdef _WIN32
	#define SYSCLEAR_STR "CLS"
#elif __linux__
	#define SYSCLEAR_STR "clear"
#else
#define SYSCLEAR_STR ""
#endif


using namespace std;
using namespace SimEco;

const char input_path[] = "../../input/";
const char output_path[]= "../../output/";

int main(){

	string presentClimFile, paleoClimFile, r;
	string outSuffix;
	double b, e, s; 

	//string input_str(100,'\0');
	char input_str[100];			// ~BUG CORRIGIDO: teve que mudar pq estava dando problema com a classe string e o uso dela para o nome dos arquivos

	//ps: aqui, precisa de saber o numero de linhas(celulas) antes de ler o arquivo;
	int numero_de_linhas=0; 

	printf("Is the grid + present climate file ''Coords and Clim.txt'' ?\n");
	printf("This file should have as many rows as there are grid cells, and 7 columns:\n");
	printf("1) Longitude\n");
	printf("2) Latitude\n");
	printf("3) Temperature in the coldest quarter\n");
	printf("4) Temperature in the warmest quarter\n");
	printf("5) Precipitation in the driest quarter\n");
	printf("6) Precipitation in the wettest quarter\n");
	printf("7) Annual NPP\n\n");
	printf("Type ENTER or \"Y\" if yes, or type the correct name of the paleoclimate file\n");

	scanf(" %[^\n]s", input_str);

	if(input_str[0] == 'Y' || string(input_str).find("yes") !=-1  || string(input_str).empty() ){
		presentClimFile = "Coords and Clim.txt";
	}else{
		presentClimFile = input_str;
		printf("Enter the number of cells(lines) in the grid: ");
		scanf("%d", &numero_de_linhas);
	}

	system(SYSCLEAR_STR);

	printf("Is the paleoclimate file ' 'xPLASIM.All3DMats.Single.Zip.Stream' '?\n");
	printf("Type ENTER or \"Y\" if yes, or the correct name of the paleoclimate file\n");

	scanf(" %[^\n]s", &input_str[0]);

	if (input_str[0] == 'Y' || string(input_str).find("yes") != -1 || string(input_str).empty()){
		paleoClimFile = "xPLASIM.All3DMats.Single.Zip.Stream";
	}
	else {
		paleoClimFile = input_str;
	}

	system(SYSCLEAR_STR);
	printf("Would you like to use the suffix '!OutPaleoClim - Var.txt*'?\n\n");
	printf("Type ENTER or \"Y\" if yes, or type the suffix for the output files\n");
	printf("If you would like to customize the suffix, enter only a single word (e.g. ''ClimDat'')\n");

	scanf(" %[^\n]s", &input_str[0]);

	if( input_str[0] == 'Y' || string(input_str).find("yes") != -1 || string(input_str).empty() ){
		outSuffix = "!OutPaleoClim";
	}else{
		outSuffix.assign( input_str);
	}

	system(SYSCLEAR_STR);
	printf("Would you like to start 5M years ago?\n\n");
	printf("Type ENTER or \"Y\" if yes, or enter the starting time in thousand of years ago (5000 = 5Mya)\n");

	scanf(" %[^\n]s", &input_str[0]);

	if (input_str[0] == 'Y' || string(input_str).find("yes") != -1 || string(input_str).empty()){
		b = 5000.0;
	}else{
		b = stod(input_str, NULL);
	}

	system(SYSCLEAR_STR);
	printf("Would you like to end in the present time?\n\n");
	printf("Type ENTER or \"Y\" if yes, or enter the ending time in thousand of years ago (1000 = 1Mya)\n");

	scanf(" %[^\n]s", &input_str[0]);

	if (input_str[0] == 'Y' || string(input_str).find("yes") != -1 || string(input_str).empty()){
		e = 0;
	}else{
		e = stod(input_str);
	}

	system(SYSCLEAR_STR);
	printf("Would you like to use 500 years as time step?\n\n");
	printf("Type ENTER or \"Y\" if yes, or enter the length of time step in thousand of years ago (0.5 = 500y)\n");

	scanf(" %[^\n]s", &input_str[0]);

	if (input_str[0] == 'Y' || string(input_str).find("yes") != -1 || string(input_str).empty()){
		s = 0.5;
	}else{
		s = stod(input_str);
	}

	//feenableexcept(FE_ALL_EXCEPT & ~FE_INEXACT); // Enable all floating point exceptions but FE_INEXACT

	//creates a new Object of class PaleoClimate
	PaleoClimate paleoClimate((input_path + paleoClimFile).c_str(), (input_path + presentClimFile).c_str(), numero_de_linhas, true);
	printf(GRN("\tObjeto " BOLD("<PaleoClimate>") " criado\n"));		// ~DebugLog

	fstream arq_outLat,arq_outLong,arq_outMinTemp, arq_outMaxTemp, arq_outMinPPTN, arq_outMaxPPTN, arq_outNPP;
	FILE *arq_outNPP_txt;
	

	try{
		arq_outLat.open(output_path + outSuffix + " - Latitude.stream", ios::trunc | ios::out | ios::binary);
		arq_outLong.open(output_path + outSuffix + " - Longitude.stream", ios::trunc | ios::out | ios::binary);
		arq_outMinTemp.open( output_path + outSuffix + " - MinTemp.stream", ios::trunc | ios::out | ios::binary);
		arq_outMaxTemp.open( output_path+outSuffix+ " - MaxTemp.stream", ios::trunc | ios::out | ios::binary);
		arq_outMinPPTN.open( output_path+outSuffix+ " - MinPPTN.stream", ios::trunc | ios::out | ios::binary);
		arq_outMaxPPTN.open( output_path+outSuffix+ " - MaxPPTN.stream", ios::trunc | ios::out | ios::binary);
		arq_outNPP.open( output_path + outSuffix + " - NPP.stream", ios::trunc | ios::out | ios::binary);
		arq_outNPP_txt = fopen((output_path + outSuffix + " - NPP.txt").c_str(), "w");
	}catch (const fstream::failure &e){
		printf(RED("Erro ao abrir arquivo de saida %s\n"), e.what()); // ~DebugLog
		exit(1);
	}
	printf(GRN("\tArquivos de saida abertos com sucesso\n")); // ~DebugLog

	
	clock_t startClock, endClock;

	startClock = clock();

	try
	{

		//aqui vai ter problema de arredondamento dependendo dos valores
		int nt = (b - e) / s;
		nt+=1;	//numero de timeSteps mais o timeStep 0(zero)
		//grava quantos timeSteps (linhas) vão ter na stream
		arq_outMinTemp.write( (const char*)&nt, sizeof(int) );
		arq_outMaxTemp.write( (const char*)&nt, sizeof(int) );
		arq_outMinPPTN.write( (const char*)&nt, sizeof(int) );
		arq_outMaxPPTN.write( (const char*)&nt, sizeof(int) );
		arq_outNPP.write( (const char*)&nt, sizeof(int) );

		for (int i = 0; i < nt; i++){

			//grava quantas células (colunas) vão ter nesse timeStep
			int numCells = paleoClimate.getCellsLen();

			arq_outMinTemp.write( (const char*)&numCells, sizeof(int) );
			arq_outMaxTemp.write( (const char*)&numCells, sizeof(int) );
			arq_outMinPPTN.write( (const char*)&numCells, sizeof(int) );
			arq_outMaxPPTN.write( (const char*)&numCells, sizeof(int) );
			arq_outNPP.write( (const char*)&numCells, sizeof(int) );

			//fprintf(arq_outNPP_txt, "T%8.8d",i);
			
			float SATMin, SATMax, PPTNMin, PPTNMax, NPP;

			for (int cell = 0; cell < numCells; cell++){
				//calcula o clima da célula usando interpolação
				paleoClimate.getClimCell(cell, (i*s), &SATMin, &SATMax, &PPTNMin, &PPTNMax, &NPP);

				// aqui começa a escrever os valores já interpolados nos arquivos de saida
				arq_outMinTemp.write( (const char*)&SATMin, sizeof(float));
				arq_outMaxTemp.write( (const char*)&SATMax, sizeof(float));
				arq_outMinPPTN.write( (const char*)&PPTNMin, sizeof(float));
				arq_outMaxPPTN.write( (const char*)&PPTNMax, sizeof(float));
				arq_outNPP.write( (const char*)&NPP, sizeof(float));
				//fprintf(arq_outNPP_txt,"%8.8g", NPP);
			}
			//fprintf(arq_outNPP_txt,"\n");
		}

		int numCells = paleoClimate.getCellsLen();
		arq_outLat.write((const char *)&numCells, sizeof(int));
		arq_outLong.write((const char *)&numCells, sizeof(int));
		for(int i=0 ; i < numCells; i++){
			float Lat, Long;

			paleoClimate.getLatLongCell(i, &Lat, &Long);
			arq_outLat.write( (const char *)&Lat, sizeof(float));
			arq_outLong.write( (const char *)&Long, sizeof(float));
		}
	}



	catch (const fstream::failure &e){
		printf(RED("Erro ao escrever em arquivo de saida %s\n"), e.what()); // ~DebugLog
		exit(1);
	}
	catch (exception &e){
		cout << "Standard exception: " << e.what() << endl;
	}

	printf(GRN("\tArquivos de saida escritos com sucesso\n")); // ~DebugLog

	endClock = clock();

	printf(MAG("Total clocks: "));printf(YEL("%lu\n"), endClock-startClock);
	printf(MAG("Total Time:   "));printf(YEL("%lf s\n"), (double)(endClock-startClock)/CLOCKS_PER_SEC);

	arq_outLat.close();
	arq_outLong.close();
	arq_outMinTemp.close();
	arq_outMaxTemp.close();
	arq_outMinPPTN.close();
	arq_outMaxPPTN.close();
	arq_outNPP.close();
}
