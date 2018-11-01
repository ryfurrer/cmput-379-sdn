# ------------------------------------------------------------
# Makefile for CMPUT 379 A2
#
# Usage: make -f Makefile compile  // compile the programs
#        make -f Makefile tar      // create a 'tar.gz' archive of 'allFiles'
#        make -f Makefile clean     // remove unneeded files
# ------------------------------------------------------------

target=	submit
progFiles= a2sdn.cpp controller.cpp switch.cpp connection.cpp packet.cpp
allFiles= $(progFiles) Makefile connection.h packet.h controller.h switch.h flowTable.h report.pdf
# ------------------------------------------------------------


compile:
	g++ -std=c++11 -Wall $(progFiles) -o a2sdn

tar:
	tar -cvf $(target).tar $(allFiles)
	gzip $(target).tar

clean:
	rm *~ out.* *.o
