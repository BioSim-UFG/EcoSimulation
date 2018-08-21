#include "LibPaleoData.h"
#include "../CompressLib/decompressData.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

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






}
