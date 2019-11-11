VPATH = src

EXECS =  tnp1 tnp2 \
		 fibonacci fibonacci2 fibonacci3 \
		 heatnptfg

EXECS_X =  heath  heath_t  dsor dsortbb

EXECSOMP = heatf heatf2 heatomp  sorf sorf2 soromp

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning library paths
# """""""""""""""""""""""""""""""""""""""
LBASE  = -lpthread

PATHV = -L../vath_tfg


include makefile-compiler

# """"""""""""""""""
# My codes
# """""""""""""""""" 
tnp1:   TNPool1.o
	${CXX} -otnp1 TNPool1.o ${PATHV} ${LBASE} -lvath 
	rm TNPool1.o

tnp2:   TNPool2.o
	${CXX} -otnp2 TNPool2.o ${PATHV} ${LBASE} -lvath
	rm TNPool2.o

fibonacci: fibonacci.o
	${CXX} -ofibonacci fibonacci.o ${PATHV} ${LBASE} -lvath
	rm fibonacci.o

fibonacci2: fibonacci2.o
	${CXX} -ofibonacci2 fibonacci2.o ${PATHV} ${LBASE} -lvath
	rm fibonacci2.o

fibonacci3: fibonacci3.o
	${CXX} -ofibonacci3 fibonacci3.o -pthread ${PATHV} ${LBASE} -lvath
	rm fibonacci3.o

heatnptfg:  HeatNPtfg.o HeatAux.o
	${CXX} -oheatnptfg HeatNPtfg.o HeatAux.o ${PATHV} ${LBASE} -lvath -ltbb 
	rm HeatNPtfg.o

addvec:  AddVectors.o
	${CXX} -oaddvec AddVectors.o ${PATHV} ${LBASE} -lvath 
	rm AddVectors.o


#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""
clean:
	rm -f ${EXECS} ${EXECS_X} ${EXECSOMP}
	rm *.o

all:    ${EXECS}
allx:    ${EXECS_X}
allomp: ${EXECSOMP}


.PHONY: runall
runall:
	./tnp1
	./tnp2
	./fibonacci
	./fibonacci2
	./fibonacci3
	./heattfg

.PHONY: runallx
runallx:
	./heath
	./heath_t
	./sorth
