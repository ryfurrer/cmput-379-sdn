# ------------------------------------------------------------
# Makefile for CMPUT 379 A2
#
# Usage: make -f Makefile compile  // compile the programs
#        make -f Makefile tar      // create a 'tar.gz' archive of 'allFiles'
#        make -f Makefile clean     // remove unneeded files
# ------------------------------------------------------------

target=	submit
progFiles= controller.cpp switch.cpp a2sdn.cpp
allFiles= $(progFiles) Makefile a2sdn.h controller.h switch.h flowTable.h report.pdf
# ------------------------------------------------------------


compile:
	g++ -std=c++11 -Wall $(progFiles) -o a2sdn

tar:
	tar -cvf $(target).tar $(allFiles)
	gzip $(target).tar

clean:
	rm *~ out.* *.o
