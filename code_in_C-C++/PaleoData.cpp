#include "LibPaleoData.h"
#include "../CompressLib/decompressData.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

using namespace std;
using namespace SimEco;

int main(){

	string presentClimFile, paleoClimFile, r;
	string outSuffix;
	double b, e, s; 

	string input_str(100, '\0');


	//temos um problema a decidir como resolver:
	// no arquivo geader (.h) o numero de linhas é definido por uma MACRO, e aqui seria uma boa pedir o numero de linhas
	//e passar como argumento para função da classe.
	//ps: aqui, precisa de saber o numero de linhas(celulas) antes de ler o arquivo;
	int numero_de_linhas=0; 

	printf("Is the grid + present climate file ' 'Coords and Clim.txt' '?\n");
	printf("This file should as many rows as there are grid cells, and 7 columns:\n");
	printf("1) Longitude\n");
	printf("2) Latitude\n");
	printf("3) Temperature in the coldest quarter\n");
	printf("4) Temperature in the warmest quarter\n");
	printf("5) Precipitation in the driest quarter\n");
	printf("6) Precipitation in the wettest quarter\n");
	printf("7) Annual NPP\n\n");
	printf("Type ENTER or \"Y\" if yes, or type the correct name of the paleoclimate file\n");

	scanf(" %[^\n]s", &input_str[0]);

	if(input_str[0] == 'Y' || input_str.find("yes") !=-1  || input_str.empty() ){
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

	if (input_str[0] == 'Y' || input_str.find("yes") != -1 || input_str.empty()) {
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

	if( input_str[0] == 'Y' || input_str.find("yes") != -1 || input_str.empty() ){
		outSuffix = "!OutPaleoClim";
	}else{
		outSuffix = input_str;
	}

	system("clear");    //for unix
	//system("CLS");      //for DOS ( windows);

	printf("Would you like to start 5M years ago?\n\n");
	printf("Type ENTER or \"Y\" if yes, or enter the starting time in thousand of years ago (5000 = 5Mya)\n");

	scanf(" %[^\n]s", &input_str[0]);

	if( input_str[0] == 'Y' || input_str.find("yes") != -1 || input_str.empty() ){
		b = 5000.0;
	}else{
		b = stod(input_str, NULL);
	}

	printf("Would you like to end in the present time?\n\n");
	printf("Type ENTER or \"Y\" if yes, or enter the ending time in thousand of years ago (1000 = 1Mya)\n");

	scanf(" %[^\n]s", &input_str[0]);

	if( input_str[0] == 'Y' || input_str.find("yes") != -1 || input_str.empty() ){
		e = 0;
	}else{
		e = stod(input_str);
	}

	printf("Would you like to use 500 years as time step?\n\n");
	printf("Type ENTER or \"Y\" if yes, or enter the length of time step in thousand of years ago (0.5 = 500y)\n");

	scanf(" %[^\n]s", &input_str[0]);

	if( input_str[0] == 'Y' || input_str.find("yes") != -1 || input_str.empty() ){
		s = 0.5;
	}else{
		s = stod(input_str);
	}

	//creates a new Object of class TPaleoClimate
	TPaleoClimate paleoClimate(paleoClimFile.c_str(), presentClimFile.c_str(), numero_de_linhas, true);

	fstream arq_outMinTemp, arq_outMaxTemp, arq_outMinPPTN, arq_outMaxPPTN, arq_outNPP;

	arq_outMinTemp.open( outSuffix + " - MinTemp.txt", ios::trunc | ios::out);
	arq_outMaxTemp.open( outSuffix + " - MaxTemp.txt", ios::trunc | ios::out);
	arq_outMinPPTN.open( outSuffix + " - MinPPTN.txt", ios::trunc | ios::out);
	arq_outMaxPPTN.open( outSuffix + " - MaxPPTN.txt", ios::trunc | ios::out);
	arq_outNPP.open(outSuffix + " - NPP.txt", ios::trunc | ios::out);


	//o que é nt?
	int nt = (b-e)/s;

	char temp_str[100];
	sprintf(temp_str,"T%.1f",b);
	streamsize size = strlen(temp_str);
	arq_outMinTemp.write(temp_str, size);
	arq_outMaxTemp.write(temp_str, size);
	arq_outMinPPTN.write(temp_str, size);
	arq_outMaxPPTN.write(temp_str, size);
	arq_outNPP.write(temp_str, size);

	for(int i=0; i<nt; i++){
		sprintf(temp_str, "\tT%.1f", b - (i * s));
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
		paleoClimate.getClimCell(cell, b, &SATMin, &SATMax, &PPTNMin, &PPTNMax, &NPP);

		sprintf(temp_str, "%.3f", SATMin);          arq_outMinTemp.write(temp_str, 5);
		sprintf(temp_str, "%.3f", SATMax);          arq_outMaxTemp.write(temp_str, 5);
		sprintf(temp_str, "%.3f", PPTNMin);         arq_outMinPPTN.write(temp_str, 5);
		sprintf(temp_str, "%.3f", PPTNMax);         arq_outMaxPPTN.write(temp_str, 5);
		sprintf(temp_str, "%.3f", NPP);        		arq_outNPP.write(temp_str, 5);

		for(int t=1; t<=nt; t++){
			paleoClimate.getClimCell(cell, b-(t*s), &SATMin, &SATMax, &PPTNMin, &PPTNMax, &NPP);

			sprintf(temp_str, "\t%.3f", SATMin); 		arq_outMinTemp.write(temp_str, strlen(temp_str));
			sprintf(temp_str, "\t%.3f", SATMax); 		arq_outMaxTemp.write(temp_str, strlen(temp_str));
			sprintf(temp_str, "\t%.3f", PPTNMin);		arq_outMinPPTN.write(temp_str, strlen(temp_str));
			sprintf(temp_str, "\t%.3f", PPTNMax);		arq_outMaxPPTN.write(temp_str, strlen(temp_str));
			sprintf(temp_str, "\t%.3f", NPP);			arq_outNPP.write(temp_str, strlen(temp_str));
		}

		arq_outMinTemp.write("\n", 1);
		arq_outMaxTemp.write("\n", 1);
		arq_outMinPPTN.write("\n", 1);
		arq_outMaxPPTN.write("\n", 1);
		arq_outNPP.write("\n", 1);
	}

	arq_outMinTemp.close();
	arq_outMaxTemp.close();
	arq_outMinPPTN.close();
	arq_outMaxPPTN.close();
	arq_outNPP.close();
}
