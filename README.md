# EcoSimulation

Use a aba _Issues_ para reportar bugs e problemas no código. **E ajude sempre que puder**.

Também comente no Código no formato " `// ~BUG: ....` ", para facilitar a localização

é recomendado que sua versão do gcc/g++ seja acima da versão 6.1  -> https://gcc.gnu.org/projects/cxx-status.html#cxx14
se nao tiver, pode usar alguma acima da 4.8.1, e alterar a flag da compilação de `-std=c++14` para `-std=c++11`



comando de terminal para compilar normalmente:

**ubuntu**:
Codigo em C:
    `~ g++ code_in_C-C++/Procedimentos.cc code_in_C-C++/PaleoData.cpp CompressLib/decompressData.c -lz -lm -o Prog.exe -std=c++11 -O3 -march=native`
Codigo em Delphi:
    `~ fpc LibPaleoData2.pas PaleoData.dpr -oprog.exe -Mdelphi && rm *.o *.ppu`
**Windows**:

?

comando de terminal para usar o Debugger:

ubuntu:
    `~ g++ code_in_C-C++/Procedimentos.cc code_in_C-C++/PaleoData.cpp CompressLib/decompressData.c -lz -lm -o Prog.exe -std=c++11 -g -fno-inline-functions -DGLIBCXX_DEBUG`

Windows:

?


ps: pretendo fazer um makeFile em breve
