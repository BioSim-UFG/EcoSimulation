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

	system("clear");    //for unix
	//system("CLS");      //for DOS ( windows);

	printf("Is the paleoclimate file ' 'xPLASIM.All3DMats.Single.Zip.Stream' '?\n");
	printf("Type ENTER or \"Y\" if yes, or the correct name of the paleoclimate file\n");

	scanf(" %[^\n]s", &input_str[0]);

	if (input_str[0] == 'Y' || string(input_str).find("yes") != -1 || string(input_str).empty()){
		paleoClimFile = "xPLASIM.All3DMats.Single.Zip.Stream";
	}
	else {
		paleoClimFile = input_str;
	}

	system("clear");    //for unix
	//system("CLS");      //for DOS ( windows);

	printf("Would you like to use the suffix '!OutPaleoClim - Var.txt*'?\n\n");
	printf("Type ENTER or \"Y\" if yes, or type the suffix for the output files\n");
	printf("If you would like to customize the suffix, enter only a single word (e.g. ''ClimDat'')\n");

	scanf(" %[^\n]s", &input_str[0]);

	if( input_str[0] == 'Y' || string(input_str).find("yes") != -1 || string(input_str).empty() ){
		outSuffix = "!OutPaleoClim";
	}else{
		outSuffix.assign( input_str);
	}


	system("clear");    //for unix
	//system("CLS");      //for DOS ( windows);
	printf("Would you like to start 5M years ago?\n\n");
	printf("Type ENTER or \"Y\" if yes, or enter the starting time in thousand of years ago (5000 = 5Mya)\n");

	scanf(" %[^\n]s", &input_str[0]);

	if (input_str[0] == 'Y' || string(input_str).find("yes") != -1 || string(input_str).empty()){
		b = 5000.0;
	}else{
		b = stod(input_str, NULL);
	}

	system("clear"); //for unix
	//system("CLS");      //for DOS ( windows);
	printf("Would you like to end in the present time?\n\n");
	printf("Type ENTER or \"Y\" if yes, or enter the ending time in thousand of years ago (1000 = 1Mya)\n");

	scanf(" %[^\n]s", &input_str[0]);

	if (input_str[0] == 'Y' || string(input_str).find("yes") != -1 || string(input_str).empty()){
		e = 0;
	}else{
		e = stod(input_str);
	}

	system("clear"); //for unix
	//system("CLS");      //for DOS ( windows);
	printf("Would you like to use 500 years as time step?\n\n");
	printf("Type ENTER or \"Y\" if yes, or enter the length of time step in thousand of years ago (0.5 = 500y)\n");

	scanf(" %[^\n]s", &input_str[0]);

	if (input_str[0] == 'Y' || string(input_str).find("yes") != -1 || string(input_str).empty()){
		s = 0.5;
	}else{
		s = stod(input_str);
	}


	//creates a new Object of class PaleoClimate
	PaleoClimate paleoClimate((input_path + paleoClimFile).c_str(), (input_path + presentClimFile).c_str(), numero_de_linhas, true);
	printf(GRN("\tObjeto " BOLD("<PaleoClimate>") " criado\n"));		// ~DebugLog

	fstream arq_outMinTemp, arq_outMaxTemp, arq_outMinPPTN, arq_outMaxPPTN, arq_outNPP;
	fstream arq_teste;

	/*
	try{
		arq_outMinTemp.open( output_path+outSuffix+ " - MinTemp.txt", ios::trunc | ios::out);
		arq_outMaxTemp.open( output_path+outSuffix+ " - MaxTemp.txt", ios::trunc | ios::out);
		arq_outMinPPTN.open( output_path+outSuffix+ " - MinPPTN.txt", ios::trunc | ios::out);
		arq_outMaxPPTN.open( output_path+outSuffix+ " - MaxPPTN.txt", ios::trunc | ios::out);
		arq_outNPP.open(output_path + outSuffix + " - NPP.txt", ios::trunc | ios::out);
	}catch (const fstream::failure &e){
		printf(RED("Erro ao abrir arquivo de saida %s\n"), e.what()); // ~DebugLog
		exit(1);
	}
	printf(GRN("\tArquivos de saida abertos com sucesso\n")); // ~DebugLog

	
	clock_t startClock, endClock;

	startClock = clock();
	try{	//o que é nt?
		int nt = (b - e) / s;

		char temp_str[100];
		sprintf(temp_str,"T%.0f",b);
		streamsize size = strlen(temp_str);

		//começa a escrever a primeira linha de "titulo" da tabela, isto é, a que informa os timesteps
		arq_outMinTemp.write(temp_str, size);		
		arq_outMaxTemp.write(temp_str, size);
		arq_outMinPPTN.write(temp_str, size);
		arq_outMaxPPTN.write(temp_str, size);
		arq_outNPP.write(temp_str, size);

		for(int i=1; i<=nt; i++){
			sprintf(temp_str, "\tT%.0f", b - (i * s));
			size = strlen(temp_str);
			arq_outMinTemp.write(temp_str, size);
			arq_outMaxTemp.write(temp_str, size);
			arq_outMinPPTN.write(temp_str, size);
			arq_outMaxPPTN.write(temp_str, size);
			arq_outNPP.write(temp_str, size);
		}
		arq_outMinTemp.write("\n", 1);
		arq_outMaxTemp.write("\n", 1);
		arq_outMinPPTN.write("\n", 1);
		arq_outMaxPPTN.write("\n", 1);
		arq_outNPP.write("\n", 1);

		float SATMin, SATMax, PPTNMin, PPTNMax, NPP;

		for(int cell=0; cell<paleoClimate.getCellsLen(); cell++){
			// ~BUG: todas variaveis estão retornando com valor 0 ( menos NPP) -> provavel erro de calculo na função getClimCell()
			paleoClimate.getClimCell(cell, b, &SATMin, &SATMax, &PPTNMin, &PPTNMax, &NPP);

			// aqui começa a escrever os valores já interpolados nos arquivos de saida
			sprintf(temp_str, "%.3f", SATMin);			arq_outMinTemp.write(temp_str, strlen(temp_str));
			sprintf(temp_str, "%.3f", SATMax);          arq_outMaxTemp.write(temp_str, strlen(temp_str));
			sprintf(temp_str, "%.3f", PPTNMin);         arq_outMinPPTN.write(temp_str, strlen(temp_str));
			sprintf(temp_str, "%.3f", PPTNMax);         arq_outMaxPPTN.write(temp_str, strlen(temp_str));
			sprintf(temp_str, "%.3f", NPP);        		arq_outNPP.write(temp_str, strlen(temp_str));

			for(int t=1; t<=nt; t++){
				paleoClimate.getClimCell(cell, b-(t*s), &SATMin, &SATMax, &PPTNMin, &PPTNMax, &NPP);

				sprintf(temp_str, "\t%.3f", SATMin); 		arq_outMinTemp.write(temp_str, strlen(temp_str));
				sprintf(temp_str, "\t%.3f", SATMax); 		arq_outMaxTemp.write(temp_str, strlen(temp_str));
				sprintf(temp_str, "\t%.3f", PPTNMin);		arq_outMinPPTN.write(temp_str, strlen(temp_str));
				sprintf(temp_str, "\t%.3f", PPTNMax);		arq_outMaxPPTN.write(temp_str, strlen(temp_str));
				sprintf(temp_str, "\t%.3f", NPP);			arq_outNPP.write(temp_str, strlen(temp_str));
			}

			//pula uma linha de cada arquivo de saida
			arq_outMinTemp.write("\n", 1);
			arq_outMaxTemp.write("\n", 1);
			arq_outMinPPTN.write("\n", 1);
			arq_outMaxPPTN.write("\n", 1);
			arq_outNPP.write("\n", 1);
		}

	} catch(const fstream::failure &e) {
		printf(RED("Erro ao escrever em arquivo de saida %s\n"), e.what()); // ~DebugLog
		exit(1);
	} catch(exception &e){
		cout << "Standard exception: " << e.what() <<endl;
	}
	*/

	try{
		arq_outMinTemp.open(output_path + outSuffix + " - MinTemp.stream", ios::trunc | ios::out | ios::binary);
		arq_outMaxTemp.open( output_path+outSuffix+ " - MaxTemp.stream", ios::trunc | ios::out | ios::binary);
		arq_outMinPPTN.open( output_path+outSuffix+ " - MinPPTN.stream", ios::trunc | ios::out | ios::binary);
		arq_outMaxPPTN.open( output_path+outSuffix+ " - MaxPPTN.stream", ios::trunc | ios::out | ios::binary);
		arq_outNPP.open(output_path + outSuffix + " - NPP.stream", ios::trunc | ios::out | ios::binary);
	}catch (const fstream::failure &e){
		printf(RED("Erro ao abrir arquivo de saida %s\n"), e.what()); // ~DebugLog
		exit(1);
	}
	printf(GRN("\tArquivos de saida abertos com sucesso\n")); // ~DebugLog

	
	clock_t startClock, endClock;

	startClock = clock();

	try
	{
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
			
			float SATMin, SATMax, PPTNMin, PPTNMax, NPP;

			for (int cell = 0; cell < numCells; cell++){
				//calcula o clima da célula usando interpolação
				paleoClimate.getClimCell(cell, b-(i*s), &SATMin, &SATMax, &PPTNMin, &PPTNMax, &NPP);

				// aqui começa a escrever os valores já interpolados nos arquivos de saida
				arq_outMinTemp.write( (const char*)&SATMin, sizeof(float));
				arq_outMaxTemp.write( (const char*)&SATMax, sizeof(float));
				arq_outMinPPTN.write( (const char*)&PPTNMin, sizeof(float));
				arq_outMaxPPTN.write( (const char*)&PPTNMax, sizeof(float));
				arq_outNPP.write( (const char*)&NPP, sizeof(float));
			}
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

	arq_outMinTemp.close();
	arq_outMaxTemp.close();
	arq_outMinPPTN.close();
	arq_outMaxPPTN.close();
	arq_outNPP.close();
}
