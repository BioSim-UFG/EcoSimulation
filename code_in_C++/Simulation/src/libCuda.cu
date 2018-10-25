#include "Simulation.h"
#include "SimTypes.h"


// from https://rosettacode.org/wiki/Sutherland-Hodgman_polygon_clipping#C
#define nSteps 5 //precisão do poligono / número de pontos no poligono
#define NENV 4
#define NESPECIES 70000
#define NCELLS 20000
#define CONST_WARPS 6 //Numero arbitrario do multiplo das warps  ( corresponde a quantidade de warps em cada bloco)


#define erfA 0.278393f      // Abramowitz e Stegun approximation to erf
#define erfB 0.230389f      // https://en.wikipedia.org/wiki/Error_function
#define erfC 0.000972f
#define erfD 0.078108f
#define pi M_PI

typedef struct { 
    float x, y; 
} vec_t, *vec;

typedef struct { 
    vec v; 
    int len;
}poly_t, *poly;

__device__ float area(poly resPoly){
    float area = 0.0f;
    float d;
    int j, i;

    if (resPoly->len > 2) {
        j = resPoly->len - 1;
        for (i = 0; (i < resPoly->len); i++) {
            d = (resPoly->v[j].x + resPoly->v[i].x);
            area = area + d * (resPoly->v[j].y - resPoly->v[i].y);
            j = i;
        }
        area = -area * 0.5f;
    }
    return area;
}

__device__ void NicheCurve(float MinTol, float MidTol, float MaxTol,float MinEnv, float MaxEnv, poly NichePoly){
    // Begin of variable declarations

    float erfX;
    float erfY;
    float PhiNum;
    float PhiDen1;
    float PhiDen2;
  
    // Read input data
    float mi =    MidTol;
    float sigma = (MaxTol - mi) / 2.0;
    float a =     MinTol;
    float b =     MaxTol;
  
    float x;// = MaxTol;
    float MinimumMax = MaxTol < MaxEnv? MaxTol:MaxEnv;
    float MaximumMin = MinTol > MinEnv? MinTol:MinEnv;
    float p;
    float Tmp;
  
    float Step;
    
    // Begin of procedure
    // resPol must be nSteps+3 long
    //Step = ((b-a) / nSteps);
    Step = ((MinimumMax-MaximumMin) / nSteps);
    x = MinimumMax;
  
    NichePoly->v[0].x = x;
    NichePoly->v[0].y = 0.0f;
  
    //printf("MaximumMin=%f MinimumMax=%f\t Step=%f\n",MaximumMin,MinimumMax,Step );
    //printf("vertice %d -> x-%f   y-%f\n",0,NichePoly->v[0].x, NichePoly->v[0].y );
  
    for(int i = 0; i <= nSteps; i++){
        // https://en.wikipedia.org/wiki/Truncated_normal_distribution
        Tmp = (x - mi) / sigma;
        //PhiNum = (1.0f/sqrtf(2.0f*pi))*__expf((-0.5f)*(Tmp*Tmp));
        PhiNum = (__frsqrt_rn(2.0f*pi))*__expf((-0.5f)*(Tmp*Tmp));

        // Error function of (x1)
        erfX = ((b-mi) / sigma) / sqrtf(2.0f);
        Tmp = fabsf(erfX);

        //aqui escolher entre qual dos dois usar
        erfY = 1.0f-(1.0f/__powf(1.0f+(erfA*Tmp)+(erfB*(Tmp*Tmp))+(erfC*__powf(Tmp,3.0f))+(erfD*__powf(Tmp,4.0f)),4.0f));
        //erfY=erff(Tmp); erfY=0.95487386

        if(erfX < 0.0f){
            erfY = -1.0f * erfY;
        }

        PhiDen1 = (1.0f+erfY) / 2.0f;

        // Error function of (x2)
        erfX = ((a-mi) / sigma) / sqrt(2.0f);
        Tmp = fabs(erfX);

        //aqui escolher entre qual dos dois usar
        erfY = 1.0f-(1.0f/__powf(1.0f+(erfA*Tmp)+(erfB*(Tmp*Tmp))+(erfC*__powf(Tmp,3.0f))+(erfD*__powf(Tmp,4.0f)),4.0f));
        //erfY=erff(Tmp);

        if(erfX < 0.0f){
            erfY = -1.0f * erfY;
        }

        PhiDen2 = (1.0f+erfY) / 2.0f;

        p = (PhiNum / (sigma * (PhiDen1 - PhiDen2)));

        NichePoly->v[i+1].x = x;
        NichePoly->v[i+1].y = p;

        //printf("vertice %d -> x-%f   y-%f\n",i+1,NichePoly->v[i+1].x, NichePoly->v[i+1].y );
        x = x - Step;
    }

    NichePoly->v[nSteps+2].x = NichePoly->v[nSteps+1].x;
    NichePoly->v[nSteps+2].y = 0.0f;

    //printf("vertice %d -> x-%f   y-%f\n\n",nSteps+2,NichePoly->v[nSteps+2].x, NichePoly->v[nSteps+2].y );
}

/*__host__ __device__*/__global__ void calcFitness(float * SpNiche,float * LocEnv,float * Fitness){
    // Declare auxiliary private data

    unsigned int espIndex =(blockDim.x * blockIdx.x) + threadIdx.x; //indice da especie
    register unsigned int cellIdx;
    //sai da função se a thread não corresponde a nenhuma espécie (indice da thread maior que quantidade de especies)
    if(espIndex >= NESPECIES) return;

    float StdAreaNoOverlap, StdSimBetweenCenters;
    float MidTol;
    float MinTempTol, MaxTempTol, MinPrecpTol, MaxPrecpTol;
    float MinTempEnv, MaxTempEnv, MinPrecpEnv, MaxPrecpEnv;
    float MidEnv;
    float LocFitness;  
    // Declare private data
    vec_t NichePtns[nSteps+3];  //pontos do poligono do nicho ( da especie ) ( struct com float x e y)
    poly_t ClipedNichePoly = {NichePtns, nSteps+3};
  

    // dados da especie especifica da thread
    //armazenados assim pois são acessados varias vezes porém nunca alterados.
    MinTempTol = SpNiche[espIndex*4 +0];
    MaxTempTol = SpNiche[espIndex*4 + 1];
    MinPrecpTol = SpNiche[espIndex*4 + 2];
    MaxPrecpTol = SpNiche[espIndex*4 + 3];
          
    for(cellIdx=0;cellIdx < NCELLS ; cellIdx++){

        MinTempEnv = LocEnv[(cellIdx*4) + 0];
        MaxTempEnv = LocEnv[(cellIdx*4) + 1];
        MinPrecpEnv = LocEnv[(cellIdx*4) + 2];
        MaxPrecpEnv = LocEnv[(cellIdx*4) + 3];

        // Does the species tolerate the local environment?
        if((MinTempEnv < MinTempTol) || (MaxTempEnv > MaxTempTol)   ||   (MinPrecpEnv < MinPrecpTol) || (MaxPrecpEnv > MaxPrecpTol) ){
            LocFitness = 0.0f;
        }
        // Yes, it tolerates, lets calculate the fitness
        else {
        //Primeira variavel de ambiente
            MidTol = ((MaxTempTol + MinTempTol) / 2.0f);
            if( (MaxTempTol - MinTempTol) < 1E-2 ){
                StdAreaNoOverlap = 1;
                MidEnv = 0.0f;
            }else{
                MidEnv = ((MaxTempEnv + MinTempEnv) / 2.0f);

                //cria poligono da Tolerancia já clipado aqui
                NicheCurve(MinTempTol,MidTol,MaxTempTol,MinTempEnv,MaxTempEnv,&ClipedNichePoly);
                StdAreaNoOverlap = 1-area(&ClipedNichePoly);
            }
            StdSimBetweenCenters = 1 - (fabsf(MidEnv - MidTol) / ((MaxTempTol - MinTempTol)/2.0f));
            // Local fitness, given the first environmental variable
            LocFitness = (StdAreaNoOverlap * StdSimBetweenCenters);


        //Segunda variavel de ambiente
            MidTol = ((MaxPrecpTol + MinPrecpTol) / 2.0f);
            if( (MaxPrecpTol - MinPrecpTol) < 1E-2 ){
                StdAreaNoOverlap = 1;
                MidEnv = 0.0f;
            }else{
                MidEnv = ((MaxPrecpEnv + MinPrecpEnv) / 2.0f);

                //cria poligono da Tolerancia já clipado aqui
                NicheCurve(MinPrecpTol,MidTol,MaxPrecpTol,MinPrecpEnv,MaxPrecpEnv,&ClipedNichePoly);            
                StdAreaNoOverlap = 1 - area(&ClipedNichePoly);
            }
            StdSimBetweenCenters = 1 - (fabsf(MidEnv - MidTol) / ((MaxPrecpTol - MinPrecpTol)/2.0f));
            // Local fitness, given both environmental variables
            LocFitness = LocFitness * (StdAreaNoOverlap * StdSimBetweenCenters);
        }

        // store fitness value of 'espIndex' especie  for 'cellIdx' cell
        Fitness[ (espIndex*NCELLS) + cellIdx ] = LocFitness;
        //printf("LocFit-%.8f  CELL- %d\t Especie- %d\t storing in index %4.4i\n",LocFitness,cellIdx,espIndex, espIndex*NCELLS + cellIdx );
    }
}