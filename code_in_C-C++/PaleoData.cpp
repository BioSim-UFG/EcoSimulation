#include "LibPaleoData.h"
#include "../CompressLib/decompressData.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

int main(){

    ifstream presentClimFile, paleoClimFile;
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

    scanf("%[^\n]s", &input_str[0]);

    if(input_str[0] == 'Y' || input_str.find("yes") !=-1  || input_str.empty() ){
        presentClimFile.open("Coords and Clim.txt");
    }else{
        printf("Enter the number of cells(lines) in the grid: ");
        scanf("%d", &numero_de_linhas);
        presentClimFile.open(input_str);
    }

    system("clear");    //for unix
    //system("CLS");      //for DOS ( windows);

    printf("Is the paleoclimate file ' 'xPLASIM.All3DMats.Single.Zip.Stream' '?\n");
    printf("Type ENTER or \"Y\" if yes, or the correct name of the paleoclimate file\n");

    scanf("%[^\n]s", &input_str[0]);

    if (input_str[0] == 'Y' || input_str.find("yes") != -1 || input_str.empty()) {
        paleoClimFile.open("xPLASIM.All3DMats.Single.Zip.Stream");
    }
    else {
        scanf("%d", &numero_de_linhas);
        paleoClimFile.open(input_str);
    }



}
