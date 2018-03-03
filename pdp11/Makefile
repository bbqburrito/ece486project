all: main.o pdp11.o
	g++ -g -Wall -std=c++11 main.o pdp11.o -o pdp11

pdp11: main.o pdp11.o
	g++ -g -Wall -std=c++11 main.o pdp11.o -o pdp11

main.o: main.cpp
	g++ -g -Wall -std=c++11 -c main.cpp -o main.o

pdp11.o: pdp11.cpp
	g++ -g -Wall -std=c++11 -c pdp11.cpp -o pdp11.o

clean:
	rm main.o pdp11.o pdp11  
