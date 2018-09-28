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

    /*
    auto startTimer(){
         auto start_clock = std::chrono::high_resolution_clock::now();
        return start_clock;
    }
    */
}