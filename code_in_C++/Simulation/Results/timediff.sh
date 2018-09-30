#!/bin/bash

min=1
flags=-

if [ "$1" = "--help" ]; then
	echo "Autor: João Gabriel S. Fernandes"
	echo "Versao 0.52"
	echo "esse script compara os arquivos de saída gerados pela simulação,"
	echo "cada espécie tem cada timeStep X seu comparado com o timeStep anterior (X-1), todos com as celulas ocupadas ordenadas."
	echo "timeSteps das espécies não são comparados entre si"
	echo ""
	echo "informe a pasta gerada pela simulação (sem a '/' no final) "
	echo "que quer checar o diff e a flag diff(opcional)"
	echo "ex: ./timediff.sh PastaSimResult -s"
	echo ""; echo ""
	echo "para mais infações sobre flags do diff, digite diff --help"

	exit 0
fi

if [ "$#" -lt 1 ];
then
	echo "use --help para mais informações"
	exit 1
fi

if [ "$#" -eq 2 ]; then
	temp=$2
	flags=-${temp:1}
fi

path=$1

cont=0

for esp in $(\ls -d ${path}/timeStep0/${path}_Esp*_Time0); do
	pastTime=$(\ls -d ${path}/timeStep0/${path}_Esp${cont}_Time0)
	#remove a primeira linha/iteração do loop, e então faz diff do ultimo elemento com o proximo
	for i in $(\ls -d ${path}/timeStep*/${path}_Esp${cont}_Time* | tail -n +2| \sort -V); do
		#numColumns=$(tail -n 1 ${i} | awk '{print NF}')
		strlen=$(basename ${pastTime})
		padSize=$(( (${#i}  - ${#strlen}) +1))

		printf '\e[1;96m'"\n $(basename ${pastTime}) %*s para %*s $(basename ${i}):\n" ${padSize} "" ${padSize} "";
		printf '\e[0m'

		numColumns=$(head -n 1 "${pastTime}" | awk '{print NF}')
		sortedprev=$(cat "${pastTime}" | tr ' ' '\n' | sort -V)
		sortedprev=$(printf "%5d\n" ${sortedprev} | xargs -n${numColumns} -d'\n')
		#echo "${sortedprev}"

		#numColumns=$(head -n 1 "${i}" | awk '{print NF}')
		sortednow=$(cat "${i}" | tr ' ' '\n' | sort -V)
		sortednow=$(printf "%5d\n" ${sortednow} | xargs -n${numColumns} -d'\n')
		#echo "${sortednow}"

		
		diff ${flags}yr <(echo "${sortedprev}")  <(echo "${sortednow}") #$(echo "${pastTime}" |sort -V) $(echo "${i}" | sort -V);
		wdiff -123s "${pastTime}" "${i}"

		pastTime=${i};
 	done
	((++cont));
done

