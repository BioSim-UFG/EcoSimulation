#include "Helper.h"
#include <chrono>

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
}