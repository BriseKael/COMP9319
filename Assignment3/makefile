EXE = a3search
OBJS = porter2_stemmer.o
CC = g++ -std=c++11 -Wall -pedantic -I.
CCOPTS = -O3
LINKER = g++ -std=c++11 -Wall -pedantic -I.

all: $(EXE)

$(EXE): $(OBJS) main.cpp
	$(LINKER) main.cpp -o $@ $(OBJS)

porter2_stemmer.o: external.tar
	tar -xvf external.tar
	$(CC) $(CCOPTS) -c porter2_stemmer.cpp -o $@

clean:
	rm -f *.o $(EXE)
	rm -f *.h porter2_stemmer.cpp