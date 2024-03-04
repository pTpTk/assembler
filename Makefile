all: $(wildcard *.cpp) $(wildcard *.h)
	# g++ -g $(wildcard *.cpp) -o assemble
	g++ -g main.cpp -o assemble
