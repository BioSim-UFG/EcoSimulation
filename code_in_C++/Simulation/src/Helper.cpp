#include "Helper.h"

#include <chrono>
#include <string.h>




namespace SimEco{

    void  testArgs(int argc,char const *argv[]){
        if(argc != N_INPUT + 1 ){
            printf(RED("Número de argumentos inválido,") GRN("formato correto: ./<Executavel> <nome_da_simulacao> <numero_de_passos>\n"));
            exit(intException(Exceptions::argException));
            //return false;
        }

        //return true;
    }

    int intException(enum Exceptions ex){
       return (int) ex;
    }

    char Configuration::NAME[];
    char Configuration::SAVEPATH[];
    unsigned int  Configuration::MAX_CELLS;
    unsigned int  Configuration::NUM_FOUNDERS;
    unsigned int  Configuration::TIMESTEPS;

    void Configuration::Configure(){
        FILE* f;
        f = fopen("Configuration.txt","r");

        if(f == NULL){
            perror(RED("Erro ao abrir arquivo de Configuração\n"));
            exit( intException(Exceptions::fileException) );
        }

        fscanf(f, "Simulation_name= %s\n", NAME);
        fscanf(f,"Num_Cells= %u\n",&MAX_CELLS);
        fscanf(f,"Num_Founders= %u\n",&NUM_FOUNDERS);
        fscanf(f, "Num_TimeSteps= %u\n",&TIMESTEPS);
        fscanf(f, "Save_Path= %s\n", SAVEPATH);

        char aux[sizeof(SAVEPATH)];
        for(int i=0 ;SAVEPATH[i]; i++)
            aux[i] = tolower(SAVEPATH[i]);

        if(strcmp(aux, "local") == 0)
            strcpy(SAVEPATH, "./Results");

        fclose(f);
    }



    //Clock class functions
    void Clock::Start(){
        //this->start = std::chrono::high_resolution_clock::now();
        //int id = timers.size();
        //timers[id] = (Clock_count){};
        timer.isPaused = false;
        timer.startClock = std::chrono::high_resolution_clock::now();
    }
    /*
    int Clock::timerStart(int id){
        timers[id] = (Clock_count){};
        timers[id].isPaused = false;
        timers[id].start = std::chrono::high_resolution_clock::now();
        return id;
    }*/

    void Clock::Pause(){
        if (timer.isPaused == true)
            return;

        timer.pausedClock = std::chrono::high_resolution_clock::now();
        timer.isPaused = true;
    }

    void Clock::Continue(){
        timer.isPaused = false;
        //faz o start avançar o mesmo tanto de clocks que passaram durante a pausa do timer
        timer.startClock += (std::chrono::high_resolution_clock::now() - timer.pausedClock );
    }

    double Clock::End(){
        //this->end = std::chrono::high_resolution_clock::now();
        //elapsed = end - start;

        Pause();    //pausa o clock/timer se ele nao estiver pausado já.
        
        std::chrono::duration<double> elapsed;
        elapsed = timer.pausedClock - timer.startClock;
        return elapsed.count();
    }

    /*double Clock::elapsedTime(){
        return elapsed.count();
    }
    */

    void recordTimeStepFiles(const char *path, int timeStep, Grid g, const char *simName){
        char fnameTxt[83];
        char fnameBin[80];
        for (uint i = 0; i < g.species.size(); i++){
            //sprintf(fname, "%s/timeStep%u", path, i);
           
           
            //sprintf(fnameTxt, "%s/%s_Esp%d_Time%d.txt", path, simName, g.species[i]._name, timeStep);
            //recordSpecieTxtFile(fnameTxt, timeStep, g.species[i]);

            sprintf(fnameBin, "%s/%s_Esp%d_Time%d", path, simName, g.species[i]._name, timeStep);
            recordSpecieFile(fnameBin, timeStep, g.species[i]);
        }
    }

    void recordSpecieTxtFile(const char *path, int timeStep, Specie &sp){
        FILE *f = fopen(path, "w");
        
        if (f == NULL){
            printf(RED("Falha ao abrir o arquivo %s\n"), path);
            fflush(stdout);
            fclose(f);
            exit(intException(Exceptions::fileException));
        }

        /*
        int cont = 0;
        for (auto &cellInfo : sp.cellsPopulation){
            fprintf(f, "%5u ", cellInfo.first); //escreve o numero de cada célula ocupada pela espécie (no timeStep indicado)
            fprintf(f, "%.4f\n",cellInfo.second); //escreve o numero de população em cada célula
        }
        */
       for(uint i=0; i<sp.cellsPopulation.size(); i++){
           if(sp.cellsPopulation[i]>0.0f){
                fprintf(f, "%5u ", i); //escreve o numero de cada célula ocupada pela espécie (no timeStep indicado)
                fprintf(f, "%f\n", sp.cellsPopulation[i]); //escreve o numero de população em cada célula
           }
       }

        fclose(f);
    }

    void recordSpecieFile(const char *path, int timeStep, Specie &sp){
        FILE *f = fopen(path, "wb");

        if (f == NULL){
            printf(RED("Falha ao abrir o arquivo %s\n"), path);
            fflush(stdout);
            fclose(f);
            exit(intException(Exceptions::fileException));
        }

        /*
        for (auto &cellInfo : sp.cellsPopulation){
            //grava no arquivo uma célula ocupada, primeiro o índice da célula e logo em seguida sua população
            fwrite(&cellInfo.first, sizeof(uint), 1, f);
            fwrite(&cellInfo.second, sizeof(float), 1, f);
        }*/
        for(uint i=0; i<sp.cellsPopulation.size(); i++){
           if(sp.cellsPopulation[i]>0.0f){
               //grava no arquivo uma célula ocupada, primeiro o índice da célula e logo em seguida sua população
               fwrite(&i, sizeof(uint), 1, f);
               fwrite(&sp.cellsPopulation[i], sizeof(float), 1, f);
           }
        }

        fclose(f);
    }

    //cria o diretorio no caminho ( e o caminho se o tal nao existir ainda), apenas se ele não existir
    void create_SimulationDirectory(const char *simName){
        char pasta[80];
        #ifdef __linux__
        sprintf(pasta, "mkdir -p Results/%s &", simName);
        #elif _WIN32
        sprintf(pasta, "mkdir Results\%s", simName);
        #endif
        
        system(pasta);
    }

    void create_timeStepsDirs(const char *_name,int nSteps, vector<bool> &createdDir){

		//cria um subdiretorio para cada timestep
		char dir[50], comand[61];
		#ifdef __linux__
		sprintf(dir, "Results/%s/timeStep", _name);
		#elif __WIN32
		sprintf(dir, "Results\\%s\\timeStep", _name);
		#endif
		
		int len = strlen(dir);
		for(int i=0; i< nSteps; i++){
			sprintf(dir+len,"%d",i);
		#ifdef __linux__
			sprintf(comand, "mkdir -p %s &", dir);
		#elif __WIN32
			sprintf(comand, "mkdir %s &", dir);
		#endif
			system(comand);

            createdDir[i]=true;
        }

    }
}