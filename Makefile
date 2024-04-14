all: $(wildcard *.cpp) $(wildcard *.h)
	g++ -g $(wildcard *.cpp) -o assemble

test: crt.o test.o output.x
	chmod +x output.x

debug:
	g++ -DDEBUG -g $(wildcard *.cpp) -o assemble

output.x:
	../linker/link crt.o test.o output.x

crt.o:
	./assemble ../linker/crt.s crt.o

test.o:
	./assemble ../linker/test.s test.o

clean:
	rm $(wildcard *.o) output.x