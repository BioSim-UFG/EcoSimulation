#include "LibPaleoData.h"
#include <math.h>

void LibPaleoData::getClimCell(int c, double timeKya, float SATMin, float SATMax, float PPTNMin, float PPTNMax, float NPP){

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
		//não existe roundTo em c++, o que fazer?
		//encontrei alternativas, mas que funcionam de forma um pouco diferentes: 
		//https://stackoverflow.com/questions/11208971/round-a-float-to-a-given-precision
		tmp = roundTo(tmp, 4);


	}

}