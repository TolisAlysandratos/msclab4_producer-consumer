CC=g++ -Wall

all: main

main: helper.o main.o helper.h
	$(CC) -pthread -o main helper.o main.o

main.o: helper.cc main.cc helper.h
	$(CC) -c helper.cc main.cc

tidy:
	rm -f *.o core

clean:
	rm -f main producer consumer *.o core
