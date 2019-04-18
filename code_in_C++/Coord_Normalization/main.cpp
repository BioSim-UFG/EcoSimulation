#include "coordinateNormalization.h"
#include <iostream>

int main(){
    Normalize normal("../../input", "DummyHex2566 - Coords and Clim", ".txt");
    std::cout << normal.getNewFilePath() << std::endl;
    std::cout << normal.getOldFilePath() << std::endl;

    return 0;
}