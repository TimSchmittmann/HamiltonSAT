RM := rm -rf

solosat: solosat.cpp
	mkdir -m 777 "../solvers/solosat"
	g++ -o "../solvers/solosat/solosat.exe" -std=c++0x solosat.cpp -I/home/furio/Downloads/boost_1_61_0 -L/home/furio/Downloads/boost_1_61_0/stage/lib -lboost_filesystem -static -lboost_system
	mkdir -m 666 "../solvers/solosat/tmp"
	rsync -a "bin" "../solvers/solosat/"
	
clean:
	-$(RM) ../solvers/solosat/
