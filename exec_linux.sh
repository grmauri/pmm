#!/bin/sh
inst="1 2 3"
seed="0 35254"

for i in $inst; 
do
	for s in $seed; 
	do 
	#SA	EXEC	SEM		MET	INST				TEMPO	SAIDA		ALFA	SA_MAX	T_0		T_C
		./pmm 	"${s}" 	0 	'pmm'"${i}"'.txt'	5		saida.txt	0.975	2		1000	0.01
	done
done

for i in $inst; 
do
	for s in $seed; 
	do 
	#BT	EXEC	SEM		MET	INST				TEMPO	SAIDA		TAM_LISTA
		./pmm 	"${s}" 	1 	'pmm'"${i}"'.txt'	5		saida.txt	2
	done
done

for i in $inst; 
do
	for s in $seed; 
	do 
	#GRASP	EXEC	SEM		MET	INST				TEMPO	SAIDA		LRC
			./pmm 	"${s}" 	2 	'pmm'"${i}"'.txt'	5		saida.txt	15
	done
done

for i in $inst; 
do
	for s in $seed; 
	do 
	#AG	EXEC	SEM		MET	INST				TEMPO	SAIDA		POP	CRO	MUT	ELI
		./pmm 	"${s}" 	3 	'pmm'"${i}"'.txt'	5		saida.txt	10	2	7	15
	done
done

for i in $inst; 
do
	for s in $seed; 
	do 
	#VNS	EXEC	SEM		MET	INST				TEMPO	SAIDA
			./pmm 	"${s}" 	4 	'pmm'"${i}"'.txt'	5		saida.txt
	done
done

for i in $inst; 
do
	for s in $seed; 
	do 
	#ILS	EXEC	SEM		MET	INST				TEMPO	SAIDA
			./pmm 	"${s}" 	5 	'pmm'"${i}"'.txt'	5		saida.txt
	done
done
