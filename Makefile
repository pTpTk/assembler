all: $(wildcard *.cpp) $(wildcard *.h)
	g++ -g $(wildcard *.cpp) -o assemble
	# g++ -g main.cpp -o assemble

test:
	./assemble ../C-Interpreter/test/test.s output.o

debug:
	g++ -DDEBUG -g $(wildcard *.cpp) -o assemble