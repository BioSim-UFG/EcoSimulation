#include "LibPaleoData.h"
#include "decompressData.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>


using namespace SimEco;



double SimEco::roundTo(double number, int precision) {
	long double num = (long double)number;

	int decimals = pow(10, precision);
	return (round(num * decimals)) / decimals;
}

float SimEco::roundTo(float number, int precision) {
	double num = (double)number;

	int decimals = pow(10, precision);
	return ( round(num * decimals)) / decimals;
}


/* uma boa dar uma alterada aqui, usando files do C++ ao inves do FILE do C classico,
	referencia: http://www.cplusplus.com/doc/tutorial/files/

também, usar biblioteca de matrizes para realizar operações de forma mais facil e rápida. 
	Aramadillo lib:	http://arma.sourceforge.net/

*/

TPaleoClimate::TPaleoClimate(char *PLASIMFile, char *presentClimateFile, bool projectAnnomalies = true){
	int PLASIM_nLat, PLASIM_nLong;

	FILE *ZStream_compressed;
	byte *stream = NULL;
	size_t cur_pos=0;		//posição atual (em bytes) da stream
	size_t total_bytes = 0;

	projAnomalies = projectAnnomalies;	//~NOTA: por que copia o valor, sendo que ele só é lido uma vez no contrutor todo?


	//le e descomprime o Grid PLASIM
	ZStream_compressed = fopen(PLASIMFile, "rb");
	if(ZStream_compressed == NULL){
		printf("Erro ao abrir arquivo %s!\n",PLASIMFile );
		//aqui usar algo para "cancelar a criação do objeto", como um throw (exceptions e etc)
	}
	if (my_decompress(ZStream_compressed, &stream, &total_bytes) != Z_OK){
		printf("Erro ao descomprimir arquivo %s!\n",PLASIMFile );
		//aqui usar algo para "cancelar a criação do objeto", como um throw (exceptions e etc)
	}
	fclose(ZStream_compressed);

	if(total_bytes <=0){
		printf("Stream vazia\n" );
		//aqui usar algo para "cancelar a criação do objeto", como um throw (exceptions e etc)
	}


	PLASIM_nLong = ((int*)stream)[0];	//obtendo a quantidade da dados de Longitude
	cur_pos += sizeof(int);
	PLASIMLongs = (TSngVector)malloc(PLASIM_nLong * sizeof(float));
	memcpy(PLASIMLongs, stream+cur_pos, PLASIM_nLong * sizeof(float));	//lendo a longitude
	cur_pos += PLASIM_nLong * sizeof(float);


	PLASIM_nLat = ((int*)stream)[cur_pos]; 	//obtendo a quantidade da dados de Latitude
	cur_pos+= sizeof(int);
	PLASIMLats = (TSngVector)malloc(PLASIM_nLat * sizeof(float));
	memcpy(PLASIMLats, stream+cur_pos, PLASIM_nLat * sizeof(float));	//lendo a latitude
	cur_pos += PLASIM_nLat * sizeof(float);



	// Original PLASIM data starts at 5.000.000 ya (5 million years ago, 5Mya), and may have different temporal resolutions (1ky or 100y)
	// For this class, user can either specify time as an integer (in that case, the time step), or actual time (in that case, kya)
	PLASIMnTime = ((int*)stream)[cur_pos];
	cur_pos += sizeof(int);

	// PLASIMNTimeOffset = 1 if PLASIMnTime = 5001, or PLASIMNTimeOffset = 10 if PLASIMnTime = 50001
	PLASIMnTimeOffset = (PLASIMnTime-1) / 5000;


	// Read PLASIM Minimum Temperature
	PLASIMDataSATMin = (TSngMatrix *)malloc( PLASIM_nLong * sizeof(TSngMatrix));	//SATmin -> Surface Air Temperature Min ( minima do ano -> inverno)
	for(int i=0; i<PLASIM_nLong; i++){
		PLASIMDataSATMin[i] = (TSngMatrix)malloc( PLASIM_nLat * sizeof(float *));
		for(int j=0; j<PLASIM_nLat; j++){
			*(PLASIMDataSATMin[i] +j) = (float *)malloc( PLASIMnTime * sizeof(float ));

			memcpy(  *(PLASIMDataSATMin[i] +j), stream+cur_pos, PLASIMnTime *sizeof(float)  );
			cur_pos += PLASIMnTime * sizeof(float);
		}
	}

	// Read PLASIM Maximum Temperature
	PLASIMDataSATMax = (float ***)malloc( PLASIM_nLong * sizeof(float **));	//SATmax -> Surface Air Temperature Max ( maxima do ano -> verão)
	for(int i=0; i<PLASIM_nLong; i++){
		PLASIMDataSATMax[i] = (float **)malloc( PLASIM_nLat * sizeof(float *));
		for(int j=0; j<PLASIM_nLat; j++){
			*(PLASIMDataSATMax[i] +j) = (float *)malloc( PLASIMnTime * sizeof(float ));

			memcpy(  *(PLASIMDataSATMax[i] +j), stream+cur_pos, PLASIMnTime *sizeof(float)  );
			cur_pos += PLASIMnTime * sizeof(float);
		}
	}

	// Read PLASIM Minimum Precipitation
	PLASIMDataPPTNMin = (float ***)malloc( PLASIM_nLong * sizeof(float **));		//PPTN -> PerciPiTatioN 
	for(int i=0; i<PLASIM_nLong; i++){
		PLASIMDataPPTNMin[i] = (float **)malloc( PLASIM_nLat * sizeof(float *));
		for(int j=0; j<PLASIM_nLat; j++){
			*(PLASIMDataPPTNMin[i] +j) = (float *)malloc( PLASIMnTime * sizeof(float ));

			memcpy(  *(PLASIMDataPPTNMin[i] +j), stream+cur_pos, PLASIMnTime *sizeof(float)  );	
			cur_pos += PLASIMnTime * sizeof(float);
			for(int k=0; k<PLASIMnTime ;k++){
				*(*(PLASIMDataPPTNMin[i] +j)+k) = (*(*(PLASIMDataPPTNMin[i] +j)+k) * 365) /4;	// Convert mm/day to mm/season
			}
		}
	}

	// Read PLASIM Maximum Precipitation
	PLASIMDataPPTNMax = (float ***)malloc( PLASIM_nLong * sizeof(float **));		//PPTN -> PerciPiTatioN 
	for(int i=0; i<PLASIM_nLong; i++){
		PLASIMDataPPTNMax[i] = (float **)malloc( PLASIM_nLat * sizeof(float *));
		for(int j=0; j<PLASIM_nLat; j++){
			*(PLASIMDataPPTNMax[i] +j) = (float *)malloc( PLASIMnTime * sizeof(float ));

			memcpy(  *(PLASIMDataPPTNMax[i] +j), stream+cur_pos, PLASIMnTime *sizeof(float)  );	
			cur_pos += PLASIMnTime * sizeof(float);
			for(int k=0; k<PLASIMnTime ;k++){
				*(*(PLASIMDataPPTNMax[i] +j)+k) = (*(*(PLASIMDataPPTNMax[i] +j)+k) * 365) /4;	// Convert mm/day to mm/season
			}
		}
	}

	// Read PLASIM NPP
	PLASIMDataNPP = (float ***)malloc( PLASIM_nLong * sizeof(float **));	//NPP -> Net Primary productivity (produtividade primaria líquida, basicamente a produtividade "ecologica", dependente da quantidade de luz e agua)
	for(int i=0; i<PLASIM_nLong; i++){
		PLASIMDataNPP[i] = (float **)malloc( PLASIM_nLat * sizeof(float *));
		for(int j=0; j<PLASIM_nLat; j++){
			*(PLASIMDataNPP[i] +j) = (float *)malloc( PLASIMnTime * sizeof(float ));

			memcpy(  *(PLASIMDataNPP[i] +j), stream+cur_pos, PLASIMnTime *sizeof(float)  );
			cur_pos += PLASIMnTime * sizeof(float);
		}
	}

	free(stream);



	// Open a file containing the coordinates of model grid, and "observed" present climate
	// The order of environmental variables must be: SAT_Min, SAT_Max, PPTN_Min, PPTN_Max
/*
	ifstream modelGrid_arq(PresentClimateFile, ios::in | ios::binary | ios:ate);	//abre arquivo como input, binario e posiciona ao final
	if(! modelGrid_arq.isOpen() ){
		printf("Erro ao abrir arquivo %s!\n",PresentClimateFile );
		//aqui usar algo para "cancelar a criação do objeto", como um throw (exceptions e etc)
	}
	streampos size = file.tellg();		//obtem tamanho do arquivo
	modelGrid_arq.seekg(0, ios::beg);	//posiciona ao inicio
	modelGridObsClimate = (double **)malloc(size);		//aloca memoria para armazenar os dados na variavel
	modelGrid_arq.read((char*)modelGridObsClimate, size);
	modelGrid_arq.close();
*/

	FILE *modelGrid_arq = fopen(presentClimateFile,"r");
		if( modelGrid_arq == NULL ){
		printf("Erro ao abrir arquivo %s!\n",presentClimateFile );
		//aqui usar algo para "cancelar a criação do objeto", como um throw (exceptions e etc)
	}

	fscanf(modelGrid_arq,"%*[^\n]\n");	//descarta a primeira linha

	//~NOTA: aqui le e armazena tudo como double, mas depois tudo é convertido para float, 
	//pq não já ler como float,e tb a variavel ser uma matriz de floats ( ao inves de doubles)?
	modelGridObsClimate = (TDblMatrix)malloc(GRID_LINES * sizeof(double*));
	for(int i=0; i<GRID_LINES;i++){
		modelGridObsClimate[i] = (double *)malloc(GRID_COLS * sizeof(double));
	}

	readGrid(modelGrid_arq);
	modelGridnCells = GRID_LINES;

	// Create a map of references between Model grid cell and PLASIM cells
	/* ~NOTA: aqui , no cod em Delphi, determinou o tamanho dos vetores, porém em C/C++ nao existem a variavel length
	a nao ser que passemos a usar uma classe de vetor (recomendo fazer isso quando der).

   SetLength(wLon, ModelGridnCells);
   SetLength(wLat, ModelGridnCells);
   SetLength(adjLeft, ModelGridnCells);
   SetLength(adjDown, ModelGridnCells);
	*/
	wLon = (TSngVector )malloc(modelGridnCells * sizeof(float));
	wLat = (TSngVector )malloc(modelGridnCells * sizeof(float));
	adjLeft = (short *)malloc(modelGridnCells * sizeof(short));
	adjDown = (short *)malloc(modelGridnCells * sizeof(short));

   // Find the four cells whose centroids form the square, in which the modeled cell is contained.
   // The relative distance to each of those centroids will be used as weights for the interpolation

	// For each cell in the modeled grid
	for(int c=0; c< modelGridnCells; c++){
		// Start with a dummy/flag index
		adjLeft[c] = -1;

		// SEARCH THROUGH LONGITUDES FIRST, FIND THE INDEX OF THE ADJACENT COLUMN ON THE LEFT

		// Progresses through all columns of the map, from west to east, starting at lower longitudes (Alaska)
		// Finds the coordinate of the adjacent cell in the west
		for(int i=0; i< PLASIM_nLong; i++){
			// Is the current column of the PLASIM grid to the left of the modeled grid?
			if( PLASIMLongs[i] < modelGridLong[c]){
				// Records the column of the adjacent column to the left (west)
				// Keep updating for as long as it can find cells in the west, until it finds the adjacent column in the left
				adjLeft[c] = i;
			}
			else{
				// and if we have already searched all columns on the left (west) cells
				if (adjLeft[c] != -1)	break;	// then there is no reason to keep searching, because all future columns will also be to the right (east)
				
			}
		}

		// SEARCH THROUGH LATITUDES, FIND THE INDEX OF THE ADJACENT ROW BELOW

		// if the current cell has the same latitude as the previous one
		if( c > 0 && modelGridLat[c]==modelGridLat[c-1] ){
			// then its neighbor to the south is also in the same row
			adjDown[c] = adjDown[c-1];
		}
		else{
			// Start with a dummy/flag index
			adjDown[c] = 0;

			// Progresses through all rows (latitudes) of the original PLASIM grid
			// We will start in the north (positive latitudes), and progresses south towards negative latitudes
			for(int j=0; j<PLASIM_nLat; j++){
				// If this PLASIM row is immediatelly to the south, then
				if( PLASIMLats[j] < modelGridLat[c] ){
					// Record the index of the row (latitude) immediatelly below the desired cell
					adjDown[c] = j;
					break;
				}
			}
		}

		// Pre-calculate the weights for interpolation of all model cells
		// Now that we have row and column of the adjacent cell in the left/bottom, we can calculate the weight given to the next row (right) and column (top)

		// Weight for western (right) PLASIM cell
		// Weight for the easten (left) PLASIM cell is (1 - western)
		wLon[c] = (PLASIMLongs[adjLeft[c]+1] - modelGridLong[c] ) / (PLASIMLongs[adjLeft[c]+1] - PLASIMLongs[adjLeft[c]]);
		if(wLon[c] > 1 || wLon[c] < 0)
			wLon[c] = 0;

		// weight for southern (bottom) PLASIM cell
		// Weight for the northern (top) PLASIM cell is (1 - northern)
		wLat[c] = (PLASIMLats[adjDown[c]-1]  - modelGridLat[c]) / (PLASIMLats[adjDown[c]-1]  - PLASIMLats[adjDown[c]]);
		if(wLat[c] > 1 || wLat[c] < 0)
			wLat = 0;
	}


	if( projAnomalies ){
		// A first call to Interpolate is necessary to calculate the PLASIM prediction of climate for the current time
		// This PLASIM prediction to the present is necessary to set a baseline for the computation of the anomalies
		modelGridPLASIMClimate = (TSngMatrix)malloc(modelGridnCells * sizeof(float *));
		for(int i=0; i<modelGridnCells; i++)
			modelGridPLASIMClimate[i] = (float *)malloc(5 * sizeof(float));


		for(int c=0; c<modelGridnCells; c++){

			getClimCell(c,(int)-1, 							// Yes, the time is negative because this is a first call
						*modelGridPLASIMClimate[c],			// Yes, the matrix is CurrentPLASIM because this is a first call
						*(modelGridPLASIMClimate[c] +1),
						*(modelGridPLASIMClimate[c] +2),
						*(modelGridPLASIMClimate[c] +3),
						*(modelGridPLASIMClimate[c] +4) );
		}
	}

}

void TPaleoClimate::readGrid(FILE *arq){
	for(int i=0;i < GRID_LINES; i++){
		//le as duas primeira colunas (de longitude de latitude)
		fscanf(arq,"%f", &modelGridLong[i]);	
		fscanf(arq,"\t%f", &modelGridLat[i]);
		//le o restante das colunas
		for(int j=2; j < GRID_COLS; j++){
			fscanf(arq,"\t%lf", (modelGridObsClimate[i]+j) );
		}
		fscanf(arq,"\n");
	}
}




void TPaleoClimate::getClimCell(int c, double timeKya, float SATMin, float SATMax, float PPTNMin, float PPTNMax, float NPP){

	int timeInt, i;
	int timeIntPrev, timeIntNext;
	double distPrev, distNext;
	double tmp;

  // A negative time should not happen
  // The only exception is the creation of the classe, when the raw PLASIM estimate of climate in the last time step is performed in the model grid
  // That estimate is used to later plug PLASIM annomalies into the current "observe" climate data
	if(timeKya < 0){
		timeInt = -1;
		getClimCell(c, timeInt,SATMin,SATMax, PPTNMin, PPTNMax, NPP);
	}
	else{	
		//troquei o função Trunc() por um cast (int), pois ela apenas trunca um ponto flutuante, retornando a parte inteira
		tmp = (timeKya * PLASIMnTimeOffset) - ((int)timeKya * PLASIMnTimeOffset);

  /************************** ~NOTA: POSSIVEL ERRO NO codigo em DELPHI************************************/
		//não existe roundTo em c++, o que fazer?
		//encontrei alternativas, mas que funcionam de forma um pouco diferentes: 
		//https://stackoverflow.com/questions/11208971/round-a-float-to-a-given-precision
		tmp = roundTo(tmp, 4);

		// User has requested the estimated climate at an exact millenia.
    	// There is no need for temporal interpolation of PLASIM data, just spatial interpolation
    	if( tmp < 0.001f){
    		// This is the time step that we want
    		timeInt = (int)((5000 - timeKya) * PLASIMnTimeOffset);

    		// Maybe the user has done some previous interpolation that may be useful here...
    		if (lastInterpolation != NULL){
    			
    			if(timeInt == lastInterpolation[c].prevTimeStep){		// Previous time step?
    				SATMin = lastInterpolation[c].prevSATMin;
					SATMax = lastInterpolation[c].prevSATMax;
					PPTNMin = lastInterpolation[c].prevPPTNMin;
					PPTNMax = lastInterpolation[c].prevPPTNMax;
					NPP = lastInterpolation[c].prevNPP;
    			}
    			else if(timeInt == lastInterpolation[c].nextTimeStep){	// Next time step?
    				SATMin = lastInterpolation[c].prevSATMin;
					SATMax = lastInterpolation[c].prevSATMax;
					PPTNMin = lastInterpolation[c].prevPPTNMin;
					PPTNMax = lastInterpolation[c].prevPPTNMax;
					NPP = lastInterpolation[c].prevNPP;
    			}
    			else{		// Yes... we have to calculate it...
    				getClimCell(c, timeInt, SATMin, SATMax, PPTNMin, PPTNMax, NPP);
    			}
    		}
    		else{
    			getClimCell(c, timeInt, SATMin, SATMax, PPTNMin, PPTNMax, NPP);
    		}
    	}
    	else{			// User has requested an estime of time in between millenia.
    					// In this case, one must temporally interpolate between two time-slices of PLASIM data

    		// Has the user done some interpolation yet?
      		// If not, then create some space to save and later recycle the estimates
    		if( lastInterpolation == NULL){
    			lastInterpolation = (TLastInterpolation*)malloc(modelGridnCells * sizeof(TLastInterpolation));	//correspondente ao SetLength()

    			for(i=0; i< modelGridnCells; i++){
					lastInterpolation[i].prevTimeStep = -1;
					lastInterpolation[i].nextTimeStep = -1;
    			}
    		}

    		// Climate in the last time step before the request
			timeIntPrev = 5000 - ((int)(timeKya * PLASIMnTimeOffset) + 1);
			// Climate in the time step immediately after the request
			timeIntNext = 5000 - (int)(timeKya * PLASIMnTimeOffset);

			// is the climate estimates that we need already computed?
			if( timeIntPrev == lastInterpolation[c].prevTimeStep ){
				// Do nothing... as we already have computed these values...
			}
			// Is it computed as future time step, and now that the user has changed the time slice it is past?
			else if( timeIntPrev == lastInterpolation[c].nextTimeStep ){	
				// Transfer the data from the future to the past
				lastInterpolation[c].prevTimeStep = lastInterpolation[c].nextTimeStep;
				lastInterpolation[c].prevSATMin = lastInterpolation[c].nextSATMin;
				lastInterpolation[c].prevSATMax = lastInterpolation[c].nextSATMax;
				lastInterpolation[c].prevPPTNMin = lastInterpolation[c].nextPPTNMin;
				lastInterpolation[c].prevPPTNMax = lastInterpolation[c].nextPPTNMax;
				lastInterpolation[c].prevNPP = lastInterpolation[c].nextNPP;
			}
			else{
				// We need to re-estimate the climate for both time steps, as they have not yet been computed

				// Saves the estimated climate in the two adjacent time steps,
				// in case the user wants to interpolate again within the interval
				lastInterpolation[c].prevTimeStep = timeIntPrev;
				getClimCell(c, timeIntPrev,
                    		lastInterpolation[c].prevSATMin,
                    		lastInterpolation[c].prevSATMax,
                    		lastInterpolation[c].prevPPTNMin,
                    		lastInterpolation[c].prevPPTNMax,
                    		lastInterpolation[c].prevNPP);
			}

			if( timeIntNext == lastInterpolation[c].nextTimeStep ){
				// Do nothing here...
			}else if( timeIntNext == lastInterpolation[c].prevTimeStep ){
				// Transfer the data from the past to the future 		/* ~NOTA: esse comentário está certo mesmo??, nao seria o contrário?
				lastInterpolation[c].nextTimeStep = lastInterpolation[c].prevTimeStep;
				lastInterpolation[c].nextSATMin = lastInterpolation[c].prevSATMin;
				lastInterpolation[c].nextSATMax = lastInterpolation[c].prevSATMax;
				lastInterpolation[c].nextPPTNMin = lastInterpolation[c].prevPPTNMin;
				lastInterpolation[c].nextPPTNMax = lastInterpolation[c].prevPPTNMax;
				lastInterpolation[c].nextNPP = lastInterpolation[c].prevNPP;
			}
			else{
				// We need to re-estimate the climate for both time steps...
				lastInterpolation[c].nextTimeStep = timeIntNext;
				getClimCell(c, timeIntNext,
			                lastInterpolation[c].nextSATMin,
			                lastInterpolation[c].nextSATMax,
			                lastInterpolation[c].nextPPTNMin,
			                lastInterpolation[c].nextPPTNMax,
			                lastInterpolation[c].nextNPP);
			}

			distNext = timeKya - ((int)timeKya);
      		distPrev = 1 - distNext;

      		SATMin =  (lastInterpolation[c].prevSATMin * distPrev)  + (lastInterpolation[c].nextSATMin * distNext);
      		SATMax =  (lastInterpolation[c].prevSATMax * distPrev)  + (lastInterpolation[c].nextSATMax * distNext);
      		PPTNMin = (lastInterpolation[c].prevPPTNMin * distPrev) + (lastInterpolation[c].nextPPTNMin * distNext);
      		PPTNMax = (lastInterpolation[c].prevPPTNMax * distPrev) + (lastInterpolation[c].nextPPTNMax * distNext);
      		NPP = (lastInterpolation[c].prevNPP * distPrev) + (lastInterpolation[c].nextNPP * distNext);
    	}
	}
}

void TPaleoClimate::getClimCell(int c, int timeStep,  float SATMin, float SATMax, float PPTNMin, float PPTNMax, float NPP){
	int t;
	double Tmp1,Tmp2;
	bool firstInterpolation;

	// With the definition of the four adjacent cells, whose centroids form the square, in which the modeled cell is contained...
	// And having calculated the relative distance to each of those centroids, to be used as weights for the interpolation...
	// We now calculate the interpolated value for the cell, for a given time step

	// A regular call to the interpolate function
	if(timeStep >= 0){
		t = ((int)timeStep);
		firstInterpolation = false;
	}

	// A special call to the interpolation function, to set up the prediction of present climate by PLASIM in the model grid
	else{
		t = PLASIMnTime - 1;
		firstInterpolation = true;
	}

	// First interpolate the absolute PLASIM climate on the model grid

	if( (adjLeft[c] >= 63 ) || (adjDown[c] <= 0)){
		//valores substituidos pelos piores ints possiveis
		SATMax = NAN;
		SATMin = NAN;
		PPTNMax = NAN;
		PPTNMin = NAN;
 		// qual numero deve ser usado para inicializar?
 		NPP = 0 ;
	}

	else{
		// If there are no PLASIM cells to the left
     	// So consider only the value at the cell to the right
		if(wLon[c] == 0){
			Tmp1 = PLASIMDataSATMax [adjLeft[c]+1][adjDown[c] ][t]; //cell at the south
			Tmp2 = PLASIMDataSATMax [adjLeft[c]+1][adjDown[c]-1][t]; //cell at the north
		}
		else{
			// If there are no cells to the right
     		// consider only the cell to the left
			if( wLon[c] == 1){
				Tmp1 = PLASIMDataSATMax[adjLeft[c]][adjDown[c]][t]; // cell at the south
       			Tmp2 = PLASIMDataSATMax[adjLeft[c]][adjDown[c]-1][t]; // cell at the north
			}
			
			// If there are both left and right cells **Aqui pode chegar? porque se tem nos dois tem em um****
			else{
				//The southern border
				Tmp1 = PLASIMDataSATMax[adjLeft[c] ][adjDown[c] ][t ] * wLon[c] +	// the eastern border       PLASIMLongs[adjLeft[c]]      PLASIMLats[adjDown[c]]
                	PLASIMDataSATMax[adjLeft[c]+1 ][adjDown[c] ][t ] * (1 - wLon[c]);	// the western border       PLASIMLongs[adjLeft[c]+1]    PLASIMLats[adjDown[c]]
				
				//The northern border 
                Tmp2 = PLASIMDataSATMax[adjLeft[c] ][ adjDown[c]-1 ][ t] * wLon[c] +   // the eastern border       PLASIMLongs[adjLeft[c]]      PLASIMLats[adjDown[c]-1]
              		PLASIMDataSATMax[adjLeft[c]+1 ][adjDown[c]-1 ][t ] * (1 - wLon[c]);   // the western border       PLASIMLongs[adjLeft[c]+1]    PLASIMLats[adjDown[c]-1]
			}
		}

		//Esse NPP está na parte do if/else correta?
		NPP =  Tmp1 * wLat[c] +
           Tmp2 * (1 - wLat[c]);

        if( NPP < 0 )
        	NPP = 0;
	}

    // A first call to Interpolate is necessary to calculate the raw PLASIM prediction of climate at present time
  	// This PLASIM prediction is later used as a baseline for the computation of the anomalies in relation to the present observed climate (i.e. worldclim)
  	// Because the code below is the application of annomalies in relation to the present observed climate, and here we need the raw PLASIm prediction, we may just exit here
    // If, instead, we want to interpolate the raw PLASIM data for all time steps, then we can just disable the "Exit" also calculate the "annomalies" during the creation of the class
	if(firstInterpolation || (! projAnomalies))
		return;

// Convert anomalies from current climate

	// Temperature
    // Additive anomalies for temperature
	SATMin = SATMin - //Current Time Step
			modelGridPLASIMClimate[c ][0 ] + //Last time step
			modelGridObsClimate[c ][0 ]; //Current climatology (sat_min, sat_max, pptn_min, pptn_max)

	SATMax = SATMax -
			modelGridPLASIMClimate[c ][1 ] +
			modelGridObsClimate[c ][1 ];

	// If SATMax is less than SatMin, then calculate the average
	if( SATMax < SATMin ){
		SATMax = (SATMax + SATMin) /2;
		SATMin = SATMax;
	} 

	// Multiplicative anomalies for precipitation
	if( modelGridPLASIMClimate[c ][3 ] > 0 ){
		if(modelGridObsClimate[c ][3 ] <= modelGridPLASIMClimate[c ][3 ] )
			PPTNMax = (PPTNMax / modelGridPLASIMClimate[c ][3 ]) * modelGridObsClimate[c ][3 ];
		else
			PPTNMax = PPTNMax - modelGridPLASIMClimate[c ][3 ] + modelGridObsClimate[c ][3 ];
	}
	else
		PPTNMax = 0;

	if(modelGridPLASIMClimate[c ][2 ] > 0){
		if(modelGridObsClimate[c ][2 ] <= modelGridPLASIMClimate[c ][2 ])
			PPTNMin = (PPTNMin / modelGridPLASIMClimate[c ][2 ]) * modelGridPLASIMClimate[c ][2 ];
		else
			PPTNMin = PPTNMin - modelGridPLASIMClimate[c ][2 ] + modelGridObsClimate[c ][2 ]; 
	}
	else
		PPTNMin = 0;

	// If PPTNMax is less than PPTNMin, then calculate the average
	if(PPTNMax < PPTNMin){
		PPTNMax = (PPTNMax + PPTNMin) / 2;
		PPTNMin = PPTNMax;
	}

/*
//Parte apenas copiada, já que estava comentada
// For testing purposes one may decide to plot the raw climatology or emulated data
{
  SATMin:= ModelGridPLASIMClimate[c,0];
  SATMax:= ModelGridPLASIMClimate[c,1];
  PPTNMin:= ModelGridPLASIMClimate[c,2];
  PPTNMax:= ModelGridPLASIMClimate[c,3];
}
{
  SATMin:= ModelGridObsClimate[c,0];
  SATMax:= ModelGridObsClimate[c,1];
  PPTNMin:= ModelGridObsClimate[c,2];
  PPTNMax:= ModelGridObsClimate[c,3];
}
{
  SATMin:= ModelGridPLASIMClimate[c,0] - ModelGridObsClimate[c,0];
  SATMax:= ModelGridPLASIMClimate[c,1] - ModelGridObsClimate[c,1];
  If SATMax > +20 then
    SATMax:= + 20;
  If SATMax < -20 then
    SATMax:= -20;
  If SATMin > +20 then
    SATMin:= + 20;
  If SATMin < -20 then
    SATMin:= -20;

  PPTNMin:= ModelGridPLASIMClimate[c,2] - ModelGridObsClimate[c,2];
  PPTNMax:= ModelGridPLASIMClimate[c,3] - ModelGridObsClimate[c,3];
  If PPTNMax > +2000 then
    PPTNMax:= + 2000;
  If PPTNMax < -2000 then
    PPTNMax:= -2000;
  If PPTNMin > +2000 then
    PPTNMin:= + 2000;
  If PPTNMin < -2000 then
    PPTNMin:= -2000;
}
*/

// Capping PPTN at 2000mm / season
	if(PPTNMin > 2000)
		PPTNMin = 2000;
	if(PPTNMax > 2000)
		PPTNMax = 2000;
	
	// Multiplicative anomalies for NPP
	if(!isnan(modelGridObsClimate[c ][4 ])){
		if(modelGridPLASIMClimate[c ][4 ] > 0){
			NPP = (NPP / modelGridPLASIMClimate[c ][4 ]) * modelGridObsClimate[c ][4 ];

		}
		else{
			NPP = 0;
		}

		if(NPP < 0 ){
			NPP = 0;
		}
	}
	else{
		NPP = NAN;
	}
}

int main(){

}