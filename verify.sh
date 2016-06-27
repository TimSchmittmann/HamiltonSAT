#!/bin/bash
FILES=graphs/*;
SOLVER=solvers/HamiltonSAT
echo "" > test.txt
for f in $FILES;
do 
	echo $f >> $SOLVER/test.txt;
	./$SOLVER/HamiltonSAT.exe $f | ./hc-verifier -v -w $f >> $SOLVER/test.txt 
done;

