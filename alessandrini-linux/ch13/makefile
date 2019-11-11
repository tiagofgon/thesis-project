VPATH = src

EXECS =  md  mdth mdtbb
EXECSOMP = mdomp mdompf

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning library paths
# """""""""""""""""""""""""""""""""""""""

LBASE  = -lpthread

ifdef CPP11_ENV
PATHV = -L../vath_std
else
PATHV = -L../vath_pth 
endif

include makefile-compiler

#""""""""""""""""""""""""""""""""""""""""""""""""""""""""
# This part deals with the applications in subdirectories
# of this directory.
# """""""""""""""""""""""""""""""""""""""""""""""""""""""

mdth:   MdTh.o MdAux.o
	${CXX} -omdth MdTh.o MdAux.o ${PATHV} ${LBASE} -lvath 

mdtbb:  MdTbb.o MdAux.o
	${CXX} -omdtbb MdTbb.o MdAux.o ${PATHV} ${LBASE} -lvath -ltbb

md:     Md.o MdAux.o
	${CXX} -omd Md.o MdAux.o ${PATHV} ${LBASE} -lvath 


mdompf:  MdOmpF.o MdAux.o
	${CXX} -omdompf ${OPENMP_FLAG} MdOmpF.o MdAux.o ${PATHV} ${LBASE} -lvath

mdomp:  MdOmp.o MdAux.o
	${CXX} -omdomp ${OPENMP_FLAG} MdOmp.o MdAux.o ${PATHV} ${LBASE} -lvath

#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS} ${EXECSOMP}
	rm *.o

all:    ${EXECS}
allomp: ${EXECSOMP}

.PHONY: runall
runall:
	./md
	./mdth
	./mdtbb

.PHONY: runomp
runomp:
	./mdomp
	./mdompf
