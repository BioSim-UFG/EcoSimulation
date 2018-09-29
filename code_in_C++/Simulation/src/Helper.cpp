#include "Helper.h"
#include <chrono>

namespace SimEco{

    bool  testArgs(int argc,char const *argv[]){
        if(argc != N_INPUT + 1 ){
            printf(RED("Número de argumentos inválido,") GRN("formato correto: ./<Executavel> <nome_da_simulacao> <numero_de_passos>\n"));
            return false;
        }

        return true;
    }

    int intException(enum Exceptions ex){
       return (int) ex;
    }


    //Clock class functions
    void Clock::timerStart(){
        this->start = std::chrono::high_resolution_clock::now();
    }

    void Clock::timerEnd(){
        this->end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
    }

    double Clock::elapsedTime(){
        return elapsed.count();
    }

    Clock::Clock(){}
    Clock::~Clock(){}

}