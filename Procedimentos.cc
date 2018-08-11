#include "LibPaleoData.h"
#include <math.h>
#include <stdlib.h>

using namespace SimEco;


double SimEco::roundTo(double number, int precision) {
	long double num = (long double)number;

	int decimals = std::pow(10, precision);
	return (std::round(num * decimals)) / decimals;
}

float SimEco::roundTo(float number, int precision) {
	double num = (double)number;

	int decimals = std::pow(10, precision);
	return (std::round(num * decimals)) / decimals;
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

/************************** ~NOTA: POSSIVEL ERRO NO DELPHI************************************/
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
				// Transfer the data from the future to the past 		/* ~NOTA: esse comentário está certo mesmo??, nao seria o contrário?
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


}

int main(){

}