SET inst=1 2 3
SET seed=0 35254

REM[												EXEC		SEM	MET	INST		TEMPO	SAIDA]

REM[SA																									ALFA	SA_MAX	T_0		T_C]
FOR %%I IN (%inst%) DO	FOR %%S IN (%seed%) DO		pmm.exe		%%S 0 	pmm%%I.txt	5		saida.txt	0.975	2		1000	0.01

REM[BT																									TAM_LISTA]
FOR %%I IN (%inst%) DO	FOR %%S IN (%seed%) DO		pmm.exe		%%S 1 	pmm%%I.txt	5		saida.txt	2

REM[GRASP																								LRC]
FOR %%I IN (%inst%) DO	FOR %%S IN (%seed%) DO		pmm.exe		%%S 2 	pmm%%I.txt	5		saida.txt	15

REM[AG																									POP	CRO	MUT	ELI]
FOR %%I IN (%inst%) DO	FOR %%S IN (%seed%) DO		pmm.exe		%%S 3 	pmm%%I.txt	5		saida.txt	10	2	7	15

REM[VNS]
FOR %%I IN (%inst%) DO	FOR %%S IN (%seed%) DO		pmm.exe		%%S 4 	pmm%%I.txt	5		saida.txt

REM[ILS]
FOR %%I IN (%inst%) DO	FOR %%S IN (%seed%) DO		pmm.exe		%%S 5 	pmm%%I.txt	5		saida.txt