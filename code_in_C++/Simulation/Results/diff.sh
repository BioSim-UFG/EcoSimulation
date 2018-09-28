#!/bin/bash

min=1
flags= 

if [ "$1" = "--help" ]; then
	echo "esse script compara os arquivos de saída gerados pela simulação"
	echo "cada espécie tem cada timeStep seu comparados com o timeStep anterior"
	echo "arquivos de saída entre espécies não são comparados entre si"
	echo ""
	echo "informe a pasta gerada pela simulação (sem a '/' no final) "
	echo "que quer checar o diff e a flag diff(opcional)"
	echo "ex: ./diff.sh PastaSimResult -s"

	exit 0
fi

if [ "$#" -lt 1 ];
then
	echo "use --help para mais informações"
	exit 1
fi

if [ "$#" -eq 2 ]; then
	temp=$2
	flags=${temp:1}
fi

path=$1

cont=0

for esp in $(\ls -d ${path}/timeStep0/${path}_Esp*_Time0); do
	pastTime=$(\ls -d ${path}/timeStep0/${path}_Esp${cont}_Time0)
	#remove a primeira linha/iteração do loop, e então faz diff do ultimo elemento com o proximo
	for i in $(\ls -d ${path}/timeStep*/${path}_Esp${cont}_Time* | tail -n +2| \sort -V); do
		diff -r${flags} ${pastTime} ${i};
		pastTime=${i};		
 	done
	((++cont));
done
