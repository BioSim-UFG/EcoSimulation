# EcoSimulation

é recomendado que sua versão do gcc/g++ seja acima da versão 6.1
-> https://gcc.gnu.org/projects/cxx-status.html#cxx14

se nao tiver, pode usar alguma acima da 4.8.1, e alterar a flag da compilação de
 -std=c++14 para -std=c++11



comando de terminal para compilar normalmente:

ubuntu:
~ g++ code_in_C-C++/Procedimentos.cc code_in_C-C++/PaleoData.cpp CompressLib/decompressData.c -lz -lm -o Prog.exe -std=c++14

Windows:

?

comando de terminal para usar o Debugger:

ubuntu:
~ g++ code_in_C-C++/Procedimentos.cc code_in_C-C++/PaleoData.cpp CompressLib/decompressData.c -lz -lm -o Prog.exe -std=c++14 -g -fno-inline-functions -DGLIBCXX_DEBUG

Windows:

?


ps: pretendo fazer um makeFile em breve
