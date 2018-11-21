# ------------------------------------------------------------
# Makefile for CMPUT 379 A3
#
# Usage: make  // compile the programs
#        make tar // create a 'tar.gz' archive of 'allFiles'
#        make clean // remove unneeded files
# ------------------------------------------------------------

CC= g++
CFLAGS= -std=c++11 -Wall -I.
OBJ = a3sdn
target=	submit
progFiles= parsers.cpp  util.cpp connection.cpp  packet.cpp switch.cpp controller.cpp a3sdn.cpp
progObects= a3sdn.o  connection.o  controller.o  packet.o  parsers.o  switch.o  util.o
allFiles= *.cpp Makefile *.h report.pdf
# ------------------------------------------------------------



all: $(progObects)
	$(CC) $(progObects) -o $(OBJ)

a3sdn.o: a3sdn.cpp switch.h controller.h
	$(CC) -c a3sdn.cpp $(CFLAGS)
connection.o: connection.cpp
	$(CC) -c $^ $(CFLAGS)
controller.o: controller.cpp connection.h packet.h util.h
	$(CC) -c controller.cpp $(CFLAGS)
packet.o: packet.cpp
	$(CC) -c $^ $(CFLAGS)
parsers.o: parsers.cpp
	$(CC) -c $^ $(CFLAGS)
switch.o: switch.cpp
	$(CC) -c $^ $(CFLAGS)
util.o: util.cpp
	$(CC) -c $^ $(CFLAGS)

compile:
	$(CC) -c $(progFiles) $(CFLAGS)

tar:
	tar -cvf $(target).tar $(allFiles)
	gzip $(target).tar

clean:
	rm *~ out.* *.o *.out -f
