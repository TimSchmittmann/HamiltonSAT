RM := rm -rf

solosat: solosat.cpp
	#mkdir -m 777 "../solvers/solosat"
	g++ -o "../solvers/solosat/solosat.exe" -std=c++0x solosat.cpp -L../solvers/solosat/lib -lboost_filesystem -lboost_system
	#mkdir -m 666 "../solvers/solosat/tmp"
	#rsync -a "bin" "../solvers/solosat/"
	
clean:
	-$(RM) ../solvers/solosat/
