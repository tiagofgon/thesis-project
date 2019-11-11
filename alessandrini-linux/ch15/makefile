VPATH = include src

EXECS =    sor sth_bl sth_sbl sth_q sortbb heat heathb heathq heatbb
EXECSOMP = somp_bl somp_sbl somp_q heatompb heatompq

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

heat:   Heat.o 
	${CXX} -oheat Heat.o ${PATHV} ${LBASE} -lvath 
	rm *.o

heathb: HeatThB.o
	${CXX} -oheathb HeatThB.o  ${PATHV} ${LBASE} -lvath 
	rm *.o

heathq: HeatThQ.o
	${CXX} -oheathq HeatThQ.o  ${PATHV} ${LBASE} -lvath -ltbb 
	rm *.o

heatbb:  HeatTBB.o
	${CXX} -oheatbb HeatTBB.o ${PATHV} ${LBASE} -lvath -ltbb 
	rm *.o


sor:    Sor.o SorAux.o
	${CXX} -osor Sor.o SorAux.o ${PATHV} ${LBASE} -lvath 
	rm Sor.o

sord:   SorDiag.o
	${CXX} -osord SorDiag.o  ${PATHV} ${LBASE} -lvath 
	rm *.o

sordb:   SorDiagb.o
	${CXX} -osordb SorDiagb.o ${PATHV} ${LBASE} -lvath 
	rm *.o

sth_bl: SorThBL.o SorAux.o
	${CXX} -osth_bl SorThBL.o SorAux.o ${PATHV} ${LBASE} -lvath 
	rm SorThBL.o

sth_sbl: SorThSBL.o SorAux.o
	${CXX} -osth_sbl SorThSBL.o SorAux.o ${PATHV} ${LBASE} -lvath 
	rm SorThSBL.o

sth_q: SorThQ.o SorAux.o
	${CXX} -osth_q SorThQ.o SorAux.o ${PATHV} ${LBASE} -lvath 
	rm SorThQ.o

sortbb: SorTBB.o SorAux.o
	${CXX} -osortbb SorTBB.o SorAux.o ${PATHV} ${LBASE} -lvath -ltbb 
	rm SorTBB.o

#---------------< OPENMP codes >----------------------
heatompb:  HeatOmpB.o 
	${CXX} -oheatompb ${OPENMP_FLAG} HeatOmpB.o ${PATHV} ${LBASE} -lvath
	rm *.o

heatompq:  HeatOmpQ.o
	${CXX} -oheatompq ${OPENMP_FLAG} HeatOmpQ.o ${PATHV} ${LBASE} -lvath -ltbb
	rm *.o

somp_bl: SorOmpBL.o SorAux.o
	${CXX} -osomp_bl ${OPENMP_FLAG} SorOmpBL.o SorAux.o ${PATHV} ${LBASE} -lvath
	rm SorOmpBL.o

somp_sbl: SorOmpSBL.o SorAux.o
	${CXX} -osomp_sbl ${OPENMP_FLAG} SorOmpSBL.o SorAux.o ${PATHV} ${LBASE} -lvath
	rm SorOmpSBL.o

somp_q: SorOmpQ.o SorAux.o
	${CXX} -osomp_q ${OPENMP_FLAG} SorOmpQ.o SorAux.o ${PATHV} ${LBASE} -lvath
	rm SorOmpQ.o


#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution with 2 threads, to verify
# that everything is OK.
# """"""""""""""""""""""""""""""""""
clean:
	rm -f ${EXECS} ${EXECSOMP}

all:     ${EXECS}
allomp:  ${EXECSOMP}

.PHONY: runall
runall:
	./heat
	./heathb  
	./heathq
	./heatbb
	./sor
	./sth_bl
	./sth_sbl
	./sth_q
	./sortbb

.PHONY: runomp
runomp:
	./heatompb
	./heatompq
	./somp_bl
	./somp_sbl
	./somp_q
