#ifndef HELPER_H
#define HELPER_H

#include "color.h"

#include <iostream>
#include <chrono>

#define N_INPUT 2


namespace SimEco{

    enum class Exceptions {fileException=1, argException = 4};

    
    int intException(enum Exceptions ex);
    bool testArgs(int argc,char const *argv[]);

    //auto startTimer();
}

#endif