#!/bin/bash

COUNT=`tasklist /FI "IMAGENAME eq ludopen.exe"| tail -n 1 |wc -l`
while [ "$COUNT" -eq "1" ]
do
	echo OK
	sleep 1
	COUNT=`tasklist /FI "IMAGENAME eq ludopen.exe"| tail -n 1 |wc -l`
done
copy /f %temp%\ludopen.exe .\
Ludopen.exe -MAJ
