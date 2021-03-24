SET inst=1 2 3
SET seed=0 35254

REM[							EXEC		SEM	INST		]
FOR %%I IN (%inst%) DO	FOR %%S IN (%seed%) DO		pmm.exe		%%S 	pmm%%I.txt	
