#!/bin/bash

flags=

if [ "$1" = "--help" ]; then
	echo "Autor: João Gabriel S. Fernandes"
	echo "Versao 0.1"
	echo "Esse script compara os arquivos de saída gerados por duas simulações,"
	echo ""
	echo "informe a pasta ou caminho para as pastas geradas pelas simulações (sem o '/' no final) "
	echo "que deseja checar a diferença, e flags diff(opcional)"
	echo "ex: ./simdiff.sh PastaSimResult Debug -qy"
	echo ""; echo ""
	echo "para mais infações sobre flags do diff, digite diff --help"

	exit 0
fi

if [ "$#" -lt 2 ];
then
	echo "argumentos insuficientes"
	echo "use --help para mais informações"
	exit 1
fi

if [ "$#" -ge 3 ]; then
	#temp=$2
	#flags=-${temp:1}
	for flg in "${@:3}"; do
		flags+=" ${flg}"
	done
fi

if [ ! -d $1 ]; then
	echo "o diretório ${1} não foi encontrado!"
	exit 2
fi

if [ ! -d $2 ]; then
	echo "o diretório ${2} não foi encontrado!"
	exit 2
fi

path1=$1
path2=$2

for file1 in $(\ls -d ${path1}/timeStep*/${path1}_Esp*_Time*); do


	file2=${file1//$path1/$path2}

	diff ${file1} ${file2} ${flags}
	#descomentar para ver estatisticas ( porcentagem de semelhanca/diferença, quantidade de itens diferentes, etc)
	#wdiff ${file1} ${file2} -123s

done

