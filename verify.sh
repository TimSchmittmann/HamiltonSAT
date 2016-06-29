#!/bin/bash
FILES=graphs/*;
SOLVER=solvers/solosat
echo "" > test.txt
for f in $FILES;
do 
	echo $f >> $SOLVER/test.txt;
	./$SOLVER/solosat.exe $f  | ./hc-verifier -v -w $f >> $SOLVER/test.txt
done;

