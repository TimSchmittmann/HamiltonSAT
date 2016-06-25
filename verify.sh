#!/bin/bash
FILES=graphs/*;
echo "" > test.txt
for f in $FILES;
do 
	echo $f >> test.txt;
	./solvers/HamiltonSAT/HamiltonSAT.exe $f | ./hc-verifier -v -w $f >> test.txt 
done;
