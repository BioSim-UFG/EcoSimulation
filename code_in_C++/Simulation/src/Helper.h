#ifndef HELPER_H
#define HELPER_H

#include "color.h"

#include <iostream>
#include <chrono>

#define N_INPUT 2


namespace SimEco{

    enum class Exceptions {fileException=1,decompStrException,memRealocException, argException = 4};
    
    int intException(enum Exceptions ex);
    bool testArgs(int argc,char const *argv[]);

    //Timer class
    class Clock
    {   public:
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
        std::chrono::duration<double> elapsed;

        Clock();
        ~Clock();
        void timerStart();
        void timerEnd();
        double elapsedTime();
    };
}

#endif