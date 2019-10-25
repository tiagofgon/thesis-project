ifdef CPP11_ENV
VPATH = src 
else
VPATH = srcbis 
endif

EXECS =   tstbb tsnp tswsp tsomp

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning library paths
# """""""""""""""""""""""""""""""""""""""
LBASE  = -lpthread

ifdef CPP11_ENV
PATHV = -L../../vath_std
else
PATHV = -L../../vath_pth 
endif

include makefile-compiler

# Tasks, NPool managed threads
# ----------------------------
tsnp:  TSnp.o SorAux.o
	${CXX} -otsnp ${OPENMP_FLAG} TSnp.o SorAux.o ${PATHV} ${LBASE} -lvath -ltbb
	rm TSnp.o

# Tasks, TBB managed threads
# --------------------------
tstbb:  TStbb.o SorAux.o
	${CXX} -otstbb TStbb.o SorAux.o ${PATHV} ${LBASE} -lvath -ltbb
	rm TStbb.o

# Tasks, OpenMP threads
# ---------------------
tsomp:  TSomp.o SorAux.o
	${CXX} -otsomp ${OPENMP_FLAG} TSomp.o SorAux.o ${PATHV} ${LBASE} -lvath
	rm TSomp.o

# Tasks, WSPool threads
# ---------------------
tswsp:  TSwsp.o SorAux.o
	${CXX} -otswsp ${OPENMP_FLAG} TSwsp.o SorAux.o ${PATHV} ${LBASE} -lvath -ltbb
	rm TSwsp.o


#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""
clean:
	rm -f ${EXECS}
	rm *.o

all:    ${EXECS}

.PHONY: runall
runall:
	./tstbb
	./tsomp
	./tsnp
	./tswsp

