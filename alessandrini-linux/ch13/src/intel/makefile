EXECS =  md mdth mdtbb :
EXECSOMP = mdomp mdompv mdompf

PATHV = -L../../vath_pth 

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning library paths
# """""""""""""""""""""""""""""""""""""""
CPPFLAGS = -I../../../vath_pth/include -I../../include
CXX = icpc

ifdef OPENMP_ENV
CXXFLAGS = -O3 -openmp -vec-report=3
else
CXXFLAGS = -O3 -vec-report=3
endif

#""""""""""""""""""""""""""""""""""""""""""""""""""""""""
# This part deals with the applications in subdirectories
# of this directory.
# """""""""""""""""""""""""""""""""""""""""""""""""""""""

md:     md.o mdaux.o
	${CXX} -omd md.o mdaux.o ${PATHV} -lvath 

mdth:   mdth.o mdaux.o
	${CXX} -omdth mdth.o mdaux.o ${PATHV} -lpthread -lvath 

mdtbb:   MdTbb.o mdaux.o
	${CXX} -omdtbb MdTbb.o mdaux.o ${PATHV} -lpthread vi-ltbb  -lvath 

mdompf:  mdompf.o mdaux.o
	${CXX} -openmp -omdompf mdompf.o mdaux.o ${PATHV} -lvath

mdomp:  mdomp.o mdaux.o
	${CXX} -openmp -omdomp mdomp.o mdaux.o 

mdompv:  mdompv.o mdaux.o
	${CXX} -openmp -omdompv mdompv.o mdaux.o 

#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS} ${EXECSOMP}
	rm *.o

all:    ${EXECS}
allomp: ${EXECSOMP}

.PHONY: test
test:
	./md

.PHONY: tomp
tomp:
	./mdomp
