#include "coordinateNormalization.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>


double Normalize::normalizeLatitude(double latitude){
    return ((latitude - MinMaxLatitude[0]) / (MinMaxLatitude[1] - MinMaxLatitude[0]));
}

double Normalize::normalizeLongitude(double longitude){
    return ((longitude - MinMaxLongitude[0]) / (MinMaxLongitude[1] - MinMaxLongitude[0]));
}

void Normalize::checkLong(double longitude){
    if(MinMaxLongitude[1] < longitude){
        MinMaxLongitude[1] = longitude;
    }
    else{
        if(MinMaxLongitude[0] > longitude){
            MinMaxLongitude[0] = longitude;
        }
    }

}

std::string Normalize::getNewFilePath(){return newName;}

std::string Normalize::getOldFilePath(){return name;}

void Normalize::checkLat(double latitude){
    if(MinMaxLatitude[1] < latitude){
        MinMaxLatitude[1] = latitude;
    }
    else{
        if(MinMaxLatitude[0] > latitude){
            MinMaxLatitude[0] = latitude;
        }
    }
}

void Normalize::generatePathNames(std::string file_path, std::string file_name, std::string file_extension){
    name = file_path;
    if(name[name.size()-1]!= '/') name+='/'; 
    name+=file_name;
    newName = name + " Normalized" + file_extension;
    name += file_extension;
}

void Normalize::treatFile(){
    getMaxMin();
    createNormalizedFile();
}

void Normalize::getMaxMin(){
    FILE *fp = fopen(name.c_str(), "r");
    double longitude,latitude,sat_min,sat_max,pptn_min,pptn_max,npp;
    char a[30], b[30], c[30], d[30], e[30], f[30], g[30], h[30];

    if(fp == NULL){
        puts("Falha ao abrir o arquivo DummyHex2566 - Coords and Clim.txt");
        exit(1);
    }

    fscanf(fp,"%s %s %s %s %s %s %s",a,b,c,d,e,f,g);
    while(!feof(fp)){
        fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf", &longitude, &latitude, &sat_min, &sat_max, &pptn_min, &pptn_max, &npp);
        checkLong(longitude);
        checkLat(latitude);
    }

    fclose(fp);
}

void Normalize::createNormalizedFile(){
    FILE *fp = fopen(name.c_str(), "r");
    FILE *newfp = fopen(newName.c_str(),"w");
    double longitude, latitude, sat_min, sat_max, pptn_min, pptn_max, npp;
    char a[30], b[30], c[30], d[30], e[30], f[30], g[30], h[30];

    if (fp == NULL){
        puts("Falha ao abrir o arquivo DummyHex2566 - Coords and Clim.txt");
        exit(1);
    }

    if(newfp == NULL){
        puts("Falha ao criar o arquivo novo");
        exit(1);
    }

    fscanf(fp, "%s %s %s %s %s %s %s", a, b, c, d, e, f, g);
    fprintf(newfp, "Long	Lat	sat_min	sat_max	pptn_min	pptn_max	npp\n");

    while(!feof(fp)){
        fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf", &longitude, &latitude, &sat_min, &sat_max, &pptn_min, &pptn_max, &npp);
        fprintf(newfp,"%lf  %lf %lf %lf %lf %lf %lf\n",normalizeLongitude(longitude), normalizeLatitude(latitude), sat_min, sat_max, pptn_min, pptn_max, npp);
    }

    fclose(fp);
    fclose(newfp);

}

Normalize::Normalize(std::string file_path, std::string file_name, std::string file_extension){
    MinMaxLongitude[0] = +180;
    MinMaxLongitude[1] = -180;
    MinMaxLatitude[0] = +90;
    MinMaxLatitude[1] = -180;

    generatePathNames(file_path,file_name,file_extension);
    treatFile();
}