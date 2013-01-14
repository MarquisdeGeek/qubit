
CC=g++
CFLAGS=-c -Wall

all: qutest

qutest: main.o quSample.o
	$(CC)  main.o quSample.o -o qutest

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

quSample.o: quSample.cpp
	$(CC) $(CFLAGS) quSample.cpp

