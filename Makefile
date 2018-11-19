# ------------------------------------------------------------
# Makefile for CMPUT 379 A3
#
# Usage: make -f Makefile compile  // compile the programs
#        make -f Makefile tar      // create a 'tar.gz' archive of 'allFiles'
#        make -f Makefile clean     // remove unneeded files
# ------------------------------------------------------------

target=	submit
progFiles= *.cpp
allFiles= $(progFiles) Makefile *.h report.pdf
# ------------------------------------------------------------


compile:
	g++ -std=c++11 -Wall $(progFiles) -o a3sdn

tar:
	tar -cvf $(target).tar $(allFiles)
	gzip $(target).tar

clean:
	rm *~ out.* *.o -f
