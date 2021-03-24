#!/bin/sh
inst="1 2 3"
seed="0 35254"

for i in $inst; 
do
	for s in $seed; 
	do 
	#	EXEC	SEM		INST			
		./pmm 	"${s}" 	'pmm'"${i}"'.txt'
	done
done
