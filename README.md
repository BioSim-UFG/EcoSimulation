# EcoSimulation

Use a aba _Issues_ para reportar bugs e problemas no código. **E ajude sempre que puder**.

Também comente no Código no formato " `// ~BUG: ....` ", para facilitar a localização

é recomendado que sua versão do gcc/g++ seja acima da versão 6.1  -> https://gcc.gnu.org/projects/cxx-status.html#cxx14
se nao tiver, pode usar alguma acima da 4.8.1, e alterar a flag da compilação de `-std=c++14` para `-std=c++11`, dentro do makefile.


comando de terminal para compilar normalmente:

**ubuntu**:
Codigo em C:
    `~ make **mode**`
Substituir *mode* pelo modo desejado, opções:

* `default`: compila da forma padrão, sem otimização (-O0);<br/>
    nao informar o modo é equivalente a selecionar `default`;
* `opt1`: compila programa otimizando-o no nivel 3, as flags `-O1 -march=native -Wno-unused-result` são adicionadas a compilação;
* `opt2`: compila programa otimizando-o no nivel 2, as flags `-O2 -march=native -Wno-unused-result` são adicionadas a compilação;
* `opt3`: compila programa otimizando-o no nivel 1, as flags `-O3 -march=native -Wno-unused-result` são adicionadas a compilação;<br/>
    Caso tenha compilado default ou outro nivel de opt na ultima vez, use o modo all optX, para assim recompilar os arquivos .o, no nivel X desejado;
* `debug`: compila programa com flags `-g -fno-inline-functions -DGLIBCXX_DEBUG`, habilitando o uso de um Debugger;
* `fast`: equivale ao modo `opt3`;
* `all`: "toca" todos os aquivos fontes, fazendo com que o proximo *make* compile todos os arquivos fontes;
* `clear`: deleta todos os aquivos .o;
<br/>dicas:<br/>usar `make all <modo>` para forçar compilação de todos aquivos, ao mudar de um modo para outro, exemplo: `make all default`



Codigo em Delphi:
    `~ fpc LibPaleoData2.pas PaleoData.dpr -oprog.exe -Mdelphi && rm *.o *.ppu`
    
**Windows**:
?

comando de terminal para usar o Debugger:   `//obsoleto, usar make all debug`

ubuntu:
    `~ g++ code_in_C-C++/Procedimentos.cc code_in_C-C++/PaleoData.cpp CompressLib/decompressData.c -lz -lm -o Prog.exe -std=c++11 -g -fno-inline-functions -DGLIBCXX_DEBUG`

Windows:

? - > creio que o makefile funcione se adaptar...
