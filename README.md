# EcoSimulation

Use a aba _Issues_ para reportar bugs e problemas no código. Ajude sempre que puder :).


é recomendado que sua versão do gcc/g++ seja acima da versão 6.1  -> https://gcc.gnu.org/projects/cxx-status.html#cxx14
se nao tiver, pode usar alguma acima da 4.8.1, e alterar a flag da compilação de `-std=c++14` para `-std=c++11`, dentro do makefile.

Vendo estatisticas (inclusive uso de memória) da execução:

- rodar o programa com o prefixo `env time -v`, por exemplo: `env time -v ./Prog`
descrição da saida: https://codeforces.com/blog/entry/49371

Este projeto possui dois programas:
- **Simulation**: programa responsável por executar a simulação em sí, e gravar os resultados na pasta *Results/*
- **Vizualization**: programa responsável por ler os resultados da simulação e mostrar a execução visual da simulação.

## Comando de terminal para compilar normalmente:

**ubuntu**:
Codigo em C:
    `~ make **mode**`
Substituir *mode* pelo modo desejado, opções:

- **`default`**: compila da forma padrão, sem otimização (-O0);<br/>
    nao informar o modo é equivalente a selecionar `default`;
- **`opt1`**: compila programa otimizando-o no nivel 3, as flags `-O1 -march=native -Wno-unused-result` são adicionadas a compilação;
- **`opt2`**: compila programa otimizando-o no nivel 2, as flags `-O2 -march=native -Wno-unused-result` são adicionadas a compilação;
- **`opt3`**: compila programa otimizando-o no nivel 1, as flags `-O3 -march=native -Wno-unused-result` são adicionadas a compilação;<br/>
    Caso tenha compilado default ou outro nivel de opt na ultima vez, use o modo all optX, para assim recompilar os arquivos .o, no nivel X desejado;
- **`debug`**: compila programa com flags `-g -fno-inline-functions -DGLIBCXX_DEBUG`, habilitando o uso de um Debugger;
- **`fast`**: equivale ao modo `opt2`;
- **`all`**: "toca" todos os aquivos fontes, fazendo com que o proximo comando *make* compile todos os arquivos fontes;
- **`clean`**: deleta todos os aquivos .o;<br/>

**dicas**:<br/>usar `make all <modo>` para forçar compilação de todos aquivos, ao mudar de um modo para outro, exemplo: `make all default`



Compilar Codigo em Delphi:
    `~ fpc LibPaleoData2.pas PaleoData.dpr -oprog.exe -Mdelphi && rm *.o *.ppu`
